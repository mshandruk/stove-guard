#include <chrono>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <utility>

#include "AppOptions.h"
#include "ArgParser.h"
#include "ConsoleNotifier.h"
#include "DemoScenario.h"
#include "DetectionFilter.h"
#include "FakeFrameAnalyzer.h"
#include "FrameAnalyzer.h"
#include "FrameTimer.h"
#include "ObjectDetection.h"
#include "OpencvFrameDisplay.h"
#include "RealClock.h"
#include "SafetyService.h"
#include "VideoFileFrameSource.h"
#include "VideoPipeline.h"
#include "YoloFrameAnalyzer.h"

namespace {
StoveGuard::AppOptions buildAppOptions(int argc, char** argv) {
    const StoveGuard::Cli::ArgParser parser(argc, argv);
    return toAppOptions(parser.parse());
}

std::unique_ptr<FrameAnalyzer> createFrameAnalyzer(const StoveGuard::AppOptions& options) {
    switch (options.analyzer) {
    case StoveGuard::Analyzer::Fake: {
        std::cout << "[DEBUG] Create frame analyzer: " << analyzerToString(options.analyzer) << '\n';
        return std::make_unique<FakeFrameAnalyzer>(getDemoScenario());
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

int main(int argc, char* argv[]) {
    const std::string_view progname =
        argc > 0 && argv != nullptr && argv[0] != nullptr ? argv[0] : "stove_guard_video_demo"; // NOLINT

    std::optional<StoveGuard::AppOptions> appOptionsOpt;
    try {
        appOptionsOpt = buildAppOptions(argc, argv);
    } catch (const StoveGuard::Cli::ParseError& e) {
        std::cerr << e.what() << '\n';
        StoveGuard::Cli::ArgParser::usage(progname);
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
        {LabelClassification::Fire, Confidence{0.3F}},
        {LabelClassification::Pot, Confidence{0.7F}},
    };
    const DetectionFilter detectionFilter{std::move(confidenceThresholds)};

    ConsoleNotifier notifier;
    RealClock realClock;
    FrameTimer frameTimer{realClock};

    SafetyService safetyService{ALARM_THRESHOLD, notifier, frameTimer};

    try {
        OpencvFrameDisplay frameDisplay;
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