#pragma once

#include <chrono>

#include "Clock.h"

class RealClock final : public Clock {
  public:
    [[nodiscard]]
    TimePoint getTime() const override;
};
