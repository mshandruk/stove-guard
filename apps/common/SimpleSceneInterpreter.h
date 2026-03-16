#ifndef STOVEGUARD_SIMPLESCENEINTERPRETER_H
#define STOVEGUARD_SIMPLESCENEINTERPRETER_H
#include "Detection.h"
#include "ObjectDetection.h"
#include "SceneInterpreter.h"

class SimpleSceneInterpreter final : public SceneInterpreter {
  public:
    Detection interpret(const ObjectDetections& objectDetections) override;
};
#endif // STOVEGUARD_SIMPLESCENEINTERPRETER_H
