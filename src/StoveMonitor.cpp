#include "StoveMonitor.h"

StoveMonitor::StoveMonitor(Duration alarmThreshold)
        : alarmThreshold_{alarmThreshold} {
}

Event StoveMonitor::process(StoveState stoveState, PersonState personState, Duration delta) {

    const auto dangerous = isDangerous(stoveState, personState);

    if (systemState_ == SystemState::Safe && dangerous) {
        systemState_ = SystemState::Dangerous;
        dangerousDuration_ = Duration{0};

        return Event::DangerousEntered;
    }

    if (systemState_ == SystemState::Dangerous && !dangerous) {
        systemState_ = SystemState::Safe;
        return Event::DangerousCleared;
    }

    if (systemState_ == SystemState::Dangerous) {
        dangerousDuration_ += delta;

        if (isTimerExpired()) {
            systemState_ = SystemState::Alarmed;
            return Event::AlarmStarted;
        }
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

bool StoveMonitor::isTimerExpired() const noexcept {
    return dangerousDuration_ >= alarmThreshold_;
}