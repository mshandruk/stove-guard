#ifndef STOVEGUARD_ANALYSISRESULT_H
#define STOVEGUARD_ANALYSISRESULT_H

#include "Detection.h"

struct BoundingBoxes {};

struct AnalyzerResult {
    Detection detection;
    BoundingBoxes boundingBoxes;
};

#endif // STOVEGUARD_ANALYSISRESULT_H
