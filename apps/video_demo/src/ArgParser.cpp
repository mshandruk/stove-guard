#include "ArgParser.h"

#include <cstddef>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

#include "AppOptions.h"

namespace StoveGuard::Cli {
AppOptions toAppOptions(const CmdOptions& options) {
    if (options.video.empty()) {
        throw ParseError("Missing required argument: --video");
    }

    if (options.analyzer.empty()) {
        throw ParseError("Missing required argument: --analyzer");
    }

    const auto analyzer = stringToAnalyzer(options.analyzer);

    if (!analyzer.has_value()) {
        throw ParseError("Unknown analyzer type: " + std::string(options.analyzer));
    }

    if (analyzer == Analyzer::Yolo && options.model.empty()) {
        throw ParseError("Argument --model is required when --analyzer is 'yolo'");
    }

    return AppOptions{
        options.video,
        analyzer.value(),
        options.model,
    };
}

ArgParser::ArgParser(const int argc, char** argv) {
    if (argc > 1) {
        args_.reserve(static_cast<std::size_t>(argc - 1));
    }

    for (int i = 1; i < argc; ++i) {
        args_.emplace_back(argv[i]); // NOLINT
    }
}

void ArgParser::usage(const std::string_view progName) {
    std::cout << progName << " --video <path> --analyzer fake|yolo [--model <path>]" << '\n';
}

CmdOptions ArgParser::parse() const {
    try {
        return collectCmdOptions();
    } catch (const std::exception& e) {
        throw ParseError(e.what());
    }
}

CmdOptions ArgParser::collectCmdOptions() const {
    CmdOptions options;
    for (std::size_t i = 0; i < args_.size(); ++i) {
        if (const auto argument = args_[i]; /*NOLINT*/ argument == "--video") {
            options.video = getNextValue(i);
            ++i;
        } else if (argument == "--analyzer") {
            options.analyzer = getNextValue(i);
            ++i;
        } else if (argument == "--model") {
            options.model = getNextValue(i);
            ++i;
        } else {
            throw std::invalid_argument("Unknown argument: " + std::string(argument));
        }
    }
    return options;
}

std::string_view ArgParser::getNextValue(const std::size_t index) const {
    if (index + 1 >= args_.size()) {
        throw std::runtime_error("Missing value for argument");
    }

    return args_[index + 1]; // NOLINT
}

} // namespace StoveGuard::Cli