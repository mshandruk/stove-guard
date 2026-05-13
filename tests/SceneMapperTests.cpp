#include <gtest/gtest.h>

#include "FakeData.h"
#include "ObjectDetection.h"
#include "SceneMapper.h"
#include "SceneState.h"
#include "StoveMonitor.h"

// Basic scenarios
TEST(SceneMapperTest, MapsToDefault_WhenDetectionsEmpty) {
    const ObjectDetections detections;

    constexpr auto expected = SceneState{StoveState::Off, PersonState::Absent, false, false};

    const auto result = SceneMapper::map(detections, FakeData::OVERLAP_THRESHOLDS);

    EXPECT_EQ(result, expected);
}

TEST(SceneMapperTest, MapsToPersonPresent_WhenOnlyPersonDetected) {
    const ObjectDetections detections = {
        FakeData::Factory::makePerson(),
    };

    constexpr auto expected = SceneState{StoveState::Off, PersonState::Present, false, false};

    const auto result = SceneMapper::map(detections, FakeData::OVERLAP_THRESHOLDS);

    EXPECT_EQ(result, expected);
}

TEST(SceneMapperTest, MapsToStoveOff_WhenFireWithoutStove) {
    const ObjectDetections detections = {
        FakeData::Factory::makeFire(),
    };

    constexpr auto expected = SceneState{StoveState::Off, PersonState::Absent, false, false};

    const auto result = SceneMapper::map(detections, FakeData::OVERLAP_THRESHOLDS);

    EXPECT_EQ(result, expected);
}

TEST(SceneMapperTest, MapsToStoveWithoutPot_WhenPotWithoutStove) {
    const ObjectDetections detections = {
        FakeData::Factory::makePot(),
    };

    constexpr auto expected = SceneState{StoveState::Off, PersonState::Absent, false, false};

    const auto result = SceneMapper::map(detections, FakeData::OVERLAP_THRESHOLDS);

    EXPECT_EQ(result, expected);
}

// Multi-object scenarios
TEST(SceneMapperTest, MapsToStoveOn_WhenFireOverlapsStove) {

    const auto stove = FakeData::Factory::makeStove();
    const auto fireOnStove = FakeData::Factory::makeFireOnStove(stove);

    const ObjectDetections detections = {
        stove,
        fireOnStove,
    };

    constexpr auto expected = SceneState{StoveState::On, PersonState::Absent, false, false};

    const auto result = SceneMapper::map(detections, FakeData::OVERLAP_THRESHOLDS);

    EXPECT_EQ(result, expected);
}

TEST(SceneMapperTest, MapsToPotOnStove_WhenPotOverlapsStoveButNotFire) {
    const auto stove = FakeData::Factory::makeStove();
    const auto fireOnStove = FakeData::Factory::makeFireOnStove(stove);
    const auto potOnStove = FakeData::Factory::makePotOnStoveWithoutFire(stove, fireOnStove);
    const ObjectDetections detections = {
        stove,
        fireOnStove,
        potOnStove,
    };

    constexpr auto expected = SceneState{StoveState::On, PersonState::Absent, true, false};

    const auto result = SceneMapper::map(detections, FakeData::OVERLAP_THRESHOLDS);

    EXPECT_EQ(result, expected);
}

TEST(SceneMapperTest, MapsToPotOnStove_WhenPotAssociatedWithStove) {
    const auto stove = FakeData::Factory::makeStove();
    const auto potOnStove = FakeData::Factory::makePotOnStove(stove);
    const ObjectDetections detections = {
        stove,
        potOnStove,
    };

    constexpr auto expected = SceneState{StoveState::Off, PersonState::Absent, true, false};

    const auto result = SceneMapper::map(detections, FakeData::OVERLAP_THRESHOLDS);

    EXPECT_EQ(result, expected);
}

TEST(SceneMapperTest, MapsToStoveOnAndPotOnStove_WhenPotOnOneStoveAndFireOnAnother) {

    const auto stove1 = FakeData::Factory::makeStove();
    const auto potOnStove1 = FakeData::Factory::makePotOnStove(stove1);

    const auto stove2 = FakeData::Factory::moveDetectionOutsideOf(FakeData::Factory::makeStove(), stove1);
    const auto fireOnStove2 = FakeData::Factory::makeFireOnStove(stove2);

    const ObjectDetections detections = {
        stove1,
        fireOnStove2,
        stove2,
        potOnStove1,

    };

    constexpr auto expected = SceneState{StoveState::On, PersonState::Absent, true, false};

    const auto result = SceneMapper::map(detections, FakeData::OVERLAP_THRESHOLDS);

    EXPECT_EQ(result, expected);
}

