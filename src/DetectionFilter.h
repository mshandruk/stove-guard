#pragma once

#include <unordered_map>

#include "ObjectDetection.h"

using ConfidenceThresholds = std::unordered_map<LabelClassification, Confidence>;

class DetectionFilter {
  public:
    explicit DetectionFilter(ConfidenceThresholds thresholds);
    [[nodiscard]]
    ObjectDetections filter(const ObjectDetections& objectDetections) const;

  private:
    ConfidenceThresholds thresholds_;
};