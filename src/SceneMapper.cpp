#include "SceneMapper.h"
#include "ObjectDetection.h"
#include "SceneState.h"
#include "StoveMonitor.h"

namespace SceneMapper {
const float PERSON_THRESHOLD = 0.7F;
const float STOVE_THRESHOLD = 0.7F;

SceneState map(const ObjectDetections& objectDetections) {
    SceneState scene{StoveState::Off, PersonState::Absent};

    for (const auto& objectDetection : objectDetections) {

        switch (objectDetection.classification) {
        case LabelClassification::Stove: {
            if (objectDetection.confidence.value() >= STOVE_THRESHOLD) {
                scene.stoveState = StoveState::On;
            }

            break;
        }
        case LabelClassification::Person: {
            if (objectDetection.confidence.value() >= PERSON_THRESHOLD) {
                scene.personState = PersonState::Present;
            }
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
