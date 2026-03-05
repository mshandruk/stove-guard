#include <chrono>

#include <gtest/gtest.h>

#include "StoveMonitor.h"

using namespace std::chrono;

class StoveMonitorTest : public testing::Test {
  protected:
    Duration alarmThreshold = seconds{15};
    StoveMonitor stoveMonitor{alarmThreshold};
};

TEST_F(StoveMonitorTest, DangerousEntered_WhenStoveOnAndPersonAbsent) {
    const auto event = stoveMonitor.process(StoveState::On, PersonState::Absent, seconds{0});
    EXPECT_EQ(event, Event::DangerousEntered);
}

TEST_F(StoveMonitorTest, DangerousCleared_WhenStoveTurnsOffWhilePersonAbsent) {
    auto event = stoveMonitor.process(StoveState::On, PersonState::Absent, seconds{0});
    ASSERT_EQ(event, Event::DangerousEntered);

    event = stoveMonitor.process(StoveState::Off, PersonState::Absent, seconds{1});
    EXPECT_EQ(event, Event::DangerousCleared);
}

TEST_F(StoveMonitorTest, DangerousCleared_WhenPersonAppearsAfterDangerousEntered) {

    auto event = stoveMonitor.process(StoveState::On, PersonState::Absent, seconds{0});
    ASSERT_EQ(event, Event::DangerousEntered);

    event = stoveMonitor.process(StoveState::On, PersonState::Present, seconds{1});
    EXPECT_EQ(event, Event::DangerousCleared);
}

TEST_F(StoveMonitorTest, None_WhenStateUnchanged) {
    auto event = stoveMonitor.process(StoveState::On, PersonState::Absent, seconds{0});
    ASSERT_EQ(event, Event::DangerousEntered);

    event = stoveMonitor.process(StoveState::On, PersonState::Absent, seconds{1});
    EXPECT_EQ(event, Event::None);
}

TEST_F(StoveMonitorTest, AlarmStarted_WhenDangerDurationExceeded) {
    // Enter in to dangerous
    auto event = stoveMonitor.process(StoveState::On, PersonState::Absent, seconds{0});
    ASSERT_EQ(event, Event::DangerousEntered);

    // Simulate AlarmedStarted
    event = stoveMonitor.process(StoveState::On, PersonState::Absent, alarmThreshold);
    EXPECT_EQ(event, Event::AlarmStarted);
}

TEST_F(StoveMonitorTest, AlarmCleared_WhenPersonAppearsAfterAlarmStarted) {
    // Enter to dangerous state
    auto event = stoveMonitor.process(StoveState::On, PersonState::Absent, seconds{0});
    ASSERT_EQ(event, Event::DangerousEntered);

    // Danger duration for reached the alarm threshold
    event = stoveMonitor.process(StoveState::On, PersonState::Absent, alarmThreshold);
    ASSERT_EQ(event, Event::AlarmStarted);

    // Person presents
    event = stoveMonitor.process(StoveState::On, PersonState::Present, alarmThreshold + seconds{1});
    EXPECT_EQ(event, Event::AlarmCleared);
}

TEST_F(StoveMonitorTest, AlarmTimeoutRestart_WhenDangerReenters) {
    auto event = stoveMonitor.process(StoveState::On, PersonState::Absent, seconds{0});
    ASSERT_EQ(event, Event::DangerousEntered);

    event = stoveMonitor.process(StoveState::On, PersonState::Present, alarmThreshold - seconds{1});
    ASSERT_EQ(event, Event::DangerousCleared);

    event = stoveMonitor.process(StoveState::On, PersonState::Absent, alarmThreshold);
    ASSERT_EQ(event, Event::DangerousEntered);

    event = stoveMonitor.process(StoveState::On, PersonState::Absent, alarmThreshold);
    EXPECT_EQ(event, Event::AlarmStarted);
}

TEST_F(StoveMonitorTest, NoEffect_WhenSafeStateAndThresholdDelta) {

    auto event = stoveMonitor.process(StoveState::Off, PersonState::Present, seconds{0});
    ASSERT_EQ(event, Event::None);

    event = stoveMonitor.process(StoveState::Off, PersonState::Present, alarmThreshold);
    EXPECT_EQ(event, Event::None);
}