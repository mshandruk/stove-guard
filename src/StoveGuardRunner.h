#ifndef STOVEGUARD_STOVEGUARDRUNNER_H
#define STOVEGUARD_STOVEGUARDRUNNER_H

#include "FrameAnalyzer.h"
#include "FrameDisplay.h"
#include "FrameSource.h"
#include "SceneInterpreter.h"
#include "StoveGuardApp.h"

class StoveGuardRunner {
  public:
    StoveGuardRunner(
        StoveGuardApp& app,
        FrameSource& frameSource,
        FrameAnalyzer& frameAnalyzer,
        SceneInterpreter& scene,
        FrameDisplay* frameDisplay = nullptr);
    ~StoveGuardRunner() = default;

    StoveGuardRunner(const StoveGuardRunner&) = delete;
    StoveGuardRunner& operator=(const StoveGuardRunner&) = delete;

    StoveGuardRunner(StoveGuardRunner&&) = delete;
    StoveGuardRunner& operator=(StoveGuardRunner&&) = delete;

    void run();

  private:
    StoveGuardApp& app_;
    FrameSource& frameSource_;
    FrameAnalyzer& frameAnalyzer_;
    SceneInterpreter& scene_;
    FrameDisplay* frameDisplay_ = nullptr;
};

#endif // STOVEGUARD_STOVEGUARDRUNNER_H
