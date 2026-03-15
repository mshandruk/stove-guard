#ifndef STOVEGUARD_FRAMEDISPLAY_H
#define STOVEGUARD_FRAMEDISPLAY_H
#include "AnalysisResult.h"
#include "Frame.h"

class FrameDisplay {
  public:
    FrameDisplay() = default;
    virtual ~FrameDisplay() = default;

    FrameDisplay(const FrameDisplay& other) = delete;
    FrameDisplay& operator=(const FrameDisplay& other) = delete;

    FrameDisplay(FrameDisplay&& other) = delete;
    FrameDisplay& operator=(FrameDisplay&& other) = delete;

    virtual void render(const Frame& frame, const ObjectDetections& objectDetections) = 0;
};

#endif // STOVEGUARD_FRAMEDISPLAY_H
