#include <cassert>
#include <chrono>
#include <cstddef>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "Clock.h"
#include "DetectionFilter.h"
#include "FakeData.h"
#include "Frame.h"
#include "FrameAnalyzer.h"
#include "FrameSource.h"
#include "FrameTimer.h"
#include "Notifier.h"
#include "ObjectDetection.h"
#include "SafetyService.h"
#include "StoveMonitor.h"
#include "VideoPipeline.h"

using namespace std::chrono;

class FakeClock final : public Clock {
  public:
    FakeClock() = default;

    [[nodiscard]]
    steady_clock::time_point getTime() const override {
        return currentTime_;
    }

    void advance(const Duration duration) {
        currentTime_ += duration;
    }

  private:
    steady_clock::time_point currentTime_;
};

using FakeScenario = std::vector<ObjectDetections>;

class FakeFrameAnalyzer final : public FrameAnalyzer {
  public:
    explicit FakeFrameAnalyzer(FakeScenario scenario)
            : scenario_{std::move(scenario)} {
    }

    ObjectDetections analyze([[maybe_unused]] const Frame& frame) override {
        if (step_ >= scenario_.size()) {
            return ObjectDetections{};
        }
        return scenario_[step_++]; // NOLINT
    }

  private:
    std::size_t step_ = 0;
    FakeScenario scenario_;
};

class FakeFrameSource final : public FrameSource {
  public:
    explicit FakeFrameSource(const std::size_t framesLeft)
            : framesLeft_{framesLeft} {

        assert(framesLeft > 0 && "Frame count must be greater than zero");
    }

    std::optional<Frame> getFrame() override {
        if (framesLeft_ == 0) {
            return std::nullopt;
        }
        --framesLeft_;

        return Frame{};
    }

  private:
    std::size_t framesLeft_;
};

class FakeNotifier final : public Notifier {
  public:
    using Events = std::vector<Event>;

    void notify(const Event event) override {
        events_.push_back(event);
    }

    [[nodiscard]]
    const Events& getEvents() const noexcept {
        return events_;
    }

  private:
    Events events_;
};

class VideoPipelineTest : public testing::Test {
  public:
    VideoPipeline makePipeline(const Duration alarmThreshold, const FakeScenario& scenario) {
        safetyService_ = std::make_unique<SafetyService>(alarmThreshold, fakeNotifier, frameTimer_);
        frameSource_ = std::make_unique<FakeFrameSource>(scenario.size());
        frameAnalyzer_ = std::make_unique<FakeFrameAnalyzer>(scenario);

        return VideoPipeline{*safetyService_, *frameSource_, *frameAnalyzer_, detectionFilter_};
    }

  protected:
    FakeNotifier fakeNotifier; // NOLINT
    FakeClock fakeClock_;      // NOLINT

  private:
    ConfidenceThresholds confidenceThresholds{
        {
            LabelClassification::Person,
            Confidence{0.7F},
        },
        {
            LabelClassification::Stove,
            Confidence{0.7F},
        },
        {
            LabelClassification::Pot,
            Confidence{0.7F},
        },
        {
            LabelClassification::Fire,
            Confidence{0.7F},
        },
    };
    DetectionFilter detectionFilter_{confidenceThresholds};

    FrameTimer frameTimer_{fakeClock_};

    std::unique_ptr<FakeFrameSource> frameSource_;
    std::unique_ptr<FakeFrameAnalyzer> frameAnalyzer_;
    std::unique_ptr<SafetyService> safetyService_;
};

namespace {
constexpr std::size_t STABILIZATION_FRAMES = 3;

namespace Scenarios {
FakeScenario repeat(const std::size_t n, const FakeScenario& scenario) {
    FakeScenario repeatedScenario;
    repeatedScenario.reserve(n * scenario.size());

    for (std::size_t i = 0; i < n; ++i) {
        repeatedScenario.insert(repeatedScenario.end(), scenario.begin(), scenario.end());
    }

    return repeatedScenario;
}

void append(FakeScenario& target, const FakeScenario& other) {
    target.insert(target.end(), other.begin(), other.end());
}

FakeScenario stoveOn() {
    auto stove = FakeData::Factory::makeStove();
    auto fireOnStove = FakeData::Factory::makeFireOnStove(stove);

    return FakeScenario{{stove, fireOnStove}};
}

FakeScenario stoveOnWithPerson() {
    const auto stove = FakeData::Factory::makeStove();
    const auto fireOnStove = FakeData::Factory::makeFireOnStove(stove);
    const auto person = FakeData::Factory::makePerson();

    return FakeScenario{{
        stove,
        fireOnStove,
        person,

    }};
}

} // namespace Scenarios

void runAllSteps(VideoPipeline& pipeline) {
    while (pipeline.processNextFrame()) {
    }
}
} // namespace

TEST_F(VideoPipelineTest, DangerousEntered_WhenStoveOnAndPersonAbsent) {
    const auto scenario = Scenarios::repeat(STABILIZATION_FRAMES, Scenarios::stoveOn());
    constexpr auto alarmThreshold = Duration{2};
    auto pipeline = makePipeline(alarmThreshold, scenario);

    runAllSteps(pipeline);

    const auto& events = fakeNotifier.getEvents();
    ASSERT_EQ(events.size(), 1);
    EXPECT_EQ(events.at(0), Event::DangerousEntered);
}

