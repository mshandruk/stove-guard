#pragma once

#include "ObjectDetection.h"
#include "SceneState.h"

namespace SceneMapper {

struct OverlapThresholds {
    float potOnStove = 0.40F;  // 40% посуды покрывает плиту
    float fireOnStove = 0.15F; // 15% огня покрывает плиту
    float fireToPot = 0.30F;   // 30% огня греет посуду
};

inline constexpr OverlapThresholds DEFAULT_THRESHOLDS{};

SceneState map(const ObjectDetections& detections, OverlapThresholds thresholds = DEFAULT_THRESHOLDS);
} // namespace SceneMapper
