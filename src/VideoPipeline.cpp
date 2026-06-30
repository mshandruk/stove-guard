#include "VideoPipeline.h"

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
    while (processNextFrame()) {
    }
}

bool VideoPipeline::processNextFrame() {
    const auto frame = frameSource_.getFrame();
    if (!frame) {
        return false;
    }

    const auto objectDetections = frameAnalyzer_.analyze(*frame);
    const auto filteredDetections = detectionFilter_.filter(objectDetections);
    auto scene = SceneMapper::map(filteredDetections);
    scene.personState = personStabilizer_.updateState(scene.personState);
    scene.stoveState = stoveStabilizer_.updateState(scene.stoveState);
    safetyService_.handle(scene);

    if (frameDisplay_ != nullptr) {
        frameDisplay_->render(*frame, objectDetections);
    }

    return true;
}
