#include <chrono>
#include <cstddef>
#include <iostream>
#include <optional>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

#include "ConsoleNotifier.h"
#include "Detection.h"
#include "Frame.h"
#include "FrameAnalyzer.h"
#include "FrameSource.h"
#include "FrameTimer.h"
#include "RealClock.h"
#include "StoveGuardApp.h"
#include "StoveGuardRunner.h"

constexpr auto ALARM_THRESHOLD = std::chrono::seconds{2};

class FakeFrameAnalyzer final : public FrameAnalyzer {
  public:
    Detection analyze([[maybe_unused]] const Frame& frame) override {
        if (step_ >= scenario_.size()) {
            std::cout << "[FakeFrameAnalyzer] Scenario ended. Defaulting to OFF/Absent\n";
            return {false, false};
        }

        const auto [detection, label] = scenario_.at(step_++);
        std::cout << "[FakeFrameAnalyzer] Step " << step_ << ": " << label << "\n";
        return {detection.flameDetected, detection.personDetected};
    }

  private:
    std::vector<std::pair<Detection, std::string_view>> scenario_ = {
        {{false, true}, "Stove is OFF and person is present"},
        {{true, false}, "Stove is ON and person is absent"},
        {{true, true}, "Stove is ON and person is appears"},
        {{true, false}, "Stove is ON and person is absent"},
        {{true, false}, "Stove is ON and person is absent"},
        {{true, false}, "Stove is ON and person is absent"},
        {{true, true}, "Stove is ON and person is appears"},
    };
    std::size_t step_ = 0;
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

    FakeFrameAnalyzer frameAnalyzer;
    TimerFrameSource frameSource;
    ConsoleNotifier consoleNotifier;
    RealClock clock;
    FrameTimer deltaFrame{clock};

    StoveGuardApp app{ALARM_THRESHOLD, frameAnalyzer, consoleNotifier, deltaFrame};
    StoveGuardRunner runner(frameSource, app);
    runner.run();

    return 0;
}