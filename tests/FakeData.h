#pragma once

#include "ObjectDetection.h"
#include "SceneMapper.h"

namespace FakeData {
constexpr SceneMapper::OverlapThresholds OVERLAP_THRESHOLDS{
    0.5F,
    0.5F,
    0.5F,
};

constexpr float EPSILON = 0.01F;

constexpr float belowThreshold(const float threshold) {
    return threshold - EPSILON;
}

constexpr float aboveThreshold(const float threshold) {
    return threshold + EPSILON;
}

constexpr float CONFIDENCE = 0.8F;

namespace Shape {
constexpr BoundingBox Stove{0, 0, 100, 100};
constexpr BoundingBox FireDefault{0, 0, 10, 10};
constexpr BoundingBox PotDefault{0, 0, 20, 20};
constexpr BoundingBox Person{500, 500, 100, 100};
} // namespace Shape

namespace Geometry {

inline BoundingBox placeBoxForOverlapX(const BoundingBox base, const BoundingBox reference, const float overlapRatio) {
    const float overlapSide = base.width * overlapRatio;
    return BoundingBox{reference.right() - overlapSide, base.y, base.width, base.height};
}

inline BoundingBox placeBoxWithoutOverlapX(const BoundingBox base, const BoundingBox reference) {
    return BoundingBox{
        reference.right() + 1.0F,
        base.y,
        base.width,
        base.height,
    };
}

inline BoundingBox
placeContainerToCoverTargetX(const BoundingBox target, const BoundingBox container, const float overlapRatio) {
    // Фиксированный target = fire, двигаем container=pot, то есть вычисляем положение container относительно target
    // fire inside pot
    const auto containerNotOverlap = container.width - (target.width * overlapRatio);
    return BoundingBox{target.left() - containerNotOverlap, container.y, container.width, container.height};
}

inline BoundingBox placeBoxWithoutOverlapY(const BoundingBox base, const BoundingBox reference) {
    return BoundingBox{
        base.x,
        reference.bottom() + 1.0F,
        base.width,
        base.height,
    };
}

} // namespace Geometry

namespace Factory {

inline ObjectDetection moveDetectionOutsideOf(const ObjectDetection& base, const ObjectDetection& reference) {
    const auto bbox = Geometry::placeBoxWithoutOverlapX(base.box, reference.box);
    return ObjectDetection{base.classification, base.confidence, bbox};
}

inline ObjectDetection person(const BoundingBox bbox, const Confidence confidence = Confidence{CONFIDENCE}) {
    return ObjectDetection{
        LabelClassification::Person,
        confidence,
        bbox,
    };
}

inline ObjectDetection stove(const BoundingBox bbox, const Confidence confidence = Confidence{CONFIDENCE}) {
    return ObjectDetection{
        LabelClassification::Stove,
        confidence,
        bbox,
    };
}

inline ObjectDetection fire(const BoundingBox bbox, const Confidence confidence = Confidence{CONFIDENCE}) {
    return ObjectDetection{
        LabelClassification::Fire,
        confidence,
        bbox,
    };
}

inline ObjectDetection pot(const BoundingBox bbox, const Confidence confidence = Confidence{CONFIDENCE}) {
    return ObjectDetection{
        LabelClassification::Pot,
        confidence,
        bbox,
    };
}

inline ObjectDetection makePerson() {
    return person(Shape::Person);
}

inline ObjectDetection makeFire() {
    return fire(Shape::FireDefault);
}

inline ObjectDetection makePot() {
    return pot(Shape::PotDefault);
}

inline ObjectDetection makeStove() {
    return stove(Shape::Stove);
}

inline ObjectDetection makeFireOnStove(const ObjectDetection& stove) {
    const auto fireOnStoveBox =
        Geometry::placeBoxForOverlapX(Shape::FireDefault, stove.box, OVERLAP_THRESHOLDS.fireOnStove);
    return fire(fireOnStoveBox);
}

inline ObjectDetection makePotOnStove(const ObjectDetection& stove) {
    const auto potOnStove = Geometry::placeBoxForOverlapX(Shape::PotDefault, stove.box, OVERLAP_THRESHOLDS.potOnStove);
    return pot(potOnStove);
}

inline ObjectDetection makePotCoveringFire(const ObjectDetection& fire) {
    const auto potOnFire =
        Geometry::placeContainerToCoverTargetX(fire.box, Shape::PotDefault, OVERLAP_THRESHOLDS.fireToPot);
    return pot(potOnFire);
}

inline ObjectDetection makePotOnStoveWithoutFire(const ObjectDetection& stove, const ObjectDetection& fire) {
    const auto potOnStove = Geometry::placeBoxForOverlapX(Shape::PotDefault, stove.box, OVERLAP_THRESHOLDS.potOnStove);
    const auto potWithoutFire = Geometry::placeBoxWithoutOverlapY(potOnStove, fire.box);
    return pot(potWithoutFire);
}

} // namespace Factory
}; // namespace FakeData
