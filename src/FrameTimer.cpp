#include "FrameTimer.h"

#include "Clock.h"
#include "StoveMonitor.h"

FrameTimer::FrameTimer(Clock& clock)
        : clock_{clock},
          previousTime_{clock_.getTime()} {
}

Duration FrameTimer::tick() {
    const TimePoint currentTime = clock_.getTime();
    const Duration delta = currentTime - previousTime_;
    previousTime_ = currentTime;

    return delta;
}
