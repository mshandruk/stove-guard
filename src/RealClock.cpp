#include "RealClock.h"
#include <chrono>

using namespace std::chrono;

steady_clock::time_point RealClock::getTime() const {
    return steady_clock::now();
}
