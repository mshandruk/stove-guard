#include <cassert>
#include <chrono>
#include <thread>

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

constexpr int ALARM_THRESHOLD_SEC = 15;

class StoveMonitor {
  public:
    [[nodiscard]] Event updateState(const StoveState stoveState, const PersonState personState) {
        const auto dangerous = isDangerous(stoveState, personState);

        if (systemState_ == SystemState::Safe && dangerous) {
            systemState_ = SystemState::Dangerous;
            startTimer_ = steady_clock::now();

            return Event::DangerousEntered;
        }

        if (systemState_ == SystemState::Dangerous && !dangerous) {
            systemState_ = SystemState::Safe;
            return Event::DangerousCleared;
        }

        if (systemState_ == SystemState::Dangerous && isTimerExpired()) {
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
    steady_clock::time_point startTimer_ = steady_clock::now();

    [[nodiscard]] static bool isDangerous(const StoveState stoveState, const PersonState personState) {
        return stoveState == StoveState::On && personState == PersonState::Absent;
    }

    [[nodiscard]] bool isTimerExpired() const {
        const auto current = steady_clock::now();
        const auto elapsed = duration_cast<seconds>(current - startTimer_).count();
        return elapsed >= ALARM_THRESHOLD_SEC;
    }
};

} // namespace stove_monitor

namespace tests {
using namespace stove_monitor;

void Given_Stove_Off_When_Stove_On_And_Person_Absent_Then_Returns_Event_DangerousEntered() {
    StoveMonitor stoveMonitor;

    const auto event = stoveMonitor.updateState(StoveState::On, PersonState::Absent);

    assert(event == Event::DangerousEntered);
}

void Given_Stove_On_When_Stove_Off_And_Person_Absent_Then_Returns_Event_DangerousCanceled() {
    StoveMonitor stoveMonitor;

    const auto event1 = stoveMonitor.updateState(StoveState::On, PersonState::Absent);
    assert(event1 == Event::DangerousEntered);

    const auto event2 = stoveMonitor.updateState(StoveState::Off, PersonState::Absent);
    assert(event2 == Event::DangerousCleared);
}

void Given_Stove_On_When_Stove_On_And_Person_Present_Then_Returns_Event_DangerousCanceled() {
    StoveMonitor stoveMonitor;

    const auto event1 = stoveMonitor.updateState(StoveState::On, PersonState::Absent);
    assert(event1 == Event::DangerousEntered);

    const auto event2 = stoveMonitor.updateState(StoveState::On, PersonState::Present);
    assert(event2 == Event::DangerousCleared);
}

void Given_StoveMonitor_EnteredSomeState_When_State_Not_Changed_Then_Returns_Event_None() {
    StoveMonitor stoveMonitor;

    const auto event1 = stoveMonitor.updateState(StoveState::On, PersonState::Absent);
    assert(event1 == Event::DangerousEntered);

    const auto event2 = stoveMonitor.updateState(StoveState::On, PersonState::Absent);
    assert(event2 == Event::None);
}

} // namespace tests

int main() {
    tests::Given_Stove_Off_When_Stove_On_And_Person_Absent_Then_Returns_Event_DangerousEntered();
    tests::Given_Stove_On_When_Stove_Off_And_Person_Absent_Then_Returns_Event_DangerousCanceled();
    tests::Given_Stove_On_When_Stove_On_And_Person_Present_Then_Returns_Event_DangerousCanceled();
    tests::Given_StoveMonitor_EnteredSomeState_When_State_Not_Changed_Then_Returns_Event_None();

    return 0;
}
