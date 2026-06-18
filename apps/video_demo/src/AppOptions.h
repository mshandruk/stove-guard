#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string_view>

namespace StoveGuard {
enum class Analyzer : uint8_t {
    Fake,
    Yolo,
};

struct AppOptions {
    std::filesystem::path videoPath;
    Analyzer analyzer;
    std::optional<std::filesystem::path> modelPath;
};

inline std::string_view analyzerToString(const Analyzer analyzer) {
    switch (analyzer) {
    case Analyzer::Fake: {
        return "fake";
    }
    case Analyzer::Yolo: {
        return "yolo";
    }
    }
    return "unknown";
}

inline std::optional<Analyzer> stringToAnalyzer(const std::string_view analyzer) {
    if (analyzer == "fake") {
        return Analyzer::Fake;
    }
    if (analyzer == "yolo") {
        return Analyzer::Yolo;
    }
    return std::nullopt;
}

} // namespace StoveGuard
