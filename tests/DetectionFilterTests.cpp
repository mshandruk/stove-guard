#include <gtest/gtest.h>

#include "DetectionFilter.h"
#include "ObjectDetection.h"

class DetectionFilterTest : public testing::Test {

  protected:
    const DetectionFilter detectionFilter{
        ConfidenceThresholds{
            {LabelClassification::Person, Confidence{0.7F}},
            {LabelClassification::Stove, Confidence{0.7F}},
        },
    };
};

TEST_F(DetectionFilterTest, ReturnsEmpty_WhenAllConfidenceBelowThreshold) {
    const ObjectDetections objectDetections{
        ObjectDetection{LabelClassification::Person, Confidence{0.1F}, {}},
        ObjectDetection{LabelClassification::Stove, Confidence{0.1F}, {}},
    };

    const auto result = detectionFilter.filter(objectDetections);

    EXPECT_TRUE(result.empty());
}

TEST_F(DetectionFilterTest, KeepsOnlyStove_WhenOnlyStoveAboutThreshold) {
    const ObjectDetections objectDetections{
        ObjectDetection{LabelClassification::Person, Confidence{0.1F}, {}},
        ObjectDetection{LabelClassification::Stove, Confidence{0.8F}, {}},
    };

    const auto result = detectionFilter.filter(objectDetections);

    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].classification, LabelClassification::Stove);
}

TEST_F(DetectionFilterTest, KeepsOnlyPerson_WhenOnlyPersonAboutThreshold) {
    const ObjectDetections objectDetections{
        ObjectDetection{LabelClassification::Person, Confidence{0.8F}, {}},
        ObjectDetection{LabelClassification::Stove, Confidence{0.1F}, {}},
    };

    const auto result = detectionFilter.filter(objectDetections);

    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].classification, LabelClassification::Person);
}

TEST_F(DetectionFilterTest, KeepsAll_WhenAllAboutThreshold) {

    const ObjectDetections objectDetections{
        ObjectDetection{LabelClassification::Person, Confidence{0.8F}, {}},
        ObjectDetection{LabelClassification::Stove, Confidence{0.8F}, {}},
    };

    const auto result = detectionFilter.filter(objectDetections);

    ASSERT_EQ(result.size(), 2);
    EXPECT_EQ(result[0].classification, LabelClassification::Person);
    EXPECT_EQ(result[1].classification, LabelClassification::Stove);
}