// Inverse Case: fire overlaps pot but fire not on stove
TEST(SceneMapperTest, DoesNotMapStoveOn_WhenFireOverlapsPotWithoutStove) {
    const auto fire = FakeData::Factory::makeFire();
    const auto potOnFire = FakeData::Factory::makePotCoveringFire(fire);

    const ObjectDetections detections = {
        fire,
        potOnFire,

    };
    constexpr auto expected = SceneState{StoveState::Off, PersonState::Absent, false, false};

    const auto result = SceneMapper::map(detections, FakeData::OVERLAP_THRESHOLDS);

    EXPECT_EQ(result, expected);
}

TEST(SceneMapperTest, MapsToStoveOn_WhenSeveralStovesAndOnlyOneOfWithFire) {
    const auto stove1 = FakeData::Factory::makeStove();
    const auto fireOnStove1 = FakeData::Factory::makeFireOnStove(stove1);
    const auto stove2 = FakeData::Factory::moveDetectionOutsideOf(FakeData::Factory::makeStove(), stove1);

    const ObjectDetections detections = {
        stove1,
        fireOnStove1,
        stove2,
    };

    constexpr auto expected = SceneState{StoveState::On, PersonState::Absent, false, false};

    const auto result = SceneMapper::map(detections, FakeData::OVERLAP_THRESHOLDS);

    EXPECT_EQ(result, expected);
}

TEST(SceneMapperTest, MapsToStoveOnWithoutPot_WhenPotNotOverlappingStoveOrFire) {
    const auto stove = FakeData::Factory::makeStove();
    const auto fireOnStove = FakeData::Factory::makeFireOnStove(stove);
    const auto pot = FakeData::Factory::makePot();
    const auto potWithoutStove = FakeData::Factory::moveDetectionOutsideOf(pot, stove);

    const ObjectDetections detections = {
        stove,
        fireOnStove,
        potWithoutStove,
    };
    constexpr auto expected = SceneState{StoveState::On, PersonState::Absent, false, false};

    const auto result = SceneMapper::map(detections, FakeData::OVERLAP_THRESHOLDS);

    EXPECT_EQ(result, expected);
}

TEST(SceneMapperTest, MapsToStoveOnAndPotOnFireAndPerson_WhenStoveOnAndPotOnFireAndPersonPresent) {
    const auto stove = FakeData::Factory::makeStove();
    const auto fireOnStove = FakeData::Factory::makeFireOnStove(stove);
    const auto potOnStove = FakeData::Factory::makePotCoveringFire(fireOnStove);
    const auto person = FakeData::Factory::makePerson();

    const ObjectDetections detections = {
        stove,
        fireOnStove,
        potOnStove,
        person,
    };

    constexpr auto expected = SceneState{StoveState::On, PersonState::Present, true, true};

    const auto result = SceneMapper::map(detections, FakeData::OVERLAP_THRESHOLDS);

    EXPECT_EQ(result, expected);
}

TEST(SceneMapperTest, MapsToPotOnFirePersonAbsent_WhenPotOverlapsFireOnStoveAndPersonAbsent) {
    const auto stove = FakeData::Factory::makeStove();
    const auto fireOnStove = FakeData::Factory::makeFireOnStove(stove);
    const auto potOnStove = FakeData::Factory::makePotCoveringFire(fireOnStove);

    const ObjectDetections detections = {
        stove,
        fireOnStove,
        potOnStove,
    };

    constexpr auto expected = SceneState{StoveState::On, PersonState::Absent, true, true};

    const auto result = SceneMapper::map(detections, FakeData::OVERLAP_THRESHOLDS);

    EXPECT_EQ(result, expected);
}

