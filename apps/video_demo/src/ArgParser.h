#pragma once

#include <filesystem>
#include <vector>

#include "AppOptions.h"

namespace StoveGuard::Cli {

class ParseError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct CmdOptions {
    std::string_view video;
    std::string_view analyzer;
    std::string_view model;
};

AppOptions toAppOptions(const CmdOptions& options);

class ArgParser {
  public:
    ArgParser(int argc, char** argv);
    static void usage(std::string_view progName);
    [[nodiscard]] CmdOptions parse() const;

  private:
    std::vector<std::string_view> args_;

    [[nodiscard]] CmdOptions collectCmdOptions() const;
    [[nodiscard]] std::string_view getNextValue(std::size_t index) const;
};
} // namespace StoveGuard::Cli