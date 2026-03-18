#pragma once

#include "ObjectDetection.h"
#include "SceneState.h"

namespace SceneMapper {
SceneState map(const ObjectDetections& objectDetections);
}
