#include <chrono>
#include <optional>
#include <thread>
#include <utility>

#include "AnalysisResult.h"
#include "ConsoleNotifier.h"
#include "Detection.h"
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
    FakeScenario scenario = {FakeScenario{
        {
            AnalyzerResult{
                Detection{false, true},
                ObjectDetections{},
            },
            "Stove is OFF and person is present",
        },
        {
            AnalyzerResult{
                Detection{true, false},
                ObjectDetections{},
            },
            "Stove is ON and person is absent",
        },
        {
            AnalyzerResult{
                Detection{true, true},
                ObjectDetections{},
            },
            "Stove is ON and person is appears",
        },
        {
            AnalyzerResult{
                Detection{true, false},
                ObjectDetections{},
            },
            "Stove is ON and person is absent",
        },
        {
            AnalyzerResult{
                Detection{true, false},
                ObjectDetections{},
            },
            "Stove is ON and person is absent",
        },
        {
            AnalyzerResult{
                Detection{true, false},
                ObjectDetections{},
            },
            "Stove is ON and person is absent",
        },
        {
            AnalyzerResult{
                Detection{true, true},
                ObjectDetections{},
            },
            "Stove is ON and person is appears",
        },
        {
            AnalyzerResult{
                Detection{false, false},
                ObjectDetections{},
            },
            "Stove is OFF and person is absent",
        },
    }};
    FakeFrameAnalyzer frameAnalyzer{std::move(scenario)};
    TimerFrameSource frameSource;

    RealClock clock;
    FrameTimer deltaFrame{clock};
    ConsoleNotifier consoleNotifier;

    constexpr auto ALARM_THRESHOLD = std::chrono::seconds{2};
    StoveGuardApp app{ALARM_THRESHOLD, consoleNotifier, deltaFrame};
    StoveGuardRunner runner(app, frameSource, frameAnalyzer);
    runner.run();

    return 0;
}