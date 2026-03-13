#include <chrono>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <thread>
#include <vector>

#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "AnalysisResult.h"
#include "ConsoleNotifier.h"
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
    void render(const Frame& frame, const std::vector<BoundingBox>& boxes) override {
        // Делаем копию кадра, чтобы не рисовать прямо на оригинале (если он нужен чистым)
        const cv::Mat canvas = frame.data.clone();

        for (const auto& [x, y, width, height] : boxes) {
            // Формируем точки: левый верхний угол и правый нижний
            const cv::Point topLeft(x, y);
            const cv::Point bottomRight(x + width, y + height);
            // Рисуем: (изображение, точки, цвет BGR, толщина линии)
            cv::rectangle(canvas, topLeft, bottomRight, cv::Scalar(0, 255, 0), 1);
        }
        cv::imshow("StoveGuard", canvas);
        cv::waitKey(1);
    }
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
    OpencvFrameDisplay frameDisplay;
    try {
        VideoFileFrameSource frameSource(videoPath);
        FakeFrameAnalyzer frameAnalyzer;
        StoveGuardRunner runner{app, frameSource, frameAnalyzer, &frameDisplay};
        runner.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return 0;
}