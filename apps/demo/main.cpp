#include <chrono>
#include <optional>
#include <thread>

#include "ConsoleNotifier.h"
#include "DetectionFilter.h"
#include "FakeFrameAnalyzer.h"
#include "Frame.h"
#include "FrameSource.h"
#include "FrameTimer.h"
#include "ObjectDetection.h"
#include "RealClock.h"
#include "SafetyService.h"
#include "VideoPipeline.h"

class TimerFrameSource final : public FrameSource {
  public:
    [[nodiscard]]
    std::optional<Frame> getFrame() override {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return Frame{};
    }
};

namespace {
FakeScenario getFakeScenario() {
    return {
        FakeScenario{
            {
                ObjectDetections{
                    ObjectDetection{
                        LabelClassification::Person,
                        Confidence{0.9F},
                        BoundingBox{200, 200, 300, 300},
                    },

                },
                "Stove is OFF and person is present",
            },
            {
                ObjectDetections{
                    ObjectDetection{
                        LabelClassification::Stove,
                        Confidence{0.9F},
                        BoundingBox{100, 100, 200, 200},
                    },
                },
                "Stove is ON and person is absent",
            },
            {
                ObjectDetections{
                    ObjectDetection{
                        LabelClassification::Stove,
                        Confidence{0.9F},
                        BoundingBox{100, 100, 200, 200},
                    },
                    ObjectDetection{
                        LabelClassification::Person,
                        Confidence{0.9F},
                        BoundingBox{200, 200, 300, 300},
                    },

                },
                "Stove is ON and person is appears",
            },
            {
                ObjectDetections{
                    ObjectDetection{
                        LabelClassification::Stove,
                        Confidence{0.9F},
                        BoundingBox{100, 100, 200, 200},
                    },
                },
                "Stove is ON and person is absent",
            },
            {
                ObjectDetections{
                    ObjectDetection{
                        LabelClassification::Stove,
                        Confidence{0.9F},
                        BoundingBox{100, 100, 200, 200},
                    },
                },
                "Stove is ON and person is absent",
            },
            {
                ObjectDetections{
                    ObjectDetection{
                        LabelClassification::Stove,
                        Confidence{0.9F},
                        BoundingBox{100, 100, 200, 200},
                    },
                },
                "Stove is ON and person is absent",
            },
            {
                ObjectDetections{
                    ObjectDetection{
                        LabelClassification::Stove,
                        Confidence{0.9F},
                        BoundingBox{100, 100, 200, 200},
                    },
                    ObjectDetection{
                        LabelClassification::Person,
                        Confidence{0.9F},
                        BoundingBox{200, 200, 300, 300},
                    },

                },
                "Stove is ON and person is appears",
            },
            {
                ObjectDetections{},
                "Stove is OFF and person is absent",
            },
        },
    };
}
} // namespace

int main() {
    constexpr auto ALARM_THRESHOLD = std::chrono::seconds{2};

    RealClock clock;
    FrameTimer deltaFrame{clock};
    ConsoleNotifier consoleNotifier;
    SafetyService safetyService{ALARM_THRESHOLD, consoleNotifier, deltaFrame};

    const ConfidenceThresholds confidenceThresholds{
        {LabelClassification::Person, Confidence{0.7F}},
        {LabelClassification::Stove, Confidence{0.7F}},
    };
    const DetectionFilter detectionFilter{confidenceThresholds};

    TimerFrameSource frameSource;
    FakeFrameAnalyzer frameAnalyzer{getFakeScenario()};
    VideoPipeline videoPipeline(safetyService, frameSource, frameAnalyzer, detectionFilter);
    videoPipeline.run();

    return 0;
}
