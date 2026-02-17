#include "StoveMonitor.h"

#include <chrono>

using namespace std::chrono;
using namespace std::chrono_literals;

Event StoveMonitor::process(
    const StoveState stoveState,
    const PersonState personState,
    const steady_clock::time_point currentTime) {

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

bool StoveMonitor::isDangerous(const StoveState stoveState, const PersonState personState) {
    return stoveState == StoveState::On && personState == PersonState::Absent;
}

bool StoveMonitor::isTimerExpired(const steady_clock::time_point currentTime) const {
    return currentTime - dangerStartTime_ >= ALARM_THRESHOLD;
}