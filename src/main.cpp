#include <cassert>
#include <chrono>
#include <cstdint>

using namespace std::chrono;

namespace stove_monitor {
enum class StoveState : uint8_t { On, Off };
enum class PersonState : uint8_t { Present, Absent };

enum class Event : uint8_t {
    None,
    DangerousEntered,
    DangerousCleared,
    AlarmStarted,
    AlarmCleared,
};

constexpr steady_clock::duration ALARM_THRESHOLD = 15s;

class StoveMonitor {
  public:
    [[nodiscard]] Event
    process(const StoveState stoveState, const PersonState personState, const steady_clock::time_point currentTime) {

        const auto dangerous = isDangerous(stoveState, personState);

        if (systemState_ == SystemState::Safe && dangerous) {
            systemState_ = SystemState::Dangerous;
            dangerStartTime_ = currentTime;

            return Event::DangerousEntered;
        }

        if (systemState_ == SystemState::Dangerous && !dangerous) {
            systemState_ = SystemState::Safe;
            return Event::DangerousCleared;
        }

        if (systemState_ == SystemState::Dangerous && isTimerExpired(currentTime)) {
            systemState_ = SystemState::Alarmed;

            return Event::AlarmStarted;
        }

        if (systemState_ == SystemState::Alarmed && !dangerous) {
            systemState_ = SystemState::Safe;
            return Event::AlarmCleared;
        }

        return Event::None;
    }

  private:
    enum class SystemState : uint8_t {
        Safe,
        Dangerous,
        Alarmed,
    };

    SystemState systemState_ = SystemState::Safe;
    steady_clock::time_point dangerStartTime_;

    [[nodiscard]] static bool isDangerous(const StoveState stoveState, const PersonState personState) {
        return stoveState == StoveState::On && personState == PersonState::Absent;
    }

    [[nodiscard]] bool isTimerExpired(const steady_clock::time_point currentTime) const {
        return currentTime - dangerStartTime_ >= ALARM_THRESHOLD;
    }
};

} // namespace stove_monitor

namespace tests {
using namespace stove_monitor;

void Given_Stove_Off_When_Stove_On_And_Person_Absent_Then_Returns_Event_DangerousEntered() {
    StoveMonitor stoveMonitor;

    const auto event = stoveMonitor.process(StoveState::On, PersonState::Absent, steady_clock::now());

    assert(event == Event::DangerousEntered);
}

void Given_Stove_On_When_Stove_Off_And_Person_Absent_Then_Returns_Event_DangerousCleared() {
    StoveMonitor stoveMonitor;
    const auto t0 = steady_clock::now();

    // Given_Stove_On
    auto event = stoveMonitor.process(StoveState::On, PersonState::Absent, t0);

    assert(event == Event::DangerousEntered);

    // Stove off
    event = stoveMonitor.process(StoveState::Off, PersonState::Absent, t0);

    assert(event == Event::DangerousCleared);
}

void Given_Stove_On_When_Stove_On_And_Person_Present_Then_Returns_Event_DangerousCleared() {
    StoveMonitor stoveMonitor;
    const auto currentTime = steady_clock::now();

    // Given Stove_On
    auto event = stoveMonitor.process(StoveState::On, PersonState::Absent, currentTime);

    assert(event == Event::DangerousEntered);

    // Person present
    event = stoveMonitor.process(StoveState::On, PersonState::Present, currentTime);

    assert(event == Event::DangerousCleared);
}

void Given_StoveMonitor_EnteredSomeState_When_State_Not_Changed_Then_Returns_Event_None() {
    StoveMonitor stoveMonitor;
    const auto t0 = steady_clock::now();

    // Enter in state
    auto event = stoveMonitor.process(StoveState::On, PersonState::Absent, t0);
    assert(event == Event::DangerousEntered);

    event = stoveMonitor.process(StoveState::On, PersonState::Absent, t0);
    assert(event == Event::None);
}

void Given_DangerousState_When_TimerExpires_Then_Returns_Event_AlarmStarted() {
    StoveMonitor stoveMonitor;
    const auto t0 = steady_clock::now();

    // Enter in to dangerous
    auto event = stoveMonitor.process(StoveState::On, PersonState::Absent, t0);

    assert(event == Event::DangerousEntered);

    // Simulate AlarmedStarted
    event = stoveMonitor.process(StoveState::On, PersonState::Absent, t0 + ALARM_THRESHOLD);

    assert(event == Event::AlarmStarted);
}

void Given_AlarmedState_When_DangerCleared_Then_Returns_AlarmCleared() {
    StoveMonitor stoveMonitor;
    auto t0 = steady_clock::now();

    // DangerEntered
    auto event = stoveMonitor.process(StoveState::On, PersonState::Absent, t0);

    assert(event == Event::DangerousEntered);

    // Danger duration for reached the alarm threshold
    event = stoveMonitor.process(StoveState::On, PersonState::Absent, t0 + ALARM_THRESHOLD);

    assert(event == Event::AlarmStarted);

    // Person present
    event = stoveMonitor.process(StoveState::On, PersonState::Present, t0 + ALARM_THRESHOLD + 1s);

    assert(event == Event::AlarmCleared);
}

void Given_Danger_Interrupted_When_Danger_Reenters_Then_Alarm_Requires_Full_Timeout() {
    StoveMonitor stoveMonitor;
    const auto t0 = steady_clock::now();

    auto event = stoveMonitor.process(StoveState::On, PersonState::Absent, t0);
    assert(event == Event::DangerousEntered);

    event = stoveMonitor.process(StoveState::On, PersonState::Absent, t0 + ALARM_THRESHOLD - 1s);
    assert(event == Event::None);

    event = stoveMonitor.process(StoveState::On, PersonState::Present, t0 + ALARM_THRESHOLD - 1s);
    assert(event == Event::DangerousCleared);

    event = stoveMonitor.process(StoveState::On, PersonState::Absent, t0 + ALARM_THRESHOLD);
    assert(event == Event::DangerousEntered);

    event = stoveMonitor.process(StoveState::On, PersonState::Absent, t0 + 30s);
    assert(event == Event::AlarmStarted);
}

} // namespace tests

int main() {
    tests::Given_Stove_Off_When_Stove_On_And_Person_Absent_Then_Returns_Event_DangerousEntered();
    tests::Given_Stove_On_When_Stove_Off_And_Person_Absent_Then_Returns_Event_DangerousCleared();
    tests::Given_Stove_On_When_Stove_On_And_Person_Present_Then_Returns_Event_DangerousCleared();
    tests::Given_StoveMonitor_EnteredSomeState_When_State_Not_Changed_Then_Returns_Event_None();
    tests::Given_DangerousState_When_TimerExpires_Then_Returns_Event_AlarmStarted();
    tests::Given_AlarmedState_When_DangerCleared_Then_Returns_AlarmCleared();
    tests::Given_Danger_Interrupted_When_Danger_Reenters_Then_Alarm_Requires_Full_Timeout();
    return 0;
}
