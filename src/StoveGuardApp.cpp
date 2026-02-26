#include "StoveGuardApp.h"

#include <cassert>
#include <chrono>

#include "DetectionMapper.h"
#include "FrameAnalyzer.h"
#include "StoveMonitor.h"

using namespace std::chrono;

StoveGuardApp::StoveGuardApp(FrameAnalyzer* frameAnalyzer)
        : frameAnalyzer_{frameAnalyzer} {
    assert(frameAnalyzer_ != nullptr);
}

Event StoveGuardApp::processFrame(const Frame& frame, const steady_clock::time_point currentTimestamp) {
    const auto detection = frameAnalyzer_->analyze(frame);
    const auto [stoveState, personState] = toDomain(detection);
    const auto delta = !lastTimestamp_ ? Duration::zero() : duration_cast<Duration>(currentTimestamp - *lastTimestamp_);
    lastTimestamp_ = currentTimestamp;
    return stoveMonitor_.process(stoveState, personState, delta);
}