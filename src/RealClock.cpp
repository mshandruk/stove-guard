#include "RealClock.h"

#include <chrono>

#include "Clock.h"

using namespace std::chrono;

TimePoint RealClock::getTime() const {
    return steady_clock::now();
}
