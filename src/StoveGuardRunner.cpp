#include "StoveGuardRunner.h"

#include <iostream>

#include "FrameAnalyzer.h"
#include "FrameDisplay.h"
#include "FrameSource.h"
#include "StoveGuardApp.h"

StoveGuardRunner::StoveGuardRunner(
    StoveGuardApp& app,
    FrameSource& frameSource,
    FrameAnalyzer& frameAnalyzer,
    FrameDisplay* frameDisplay)
        : app_{app},
          frameSource_{frameSource},
          frameAnalyzer_{frameAnalyzer},
          frameDisplay_{frameDisplay}

{
}

void StoveGuardRunner::run() {
    while (const auto frame = frameSource_.getFrame()) {
        const auto& [detection, objectDetections] = frameAnalyzer_.analyze(*frame);
        app_.processFrame(detection);
        std::cout << "[Runner] frame processed" << '\n';
        if (frameDisplay_ != nullptr) {
            frameDisplay_->render(*frame, objectDetections);
        }
    }
}
