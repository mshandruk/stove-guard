#include "YoloFrameAnalyzer.h"

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <utility>

#include <onnxruntime_c_api.h>
#include <onnxruntime_cxx_api.h>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>

#include "Frame.h"
#include "ObjectDetection.h"

YoloFrameAnalyzer::YoloFrameAnalyzer(std::filesystem::path modelPath)
        : modelPath_{std::move(modelPath)},
          env_{ORT_LOGGING_LEVEL_WARNING, "stove_guard_video"} {
    if (!isValidModelPath(modelPath_)) {
        throw std::runtime_error("Invalid or missing model file at: " + std::filesystem::absolute(modelPath_).string());
    }
    const Ort::SessionOptions options{};
    session_ = Ort::Session{env_, modelPath_.c_str(), options};
}

bool YoloFrameAnalyzer::isValidModelPath(const std::filesystem::path& path) {
    return std::filesystem::is_regular_file(path) && path.extension() == ".onnx";
}

ObjectDetections YoloFrameAnalyzer::analyze(const Frame& frame) {
    cv::Mat resizedFrame;
    cv::resize(frame.data, resizedFrame, cv::Size{640, 640});

    cv::Mat floatFrame;
    resizedFrame.convertTo(floatFrame, CV_32F, 1.0 / 255.0);
    std::cout << "Inference placeholder\n";
    return {};
}
