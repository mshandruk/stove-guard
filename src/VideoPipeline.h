#pragma once

#include "DetectionFilter.h"
#include "FrameAnalyzer.h"
#include "FrameDisplay.h"
#include "FrameSource.h"
#include "SafetyService.h"
#include "StateStabilizer.h"

class VideoPipeline {
  public:
    VideoPipeline(
        SafetyService& safetyService,
        FrameSource& frameSource,
        FrameAnalyzer& frameAnalyzer,
        const DetectionFilter& detectionFilter,
        FrameDisplay* frameDisplay = nullptr);
    ~VideoPipeline() = default;

    VideoPipeline(const VideoPipeline&) = delete;
    VideoPipeline& operator=(const VideoPipeline&) = delete;

    VideoPipeline(VideoPipeline&&) = delete;
    VideoPipeline& operator=(VideoPipeline&&) = delete;

    void run();

    bool processNextFrame();

  private:
    SafetyService& safetyService_;
    FrameSource& frameSource_;
    FrameAnalyzer& frameAnalyzer_;
    const DetectionFilter& detectionFilter_;
    FrameDisplay* frameDisplay_ = nullptr;
    StateStabilizer<PersonState> personStabilizer_{PersonState::Absent, 3};
    StateStabilizer<StoveState> stoveStabilizer_{StoveState::Off, 3};
};