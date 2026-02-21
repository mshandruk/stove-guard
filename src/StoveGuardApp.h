#ifndef STOVEGUARD_STOVEGUARDAPP_H
#define STOVEGUARD_STOVEGUARDAPP_H

#include "FrameAnalyzer.h"
#include "StoveMonitor.h"

class StoveGuardApp {
  public:
    explicit StoveGuardApp(FrameAnalyzer* frameAnalyzer);
    Event processFrame(const Frame& frame, std::chrono::steady_clock::time_point currentTime);

  private:
    StoveMonitor stoveMonitor_;
    FrameAnalyzer* frameAnalyzer_;
};
#endif // STOVEGUARD_STOVEGUARDAPP_H
