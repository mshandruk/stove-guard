#include "FrameTimer.h"

#include <chrono>

#include "Clock.h"
#include "StoveMonitor.h"

FrameTimer::FrameTimer(Clock& clock)
        : clock_{clock} {
}

Duration FrameTimer::tick() {
    const auto currentTime = clock_.getTime();
    const auto delta =
        !previousTime_ ? Duration::zero() : std::chrono::duration_cast<Duration>(currentTime - *previousTime_);
    previousTime_ = currentTime;

    return delta;
}
