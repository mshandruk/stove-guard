#include <chrono>
#include <cstddef>
#include <optional>
#include <thread>

#include "Clock.h"
#include "ConsoleNotifier.h"
#include "Detection.h"
#include "Frame.h"
#include "FrameAnalyzer.h"
#include "FrameSource.h"
#include "StoveGuardApp.h"

class FakeFrameAnalyzer final : public FrameAnalyzer {
  public:
    Detection analyze([[maybe_unused]] const Frame& frame) override {
        ++detectionCount_;

        if (detectionCount_ <= 4) {
            return {false, true};
        }
        if (detectionCount_ <= 20) {
            return {true, false};
        }
        if (detectionCount_ <= 25) {
            return {true, true};
        }

        return {false, true};
    }

  private:
    std::size_t detectionCount_ = 0;
};

class TimerFrameSource final : public FrameSource {
  public:
    [[nodiscard]]
    std::optional<Frame> getFrame() override {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return Frame{};
    }
};

class StoveGuardRunner {
  public:
    StoveGuardRunner(FrameSource& frameSource, StoveGuardApp& app)
            : frameSource_{frameSource},
              app_{app} {
    }

    StoveGuardRunner(const StoveGuardRunner&) = delete;
    StoveGuardRunner& operator=(const StoveGuardRunner&) = delete;
    StoveGuardRunner(StoveGuardRunner&&) = delete;
    StoveGuardRunner& operator=(StoveGuardRunner&&) = delete;
    ~StoveGuardRunner() = default;

    void run() {
        while (const auto frame = frameSource_.getFrame()) {
            app_.processFrame(*frame);
        }
    }

  private:
    FrameSource& frameSource_;
    StoveGuardApp& app_;
};

class RealClock final : public Clock {
  public:
    [[nodiscard]]
    std::chrono::steady_clock::time_point getTime() const override {
        return std::chrono::steady_clock::now();
    }
};

int main() {

    FakeFrameAnalyzer frameAnalyzer;
    TimerFrameSource frameSource;
    ConsoleNotifier consoleNotifier;
    RealClock clock;
    StoveGuardApp app{frameAnalyzer, consoleNotifier, clock};
    StoveGuardRunner runner(frameSource, app);
    runner.run();

    return 0;
}