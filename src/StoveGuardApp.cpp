#include "StoveGuardApp.h"

#include <chrono>

#include "DetectionMapper.h"
#include "FrameAnalyzer.h"
#include "Notifier.h"
#include "StoveMonitor.h"

using namespace std::chrono;

StoveGuardApp::StoveGuardApp(FrameAnalyzer& frameAnalyzer, Notifier& notifier)
        : frameAnalyzer_{frameAnalyzer},
          notifier_{notifier} {
}

Event StoveGuardApp::processFrame(const Frame& frame, const steady_clock::time_point currentTimestamp) {
    const auto detection = frameAnalyzer_.analyze(frame);
    const auto [stoveState, personState] = toDomain(detection);
    const auto delta = !lastTimestamp_ ? Duration::zero() : duration_cast<Duration>(currentTimestamp - *lastTimestamp_);
    lastTimestamp_ = currentTimestamp;
    const auto event = stoveMonitor_.process(stoveState, personState, delta);

    if (event != Event::None) {
        notifier_.notify(event);
    }

    return event;
}