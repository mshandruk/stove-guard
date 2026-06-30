#ifndef STOVEGUARD_FRAMEANALYZER_H
#define STOVEGUARD_FRAMEANALYZER_H

#include "Frame.h"
#include "ObjectDetection.h"

class FrameAnalyzer {
  public:
    FrameAnalyzer() = default;
    virtual ~FrameAnalyzer() = default;

    FrameAnalyzer(const FrameAnalyzer& other) = delete;
    FrameAnalyzer& operator=(const FrameAnalyzer& other) = delete;

    FrameAnalyzer(FrameAnalyzer&& other) = default;
    FrameAnalyzer& operator=(FrameAnalyzer&& other) = default;

    virtual ObjectDetections analyze(const Frame& frame) = 0;
};

#endif // STOVEGUARD_FRAMEANALYZER_H
