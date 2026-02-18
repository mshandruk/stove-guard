#include <utility>

#include "DetectionMapper.h"
#include "StoveMonitor.h"

std::pair<StoveState, PersonState> toDomain(const Detection& detection) {
    const StoveState stoveState = detection.flameDetected ? StoveState::On : StoveState::Off;
    const PersonState personState = detection.personDetected ? PersonState::Present : PersonState::Absent;
    return {stoveState, personState};
}
