#include "StoveGuardRunner.h"

#include "FrameSource.h"
#include "StoveGuardApp.h"

StoveGuardRunner::StoveGuardRunner(FrameSource& frameSource, StoveGuardApp& app)
        : frameSource_{frameSource},
          app_{app} {
}

void StoveGuardRunner::run() {
    while (const auto frame = frameSource_.getFrame()) {
        app_.processFrame(*frame);
    }
}
