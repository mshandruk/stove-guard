#include "SimpleSceneInterpreter.h"
#include "Detection.h"
#include "ObjectDetection.h"

Detection SimpleSceneInterpreter::interpret(const ObjectDetections& objectDetections) {
    bool flame = false;
    bool person = false;
    for (const auto objectDetection : objectDetections) {
        switch (objectDetection.label) {
        case LabelClassification::Person: {
            person = true;
            break;
        }
        case LabelClassification::Stove: {
            flame = true;
            break;
        }
        }
    }
    return Detection{flame, person};
}
