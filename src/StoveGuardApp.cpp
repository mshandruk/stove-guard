#include "StoveGuardApp.h"

#include "DetectionMapper.h"
#include "Frame.h"
#include "FrameAnalyzer.h"
#include "FrameTimer.h"
#include "Notifier.h"
#include "StoveMonitor.h"

using namespace std::chrono;

StoveGuardApp::StoveGuardApp(
    const Duration alarmThreshold,
    FrameAnalyzer& frameAnalyzer,
    Notifier& notifier,
    FrameTimer& frameTimer)
        : stoveMonitor_{alarmThreshold},
          frameAnalyzer_{frameAnalyzer},
          notifier_{notifier},
          frameTimer_{frameTimer} {
}

Event StoveGuardApp::processFrame(const Frame& frame) {
    const auto detection = frameAnalyzer_.analyze(frame);
    const auto [stoveState, personState] = toDomain(detection);
    const auto delta = frameTimer_.tick();

    const auto event = stoveMonitor_.process(stoveState, personState, delta);
    if (event != Event::None) {
        notifier_.notify(event);
    }

    return event;
}