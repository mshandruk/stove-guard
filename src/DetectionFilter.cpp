#include "DetectionFilter.h"

#include <utility>

#include "ObjectDetection.h"

DetectionFilter::DetectionFilter(ConfidenceThresholds thresholds)
        : thresholds_{std::move(thresholds)} {
}

ObjectDetections DetectionFilter::filter(const ObjectDetections& objectDetections) const {
    ObjectDetections filtered;

    for (const auto& objectDetection : objectDetections) {
        const auto threshold = thresholds_.find(objectDetection.classification);
        if (threshold == thresholds_.end()) {
            continue;
        }
        if (objectDetection.confidence.value() < threshold->second.value()) {
            continue;
        }
        filtered.push_back(objectDetection);
    }

    return filtered;
}
