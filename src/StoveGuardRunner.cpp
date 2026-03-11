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
        const auto result = frameAnalyzer_.analyze(*frame);
        app_.processFrame(result);
        std::cout << "[Runner] frame processed" << '\n';
    }
}
