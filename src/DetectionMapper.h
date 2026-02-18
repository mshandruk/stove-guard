#ifndef STOVEGUARD_DETECTIONMAPPER_H
#define STOVEGUARD_DETECTIONMAPPER_H
#include <utility>

#include "Detection.h"
#include "StoveMonitor.h"

std::pair<StoveState, PersonState> toDomain(const Detection& detection);

#endif // STOVEGUARD_DETECTIONMAPPER_H
