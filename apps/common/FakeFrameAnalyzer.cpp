#include "FakeFrameAnalyzer.h"

#include <iostream>
#include <stdexcept>
#include <utility>

#include "AnalysisResult.h"
#include "Frame.h"

FakeFrameAnalyzer::FakeFrameAnalyzer(FakeScenario scenario)
        : scenario_{std::move(scenario)} {
    if (scenario_.empty()) {
        throw std::runtime_error("Fake scenario should be is not empty");
    }
}

AnalyzerResult FakeFrameAnalyzer::analyze([[maybe_unused]] const Frame& frame) {
    step_ = step_ % scenario_.size();
    const auto& [result, label] = scenario_.at(step_);
    std::cout << "[FakeFrameAnalyzer] Step " << step_ + 1 << ": " << label << "\n";
    ++step_;
    return result;
}