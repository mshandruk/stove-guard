#include "SafetyService.h"

#include "FrameTimer.h"
#include "Notifier.h"
#include "SceneState.h"
#include "StoveMonitor.h"

using namespace std::chrono;

SafetyService::SafetyService(const Duration alarmThreshold, Notifier& notifier, FrameTimer& frameTimer)
        : stoveMonitor_{alarmThreshold},
          notifier_{notifier},
          frameTimer_{frameTimer} {
}

Event SafetyService::handle(const SceneState& scene) {
    const auto delta = frameTimer_.tick();
    const auto event = stoveMonitor_.process(scene.stoveState, scene.personState, delta);
    if (event != Event::None) {
        notifier_.notify(event);
    }
    return event;
}