#ifndef STOVEGUARD_STOVEGUARDAPP_H
#define STOVEGUARD_STOVEGUARDAPP_H

#include "FrameAnalyzer.h"
#include "FrameTimer.h"
#include "Notifier.h"
#include "StoveMonitor.h"

class StoveGuardApp {
  public:
    StoveGuardApp(const StoveGuardApp&) = delete;
    StoveGuardApp& operator=(const StoveGuardApp&) = delete;
    StoveGuardApp(StoveGuardApp&&) = delete;
    StoveGuardApp& operator=(StoveGuardApp&&) = delete;

    ~StoveGuardApp() = default;

    StoveGuardApp(Duration alarmThreshold, FrameAnalyzer& frameAnalyzer, Notifier& notifier, FrameTimer& frameTimer);
    Event processFrame(const Frame& frame);

  private:
    StoveMonitor stoveMonitor_;
    FrameAnalyzer& frameAnalyzer_;
    Notifier& notifier_;
    FrameTimer& frameTimer_;
};
#endif // STOVEGUARD_STOVEGUARDAPP_H
