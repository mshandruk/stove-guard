#include <chrono>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <thread>

#include "ConsoleNotifier.h"
#include "FakeFrameAnalyzer.h"
#include "Frame.h"
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

namespace {
void printUsage(const std::string_view progname) {
    std::cout << "Usage: " << progname << " <path-to-video-file>\n";
}
} // namespace

int main(int argc, char* argv[]) {
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

    try {
        VideoFileFrameSource frameSource(videoPath);
        FakeFrameAnalyzer frameAnalyzer;
        StoveGuardRunner runner{app, frameSource, frameAnalyzer};
        runner.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return 0;
}