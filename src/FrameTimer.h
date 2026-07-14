#pragma once

#include "Clock.h"
#include "StoveMonitor.h"

class FrameTimer {
  public:
    ~FrameTimer() = default;

    explicit FrameTimer(Clock& clock);

    FrameTimer(const FrameTimer&) = delete;
    FrameTimer& operator=(const FrameTimer&) = delete;
    FrameTimer(FrameTimer&&) = delete;
    FrameTimer& operator=(FrameTimer&&) = delete;

    Duration tick();

  private:
    Clock& clock_;
    TimePoint previousTime_;
};
