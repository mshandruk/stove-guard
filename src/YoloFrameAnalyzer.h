#pragma once

#include <filesystem>

#include <onnxruntime_cxx_api.h>

#include "FrameAnalyzer.h"
#include "ObjectDetection.h"

class YoloFrameAnalyzer final : public FrameAnalyzer {
  public:
    explicit YoloFrameAnalyzer(std::filesystem::path modelPath);
    [[nodiscard]] ObjectDetections analyze(const Frame& frame) override;

    [[nodiscard]] static bool isValidModelPath(const std::filesystem::path& path);

  private:
    std::filesystem::path modelPath_;
    Ort::Env env_;
    Ort::Session session_{nullptr};
};