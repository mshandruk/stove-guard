#include "StateStabilizer.h"
#include "StoveMonitor.h"

#include <stdexcept>

#include <gtest/gtest.h>

TEST(StateStabilizerTest, StableState_Changes_AfterThreshold) {
    StateStabilizer stateStabilizer(PersonState::Absent, 3);

    ASSERT_EQ(stateStabilizer.updateState(PersonState::Present), PersonState::Absent);
    ASSERT_EQ(stateStabilizer.updateState(PersonState::Present), PersonState::Absent);
    EXPECT_EQ(stateStabilizer.updateState(PersonState::Present), PersonState::Present);
}

TEST(StateStabilizerTest, StableState_NotChanges_WhenBelowThreshold) {
    StateStabilizer stateStabilizer(PersonState::Absent, 3);

    ASSERT_EQ(stateStabilizer.updateState(PersonState::Present), PersonState::Absent);
    ASSERT_EQ(stateStabilizer.updateState(PersonState::Absent), PersonState::Absent);
    EXPECT_EQ(stateStabilizer.updateState(PersonState::Present), PersonState::Absent);
}

TEST(StateStabilizerTest, StableState_AfterContinuesIdenticalStates) {
    StateStabilizer stabilizer(PersonState::Absent, 3);

    ASSERT_EQ(stabilizer.updateState(PersonState::Present), PersonState::Absent);
    ASSERT_EQ(stabilizer.updateState(PersonState::Absent), PersonState::Absent);
    ASSERT_EQ(stabilizer.updateState(PersonState::Present), PersonState::Absent);
    ASSERT_EQ(stabilizer.updateState(PersonState::Present), PersonState::Absent);
    ASSERT_EQ(stabilizer.updateState(PersonState::Present), PersonState::Present);
}

TEST(StateStabilizerTest, ConstructorThrows_WhenThresholdIsZero) {

    EXPECT_THROW(StateStabilizer(PersonState::Absent, 0), std::invalid_argument);
}

TEST(StateStabilizerTest, ThresholdOne_ChangesImmediately) {
    StateStabilizer stateStabilizer{PersonState::Absent, 1};
    EXPECT_EQ(stateStabilizer.updateState(PersonState::Present), PersonState::Present);
}