#include <chrono>
#include <optional>
#include <thread>

#include "ConsoleNotifier.h"
#include "Detection.h"
#include "Frame.h"
#include "FrameAnalyzer.h"
#include "FrameSource.h"
#include "StoveGuardApp.h"

class StubFrameAnalyzer final : public FrameAnalyzer {
  public:
    Detection analyze([[maybe_unused]] const Frame& frame) override {
        return nextDetection_;
    }

    void nextDetection(const Detection detection) {
        nextDetection_ = detection;
    }

  private:
    Detection nextDetection_ = Detection{false, false};
};

class TimerFrameSource final : public FrameSource {
  public:
    [[nodiscard]]
    std::optional<Frame> getFrame() override {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return Frame{};
    }
};

int main() {
    StubFrameAnalyzer frameAnalyzer;
    TimerFrameSource frameSource;
    ConsoleNotifier consoleNotifier;
    StoveGuardApp app{frameAnalyzer, consoleNotifier};

    constexpr int framesCount = 20;
    for (int i = 1; i <= framesCount; ++i) {
        switch (i) {
        case 3:
            frameAnalyzer.nextDetection({true, false});
            break;
        case 5:
            frameAnalyzer.nextDetection({true, true});
            break;
        case framesCount: {
            frameAnalyzer.nextDetection({false, true});
            break;
        }
        default:
            frameAnalyzer.nextDetection({true, false});
            break;
        }

        if (const auto frame = frameSource.getFrame()) {
            const auto currentTimestamp = std::chrono::steady_clock::now();
            app.processFrame(*frame, currentTimestamp);
        }
    }

    return 0;
}