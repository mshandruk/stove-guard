#pragma once

#include <filesystem>

#include <opencv2/videoio.hpp>

#include "FrameSource.h"

class VideoFileFrameSource final : public FrameSource {
  public:
    explicit VideoFileFrameSource(const std::filesystem::path& videoPath);
    [[nodiscard]] std::optional<Frame> getFrame() override;

  private:
    cv::VideoCapture capture_;
    std::chrono::milliseconds delay_ = std::chrono::milliseconds{30};
};