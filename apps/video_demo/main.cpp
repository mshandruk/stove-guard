#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iostream>
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

#include "AnalysisResult.h"
#include "ConsoleNotifier.h"
#include "Detection.h"
#include "FakeFrameAnalyzer.h"
#include "Frame.h"
#include "FrameDisplay.h"
#include "FrameSource.h"
#include "FrameTimer.h"
#include "RealClock.h"
#include "StoveGuardApp.h"
#include "StoveGuardRunner.h"

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
                std::string(labelToString(label)) + " " + std::to_string(confidence),
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
void printUsage(const std::string_view progname) {
    std::cout << "Usage: " << progname << " <path-to-video-file>\n";
}
} // namespace

int main(const int argc, char* argv[]) {
    if (argc != 2) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        const std::string_view progname = argv[0] != nullptr ? argv[0] : "stove_guard_video_demo";
        printUsage(progname);
        return EXIT_FAILURE;
    }

    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    const std::string_view videoPath = argv[1];

    if (!std::filesystem::is_regular_file(videoPath)) {
        std::cerr << "Error: File-> " << videoPath << " not found." << '\n';
        return EXIT_FAILURE;
    }

    ConsoleNotifier consoleNotifier;
    RealClock realClock;
    FrameTimer frameTimer{realClock};

    constexpr auto ALARM_THRESHOLD = std::chrono::seconds{2};

    StoveGuardApp app{ALARM_THRESHOLD, consoleNotifier, frameTimer};
    OpencvFrameDisplay frameDisplay;

    FakeScenario scenario = {{
        {
            {
                AnalyzerResult{
                    Detection{false, true},
                    ObjectDetections{
                        ObjectDetection{
                            LabelClassification::Person,
                            99.0,
                            BoundingBox{100, 100, 200, 200},
                        },
                    },
                },
            },
            "Stove is OFF and person is present",
        },
        {
            {
                AnalyzerResult{
                    Detection{true, true},
                    ObjectDetections{
                        ObjectDetection{
                            LabelClassification::Stove,
                            99.0,
                            BoundingBox{100, 100, 200, 200},
                        },
                        ObjectDetection{
                            LabelClassification::Person,
                            99.0,
                            BoundingBox{200, 200, 300, 300},
                        },
                    },
                },
            },
            "Stove is ON and person is present",
        },
        {
            {
                AnalyzerResult{
                    Detection{true, false},
                    ObjectDetections{
                        ObjectDetection{
                            LabelClassification::Stove,
                            99.0,
                            BoundingBox{200, 200, 300, 300},
                        },
                    },
                },
            },
            "Stove is ON and person is absent",
        },

    }};

    try {
        VideoFileFrameSource frameSource(videoPath);
        FakeFrameAnalyzer frameAnalyzer{std::move(scenario)};
        StoveGuardRunner runner{app, frameSource, frameAnalyzer, &frameDisplay};
        runner.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return 0;
}