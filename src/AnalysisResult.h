#ifndef STOVEGUARD_ANALYSISRESULT_H
#define STOVEGUARD_ANALYSISRESULT_H
#include <cstdint>
#include <string_view>
#include <vector>

#include "Detection.h"

struct BoundingBox {
    float x;
    float y;
    float width;
    float height;
};

enum class LabelClassification : uint8_t {
    Person,
    Stove,
};

inline std::string_view labelToString(const LabelClassification& label) {
    switch (label) {
    case LabelClassification::Person: {
        return "person";
    }
    case LabelClassification::Stove: {
        return "stove";
    }
    default: {
        return "unknown";
    }
    }
}

struct ObjectDetection {
    LabelClassification label;
    float confidence;
    BoundingBox boundingBox;
};

using ObjectDetections = std::vector<ObjectDetection>;

struct AnalyzerResult {
    Detection detection;
    ObjectDetections objectDetections;
};

#endif // STOVEGUARD_ANALYSISRESULT_H
