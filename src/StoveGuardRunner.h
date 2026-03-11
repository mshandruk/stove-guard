#ifndef STOVEGUARD_STOVEGUARDRUNNER_H
#define STOVEGUARD_STOVEGUARDRUNNER_H

#include "FrameAnalyzer.h"
#include "FrameSource.h"
#include "StoveGuardApp.h"

class StoveGuardRunner {
  public:
    ~StoveGuardRunner() = default;

    StoveGuardRunner(StoveGuardApp& app, FrameSource& frameSource, FrameAnalyzer& frameAnalyzer);
    StoveGuardRunner(const StoveGuardRunner&) = delete;
    StoveGuardRunner& operator=(const StoveGuardRunner&) = delete;
    StoveGuardRunner(StoveGuardRunner&&) = delete;
    StoveGuardRunner& operator=(StoveGuardRunner&&) = delete;

    void run();

  private:
    StoveGuardApp& app_;
    FrameSource& frameSource_;
    FrameAnalyzer& frameAnalyzer_;
};

#endif // STOVEGUARD_STOVEGUARDRUNNER_H
