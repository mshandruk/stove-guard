#include <gtest/gtest.h>

#include "ObjectDetection.h"
#include "SceneMapper.h"
#include "SceneState.h"
#include "StoveMonitor.h"

TEST(DetectionMapperTest, MapsToOnAndAbsent_WhenOnlyStoveDetected) {
    const ObjectDetections objectDetections = {
        ObjectDetection{LabelClassification::Stove, 0.0, {}},
    };
    constexpr SceneState expected{StoveState::On, PersonState::Absent};

    const auto result = SceneMapper::map(objectDetections);

    EXPECT_EQ(result, expected);
}

TEST(DetectionMapperTest, MapsToOnAndPresent_WhenStoveAndPersonDetected) {
    const ObjectDetections objectDetections = {
        ObjectDetection{LabelClassification::Stove, 0.0, {}},
        ObjectDetection{LabelClassification::Person, 0.0, {}},
    };

    const auto result = SceneMapper::map(objectDetections);
    constexpr SceneState expected{StoveState::On, PersonState::Present};

    EXPECT_EQ(result, expected);
}

TEST(DetectionMapperTest, MapsToOffAndPresent_WhenOnlyPersonDetected) {
    const ObjectDetections objectDetections = {
        ObjectDetection{LabelClassification::Person, 0.0, {}},
    };

    const auto result = SceneMapper::map(objectDetections);
    constexpr SceneState expected{StoveState::Off, PersonState::Present};

    EXPECT_EQ(result, expected);
}

TEST(DetectionMapperTest, MapsToOffAndAbsent_WhenNothingDetected) {
    const ObjectDetections objectDetections = {};

    const auto result = SceneMapper::map(objectDetections);
    constexpr SceneState expected{StoveState::Off, PersonState::Absent};

    EXPECT_EQ(result, expected);
}