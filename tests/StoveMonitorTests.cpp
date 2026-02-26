#include <chrono>

#include <gtest/gtest.h>

#include "StoveMonitor.h"

using namespace std::chrono;

TEST(StoveMonitorTest, DangerousEntered_WhenStoveOnAndPersonAbsent) {
    StoveMonitor stoveMonitor;

    const auto event = stoveMonitor.process(StoveState::On, PersonState::Absent, seconds{0});
    EXPECT_EQ(event, Event::DangerousEntered);
}

TEST(StoveMonitorTest, DangerousCleared_WhenStoveTurnsOffWhilePersonAbsent) {
    StoveMonitor stoveMonitor;
    auto event = stoveMonitor.process(StoveState::On, PersonState::Absent, seconds{0});
    ASSERT_EQ(event, Event::DangerousEntered);

    event = stoveMonitor.process(StoveState::Off, PersonState::Absent, seconds{1});
    EXPECT_EQ(event, Event::DangerousCleared);
}

TEST(StoveMonitorTest, DangerousCleared_WhenPersonAppearsAfterDangerousEntered) {
    StoveMonitor stoveMonitor;

    auto event = stoveMonitor.process(StoveState::On, PersonState::Absent, seconds{0});
    ASSERT_EQ(event, Event::DangerousEntered);

    event = stoveMonitor.process(StoveState::On, PersonState::Present, seconds{1});
    EXPECT_EQ(event, Event::DangerousCleared);
}

TEST(StoveMonitorTest, None_WhenStateUnchanged) {
    StoveMonitor stoveMonitor;

    auto event = stoveMonitor.process(StoveState::On, PersonState::Absent, seconds{0});
    ASSERT_EQ(event, Event::DangerousEntered);

    event = stoveMonitor.process(StoveState::On, PersonState::Absent, seconds{1});
    EXPECT_EQ(event, Event::None);
}

TEST(StoveMonitorTest, AlarmStarted_WhenDangerDurationExceeded) {
    StoveMonitor stoveMonitor;

    // Enter in to dangerous
    auto event = stoveMonitor.process(StoveState::On, PersonState::Absent, seconds{0});
    ASSERT_EQ(event, Event::DangerousEntered);

    // Simulate AlarmedStarted
    event = stoveMonitor.process(StoveState::On, PersonState::Absent, ALARM_THRESHOLD);
    EXPECT_EQ(event, Event::AlarmStarted);
}

TEST(StoveMonitorTest, AlarmCleared_WhenPersonAppearsAfterAlarmStarted) {
    StoveMonitor stoveMonitor;

    // Enter to dangerous state
    auto event = stoveMonitor.process(StoveState::On, PersonState::Absent, seconds{0});
    ASSERT_EQ(event, Event::DangerousEntered);

    // Danger duration for reached the alarm threshold
    event = stoveMonitor.process(StoveState::On, PersonState::Absent, ALARM_THRESHOLD);
    ASSERT_EQ(event, Event::AlarmStarted);

    // Person presents
    event = stoveMonitor.process(StoveState::On, PersonState::Present, ALARM_THRESHOLD + seconds{1});
    EXPECT_EQ(event, Event::AlarmCleared);
}

TEST(StoveMonitorTest, AlarmTimeoutRestart_WhenDangerReenters) {
    StoveMonitor stoveMonitor;

    auto event = stoveMonitor.process(StoveState::On, PersonState::Absent, seconds{0});
    ASSERT_EQ(event, Event::DangerousEntered);

    event = stoveMonitor.process(StoveState::On, PersonState::Present, ALARM_THRESHOLD - seconds{1});
    ASSERT_EQ(event, Event::DangerousCleared);

    event = stoveMonitor.process(StoveState::On, PersonState::Absent, ALARM_THRESHOLD);
    ASSERT_EQ(event, Event::DangerousEntered);

    event = stoveMonitor.process(StoveState::On, PersonState::Absent, ALARM_THRESHOLD);
    EXPECT_EQ(event, Event::AlarmStarted);
}

TEST(StoveMonitorTest, NoEffect_WhenSafeStateAndThresholdDelta) {
    StoveMonitor stoveMonitor;

    auto event = stoveMonitor.process(StoveState::Off, PersonState::Present, seconds{0});
    ASSERT_EQ(event, Event::None);

    event = stoveMonitor.process(StoveState::Off, PersonState::Present, ALARM_THRESHOLD);
    EXPECT_EQ(event, Event::None);
}