#pragma once

#include "ObjectDetection.h"

namespace FakeData {

namespace BBox {
constexpr BoundingBox shift(const BoundingBox& base, const float dX, const float dY) {
    return BoundingBox{
        base.x + dX,
        base.y + dY,
        base.width,
        base.height,
    };
}

constexpr BoundingBox Stove{0, 0, 100, 100};

namespace Fire {
constexpr BoundingBox OnStove{10, 10, 10, 10};

}

namespace Pot {
constexpr BoundingBox OnStove{30, 30, 20, 20};
constexpr BoundingBox OnFire{5, 5, 20, 20};
} // namespace Pot

constexpr BoundingBox FireAlone{300, 300, 15, 15};
constexpr BoundingBox PotAlone{400, 400, 30, 30};
constexpr BoundingBox PersonFar{500, 500, 100, 100};
} // namespace BBox

constexpr float DEFAULT_CONFIDENCE = 0.8F;

namespace Factory {

inline ObjectDetection shift(const ObjectDetection& detection, const float dX, const float dY) {
    return ObjectDetection{
        detection.classification,
        detection.confidence,
        BBox::shift(detection.box, dX, dY),
    };
}

inline ObjectDetection person(const Confidence confidence = Confidence{DEFAULT_CONFIDENCE}) {
    return ObjectDetection{
        LabelClassification::Person,
        confidence,
        BBox::PersonFar,
    };
}

inline ObjectDetection fireOnStove(const Confidence confidence = Confidence{DEFAULT_CONFIDENCE}) {
    return ObjectDetection{
        LabelClassification::Fire,
        confidence,
        BBox::Fire::OnStove,
    };
}

inline ObjectDetection stove(const Confidence confidence = Confidence{DEFAULT_CONFIDENCE}) {
    return ObjectDetection{
        LabelClassification::Stove,
        confidence,
        BBox::Stove,
    };
}

inline ObjectDetection fireAlone(const Confidence confidence = Confidence{DEFAULT_CONFIDENCE}) {
    return ObjectDetection{
        LabelClassification::Fire,
        confidence,
        BBox::FireAlone,
    };
}

inline ObjectDetection potAlone(const Confidence confidence = Confidence{DEFAULT_CONFIDENCE}) {
    return ObjectDetection{
        LabelClassification::Pot,
        confidence,
        BBox::PotAlone,
    };
}

inline ObjectDetection potOnStove(const Confidence confidence = Confidence{DEFAULT_CONFIDENCE}) {
    return ObjectDetection{
        LabelClassification::Pot,
        confidence,
        BBox::Pot::OnStove,
    };
}

inline ObjectDetection potOnFire(const Confidence confidence = Confidence{DEFAULT_CONFIDENCE}) {
    return ObjectDetection{
        LabelClassification::Pot,
        confidence,
        BBox::Pot::OnFire,
    };
}

} // namespace Factory
}; // namespace FakeData
