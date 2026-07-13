#include "VideoFileFrameSource.h"

#include <chrono>
#include <cmath>
#include <filesystem>
#include <optional>
#include <stdexcept>
#include <thread>

#include <opencv2/videoio.hpp>

#include "Frame.h"

constexpr double MS_PER_SEC = 1000.0;
constexpr double DEFAULT_FPS = 30.0;

VideoFileFrameSource::VideoFileFrameSource(const std::filesystem::path& videoPath)
        : capture_{videoPath.string()} {
    if (!capture_.isOpened()) {
        throw std::runtime_error("Could not open file: " + videoPath.string());
    }

    double fps = capture_.get(cv::CAP_PROP_FPS);
    if (fps <= 0.0) {
        fps = DEFAULT_FPS;
    }

    delay_ = std::chrono::milliseconds(static_cast<int>(std::round(MS_PER_SEC / fps)));
}

std::optional<Frame> VideoFileFrameSource::getFrame() {
    Frame frame;
    const auto startTime = std::chrono::steady_clock::now();

    if (!capture_.read(frame.data)) {
        return std::nullopt;
    }

    const auto endTime = std::chrono::steady_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    const auto timeToSleep = delay_ - elapsed;
    std::this_thread::sleep_for(timeToSleep);

    return frame;
}