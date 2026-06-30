#pragma once

#include <cstddef>
#include <string_view>
#include <utility>
#include <vector>

#include "Frame.h"
#include "FrameAnalyzer.h"
#include "ObjectDetection.h"

using FakeScenario = std::vector<std::pair<ObjectDetections, std::string_view>>;

class FakeFrameAnalyzer final : public FrameAnalyzer {
  public:
    explicit FakeFrameAnalyzer(FakeScenario scenario);
    ObjectDetections analyze(const Frame& frame) override;

  private:
    FakeScenario scenario_;
    std::size_t step_ = 0;
};
