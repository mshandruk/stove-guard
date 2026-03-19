#pragma once

#include <cstdint>
#include <stdexcept>
#include <string_view>
#include <vector>

struct BoundingBox {
    float x;
    float y;
    float width;
    float height;
};

enum class LabelClassification : uint8_t {
    Unknown,
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

class Confidence {
  public:
    explicit Confidence(const float value)
            : value_(value) {
        if (value < 0.0F || value > 1.0F) {
            throw std::invalid_argument("Confidence value must be in between [0, 1]");
        }
    }

    [[nodiscard]] float value() const noexcept {
        return value_;
    }

  private:
    float value_;
};

struct ObjectDetection {
    LabelClassification classification = LabelClassification::Unknown;
    Confidence confidence = Confidence{0.0F};
    BoundingBox box{};
};

using ObjectDetections = std::vector<ObjectDetection>;
