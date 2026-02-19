#include <gtest/gtest.h>

#include "Detection.h"
#include "DetectionMapper.h"

TEST(DetectionMapperTest, Given_FlameDetect_And_No_Person_Detect_When_Convert_To_Domain_Then_Returns_Correctly) {
    constexpr Detection detection{true, false};

    const auto [stoveState, personState] = toDomain(detection);

    EXPECT_EQ(stoveState, StoveState::On);
    EXPECT_EQ(personState, PersonState::Absent);
}

TEST(DetectionMapperTest, Given_FlameDetect_And_Person_Detect_When_Convert_To_Domain_Then_Returns_Correctly) {
    constexpr Detection detection{true, true};

    const auto [stoveState, personState] = toDomain(detection);

    EXPECT_EQ(stoveState, StoveState::On);
    EXPECT_EQ(personState, PersonState::Present);
}

TEST(DetectionMapperTest, Given_FlameNotDetect_And_Person_Detect_When_Convert_To_Domain_Then_Returns_Correctly) {
    constexpr Detection detection{false, true};

    const auto [stoveState, personState] = toDomain(detection);

    EXPECT_EQ(stoveState, StoveState::Off);
    EXPECT_EQ(personState, PersonState::Present);
}