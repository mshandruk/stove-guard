#ifndef STOVEGUARD_FRAMESOURCE_H
#define STOVEGUARD_FRAMESOURCE_H
#include <optional>

#include "Frame.h"

class FrameSource {
  public:
    FrameSource(const FrameSource&) = delete;
    FrameSource& operator=(const FrameSource&) = delete;
    FrameSource(FrameSource&&) = default;
    FrameSource& operator=(FrameSource&&) = default;
    FrameSource() = default;
    virtual ~FrameSource() = default;

    [[nodiscard]] virtual std::optional<Frame> getFrame() = 0;
};
#endif // STOVEGUARD_FRAMESOURCE_H
