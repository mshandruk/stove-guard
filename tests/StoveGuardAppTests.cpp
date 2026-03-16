#include <chrono>
#include <cstddef>
#include <vector>

#include <gtest/gtest.h>

#include "Clock.h"
#include "Detection.h"
#include "Frame.h"
#include "FrameTimer.h"
#include "Notifier.h"
#include "ObjectDetection.h"
#include "SceneInterpreter.h"
#include "StoveGuardApp.h"
#include "StoveMonitor.h"

using namespace std::chrono;

class FakeSceneInterpreter : public SceneInterpreter {
  public:
    void nextDetection(const Detection& detection) noexcept {
        nextDetection_ = detection;
    }

    Detection interpret([[maybe_unused]] const ObjectDetections& objectDetections) override {
        return nextDetection_;
    }

  private:
    Detection nextDetection_ = Detection{false, false};
};

class FakeNotifier : public Notifier {
  public:
    using Events = std::vector<Event>;

    void notify(const Event event) override {
        events_.push_back(event);
    }

    [[nodiscard]] const Events& events() const noexcept {
        return events_;
    }

    [[nodiscard]] std::size_t callCount() const noexcept {
        return events_.size();
    }

  private:
    Events events_;
};

class FakeClock final : public Clock {
  public:
    FakeClock()
            : current_{steady_clock::now()} {
    }

    [[nodiscard]]
    steady_clock::time_point getTime() const override {
        return current_;
    }

    void advance(const seconds seconds) {
        current_ += seconds;
    }

  private:
    steady_clock::time_point current_;
};

class StoveGuardAppTest : public testing::Test {
  protected:
    FakeSceneInterpreter scene;
    FakeNotifier notifier;
    FakeClock clock;
    Duration alarmThreshold = seconds{15};
    FrameTimer frameTimer{clock};
    StoveGuardApp app{alarmThreshold, notifier, frameTimer};
    Frame frame{};
};

TEST_F(StoveGuardAppTest, DangerousEntered_WhenStoveOnAndNoPerson) {
    scene.nextDetection({true, false});
    const auto result = scene.interpret({});
    EXPECT_EQ(app.processFrame(result), Event::DangerousEntered);
}

TEST_F(StoveGuardAppTest, ReturnsNone_WhenStoveOnAndPersonPresent) {
    scene.nextDetection({true, true});
    const auto result = scene.interpret({});
    EXPECT_EQ(app.processFrame(result), Event::None);
}

TEST_F(StoveGuardAppTest, ReturnsNone_WhenStoveOffAndNoPerson) {
    scene.nextDetection({false, false});
    const auto result = scene.interpret({});
    EXPECT_EQ(app.processFrame(result), Event::None);
}

TEST_F(StoveGuardAppTest, AlarmStarted_WhenNoPersonForAlarmThreshold) {
    scene.nextDetection({true, false});
    const auto result = scene.interpret({});
    ASSERT_EQ(app.processFrame(result), Event::DangerousEntered);

    clock.advance(alarmThreshold);
    EXPECT_EQ(app.processFrame(result), Event::AlarmStarted);
}

TEST_F(StoveGuardAppTest, AlarmCanceled_WhenPersonAppearsAfterAlarm) {
    scene.nextDetection({true, false});
    auto result = scene.interpret({});
    ASSERT_EQ(app.processFrame(result), Event::DangerousEntered);

    clock.advance(alarmThreshold);
    ASSERT_EQ(app.processFrame(result), Event::AlarmStarted);

    clock.advance(alarmThreshold + seconds{1});
    scene.nextDetection({true, true});
    result = scene.interpret({});
    EXPECT_EQ(app.processFrame(result), Event::AlarmCleared);
}

TEST_F(StoveGuardAppTest, AlarmNotStarted_WhenPersonAppearsBeforeAlarm) {

    scene.nextDetection({true, false});
    auto result = scene.interpret({});
    ASSERT_EQ(app.processFrame(result), Event::DangerousEntered);

    clock.advance(alarmThreshold - seconds{1});
    ASSERT_EQ(app.processFrame(result), Event::None);

    clock.advance(alarmThreshold);
    scene.nextDetection({true, true});
    result = scene.interpret({});
    EXPECT_EQ(app.processFrame(result), Event::DangerousCleared);
}

TEST_F(StoveGuardAppTest, AlarmRestarted_WhenPersonLeavesAfterClearingAlarm) {
    scene.nextDetection({true, false});
    auto result = scene.interpret({});
    ASSERT_EQ(app.processFrame(result), Event::DangerousEntered);

    clock.advance(alarmThreshold);
    ASSERT_EQ(app.processFrame(result), Event::AlarmStarted);

    scene.nextDetection({true, true});
    result = scene.interpret({});
    ASSERT_EQ(app.processFrame(result), Event::AlarmCleared);

    clock.advance(seconds{2});
    scene.nextDetection({true, false});
    result = scene.interpret({});
    ASSERT_EQ(app.processFrame(result), Event::DangerousEntered);

    clock.advance(alarmThreshold);
    EXPECT_EQ(app.processFrame(result), Event::AlarmStarted);
}

TEST_F(StoveGuardAppTest, Notify_WhenEventIsNotNone) {
    scene.nextDetection({true, false});
    const auto result = scene.interpret({});

    ASSERT_EQ(app.processFrame(result), Event::DangerousEntered);
    ASSERT_EQ(notifier.events().at(0), Event::DangerousEntered);

    clock.advance(alarmThreshold);
    ASSERT_EQ(app.processFrame(result), Event::AlarmStarted);
    EXPECT_EQ(notifier.events().at(1), Event::AlarmStarted);

    ASSERT_EQ(notifier.callCount(), 2);
    clock.advance(seconds{1});
    ASSERT_EQ(app.processFrame(result), Event::None);
    EXPECT_EQ(notifier.callCount(), 2);
}
