#include "VideoPipeline.h"

#include <iostream>

#include "FrameAnalyzer.h"
#include "FrameDisplay.h"
#include "FrameSource.h"
#include "SafetyService.h"
#include "SceneMapper.h"

VideoPipeline::VideoPipeline(
    SafetyService& safetyService,
    FrameSource& frameSource,
    FrameAnalyzer& frameAnalyzer,
    FrameDisplay* frameDisplay)
        : safetyService_{safetyService},
          frameSource_{frameSource},
          frameAnalyzer_{frameAnalyzer},
          frameDisplay_{frameDisplay}

{
}

void VideoPipeline::run() {
    while (const auto frame = frameSource_.getFrame()) {
        const auto& objectDetections = frameAnalyzer_.analyze(*frame);
        const auto scene = SceneMapper::map(objectDetections);
        safetyService_.handle(scene);
        std::cout << "[Pipeline] frame processed" << '\n';
        if (frameDisplay_ != nullptr) {
            frameDisplay_->render(*frame, objectDetections);
        }
    }
}
