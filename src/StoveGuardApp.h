#ifndef STOVEGUARD_STOVEGUARDAPP_H
#define STOVEGUARD_STOVEGUARDAPP_H

#include "FrameAnalyzer.h"
#include "StoveMonitor.h"

class StoveGuardApp {
  public:
    explicit StoveGuardApp(FrameAnalyzer& frameAnalyzer);
    Event run();

  private:
    StoveMonitor stoveMonitor_;
    FrameAnalyzer& frameAnalyzer_;
};
#endif // STOVEGUARD_STOVEGUARDAPP_H
