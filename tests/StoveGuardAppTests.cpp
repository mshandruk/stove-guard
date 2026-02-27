#include <chrono>
#include <cstddef>
#include <vector>

#include <gtest/gtest.h>

#include "Detection.h"
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

class StoveGuardAppTest : public testing::Test {
  protected:
    FakeFrameAnalyzer analyzer;
    FakeNotifier notifier;
    StoveGuardApp app{analyzer, notifier};
    static constexpr Frame frame{};
};

TEST_F(StoveGuardAppTest, DangerousEntered_WhenStoveOnAndNoPerson) {
    analyzer.nextDetection({true, false});
    const auto start = steady_clock::now();

    EXPECT_EQ(app.processFrame(frame, start), Event::DangerousEntered);
}

TEST_F(StoveGuardAppTest, ReturnsNone_WhenStoveOnAndPersonPresent) {
    analyzer.nextDetection({true, true});
    const auto start = steady_clock::now();

    EXPECT_EQ(app.processFrame(frame, start), Event::None);
}

TEST_F(StoveGuardAppTest, ReturnsNone_WhenStoveOffAndNoPerson) {
    analyzer.nextDetection({false, false});
    const auto start = steady_clock::now();

    EXPECT_EQ(app.processFrame(frame, start), Event::None);
}

TEST_F(StoveGuardAppTest, AlarmStarted_WhenNoPersonForAlarmThreshold) {
    const auto start = steady_clock::now();

    analyzer.nextDetection({true, false});
    ASSERT_EQ(app.processFrame(frame, start), Event::DangerousEntered);

    const auto alarmAt = start + ALARM_THRESHOLD;
    EXPECT_EQ(app.processFrame(frame, alarmAt), Event::AlarmStarted);
}

TEST_F(StoveGuardAppTest, AlarmCanceled_WhenPersonAppearsAfterAlarm) {
    const auto start = steady_clock::now();

    analyzer.nextDetection({true, false});
    ASSERT_EQ(app.processFrame(frame, start), Event::DangerousEntered);

    const auto alarmAt = start + ALARM_THRESHOLD;
    ASSERT_EQ(app.processFrame(frame, alarmAt), Event::AlarmStarted);

    const auto personAppearedAt = start + ALARM_THRESHOLD + seconds{1};
    analyzer.nextDetection({true, true});
    EXPECT_EQ(app.processFrame(frame, personAppearedAt), Event::AlarmCleared);
}

TEST_F(StoveGuardAppTest, AlarmNotStarted_WhenPersonAppearsBeforeAlarm) {
    const auto start = steady_clock::now();

    analyzer.nextDetection({true, false});
    ASSERT_EQ(app.processFrame(frame, start), Event::DangerousEntered);

    const auto beforeAlarmAt = start + ALARM_THRESHOLD - seconds{1};
    ASSERT_EQ(app.processFrame(frame, beforeAlarmAt), Event::None);

    const auto alarmAt = start + ALARM_THRESHOLD;
    analyzer.nextDetection({true, true});
    EXPECT_EQ(app.processFrame(frame, alarmAt), Event::DangerousCleared);
}

TEST_F(StoveGuardAppTest, AlarmRestarted_WhenPersonLeavesAfterClearingAlarm) {
    const auto start = steady_clock::now();

    analyzer.nextDetection({true, false});
    ASSERT_EQ(app.processFrame(frame, start), Event::DangerousEntered);

    const auto alarmAt = start + ALARM_THRESHOLD;
    ASSERT_EQ(app.processFrame(frame, alarmAt), Event::AlarmStarted);

    const auto personAppearedAt = alarmAt + seconds{1};
    analyzer.nextDetection({true, true});
    ASSERT_EQ(app.processFrame(frame, personAppearedAt), Event::AlarmCleared);

    const auto personLeftAt = personAppearedAt + seconds{2};
    analyzer.nextDetection({true, false});
    ASSERT_EQ(app.processFrame(frame, personLeftAt), Event::DangerousEntered);

    const auto alarmRestartAt = personLeftAt + ALARM_THRESHOLD;
    EXPECT_EQ(app.processFrame(frame, alarmRestartAt), Event::AlarmStarted);
}

TEST_F(StoveGuardAppTest, Notify_WhenEventIsNotNone) {
    const auto startAt = steady_clock::now();
    analyzer.nextDetection({true, false});

    ASSERT_EQ(app.processFrame(frame, startAt), Event::DangerousEntered);
    ASSERT_EQ(notifier.events().at(0), Event::DangerousEntered);

    const auto alarmAt = startAt + ALARM_THRESHOLD;
    ASSERT_EQ(app.processFrame(frame, alarmAt), Event::AlarmStarted);
    EXPECT_EQ(notifier.events().at(1), Event::AlarmStarted);

    ASSERT_EQ(notifier.callCount(), 2);
    ASSERT_EQ(app.processFrame(frame, alarmAt + seconds{1}), Event::None);
    EXPECT_EQ(notifier.callCount(), 2);
}
