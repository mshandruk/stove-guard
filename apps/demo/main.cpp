#include <chrono>
#include <optional>
#include <thread>

#include "ConsoleNotifier.h"
#include "FakeFrameAnalyzer.h"
#include "Frame.h"
#include "FrameSource.h"
#include "FrameTimer.h"
#include "RealClock.h"
#include "StoveGuardApp.h"
#include "StoveGuardRunner.h"

class TimerFrameSource final : public FrameSource {
  public:
    [[nodiscard]]
    std::optional<Frame> getFrame() override {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return Frame{};
    }
};

int main() {

    FakeFrameAnalyzer frameAnalyzer;
    TimerFrameSource frameSource;
    ConsoleNotifier consoleNotifier;
    RealClock clock;
    FrameTimer deltaFrame{clock};

    constexpr auto ALARM_THRESHOLD = std::chrono::seconds{2};
    StoveGuardApp app{ALARM_THRESHOLD, consoleNotifier, deltaFrame};
    StoveGuardRunner runner(app, frameSource, frameAnalyzer);
    runner.run();

    return 0;
}