#ifndef STOVEGUARD_STOVEGUARDRUNNER_H
#define STOVEGUARD_STOVEGUARDRUNNER_H

#include "FrameSource.h"
#include "StoveGuardApp.h"

class StoveGuardRunner {
  public:
    ~StoveGuardRunner() = default;

    StoveGuardRunner(FrameSource& frameSource, StoveGuardApp& app);
    StoveGuardRunner(const StoveGuardRunner&) = delete;
    StoveGuardRunner& operator=(const StoveGuardRunner&) = delete;
    StoveGuardRunner(StoveGuardRunner&&) = delete;
    StoveGuardRunner& operator=(StoveGuardRunner&&) = delete;

    void run();

  private:
    FrameSource& frameSource_;
    StoveGuardApp& app_;
};

#endif // STOVEGUARD_STOVEGUARDRUNNER_H
