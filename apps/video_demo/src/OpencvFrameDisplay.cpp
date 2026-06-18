#include "OpencvFrameDisplay.h"

#include <string>

#include "opencv2/core/mat.hpp"
#include "opencv2/core/types.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include "Frame.h"
#include "ObjectDetection.h"

void OpencvFrameDisplay::render(const Frame& frame, const ObjectDetections& objectDetections) {

    const cv::Mat canvas = frame.data.clone();
    const auto scaleX = static_cast<float>(frame.data.cols) / 640.0F;
    const auto scaleY = static_cast<float>(frame.data.rows) / 640.0F;

    for (const auto& detection : objectDetections) {
        const cv::Rect objectBox = {
            static_cast<int>(detection.box.x * scaleX),
            static_cast<int>(detection.box.y * scaleY),
            static_cast<int>(detection.box.width * scaleX),
            static_cast<int>(detection.box.height * scaleY),
        };

        constexpr int fontFace = cv::FONT_HERSHEY_SIMPLEX;
        constexpr double fontScale = 0.8F;
        constexpr int thickness = 2;

        const auto labelText =
            std::string(labelToString(detection.classification)) + " " + std::to_string(detection.confidence.value());

        int textDescent = 0; // Дополнительное пространство под текстом для символов с хвостиками (g, p, q, y)
        const cv::Size textSize = cv::getTextSize(labelText, fontFace, fontScale, thickness, &textDescent);

        cv::Point textOrigin;
        cv::Rect labelBox;
        constexpr int padding = 5;

        // LabelBox inner objectBox
        if (objectBox.y > textSize.height + padding) {
            textOrigin = cv::Point{
                objectBox.x,
                objectBox.y - padding,
            };

            labelBox = {
                textOrigin.x,
                textOrigin.y - textSize.height,
                textSize.width,
                textSize.height + textDescent,
            };
        } else {
            // LabelBox over objectBox
            textOrigin = cv::Point{
                objectBox.x,
                objectBox.y + textSize.height,
            };

            labelBox = {
                textOrigin.x,
                objectBox.y,
                textSize.width,
                textSize.height + textDescent,
            };
        }

        const auto boxColor = getLabelColor(detection.classification);
        cv::rectangle(canvas, labelBox, boxColor, cv::FILLED);
        cv::putText(canvas, labelText, textOrigin, fontFace, fontScale, {255, 255, 255}, thickness);
        cv::rectangle(canvas, objectBox, boxColor, thickness);
    }

    cv::imshow("StoveGuard", canvas);
    cv::waitKey(1);
}

cv::Scalar OpencvFrameDisplay::getLabelColor(LabelClassification label) {
    switch (label) {
    case LabelClassification::Person: {
        return cv::Scalar{0, 255, 0}; // green
    }
    case LabelClassification::Stove: {
        return cv::Scalar{0, 165, 255}; // orange
    }
    case LabelClassification::Fire: {
        return cv::Scalar{0, 0, 255}; // red
    }
    case LabelClassification::Pot: {
        return cv::Scalar{255, 0, 0}; // blue
    }
    default:
        return cv::Scalar{200, 200, 200}; // gray
    }
}
