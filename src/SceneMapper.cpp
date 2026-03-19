#include "SceneMapper.h"
#include "ObjectDetection.h"
#include "SceneState.h"
#include "StoveMonitor.h"

namespace SceneMapper {
SceneState map(const ObjectDetections& objectDetections) {
    SceneState scene{StoveState::Off, PersonState::Absent};

    for (const auto& objectDetection : objectDetections) {
        switch (objectDetection.classification) {
        case LabelClassification::Stove: {
            scene.stoveState = StoveState::On;
            break;
        }
        case LabelClassification::Person: {
            scene.personState = PersonState::Present;
            break;
        }
        case LabelClassification::Unknown: {
            break;
        }
        }
    }

    return scene;
}

} // namespace SceneMapper
