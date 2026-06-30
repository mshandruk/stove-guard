#pragma once

#include "FrameTimer.h"
#include "Notifier.h"
#include "SceneState.h"
#include "StoveMonitor.h"

class SafetyService {
  public:
    SafetyService(Duration alarmThreshold, Notifier& notifier, FrameTimer& frameTimer);
    ~SafetyService() = default;

    SafetyService(const SafetyService&) = delete;
    SafetyService& operator=(const SafetyService&) = delete;

    SafetyService(SafetyService&&) = delete;
    SafetyService& operator=(SafetyService&&) = delete;

    Event handle(const SceneState& scene);

  private:
    StoveMonitor stoveMonitor_;
    Notifier& notifier_;
    FrameTimer& frameTimer_;
};