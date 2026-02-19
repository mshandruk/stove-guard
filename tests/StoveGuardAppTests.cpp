#include <gtest/gtest.h>

#include "FrameAnalyzer.h"
#include "StoveGuardApp.h"

class FakeFrameAnalyzer : public FrameAnalyzer {
  public:
    Detection nextDetection{false, false};

    Detection analyze([[maybe_unused]] const Frame& frame) override {
        return nextDetection;
    }
};

TEST(StoveGuardAppTest, DangerousEntered_WhenStoveOnAndNoPerson) {
    FakeFrameAnalyzer fakeFrameAnalyzer;
    fakeFrameAnalyzer.nextDetection = {true, false};
    StoveGuardApp stoveGuardApp{fakeFrameAnalyzer};

    const auto event = stoveGuardApp.run();

    EXPECT_EQ(event, Event::DangerousEntered);
}

TEST(StoveGuardAppTest, ReturnsNone_WhenStoveOnAndPersonPresent) {
    FakeFrameAnalyzer fakeFrameAnalyzer;
    fakeFrameAnalyzer.nextDetection = {true, true};
    StoveGuardApp stoveGuardApp{fakeFrameAnalyzer};

    const auto event = stoveGuardApp.run();

    EXPECT_EQ(event, Event::None);
}

TEST(StoveGuardAppTest, ReturnsNone_WhenNoStoveOffAndNoPerson) {
    FakeFrameAnalyzer fakeFrameAnalyzer;
    fakeFrameAnalyzer.nextDetection = {false, false};
    StoveGuardApp stoveGuardApp{fakeFrameAnalyzer};

    const auto event = stoveGuardApp.run();

    EXPECT_EQ(event, Event::None);
}