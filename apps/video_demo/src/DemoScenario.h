#pragma once

#include "FakeFrameAnalyzer.h"
#include "ObjectDetection.h"

inline FakeScenario getDemoScenario() {
    return {
        {

            ObjectDetections{
                ObjectDetection{
                    LabelClassification::Person,
                    Confidence{0.9F},
                    BoundingBox{100, 100, 200, 200},
                },

            },
            "Stove is OFF and person is present",
        },

        {

            ObjectDetections{
                ObjectDetection{
                    LabelClassification::Stove,
                    Confidence{0.9F},
                    BoundingBox{100, 100, 200, 200},
                },
                ObjectDetection{
                    LabelClassification::Person,
                    Confidence{0.9F},
                    BoundingBox{200, 200, 300, 300},
                },

            },
            "Stove is ON and person is present",
        },

        {

            ObjectDetections{
                ObjectDetection{
                    LabelClassification::Stove,
                    Confidence{0.9F},
                    BoundingBox{200, 200, 300, 300},
                },

            },
            "Stove is ON and person is absent",
        },

    };
}