TEST_F(VideoPipelineTest, NotCancelDangerousState_WhenDangerousEnteredAndNoicePersonAppears) {
    FakeScenario scenario;
    const auto dangerScene = Scenarios::repeat(STABILIZATION_FRAMES, Scenarios::stoveOn());
    Scenarios::append(scenario, dangerScene);

    const auto noiseScene = Scenarios::repeat(STABILIZATION_FRAMES - 1, Scenarios::stoveOnWithPerson());
    Scenarios::append(scenario, noiseScene);

    constexpr auto alarmThreshold = Duration{2};
    auto pipeline = makePipeline(alarmThreshold, scenario);

    runAllSteps(pipeline);

    const auto& events = fakeNotifier.getEvents();
    ASSERT_EQ(events.size(), 1);
    EXPECT_EQ(events.at(0), Event::DangerousEntered);
}

TEST_F(VideoPipelineTest, DangerousCleared_WhenPersonAppears) {
    FakeScenario scenario;
    const auto dangerScene = Scenarios::repeat(STABILIZATION_FRAMES, Scenarios::stoveOn());
    Scenarios::append(scenario, dangerScene);

    const auto safeScene = Scenarios::repeat(STABILIZATION_FRAMES, Scenarios::stoveOnWithPerson());
    Scenarios::append(scenario, safeScene);

    constexpr auto alarmThreshold = Duration{60};
    auto pipeline = makePipeline(alarmThreshold, scenario);

    runAllSteps(pipeline);

    const auto& events = fakeNotifier.getEvents();
    ASSERT_EQ(events.size(), 2);
    EXPECT_EQ(events, (std::vector<Event>{Event::DangerousEntered, Event::DangerousCleared}));
}

TEST_F(VideoPipelineTest, NotEvents_WhenSafeState) {
    FakeScenario scenario;
    const auto safeScene = Scenarios::repeat(STABILIZATION_FRAMES, Scenarios::stoveOnWithPerson());
    Scenarios::append(scenario, safeScene);

    const auto noiceScene = Scenarios::repeat(STABILIZATION_FRAMES - 1, Scenarios::stoveOn());
    Scenarios::append(scenario, noiceScene);

    constexpr auto alarmThreshold = Duration{2};
    auto pipeline = makePipeline(alarmThreshold, scenario);

    runAllSteps(pipeline);

    const auto& events = fakeNotifier.getEvents();
    EXPECT_EQ(events.size(), 0);
}

TEST_F(VideoPipelineTest, AlarmStarted_WhenDangerousAboveAlarmThreshold) {
    FakeScenario scenario;
    const auto dangerScene = Scenarios::repeat(STABILIZATION_FRAMES, Scenarios::stoveOn());
    Scenarios::append(scenario, dangerScene);

    const auto alarmScene = Scenarios::repeat(1, Scenarios::stoveOn());
    Scenarios::append(scenario, alarmScene);

    constexpr auto alarmThreshold = Duration{2};
    auto pipeline = makePipeline(alarmThreshold, scenario);

    pipeline.processNextFrame();
    pipeline.processNextFrame();
    pipeline.processNextFrame();
    auto events = fakeNotifier.getEvents();
    ASSERT_EQ(events.size(), 1);
    ASSERT_EQ(events.at(0), Event::DangerousEntered);

    fakeClock_.advance(alarmThreshold);
    pipeline.processNextFrame();
    events = fakeNotifier.getEvents();
    ASSERT_EQ(events.size(), 2);
    EXPECT_EQ(events.at(1), Event::AlarmStarted);
}

TEST_F(VideoPipelineTest, AlarmCleared_WhenPersonAppears) {
    FakeScenario scenario;
    const auto dangerousScene = Scenarios::repeat(STABILIZATION_FRAMES, Scenarios::stoveOn());
    Scenarios::append(scenario, dangerousScene);

    const auto alarmScene = Scenarios::repeat(1, Scenarios::stoveOn());
    Scenarios::append(scenario, alarmScene);

    const auto safeScene = Scenarios::repeat(STABILIZATION_FRAMES, Scenarios::stoveOnWithPerson());
    Scenarios::append(scenario, safeScene);

    constexpr auto alarmThreshold = Duration{2};
    auto pipeline = makePipeline(alarmThreshold, scenario);

    pipeline.processNextFrame();
    pipeline.processNextFrame();
    pipeline.processNextFrame();
    auto events = fakeNotifier.getEvents();
    ASSERT_EQ(events.size(), 1);
    ASSERT_EQ(events.at(0), Event::DangerousEntered);

    fakeClock_.advance(alarmThreshold);
    pipeline.processNextFrame();
    events = fakeNotifier.getEvents();
    ASSERT_EQ(events.size(), 2);
    ASSERT_EQ(events.at(1), Event::AlarmStarted);

    runAllSteps(pipeline);
    events = fakeNotifier.getEvents();
    ASSERT_EQ(events.size(), 3);
    EXPECT_EQ(events.at(2), Event::AlarmCleared);
}

TEST_F(VideoPipelineTest, AlarmNotStarted_WhenDangerousAboveAlarmThresholdAndTimeNotChanged) {
    FakeScenario fullScenario;
    // Prepare for dangerous test
    const auto dangerScene = Scenarios::repeat(STABILIZATION_FRAMES, Scenarios::stoveOn());
    Scenarios::append(fullScenario, dangerScene);

    const auto alarmScene = Scenarios::repeat(10, Scenarios::stoveOn());
    Scenarios::append(fullScenario, alarmScene);

    constexpr auto alarmThreshold = Duration{2};
    auto pipeline = makePipeline(alarmThreshold, fullScenario);

    for (std::size_t step = 0; step < dangerScene.size(); ++step) {
        pipeline.processNextFrame();
    }
    auto events = fakeNotifier.getEvents();
    ASSERT_EQ(events.size(), 1);
    ASSERT_EQ(events.at(0), Event::DangerousEntered);

    for (std::size_t step = 0; step < alarmScene.size(); ++step) {
        pipeline.processNextFrame();
        fakeClock_.advance(Duration{0});
    }
    events = fakeNotifier.getEvents();
    EXPECT_EQ(events.size(), 1);
}
