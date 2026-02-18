#ifndef STOVEGUARD_FRAMEANALYZER_H
#define STOVEGUARD_FRAMEANALYZER_H

#include "Detection.h"

struct Frame{};

class FrameAnalyzer {
  public:
    FrameAnalyzer() = default;

    FrameAnalyzer(const FrameAnalyzer& rhs) = delete;
    FrameAnalyzer& operator=(const FrameAnalyzer& rhs) = delete;

    FrameAnalyzer(FrameAnalyzer&& rhs) = default;
    FrameAnalyzer& operator=(FrameAnalyzer&& rhs) = default;

    virtual ~FrameAnalyzer() = default;

    virtual Detection analyze(const Frame& frame) = 0;
};

#endif // STOVEGUARD_FRAMEANALYZER_H
