#pragma once

#include <chrono>

using TimePoint = std::chrono::steady_clock::time_point;

class Clock {
  public:
    Clock() = default;
    virtual ~Clock() = default;

    Clock(const Clock&) = delete;
    Clock& operator=(const Clock&) = delete;
    Clock(const Clock&&) = delete;
    Clock& operator=(const Clock&&) = delete;

    [[nodiscard]]
    virtual TimePoint getTime() const = 0;
};
