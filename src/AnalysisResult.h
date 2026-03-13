#ifndef STOVEGUARD_ANALYSISRESULT_H
#define STOVEGUARD_ANALYSISRESULT_H
#include <vector>

#include "Detection.h"


struct BoundingBox {
    int x;
    int y;
    int width;
    int height;
};

struct AnalyzerResult {
    Detection detection;
    std::vector<BoundingBox> boundingBoxes;
};

#endif // STOVEGUARD_ANALYSISRESULT_H
