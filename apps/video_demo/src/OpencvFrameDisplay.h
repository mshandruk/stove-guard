#pragma once

#include "FrameDisplay.h"

class OpencvFrameDisplay final : public FrameDisplay {
  public:
    void render(const Frame& frame, const ObjectDetections& objectDetections) override;

  private:
    static cv::Scalar getLabelColor(LabelClassification label);
};