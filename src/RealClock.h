#ifndef STOVEGUARD_REALCLOCK_H
#define STOVEGUARD_REALCLOCK_H

#include <chrono>

#include "Clock.h"

class RealClock final : public Clock {
  public:
    [[nodiscard]]
    std::chrono::steady_clock::time_point getTime() const override;
};

#endif // STOVEGUARD_REALCLOCK_H
