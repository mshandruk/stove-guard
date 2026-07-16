#include "YoloFrameAnalyzer.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <ostream>
#include <stdexcept>
#include <utility>
#include <vector>

#include <onnxruntime_c_api.h>
#include <onnxruntime_cxx_api.h>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/imgproc.hpp>

#include "Frame.h"
#include "ObjectDetection.h"

struct ImageShape {
    int width;
    int height;
};

constexpr ImageShape IMAGE_SHAPE{640, 640};

constexpr int CHANNELS = 3;

struct Candidate {
    float x{};
    float y{};
    float w{};
    float h{};

    std::size_t classId{};
    float score{};
};

namespace {

LabelClassification mapToLabelClassification(std::size_t classId) {
    switch (classId) {
    case 0: {
        return LabelClassification::Pot;
    }
    case 1: {
        return LabelClassification::Fire;
    }
    case 2: {
        return LabelClassification::Person;
    }
    case 3: {
        return LabelClassification::Stove;
    }
    default: {
        return LabelClassification::Unknown;
    }
    }
}

[[maybe_unused]]
std::ostream& operator<<(std::ostream& oss, const Candidate& candidate) {

    return oss << "Candidate(" << "x=" << candidate.x << " y=" << candidate.y << " w=" << candidate.w
               << " h=" << candidate.h << " classId=" << candidate.classId << " score=" << candidate.score;
}

float calculateArea(const float width, const float height) {
    return width * height;
}

float calculateIntersectionArea(const BoundingBox& a, const BoundingBox& b) {
    const float interLeft = std::max(a.left(), b.left());
    const float interRight = std::min(a.right(), b.right());
    const float interTop = std::max(a.top(), b.top());
    const float interBottom = std::min(a.bottom(), b.bottom());

    const float width = std::max(0.0F, interRight - interLeft);
    const float height = std::max(0.0F, interBottom - interTop);

    return calculateArea(width, height);
}

bool areaDuplicates(const Candidate& a, const Candidate& b) {
    const float intersectionArea =
        calculateIntersectionArea(BoundingBox{a.x, a.y, a.w, a.h}, BoundingBox{b.x, b.y, b.w, b.h});

    const float areaA = calculateArea(a.w, a.h); // NOLINT
    const float areaB = calculateArea(b.w, b.h); // NOLINT

    if (const auto unionArea = areaA + areaB - intersectionArea; unionArea > 0.0F) {
        const float intersectionOverlapUnion = (intersectionArea / unionArea);
        return intersectionOverlapUnion > 0.45F;
    }

    return false;
}

std::size_t classIdFromChannel(std::size_t channel) {
    if (channel < 4) {
        return 0;
    }
    return channel - 4;
}

cv::Mat preprocessFrame(const Frame& frame) {
    // Preprocessing;
    // Resize to yolo model input size
    cv::Mat resizedImage;
    cv::resize(frame.data, resizedImage, cv::Size{IMAGE_SHAPE.width, IMAGE_SHAPE.height});

    // Convert OpenCV BGR -> RGB (YOLO)
    cv::Mat rgbFrame;
    cv::cvtColor(resizedImage, rgbFrame, cv::COLOR_BGR2RGB);

    // Normalize image
    // Convert from unit8_t [0..255] -> float32 [0..1]
    cv::Mat floatImage;
    rgbFrame.convertTo(floatImage, CV_32F, 1.0 / 255.0);

    return floatImage;
}

std::vector<float> createTensorData(const cv::Mat& preprocessedFrame) {
    std::vector<float> tensorData;
    constexpr auto tensorDataSize = CHANNELS * IMAGE_SHAPE.width * IMAGE_SHAPE.height;
    tensorData.reserve(tensorDataSize);

    for (int channel = 0; channel < CHANNELS; ++channel) {
        for (int y = 0; y < IMAGE_SHAPE.height; ++y) {
            for (int x = 0; x < IMAGE_SHAPE.width; ++x) {
                const auto& pixel = preprocessedFrame.at<cv::Vec3f>(y, x);
                tensorData.push_back(pixel.val[channel]); // NOLINT
            }
        }
    }

    return tensorData;
}

using TensorShape = std::array<int64_t, 4>;
using TensorData = std::vector<float>;

TensorShape createTensorShape() {
    return TensorShape{1, CHANNELS, IMAGE_SHAPE.width, IMAGE_SHAPE.height};
}

Ort::Value createInputTensor(TensorData& tensorData, const TensorShape& tensorShape) {
    return Ort::Value::CreateTensor(
        Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU),
        tensorData.data(),
        tensorData.size(),
        tensorShape.data(),
        tensorShape.size());
}

