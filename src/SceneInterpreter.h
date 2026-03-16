#ifndef STOVEGUARD_SCENEINTERPRETER_H
#define STOVEGUARD_SCENEINTERPRETER_H
#include "Detection.h"
#include "ObjectDetection.h"

class SceneInterpreter {
  public:
    SceneInterpreter() = default;
    virtual ~SceneInterpreter() = default;

    SceneInterpreter(const SceneInterpreter& other) = delete;
    SceneInterpreter& operator=(const SceneInterpreter& other) = delete;

    SceneInterpreter(SceneInterpreter&& other) = delete;
    SceneInterpreter& operator=(SceneInterpreter&& other) = delete;

    virtual Detection interpret(const ObjectDetections& objectDetections) = 0;
};
#endif // STOVEGUARD_SCENEINTERPRETER_H
