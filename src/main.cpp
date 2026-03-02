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
        return {true, false};
    }
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
    using namespace std::chrono;
    StubFrameAnalyzer frameAnalyzer;
    TimerFrameSource frameSource;
    ConsoleNotifier consoleNotifier;
    StoveGuardApp app{frameAnalyzer, consoleNotifier};

    if (const auto frame = frameSource.getFrame()) {
        const auto currentTimestamp = std::chrono::steady_clock::now();
        app.processFrame(*frame, currentTimestamp);
    }

    return 0;
}