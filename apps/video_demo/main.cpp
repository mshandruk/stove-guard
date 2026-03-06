#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iostream>
#include <optional>
#include <stdexcept>

#include <opencv2/highgui.hpp>

#include "Frame.h"
#include "FrameSource.h"

class VideoFileFrameSource final : public FrameSource {
  public:
    explicit VideoFileFrameSource(const std::filesystem::path& filePath)
            : videoCapture_{filePath} {
        if (!videoCapture_.isOpened()) {
            throw std::runtime_error("Could not open file: " + filePath.string());
        }
    }

    std::optional<Frame> getFrame() override {
        Frame frame;
        if (!videoCapture_.read(frame.data)) {
            return std::nullopt;
        }
        return frame;
    }

  private:
    cv::VideoCapture videoCapture_;
};

int main() {
    try {
        VideoFileFrameSource frameSource("video/VID_20260117_204915.mp4");
        while (true) {
            const auto frame = frameSource.getFrame();
            if (!frame) {
                std::cout << "Video ended" << '\n';
                break;
            }

            cv::imshow("Playback", frame->data);

            if (cv::waitKey(30) == 27) {
                std::cout << "Exit..." << '\n';
                break;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }
    return 0;
}