// Threshold stability tests
TEST(SceneMapperTest, MapsToStoveOn_WhenFireOverlapsStoveAboveThreshold) {
    const auto fireOnStoveBox = FakeData::Geometry::placeBoxForOverlapX(
        FakeData::Shape::FireDefault,
        FakeData::Shape::Stove,
        FakeData::aboveThreshold(FakeData::OVERLAP_THRESHOLDS.fireOnStove));

    const auto detections = ObjectDetections{
        FakeData::Factory::stove(FakeData::Shape::Stove),
        FakeData::Factory::fire(fireOnStoveBox),
    };

    constexpr auto expected = SceneState{StoveState::On, PersonState::Absent, false, false};

    const auto result = SceneMapper::map(detections, FakeData::OVERLAP_THRESHOLDS);

    EXPECT_EQ(result, expected);
}

TEST(SceneMapperTest, MapsToStoveOn_WhenFireOverlapsStoveEqualsThreshold) {
    const auto fireOnStoveBox = FakeData::Geometry::placeBoxForOverlapX(
        FakeData::Shape::FireDefault, FakeData::Shape::Stove, FakeData::OVERLAP_THRESHOLDS.fireOnStove);

    const auto detections = ObjectDetections{
        FakeData::Factory::stove(FakeData::Shape::Stove),
        FakeData::Factory::fire(fireOnStoveBox),
    };

    constexpr auto expected = SceneState{StoveState::On, PersonState::Absent, false, false};

    const auto result = SceneMapper::map(detections, FakeData::OVERLAP_THRESHOLDS);

    EXPECT_EQ(result, expected);
}

TEST(SceneMapperTest, DoesNotMapToStoveOn_WhenFireOverlapsStoveBelowThreshold) {
    const auto fireWithoutStoveBox = FakeData::Geometry::placeBoxForOverlapX(
        FakeData::Shape::FireDefault,
        FakeData::Shape::Stove,
        FakeData::belowThreshold(FakeData::OVERLAP_THRESHOLDS.fireOnStove));

    const auto detections = ObjectDetections{
        FakeData::Factory::stove(FakeData::Shape::Stove),
        FakeData::Factory::fire(fireWithoutStoveBox),
    };

    constexpr auto expected = SceneState{StoveState::Off, PersonState::Absent, false, false};

    const auto result = SceneMapper::map(detections, FakeData::OVERLAP_THRESHOLDS);

    EXPECT_EQ(result, expected);
}

TEST(SceneMapperTest, MapsToPotOnStove_WhenPotOverlapsStoveAboveThreshold) {
    constexpr auto stoveBox = FakeData::Shape::Stove;
    constexpr auto potBox = FakeData::Shape::PotDefault;

    const auto potOnStoveBox = FakeData::Geometry::
        placeBoxForOverlapX(potBox, stoveBox, FakeData::aboveThreshold(FakeData::OVERLAP_THRESHOLDS.potOnStove));

    const auto detections = ObjectDetections{
        FakeData::Factory::stove(stoveBox),
        FakeData::Factory::pot(potOnStoveBox),
    };

    constexpr auto expected = SceneState{StoveState::Off, PersonState::Absent, true, false};

    const auto result = SceneMapper::map(detections, FakeData::OVERLAP_THRESHOLDS);

    EXPECT_EQ(result, expected);
}

TEST(SceneMapperTest, MapsToPotOnStove_WhenPotOverlapsStoveEqualsThreshold) {
    constexpr auto stoveBox = FakeData::Shape::Stove;
    constexpr auto potBox = FakeData::Shape::PotDefault;

    const auto potOnStoveBox =
        FakeData::Geometry::placeBoxForOverlapX(potBox, stoveBox, FakeData::OVERLAP_THRESHOLDS.potOnStove);

    const auto detections = ObjectDetections{
        FakeData::Factory::stove(stoveBox),
        FakeData::Factory::pot(potOnStoveBox),
    };

    constexpr auto expected = SceneState{StoveState::Off, PersonState::Absent, true, false};

    const auto result = SceneMapper::map(detections, FakeData::OVERLAP_THRESHOLDS);

    EXPECT_EQ(result, expected);
}

