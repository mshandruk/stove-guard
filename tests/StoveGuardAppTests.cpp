#include <chrono>
#include <cstddef>
#include <vector>

#include <gtest/gtest.h>

#include "Detection.h"
#include "Frame.h"
#include "FrameAnalyzer.h"
#include "Notifier.h"
#include "StoveGuardApp.h"
#include "StoveMonitor.h"

using namespace std::chrono;

class FakeFrameAnalyzer : public FrameAnalyzer {
  public:
    void nextDetection(const Detection& detection) noexcept {
        nextDetection_ = detection;
    }

    Detection analyze([[maybe_unused]] const Frame& frame) override {
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
    FakeFrameAnalyzer analyzer;
    FakeNotifier notifier;
    FakeClock clock;
    StoveGuardApp app{analyzer, notifier, clock};
    static constexpr Frame frame{};
};

TEST_F(StoveGuardAppTest, DangerousEntered_WhenStoveOnAndNoPerson) {
    analyzer.nextDetection({true, false});
    EXPECT_EQ(app.processFrame(frame), Event::DangerousEntered);
}

TEST_F(StoveGuardAppTest, ReturnsNone_WhenStoveOnAndPersonPresent) {
    analyzer.nextDetection({true, true});
    EXPECT_EQ(app.processFrame(frame), Event::None);
}

TEST_F(StoveGuardAppTest, ReturnsNone_WhenStoveOffAndNoPerson) {
    analyzer.nextDetection({false, false});
    EXPECT_EQ(app.processFrame(frame), Event::None);
}

TEST_F(StoveGuardAppTest, AlarmStarted_WhenNoPersonForAlarmThreshold) {
    analyzer.nextDetection({true, false});
    ASSERT_EQ(app.processFrame(frame), Event::DangerousEntered);

    clock.advance(ALARM_THRESHOLD);
    EXPECT_EQ(app.processFrame(frame), Event::AlarmStarted);
}

TEST_F(StoveGuardAppTest, AlarmCanceled_WhenPersonAppearsAfterAlarm) {
    analyzer.nextDetection({true, false});
    ASSERT_EQ(app.processFrame(frame), Event::DangerousEntered);

    clock.advance(ALARM_THRESHOLD);
    ASSERT_EQ(app.processFrame(frame), Event::AlarmStarted);

    clock.advance(ALARM_THRESHOLD + seconds{1});
    analyzer.nextDetection({true, true});
    EXPECT_EQ(app.processFrame(frame), Event::AlarmCleared);
}

TEST_F(StoveGuardAppTest, AlarmNotStarted_WhenPersonAppearsBeforeAlarm) {

    analyzer.nextDetection({true, false});
    ASSERT_EQ(app.processFrame(frame), Event::DangerousEntered);

    clock.advance(ALARM_THRESHOLD - seconds{1});
    ASSERT_EQ(app.processFrame(frame), Event::None);

    clock.advance(ALARM_THRESHOLD);
    analyzer.nextDetection({true, true});
    EXPECT_EQ(app.processFrame(frame), Event::DangerousCleared);
}

TEST_F(StoveGuardAppTest, AlarmRestarted_WhenPersonLeavesAfterClearingAlarm) {
    analyzer.nextDetection({true, false});
    ASSERT_EQ(app.processFrame(frame), Event::DangerousEntered);

    clock.advance(ALARM_THRESHOLD);
    ASSERT_EQ(app.processFrame(frame), Event::AlarmStarted);

    analyzer.nextDetection({true, true});
    ASSERT_EQ(app.processFrame(frame), Event::AlarmCleared);

    clock.advance(seconds{2});
    analyzer.nextDetection({true, false});
    ASSERT_EQ(app.processFrame(frame), Event::DangerousEntered);

    clock.advance(ALARM_THRESHOLD);
    EXPECT_EQ(app.processFrame(frame), Event::AlarmStarted);
}

TEST_F(StoveGuardAppTest, Notify_WhenEventIsNotNone) {
    analyzer.nextDetection({true, false});

    ASSERT_EQ(app.processFrame(frame), Event::DangerousEntered);
    ASSERT_EQ(notifier.events().at(0), Event::DangerousEntered);

    clock.advance(ALARM_THRESHOLD);
    ASSERT_EQ(app.processFrame(frame), Event::AlarmStarted);
    EXPECT_EQ(notifier.events().at(1), Event::AlarmStarted);

    ASSERT_EQ(notifier.callCount(), 2);
    clock.advance(seconds{1});
    ASSERT_EQ(app.processFrame(frame), Event::None);
    EXPECT_EQ(notifier.callCount(), 2);
}
