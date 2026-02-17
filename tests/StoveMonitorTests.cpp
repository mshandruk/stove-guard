#include <chrono>

#include <gtest/gtest.h>

#include "StoveMonitor.h"

using namespace std::chrono;
using namespace std::chrono_literals;

TEST(StoveMonitorTest, Given_Stove_Off_When_Stove_On_And_Person_Absent_Then_Returns_Event_DangerousEntered) {
    StoveMonitor stoveMonitor;

    const auto event = stoveMonitor.process(StoveState::On, PersonState::Absent, steady_clock::now());

    EXPECT_EQ(event, Event::DangerousEntered);
}

TEST(StoveMonitorTest, Given_Stove_On_When_Stove_Off_And_Person_Absent_Then_Returns_Event_DangerousCleared) {
    StoveMonitor stoveMonitor;
    const auto t0 = steady_clock::now();

    // Given_Stove_On
    auto event = stoveMonitor.process(StoveState::On, PersonState::Absent, t0);

    ASSERT_EQ(event, Event::DangerousEntered);

    // Stove off
    event = stoveMonitor.process(StoveState::Off, PersonState::Absent, t0);

    EXPECT_EQ(event, Event::DangerousCleared);
}

TEST(StoveMonitorTest, Given_Stove_On_When_Stove_On_And_Person_Present_Then_Returns_Event_DangerousCleared) {
    StoveMonitor stoveMonitor;
    const auto currentTime = steady_clock::now();

    // Given Stove_On
    auto event = stoveMonitor.process(StoveState::On, PersonState::Absent, currentTime);

    ASSERT_EQ(event, Event::DangerousEntered);

    // Person present
    event = stoveMonitor.process(StoveState::On, PersonState::Present, currentTime);

    EXPECT_EQ(event, Event::DangerousCleared);
}

TEST(StoveMonitorTest, Given_StoveMonitor_EnteredSomeState_When_State_Not_Changed_Then_Returns_Event_None) {
    StoveMonitor stoveMonitor;
    const auto t0 = steady_clock::now();

    // Enter in state
    auto event = stoveMonitor.process(StoveState::On, PersonState::Absent, t0);
    ASSERT_EQ(event, Event::DangerousEntered);

    event = stoveMonitor.process(StoveState::On, PersonState::Absent, t0);
    EXPECT_EQ(event, Event::None);
}

TEST(StoveMonitorTest, Given_DangerousState_When_TimerExpires_Then_Returns_Event_AlarmStarted) {
    StoveMonitor stoveMonitor;
    const auto t0 = steady_clock::now();

    // Enter in to dangerous
    auto event = stoveMonitor.process(StoveState::On, PersonState::Absent, t0);

    ASSERT_EQ(event, Event::DangerousEntered);

    // Simulate AlarmedStarted
    event = stoveMonitor.process(StoveState::On, PersonState::Absent, t0 + ALARM_THRESHOLD);

    EXPECT_EQ(event, Event::AlarmStarted);
}

TEST(StoveMonitorTest, Given_AlarmedState_When_DangerCleared_Then_Returns_AlarmCleared) {
    StoveMonitor stoveMonitor;
    const auto t0 = steady_clock::now();

    // DangerEntered
    auto event = stoveMonitor.process(StoveState::On, PersonState::Absent, t0);

    ASSERT_EQ(event, Event::DangerousEntered);

    // Danger duration for reached the alarm threshold
    event = stoveMonitor.process(StoveState::On, PersonState::Absent, t0 + ALARM_THRESHOLD);

    ASSERT_EQ(event, Event::AlarmStarted);

    // Person present
    event = stoveMonitor.process(StoveState::On, PersonState::Present, t0 + ALARM_THRESHOLD + 1s);

    EXPECT_EQ(event, Event::AlarmCleared);
}

TEST(StoveMonitorTest, Given_Danger_Interrupted_When_Danger_Reenters_Then_Alarm_Requires_Full_Timeout) {
    StoveMonitor stoveMonitor;
    const auto t0 = steady_clock::now();

    auto event = stoveMonitor.process(StoveState::On, PersonState::Absent, t0);
    ASSERT_EQ(event, Event::DangerousEntered);

    event = stoveMonitor.process(StoveState::On, PersonState::Absent, t0 + ALARM_THRESHOLD - 1s);
    ASSERT_EQ(event, Event::None);

    event = stoveMonitor.process(StoveState::On, PersonState::Present, t0 + ALARM_THRESHOLD - 1s);
    ASSERT_EQ(event, Event::DangerousCleared);

    event = stoveMonitor.process(StoveState::On, PersonState::Absent, t0 + ALARM_THRESHOLD);
    ASSERT_EQ(event, Event::DangerousEntered);

    event = stoveMonitor.process(StoveState::On, PersonState::Absent, t0 + 30s);
    EXPECT_EQ(event, Event::AlarmStarted);
}