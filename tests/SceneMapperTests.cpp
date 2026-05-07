#include <gtest/gtest.h>

#include "FakeData.h"
#include "ObjectDetection.h"
#include "SceneMapper.h"
#include "SceneState.h"
#include "StoveMonitor.h"

// Ничего не найдено
TEST(SceneMapperTest, MapsToDefault_WhenDetectionsEmpty) {
    const ObjectDetections detections;

    constexpr auto expected = SceneState{StoveState::Off, PersonState::Absent, false, false};

    const auto result = SceneMapper::map(detections);

    EXPECT_EQ(result, expected);
}

// Только человек
TEST(SceneMapperTest, MapsToPersonPresent_WhenOnlyPersonDetected) {
    const ObjectDetections detections = {
        FakeData::Factory::person(),
    };

    constexpr auto expected = SceneState{StoveState::Off, PersonState::Present, false, false};

    const auto result = SceneMapper::map(detections);

    EXPECT_EQ(result, expected);
}

// Только огонь
TEST(SceneMapperTest, MapsToStoveOff_WhenFireWithoutStove) {
    const ObjectDetections detections = {
        FakeData::Factory::fireAlone(),
    };

    constexpr auto expected = SceneState{StoveState::Off, PersonState::Absent, false, false};

    const auto result = SceneMapper::map(detections);

    EXPECT_EQ(result, expected);
}

// Только посуда
TEST(SceneMapperTest, MapsToStoveWithoutPot_WhenPotWithoutStove) {
    const ObjectDetections detections = {
        FakeData::Factory::potAlone(),
    };

    constexpr auto expected = SceneState{StoveState::Off, PersonState::Absent, false, false};

    const auto result = SceneMapper::map(detections);

    EXPECT_EQ(result, expected);
}

// Плита включена без посуды
TEST(SceneMapperTest, MapsToStoveOn_WhenFireOverlapsStove) {
    const ObjectDetections detections = {
        FakeData::Factory::stove(),
        FakeData::Factory::fireOnStove(),
    };

    constexpr auto expected = SceneState{StoveState::On, PersonState::Absent, false, false};

    const auto result = SceneMapper::map(detections);

    EXPECT_EQ(result, expected);
}

// Плита включена посуда на плите, но не на огне
TEST(SceneMapperTest, MapsToPotOnStove_WhenPotOverlapsStoveButNotFire) {
    const ObjectDetections detections = {
        FakeData::Factory::stove(),
        FakeData::Factory::fireOnStove(),
        FakeData::Factory::potOnStove(),
    };

    constexpr auto expected = SceneState{StoveState::On, PersonState::Absent, true, false};

    const auto result = SceneMapper::map(detections);

    EXPECT_EQ(result, expected);
}

// Плита включена посуда на огне.
TEST(SceneMapperTest, MapsToPotOnFire_WhenPotAssociateWithFireOnStove) {
    const ObjectDetections detections = {
        FakeData::Factory::stove(),
        FakeData::Factory::fireOnStove(),
        FakeData::Factory::potOnFire(),
    };

    constexpr auto expected = SceneState{StoveState::On, PersonState::Absent, true, true};

    const auto result = SceneMapper::map(detections);

    EXPECT_EQ(result, expected);
}

// Плита выключена посуда на плите
TEST(SceneMapperTest, MapsToPotOnStove_WhenPotAssociatedWithStove) {
    const ObjectDetections detections = {
        FakeData::Factory::stove(),
        FakeData::Factory::potOnStove(),
    };

    constexpr auto expected = SceneState{StoveState::Off, PersonState::Absent, true, false};

    const auto result = SceneMapper::map(detections);

    EXPECT_EQ(result, expected);
}

// Две плиты: Плита + посуда на первой плите + огонь включен на другой плите без посуды
TEST(SceneMapperTest, MapsToStoveOnAndPotOnStove_WhenPotOnOneStoveAndFireOnAnother) {
    const auto stove1 = FakeData::Factory::stove();
    const auto potOnStove1 = FakeData::Factory::potOnStove();

    const auto stove2 = FakeData::Factory::shift(stove1, 200, 0);
    const auto fireOnStove2 = FakeData::Factory::shift(FakeData::Factory::fireOnStove(), 200, 0);

    const ObjectDetections detections = {
        stove1,
        potOnStove1,
        stove2,
        fireOnStove2,
    };

    constexpr auto expected = SceneState{StoveState::On, PersonState::Absent, true, false};

    const auto result = SceneMapper::map(detections);

    EXPECT_EQ(result, expected);
}

// Две плиты: Плита с огнем, плита без огня
TEST(SceneMapperTest, MapsToStoveOn_WhenSeveralStovesAndOnlyOneOfWithFire) {
    // Плита1 и огонь
    const auto stove1 = FakeData::Factory::stove();
    const auto fireOnStove1 = FakeData::Factory::fireOnStove();

    // Плита2 без огня
    const auto stove2 = FakeData::Factory::shift(FakeData::Factory::stove(), 200, 0);

    const ObjectDetections detections = {
        stove1,
        stove2,
        fireOnStove1,
    };

    constexpr auto expected = SceneState{StoveState::On, PersonState::Absent, false, false};

    const auto result = SceneMapper::map(detections);

    EXPECT_EQ(result, expected);
}

// Плита включена посуда рядом
TEST(SceneMapperTest, MapsToStoveOnWithoutPot_WhenPotNotOverlappingStoveOrFire) {
    const ObjectDetections detections = {
        FakeData::Factory::stove(),
        FakeData::Factory::fireOnStove(),
        FakeData::Factory::potAlone(),
    };
    constexpr auto expected = SceneState{StoveState::On, PersonState::Absent, false, false};

    const auto result = SceneMapper::map(detections);

    EXPECT_EQ(result, expected);
}

// Плита включена + посуда на огне + человек есть
TEST(SceneMapperTest, MapsToStoveOnAndPotOnFireAndPerson_WhenStoveOnAndPotOnFireAndPersonPresent) {
    const ObjectDetections detections = {
        FakeData::Factory::stove(),
        FakeData::Factory::fireOnStove(),
        FakeData::Factory::potOnFire(),
        FakeData::Factory::person(),
    };
    constexpr auto expected = SceneState{StoveState::On, PersonState::Present, true, true};

    const auto result = SceneMapper::map(detections);

    EXPECT_EQ(result, expected);
}

// Плита + огонь + посуда на огне + человека нет
TEST(SceneMapperTest, MapsToPotOnFire_WhenPotOverlapsFireOnStove) {
    const ObjectDetections detections = {
        FakeData::Factory::stove(),
        FakeData::Factory::fireOnStove(),
        FakeData::Factory::potOnFire(),
    };
    constexpr auto expected = SceneState{StoveState::On, PersonState::Absent, true, true};

    const auto result = SceneMapper::map(detections);

    EXPECT_EQ(result, expected);
}
