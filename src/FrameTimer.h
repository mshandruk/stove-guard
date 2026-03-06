#ifndef STOVEGUARD_FRAMETIMER_H
#define STOVEGUARD_FRAMETIMER_H

#include <optional>

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
    std::optional<std::chrono::steady_clock::time_point> previousTime_;
};

#endif // STOVEGUARD_FRAMETIMER_H
