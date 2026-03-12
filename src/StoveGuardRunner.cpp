#include "StoveGuardRunner.h"

#include <iostream>

#include "FrameAnalyzer.h"
#include "FrameSource.h"
#include "StoveGuardApp.h"

StoveGuardRunner::StoveGuardRunner(StoveGuardApp& app, FrameSource& frameSource, FrameAnalyzer& frameAnalyzer)
        : app_{app},
          frameSource_{frameSource},
          frameAnalyzer_{frameAnalyzer}

{
}

void StoveGuardRunner::run() {
    while (const auto frame = frameSource_.getFrame()) {
        const auto [detection, boundingBoxes] = frameAnalyzer_.analyze(*frame);
        app_.processFrame(detection);
        std::cout << "[Runner] frame processed" << '\n';
    }
}
