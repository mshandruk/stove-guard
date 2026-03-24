#include "VideoPipeline.h"

#include <iostream>

#include "DetectionFilter.h"
#include "FrameAnalyzer.h"
#include "FrameDisplay.h"
#include "FrameSource.h"
#include "SafetyService.h"
#include "SceneMapper.h"

VideoPipeline::VideoPipeline(
    SafetyService& safetyService,
    FrameSource& frameSource,

    FrameAnalyzer& frameAnalyzer,
    const DetectionFilter& detectionFilter,
    FrameDisplay* frameDisplay)
        : safetyService_{safetyService},
          frameSource_{frameSource},
          frameAnalyzer_{frameAnalyzer},
          detectionFilter_{detectionFilter},
          frameDisplay_{frameDisplay}

{
}

void VideoPipeline::run() {
    while (const auto frame = frameSource_.getFrame()) {
        const auto objectDetections = frameAnalyzer_.analyze(*frame);
        const auto filteredDetections = detectionFilter_.filter(objectDetections);
        auto scene = SceneMapper::map(filteredDetections);
        scene.stoveState = stoveStabilizer_.updateState(scene.stoveState);
        scene.personState = personStabilizer_.updateState(scene.personState);
        safetyService_.handle(scene);
        std::cout << "[Pipeline] frame processed" << '\n';
        if (frameDisplay_ != nullptr) {
            frameDisplay_->render(*frame, objectDetections);
        }
    }
}