std::vector<Ort::Value> runInference(const Ort::Value* inputTensors, Ort::Session& session) {
    constexpr std::array<const char*, 1> inputNames{"images"};
    constexpr std::array<const char*, 1> outputNames{"output0"};
    return session.Run({}, inputNames.data(), inputTensors, inputNames.size(), outputNames.data(), outputNames.size());
}

float thresholdByLabel(LabelClassification label) {
    switch (label) {
    case LabelClassification::Fire: {
        return 0.25F;
    }
    default:
        return 0.5F;
    }
}

using Channel = std::size_t;

struct ChannelColumns {
    Channel x = 0;
    Channel y = 1;
    Channel width = 2;
    Channel height = 3;
    Channel score = 4;
};

constexpr ChannelColumns channelColumns;

std::vector<Candidate> decodeOutputTensor(const Ort::Value& outputTensor) {
    const auto* outputTensorData = outputTensor.GetTensorData<float>();
    const auto outputTensorShape = outputTensor.GetTensorTypeAndShapeInfo().GetShape();

    const auto channels = static_cast<Channel>(outputTensorShape.at(1)); // TODO: implement safe for avoid overflow
    const auto candidates = static_cast<Channel>(outputTensorShape.at(2));

    std::vector<Candidate> candidatesResult;
    for (std::size_t candidate = 0; candidate < candidates; ++candidate) {
        auto x = outputTensorData[channelColumns.x * candidates + candidate];      // NOLINT
        auto y = outputTensorData[channelColumns.y * candidates + candidate];      // NOLINT
        auto w = outputTensorData[channelColumns.width * candidates + candidate];  // NOLINT
        auto h = outputTensorData[channelColumns.height * candidates + candidate]; // NOLINT

        Channel bestChannel = channels;
        float bestScore = 0.0F;
        for (Channel channel = channelColumns.score; channel < channels; ++channel) {
            const std::size_t index = (channel * candidates) + candidate;
            const float score = outputTensorData[index]; // NOLINT
            if (score > bestScore) {
                bestChannel = channel;
                bestScore = score;
            }
        }

        if (bestScore > thresholdByLabel(mapToLabelClassification(classIdFromChannel(bestChannel)))) {
            const float left = std::max(0.0F, x - (w / 2.0F));
            const float top = std::max(0.0F, y - (h / 2.0F));
            candidatesResult.push_back({
                left,
                top,
                w,
                h,
                classIdFromChannel(bestChannel),
                bestScore,
            });
        }
    }

    return candidatesResult;
}

void applyNMS(std::vector<Candidate>& candidates) {
    if (candidates.size() < 2) {
        return;
    }

    std::sort(candidates.begin(), candidates.end(), [](const Candidate& lhs, const Candidate& rhs) {
        if (lhs.classId != rhs.classId) {
            return lhs.classId < rhs.classId;
        }
        return lhs.score > rhs.score;
    });

    std::vector<bool> isDuplicates(candidates.size(), false);

    for (std::size_t i = 0; i < candidates.size(); ++i) {
        if (isDuplicates[i]) { // NOLINT
            continue;
        }

        for (std::size_t j = i + 1; j < candidates.size(); ++j) {
            if (candidates[i].classId != candidates[j].classId) { // NOLINT
                break;
            }

            if (areaDuplicates(candidates[i], candidates[j])) { // NOLINT
                isDuplicates[j] = true;                         // NOLINT
            }
        }
    }

    std::size_t write = 0;
    for (std::size_t read = 0; read < candidates.size(); ++read) {
        if (!isDuplicates[read]) { // NOLINT
            if (read != write) {
                candidates[write] = std::move(candidates[read]); // NOLINT
            }
            ++write;
        }
    }

    candidates.resize(write);
}
} // namespace

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
    const auto preprocessedFrame = preprocessFrame(frame);
    auto tensorData = createTensorData(preprocessedFrame);
    const auto tensorShape = createTensorShape();
    auto inputTensor = createInputTensor(tensorData, tensorShape);

    const std::array<Ort::Value, 1> inputTensors{std::move(inputTensor)};

    const auto outputTensors = runInference(inputTensors.data(), session_);
    const auto& outputTensor = outputTensors.at(0);

    auto candidates = decodeOutputTensor(outputTensor);

    applyNMS(candidates);

    ObjectDetections objectDetections;
    for (const auto& candidate : candidates) {
        const auto label = mapToLabelClassification(candidate.classId);
        objectDetections.push_back({
            label,
            Confidence{candidate.score},
            {
                candidate.x,
                candidate.y,
                candidate.w,
                candidate.h,
            },
        });
    }

    return objectDetections;
}
