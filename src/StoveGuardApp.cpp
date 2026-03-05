#include "StoveGuardApp.h"

#include <chrono>

#include "Clock.h"
#include "DetectionMapper.h"
#include "Frame.h"
#include "FrameAnalyzer.h"
#include "Notifier.h"
#include "StoveMonitor.h"

using namespace std::chrono;

StoveGuardApp::StoveGuardApp(
    const Duration alarmThreshold,
    FrameAnalyzer& frameAnalyzer,
    Notifier& notifier,
    Clock& clock)
        : stoveMonitor_{alarmThreshold},
          frameAnalyzer_{frameAnalyzer},
          notifier_{notifier},
          clock_{clock} {
}

Event StoveGuardApp::processFrame(const Frame& frame) {
    const auto detection = frameAnalyzer_.analyze(frame);
    const auto [stoveState, personState] = toDomain(detection);
    const auto currentTimestamp = clock_.getTime();
    const auto delta = !lastTimestamp_ ? Duration::zero() : duration_cast<Duration>(currentTimestamp - *lastTimestamp_);
    lastTimestamp_ = currentTimestamp;
    const auto event = stoveMonitor_.process(stoveState, personState, delta);

    if (event != Event::None) {
        notifier_.notify(event);
    }

    return event;
}