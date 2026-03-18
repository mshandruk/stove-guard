#pragma once

#include "StoveMonitor.h"

struct SceneState {
    StoveState stoveState;
    PersonState personState;
};

inline bool operator==(const SceneState& lhs, const SceneState& rhs) {
    return lhs.stoveState == rhs.stoveState && lhs.personState == rhs.personState;
}
