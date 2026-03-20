#pragma once

#include "ObjectDetection.h"
#include "SceneState.h"

namespace SceneMapper {
// TODO: Move to DI Configuration
extern const float PERSON_THRESHOLD;
extern const float STOVE_THRESHOLD;
SceneState map(const ObjectDetections& objectDetections);
} // namespace SceneMapper
