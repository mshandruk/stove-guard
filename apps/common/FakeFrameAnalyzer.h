#ifndef STOVEGUARD_FAKEFRAMEANALYZER_H
#define STOVEGUARD_FAKEFRAMEANALYZER_H
#include <array>
#include <cstddef>

#include "AnalysisResult.h"
#include "Frame.h"
#include "FrameAnalyzer.h"

class FakeFrameAnalyzer final : public FrameAnalyzer {
  public:
    AnalyzerResult analyze(const Frame& frame) override;

  private:
    static constexpr std::array<std::pair<Detection, std::string_view>, 8> scenario_ = {{
        {{false, true}, "Stove is OFF and person is present"},
        {{true, false}, "Stove is ON and person is absent"},
        {{true, true}, "Stove is ON and person is appears"},
        {{true, false}, "Stove is ON and person is absent"},
        {{true, false}, "Stove is ON and person is absent"},
        {{true, false}, "Stove is ON and person is absent"},
        {{true, true}, "Stove is ON and person is appears"},
        {{false, false}, "Stove is OFF and person is absent"},
    }};
    std::size_t step_ = 0;
};
#endif // STOVEGUARD_FAKEFRAMEANALYZER_H
