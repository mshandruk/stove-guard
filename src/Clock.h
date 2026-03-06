#ifndef STOVEGUARD_CLOCK_H
#define STOVEGUARD_CLOCK_H
#include <chrono>

class Clock {
  public:
    Clock() = default;
    virtual ~Clock() = default;

    Clock(const Clock&) = delete;
    Clock& operator=(const Clock&) = delete;
    Clock(const Clock&&) = delete;
    Clock& operator=(const Clock&&) = delete;

    [[nodiscard]]
    virtual std::chrono::steady_clock::time_point getTime() const = 0;
};
#endif // STOVEGUARD_CLOCK_H
