#include "SceneMapper.h"
#include "ObjectDetection.h"
#include "SceneState.h"
#include "StoveMonitor.h"

namespace SceneMapper {
SceneState map(const ObjectDetections& objectDetections) {
    bool hasFlame = false;
    bool hasPerson = false;

    for (const auto& objectDetection : objectDetections) {
        switch (objectDetection.label) {
        case LabelClassification::Stove: {
            hasFlame = true;
            break;
        }
        case LabelClassification::Person: {
            hasPerson = true;
            break;
        }
        }
    }
    SceneState scene{};
    scene.stoveState = hasFlame ? StoveState::On : StoveState::Off;
    scene.personState = hasPerson ? PersonState::Present : PersonState::Absent;
    return scene;
}

} // namespace SceneMapper
