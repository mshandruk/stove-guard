#include "StoveGuardApp.h"

#include <chrono>

#include "Detection.h"
#include "DetectionMapper.h"
#include "FrameAnalyzer.h"
#include "StoveMonitor.h"

StoveGuardApp::StoveGuardApp(FrameAnalyzer& frameAnalyzer)
        : frameAnalyzer_{frameAnalyzer} {
}

Event StoveGuardApp::run() {
    const auto detection = frameAnalyzer_.analyze(Frame{});
    const auto [stoveState, personState] = toDomain(detection);
    const auto currentTime = std::chrono::steady_clock::now();

    return stoveMonitor_.process(stoveState, personState, currentTime);
}