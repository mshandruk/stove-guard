#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <utility>

#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "AppOptions.h"
#include "ArgParser.h"
#include "ConsoleNotifier.h"
#include "DetectionFilter.h"
#include "FakeFrameAnalyzer.h"
#include "Frame.h"
#include "FrameAnalyzer.h"
#include "FrameDisplay.h"
#include "FrameSource.h"
#include "FrameTimer.h"
#include "ObjectDetection.h"
#include "RealClock.h"
#include "SafetyService.h"
#include "VideoPipeline.h"
#include "YoloFrameAnalyzer.h"

class VideoFileFrameSource final : public FrameSource {
  public:
    explicit VideoFileFrameSource(const std::filesystem::path& filePath)
            : videoCapture_{filePath.string()} {
        if (!videoCapture_.isOpened()) {
            throw std::runtime_error("Could not open file: " + filePath.string());
        }
    }

    std::optional<Frame> getFrame() override {
        Frame frame;
        if (!videoCapture_.read(frame.data)) {
            return std::nullopt;
        }
        // Simulate 1 frame per second
        std::this_thread::sleep_for(std::chrono::seconds{1});
        return frame;
    }

  private:
    cv::VideoCapture videoCapture_;
};

class OpencvFrameDisplay final : public FrameDisplay {
  public:
    void render(const Frame& frame, const ObjectDetections& objectDetections) override {
        const cv::Mat canvas = frame.data.clone();
        for (const auto& [label, confidence, boundingBox] : objectDetections) {
            const cv::Rect box{
                static_cast<int>(boundingBox.x),
                static_cast<int>(boundingBox.y),
                static_cast<int>(boundingBox.width),
                static_cast<int>(boundingBox.height),
            };
            const auto labelColor = getLabelColor(label);
            cv::rectangle(canvas, box, labelColor);
            cv::putText(
                canvas,
                std::string(labelToString(label)) + " " + std::to_string(confidence.value()),
                {box.x, std::max(box.y - 5, 15)},
                cv::FONT_HERSHEY_SIMPLEX,
                0.5,
                labelColor);
        }
        cv::imshow("StoveGuard", canvas);
        cv::waitKey(1);
    }

  private:
    static cv::Scalar getLabelColor(const LabelClassification& label) noexcept {
        switch (label) {
        case LabelClassification::Person: {
            return cv::Scalar{0, 255, 0};
        }
        case LabelClassification::Stove: {
            return cv::Scalar{0, 0, 255};
        }
        default:
            return cv::Scalar{0, 0, 0};
        }
    }
};

namespace {
[[maybe_unused]] FakeScenario getFakeScenario() {

    return {

        {

            ObjectDetections{
                ObjectDetection{
                    LabelClassification::Person,
                    Confidence{0.9F},
                    BoundingBox{100, 100, 200, 200},
                },

            },
            "Stove is OFF and person is present",
        },

        {

            ObjectDetections{
                ObjectDetection{
                    LabelClassification::Stove,
                    Confidence{0.9F},
                    BoundingBox{100, 100, 200, 200},
                },
                ObjectDetection{
                    LabelClassification::Person,
                    Confidence{0.9F},
                    BoundingBox{200, 200, 300, 300},
                },

            },
            "Stove is ON and person is present",
        },

        {

            ObjectDetections{
                ObjectDetection{
                    LabelClassification::Stove,
                    Confidence{0.9F},
                    BoundingBox{200, 200, 300, 300},
                },

            },
            "Stove is ON and person is absent",
        },

    };
}

StoveGuard::AppOptions buildAppOptions(const int argc, char** argv) {
    const StoveGuard::Cli::ArgParser parser(argc, argv);
    return toAppOptions(parser.parse());
}

std::unique_ptr<FrameAnalyzer> createFrameAnalyzer(const StoveGuard::AppOptions& options) {
    switch (options.analyzer) {
    case StoveGuard::Analyzer::Fake: {
        std::cout << "[DEBUG] Create frame analyzer: " << analyzerToString(options.analyzer) << '\n';
        return std::make_unique<FakeFrameAnalyzer>(getFakeScenario());
    }
    case StoveGuard::Analyzer::Yolo: {
        std::cout << "[DEBUG] Create frame analyzer: " << analyzerToString(options.analyzer) << '\n';
        // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
        return std::make_unique<YoloFrameAnalyzer>(options.modelPath.value());
    }
    }
    throw std::runtime_error("Unknown frame analyzer.");
}

} // namespace

int main(const int argc, char* argv[]) {
    const std::string_view progname =
        argc > 0 && argv != nullptr && argv[0] != nullptr ? argv[0] : "stove_guard_video_demo"; // NOLINT

    std::optional<StoveGuard::AppOptions> appOptionsOpt;
    try {
        appOptionsOpt = buildAppOptions(argc, argv);
    } catch (const StoveGuard::Cli::ParseError& e) {
        std::cerr << e.what() << '\n';
        StoveGuard::Cli::ArgParser::usage(progname);
        return EXIT_FAILURE;
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    if (!appOptionsOpt.has_value()) {
        return EXIT_FAILURE;
    }

    const auto& appOptions = *appOptionsOpt;
    if (!std::filesystem::is_regular_file(appOptions.videoPath)) {
        std::cerr << "[ERROR] Please provide a valid video file path.\n";
        return EXIT_FAILURE;
    }

    if (appOptions.analyzer == StoveGuard::Analyzer::Yolo) {
        // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
        if (!YoloFrameAnalyzer::isValidModelPath(appOptions.modelPath.value())) {
            std::cerr << "[ERROR] Please provide a valid '.onnx' model file path.\n";
            return EXIT_FAILURE;
        }
    }

    constexpr auto ALARM_THRESHOLD = std::chrono::seconds{2};
    ConfidenceThresholds confidenceThresholds{
        {LabelClassification::Person, Confidence{0.7F}},
        {LabelClassification::Stove, Confidence{0.7F}},
    };
    const DetectionFilter detectionFilter{std::move(confidenceThresholds)};

    ConsoleNotifier consoleNotifier;
    RealClock realClock;
    FrameTimer frameTimer{realClock};

    SafetyService safetyService{ALARM_THRESHOLD, consoleNotifier, frameTimer};
    OpencvFrameDisplay frameDisplay;

    try {
        VideoFileFrameSource frameSource(appOptions.videoPath);
        auto frameAnalyzer = createFrameAnalyzer(appOptions);

        // TODO: move to factory function with smart pointer
        VideoPipeline videoPipeline{safetyService, frameSource, *frameAnalyzer, detectionFilter, &frameDisplay};
        videoPipeline.run();

    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }
    return 0;
}