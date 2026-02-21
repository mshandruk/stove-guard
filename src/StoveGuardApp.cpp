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

Event StoveGuardApp::processFrame(const Frame& frame, const steady_clock::time_point currentTime) {
    const auto detection = frameAnalyzer_->analyze(frame);
    const auto [stoveState, personState] = toDomain(detection);

    return stoveMonitor_.process(stoveState, personState, currentTime);
}