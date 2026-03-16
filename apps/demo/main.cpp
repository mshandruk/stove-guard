#include <chrono>
#include <optional>
#include <thread>
#include <utility>

#include "ConsoleNotifier.h"
#include "FakeFrameAnalyzer.h"
#include "Frame.h"
#include "FrameSource.h"
#include "FrameTimer.h"
#include "ObjectDetection.h"
#include "RealClock.h"
#include "SimpleSceneInterpreter.h"
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
            ObjectDetections{
                ObjectDetection{
                    LabelClassification::Person,
                    99.0,
                    BoundingBox{200, 200, 300, 300},
                },

            },
            "Stove is OFF and person is present",
        },
        {
            ObjectDetections{
                ObjectDetection{
                    LabelClassification::Stove,
                    99.0,
                    BoundingBox{100, 100, 200, 200},
                },
            },
            "Stove is ON and person is absent",
        },
        {
            ObjectDetections{
                ObjectDetection{
                    LabelClassification::Stove,
                    99.0,
                    BoundingBox{100, 100, 200, 200},
                },
                ObjectDetection{
                    LabelClassification::Person,
                    99.0,
                    BoundingBox{200, 200, 300, 300},
                },

            },
            "Stove is ON and person is appears",
        },
        {
            ObjectDetections{
                ObjectDetection{
                    LabelClassification::Stove,
                    99.0,
                    BoundingBox{100, 100, 200, 200},
                },
            },
            "Stove is ON and person is absent",
        },
        {
            ObjectDetections{
                ObjectDetection{
                    LabelClassification::Stove,
                    99.0,
                    BoundingBox{100, 100, 200, 200},
                },
            },
            "Stove is ON and person is absent",
        },
        {
            ObjectDetections{
                ObjectDetection{
                    LabelClassification::Stove,
                    99.0,
                    BoundingBox{100, 100, 200, 200},
                },
            },
            "Stove is ON and person is absent",
        },
        {
            ObjectDetections{
                ObjectDetection{
                    LabelClassification::Stove,
                    99.0,
                    BoundingBox{100, 100, 200, 200},
                },
                ObjectDetection{
                    LabelClassification::Person,
                    99.0,
                    BoundingBox{200, 200, 300, 300},
                },

            },
            "Stove is ON and person is appears",
        },
        {
            ObjectDetections{},
            "Stove is OFF and person is absent",
        },
    }};
    constexpr auto ALARM_THRESHOLD = std::chrono::seconds{2};

    FakeSceneInterpreter frameAnalyzer{std::move(scenario)};
    TimerFrameSource frameSource;

    RealClock clock;
    FrameTimer deltaFrame{clock};
    ConsoleNotifier consoleNotifier;
    StoveGuardApp app{ALARM_THRESHOLD, consoleNotifier, deltaFrame};

    SimpleSceneInterpreter scene;
    StoveGuardRunner runner(app, frameSource, frameAnalyzer, scene);
    runner.run();

    return 0;
}