#include "FakeFrameAnalyzer.h"

#include <iostream>

#include "AnalysisResult.h"
#include "Frame.h"

AnalyzerResult FakeFrameAnalyzer::analyze([[maybe_unused]] const Frame& frame) {
    step_ = step_ % scenario_.size();
    const auto [detection, label] = scenario_.at(step_);
    std::cout << "[FakeFrameAnalyzer] Step " << step_ + 1 << ": " << label << "\n";
    ++step_;

    return AnalyzerResult{
        detection,
        {
            BoundingBox{100, 100, 200, 200},
            BoundingBox{200, 200, 300, 300},
        },
    };
}
