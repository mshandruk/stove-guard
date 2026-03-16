#include "StoveGuardRunner.h"

#include <iostream>

#include "FrameAnalyzer.h"
#include "FrameDisplay.h"
#include "FrameSource.h"
#include "SceneInterpreter.h"
#include "StoveGuardApp.h"

StoveGuardRunner::StoveGuardRunner(
    StoveGuardApp& app,
    FrameSource& frameSource,
    FrameAnalyzer& frameAnalyzer,
    SceneInterpreter& scene,
    FrameDisplay* frameDisplay)
        : app_{app},
          frameSource_{frameSource},
          frameAnalyzer_{frameAnalyzer},
          scene_{scene},
          frameDisplay_{frameDisplay}

{
}

void StoveGuardRunner::run() {
    while (const auto frame = frameSource_.getFrame()) {
        const auto& objectDetections = frameAnalyzer_.analyze(*frame);
        const auto detected = scene_.interpret(objectDetections);
        app_.processFrame(detected);
        std::cout << "[Runner] frame processed" << '\n';
        if (frameDisplay_ != nullptr) {
            frameDisplay_->render(*frame, objectDetections);
        }
    }
}
