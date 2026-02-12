#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>

using namespace std::chrono;

namespace gas_mon {
enum class StoveState : uint8_t {
    Idle,
    Cooking

};
constexpr int TIMER_SEC = 15;

// TODO: encapsulate global state
steady_clock::time_point timerStart{};
auto stoveState = StoveState::Idle;

bool isTimerExpired() {
    const auto currentTime = steady_clock::now();
    const auto elapsed = duration_cast<seconds>(currentTime - timerStart).count();
    return elapsed >= TIMER_SEC;
}

void updateState(const bool stoveOn, const bool cookwarePresent, const bool personPresent) {
    switch (stoveState) {
    case StoveState::Idle: {
        if (stoveOn && cookwarePresent) {
            std::cout << "The gas stove is ON and there is cookware on it." << '\n';
            stoveState = StoveState::Cooking;
            timerStart = steady_clock::now();
            std::cout << "Timer is started." << '\n';
        } else if (stoveOn && !personPresent) {
            if (isTimerExpired()) {
                std::cout << "The gas stove has been ON for a long time and there is not cookware on it but the "
                             "person is not present."
                          << '\n';
            } else {
                std::cout << "The gas stove is ON but the person is not present" << '\n';
            }
        } else {

            /*TODO: This is a side effect because the timer is reset every time so that it does not expire. It would be
                better not to start. The timer should be remain inactive here.
            */

            std::cout << "The gas stove is OFF and in the idle state" << '\n';
            timerStart = steady_clock::now();
            std::cout << "Timer is reset." << '\n';
        }
        break;
    }
    case StoveState::Cooking: {
        if (!stoveOn || !cookwarePresent) {
            stoveState = StoveState::Idle;
            std::cout << "The gas stove is OFF or there is not cookware on it." << '\n';
            timerStart = steady_clock::now();
            std::cout << "Timer is reset." << '\n';
            break;
        }

        if (personPresent) {
            std::cout << "The gas stove is ON and the person is present." << '\n';
            timerStart = steady_clock::now();
            std::cout << "Timer is reset." << '\n';
            break;
        }

        if (isTimerExpired()) {
            std::cout << "The gas stove has been ON for a long time and there is cookware on it but the person is not "
                         "present."
                      << '\n';
        }
        break;
    }
    }
}
} // namespace gas_mon

namespace tests {
using namespace gas_mon;

void Given_Stove_Idle_When_Stove_On_And_Cookware_And_Person_Then_Stove_Cooking() {
    timerStart = steady_clock::now();
    stoveState = StoveState::Idle;

    updateState(true, true, true);

    assert(stoveState == StoveState::Cooking);
}

void Given_Stove_Idle_When_Stove_Off_And_No_Cookware_And_No_Person_Then_Stove_Idle() {
    timerStart = steady_clock::now();
    stoveState = StoveState::Idle;

    updateState(false, false, false);

    assert(stoveState == StoveState::Idle);
}

void Given_Stove_Idle_When_Stove_On_And_No_Cookware_And_No_Person_Then_Stove_Idle() {
    timerStart = steady_clock::now();
    stoveState = StoveState::Idle;

    updateState(true, false, false);

    assert(stoveState == StoveState::Idle);
}

void Given_Stove_Cooking_When_Stove_Off_Or_No_Cookware_Then_Stove_Idle() {
    // Scenario 1: the gas stove is off and there is cookware not present on it.
    timerStart = steady_clock::now();
    stoveState = StoveState::Cooking;

    updateState(false, false, false);

    assert(stoveState == StoveState::Idle);

    // Scenario 2: the gas stove is on and there is cookware not present on it.
    timerStart = steady_clock::now();
    stoveState = StoveState::Cooking;

    updateState(true, false, false);

    // Scenario 3: the gas stove is off and there is cookware present on it.
    timerStart = steady_clock::now();
    stoveState = StoveState::Cooking;

    updateState(false, true, false);

    assert(stoveState == StoveState::Idle);
}

void Given_Gas_Stove_Cooking_When_Stove_On_And_Cookware_Then_Stove_Cooking() {
    timerStart = steady_clock::now();
    stoveState = StoveState::Cooking;

    updateState(true, true, true);
    assert(stoveState == StoveState::Cooking);
}

} // namespace tests

int main() {
    tests::Given_Stove_Idle_When_Stove_On_And_Cookware_And_Person_Then_Stove_Cooking();
    tests::Given_Stove_Idle_When_Stove_On_And_No_Cookware_And_No_Person_Then_Stove_Idle();
    tests::Given_Stove_Idle_When_Stove_Off_And_No_Cookware_And_No_Person_Then_Stove_Idle();
    tests::Given_Stove_Cooking_When_Stove_Off_Or_No_Cookware_Then_Stove_Idle();
    tests::Given_Gas_Stove_Cooking_When_Stove_On_And_Cookware_Then_Stove_Cooking();

    /* The timer does not make sense as a standalone process. It should be started only when an event
     * occurs(event-based-design)
    Scenario/events:
     * 1. The stove is ON, no cookware is on burner, and no person is present -> start the timer
     * 2. The stove is ON, cookware is on the burner with flame, and no person is present -> start the timer
     * 3. The stove is ON, cookware is on the burner with flame, and a person appears -> reset the timer?
     * (Needs design consideration to avoid overheating food).
     * 4. The stove is off -> stop the timer.

     * The timer can have the following states:
     * Running - when a key event occurs
     * Reset - when a recovery event occurs
     * Stopped - no activity
     */

    return 0;
}
