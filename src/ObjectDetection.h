#pragma once

#include <cstdint>
#include <stdexcept>
#include <string_view>
#include <vector>

struct BoundingBox {
    float x{0.0F};      // NOLINT
    float y{0.0F};      // NOLINT
    float width{0.0F};  // NOLINT
    float height{0.0F}; // NOLINT

    [[nodiscard]] float left() const noexcept {
        return x;
    }

    [[nodiscard]] float right() const noexcept {
        return x + width;
    }

    [[nodiscard]] float top() const noexcept {
        return y;
    }

    [[nodiscard]] float bottom() const noexcept {
        return y + height;
    }
};

enum class LabelClassification : uint8_t {
    Unknown,
    Person,
    Stove,
    Fire,
    Pot,
};

inline std::string_view labelToString(const LabelClassification& label) {
    switch (label) {
    case LabelClassification::Person: {
        return "person";
    }
    case LabelClassification::Stove: {
        return "stove";
    }
    case LabelClassification::Fire: {
        return "fire";
    }
    case LabelClassification::Pot: {
        return "pot";
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
