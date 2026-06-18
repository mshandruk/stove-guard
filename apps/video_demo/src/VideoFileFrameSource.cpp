#include "VideoFileFrameSource.h"
#include "Frame.h"

#include <filesystem>
#include <optional>
#include <stdexcept>
#include <thread>

VideoFileFrameSource::VideoFileFrameSource(const std::filesystem::path& videoPath)
        : capture_{videoPath.string()} {
    if (!capture_.isOpened()) {
        throw std::runtime_error("Could not open file: " + videoPath.string());
    }
}

std::optional<Frame> VideoFileFrameSource::getFrame() {
    Frame frame;

    if (!capture_.read(frame.data)) {
        return std::nullopt;
    }

    std::this_thread::sleep_for(std::chrono::seconds{1});

    return frame;
}