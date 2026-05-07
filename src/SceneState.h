#pragma once

#include "StoveMonitor.h"

struct SceneState {
    StoveState stoveState{StoveState::Off};
    PersonState personState{PersonState::Absent};
    bool potOnStove{false};
    bool potOnFire{false};
};

inline bool operator==(const SceneState& lhs, const SceneState& rhs) {
    return lhs.stoveState == rhs.stoveState && lhs.personState == rhs.personState && lhs.potOnStove == rhs.potOnStove &&
           lhs.potOnFire == rhs.potOnFire;
}
