#ifndef STOVEGUARD_STOVEGUARDAPP_H
#define STOVEGUARD_STOVEGUARDAPP_H

#include <optional>

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

    StoveGuardApp(FrameAnalyzer& frameAnalyzer, Notifier& notifier);
    Event processFrame(const Frame& frame, std::chrono::steady_clock::time_point currentTimestamp);

  private:
    std::optional<std::chrono::steady_clock::time_point> lastTimestamp_;
    StoveMonitor stoveMonitor_;
    FrameAnalyzer& frameAnalyzer_;
    Notifier& notifier_;
};
#endif // STOVEGUARD_STOVEGUARDAPP_H
