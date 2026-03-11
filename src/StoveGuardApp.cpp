#include "StoveGuardApp.h"

#include "Detection.h"
#include "DetectionMapper.h"
#include "FrameTimer.h"
#include "Notifier.h"
#include "StoveMonitor.h"

using namespace std::chrono;

StoveGuardApp::StoveGuardApp(const Duration alarmThreshold, Notifier& notifier, FrameTimer& frameTimer)
        : stoveMonitor_{alarmThreshold},
          notifier_{notifier},
          frameTimer_{frameTimer} {
}

Event StoveGuardApp::processFrame(const Detection detection) {
    const auto [stoveState, personState] = toDomain(detection);
    const auto delta = frameTimer_.tick();

    const auto event = stoveMonitor_.process(stoveState, personState, delta);
    if (event != Event::None) {
        notifier_.notify(event);
    }

    return event;
}