TEST(SceneMapperTest, DoesNotMapToPotOnStove_WhenPotOverlapsBelowThreshold) {
    constexpr auto stoveBox = FakeData::Shape::Stove;
    constexpr auto potBox = FakeData::Shape::PotDefault;

    const auto potWithoutStoveBox = FakeData::Geometry::
        placeBoxForOverlapX(potBox, stoveBox, FakeData::belowThreshold(FakeData::OVERLAP_THRESHOLDS.potOnStove));

    const auto detections = ObjectDetections{
        FakeData::Factory::stove(stoveBox),
        FakeData::Factory::pot(potWithoutStoveBox),
    };

    constexpr auto expected = SceneState{StoveState::Off, PersonState::Absent, false, false};

    const auto result = SceneMapper::map(detections, FakeData::OVERLAP_THRESHOLDS);

    EXPECT_EQ(result, expected);
}

TEST(SceneMapperTest, MapsToPotOnFire_WhenFireOverlapsPotAboveThreshold) {
    constexpr auto stoveBox = FakeData::Shape::Stove;
    constexpr auto potBox = FakeData::Shape::PotDefault;
    constexpr auto fireBox = FakeData::Shape::FireDefault;

    const auto fireOnStoveBox = FakeData::Geometry::
        placeBoxForOverlapX(fireBox, stoveBox, FakeData::aboveThreshold(FakeData::OVERLAP_THRESHOLDS.fireOnStove));

    const auto potOnFireBox = FakeData::Geometry::
        placeContainerToCoverTargetX(fireOnStoveBox, potBox, FakeData::OVERLAP_THRESHOLDS.fireToPot);

    constexpr auto expected = SceneState{StoveState::On, PersonState::Absent, true, true};

    const auto detections = ObjectDetections{
        FakeData::Factory::stove(stoveBox),
        FakeData::Factory::fire(fireOnStoveBox),
        FakeData::Factory::pot(potOnFireBox),
    };

    const auto result = SceneMapper::map(detections, FakeData::OVERLAP_THRESHOLDS);

    EXPECT_EQ(result, expected);
}

TEST(SceneMapperTest, MapsToPotOnFire_WhenFireOverlapsPotEqualThreshold) {
    constexpr auto stoveBox = FakeData::Shape::Stove;
    constexpr auto potBox = FakeData::Shape::PotDefault;
    constexpr auto fireBox = FakeData::Shape::FireDefault;

    // Включили плиту
    const auto fireOnStoveBox =
        FakeData::Geometry::placeBoxForOverlapX(fireBox, stoveBox, FakeData::OVERLAP_THRESHOLDS.fireOnStove);

    const auto potOnFireBox = FakeData::Geometry::
        placeContainerToCoverTargetX(fireOnStoveBox, potBox, FakeData::OVERLAP_THRESHOLDS.fireToPot);

    constexpr auto expected = SceneState{StoveState::On, PersonState::Absent, true, true};

    const auto detections = ObjectDetections{
        FakeData::Factory::stove(stoveBox),
        FakeData::Factory::fire(fireOnStoveBox),
        FakeData::Factory::pot(potOnFireBox),
    };

    const auto result = SceneMapper::map(detections, FakeData::OVERLAP_THRESHOLDS);

    EXPECT_EQ(result, expected);
}

TEST(SceneMapperTest, DoesNotMapToPotOnFire_WhenFireOverlapsPotBelowThreshold) {

    constexpr auto stove = FakeData::Shape::Stove;
    constexpr auto pot = FakeData::Shape::PotDefault;
    constexpr auto fire = FakeData::Shape::FireDefault;

    constexpr auto bellowThreshold = FakeData::belowThreshold(FakeData::OVERLAP_THRESHOLDS.fireToPot);

    // Включили плиту
    const auto fireOnStove =
        FakeData::Geometry::placeBoxForOverlapX(fire, stove, FakeData::OVERLAP_THRESHOLDS.fireOnStove);

    const auto potOnFire = FakeData::Geometry::placeContainerToCoverTargetX(fireOnStove, pot, bellowThreshold);

    constexpr auto expected = SceneState{StoveState::On, PersonState::Absent, true, false};

    const auto detections = ObjectDetections{
        FakeData::Factory::stove(stove),
        FakeData::Factory::fire(fireOnStove),
        FakeData::Factory::pot(potOnFire),
    };

    const auto result = SceneMapper::map(detections, FakeData::OVERLAP_THRESHOLDS);

    EXPECT_EQ(result, expected);
}