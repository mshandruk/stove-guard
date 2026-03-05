#ifndef STOVEGUARD_STOVEGUARDAPP_H
#define STOVEGUARD_STOVEGUARDAPP_H

#include <optional>

#include "Clock.h"
#include "FrameAnalyzer.h"
#include "Notifier.h"
#include "StoveMonitor.h"

class StoveGuardApp {
  public:
    StoveGuardApp(const StoveGuardApp&) = delete;
    StoveGuardApp& operator=(const StoveGuardApp&) = delete;
    StoveGuardApp(StoveGuardApp&&) = delete;
    StoveGuardApp& operator=(StoveGuardApp&&) = delete;

    ~StoveGuardApp() = default;

    StoveGuardApp(Duration alarmThreshold, FrameAnalyzer& frameAnalyzer, Notifier& notifier, Clock& clock);
    Event processFrame(const Frame& frame);

  private:
    std::optional<std::chrono::steady_clock::time_point> lastTimestamp_;
    StoveMonitor stoveMonitor_;
    FrameAnalyzer& frameAnalyzer_;
    Notifier& notifier_;
    Clock& clock_;
};
#endif // STOVEGUARD_STOVEGUARDAPP_H
