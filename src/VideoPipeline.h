#pragma once

#include "FrameAnalyzer.h"
#include "FrameDisplay.h"
#include "FrameSource.h"
#include "SafetyService.h"

class VideoPipeline {
  public:
    VideoPipeline(
        SafetyService& safetyService,
        FrameSource& frameSource,
        FrameAnalyzer& frameAnalyzer,
        FrameDisplay* frameDisplay = nullptr);
    ~VideoPipeline() = default;

    VideoPipeline(const VideoPipeline&) = delete;
    VideoPipeline& operator=(const VideoPipeline&) = delete;

    VideoPipeline(VideoPipeline&&) = delete;
    VideoPipeline& operator=(VideoPipeline&&) = delete;

    void run();

  private:
    SafetyService& safetyService_;
    FrameSource& frameSource_;
    FrameAnalyzer& frameAnalyzer_;
    FrameDisplay* frameDisplay_ = nullptr;
};