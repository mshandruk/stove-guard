#include "SceneMapper.h"

#include <algorithm>
#include <cstddef>
#include <optional>
#include <vector>

#include "ObjectDetection.h"
#include "SceneState.h"
#include "StoveMonitor.h"

namespace SceneMapper {
namespace {
float calculateIntersectionArea(const BoundingBox& a, const BoundingBox& b) {
    const float interLeft = std::max(a.left(), b.left());
    const float interRight = std::min(a.right(), b.right());
    const float interTop = std::max(a.top(), b.top());
    const float interBottom = std::min(a.bottom(), b.bottom());

    const float width = std::max(0.0F, interRight - interLeft);
    const float height = std::max(0.0F, interBottom - interTop);

    return width * height;
}

float calculateOverlapRatio(const BoundingBox& target, const BoundingBox& container) {
    const float targetArea = target.width * target.height;
    if (targetArea <= 0.0F) {
        return 0.0F;
    }

    const float intersectionArea = calculateIntersectionArea(target, container);

    // Результат: какую долю (от 0.0 до 1.0) своей площади 'target' "отдал" в пересечение с 'container'
    return intersectionArea / targetArea;
}

class StoveCluster {
  public:
    explicit StoveCluster(const BoundingBox& stove)
            : stove_{stove} {
    }

    void addFire(const BoundingBox& fire) {
        fires_.push_back(fire);
    }

    void addPot(const BoundingBox& pot) {
        pots_.push_back(pot);
    }

    [[nodiscard]] bool isActive() const noexcept {
        return !fires_.empty();
    }

    [[nodiscard]] bool hasPot() const noexcept {
        return !pots_.empty();
    }

    [[nodiscard]] bool hasPotOnFire(const float minScore) const noexcept {
        if (!isActive() || !hasPot()) {
            return false;
        }

        for (const auto& fire : fires_) {
            for (const auto& pot : pots_) {
                if (calculateOverlapRatio(fire, pot) >= minScore) {
                    return true;
                }
            }
        }
        return false;
    }

    [[nodiscard]] const BoundingBox& stove() const noexcept {
        return stove_;
    }

  private:
    BoundingBox stove_;
    std::vector<BoundingBox> fires_;
    std::vector<BoundingBox> pots_;
};

struct SceneContext {
    std::vector<BoundingBox> stoves;
    std::vector<BoundingBox> fires;
    std::vector<BoundingBox> pots;
    std::vector<BoundingBox> persons;
};

SceneContext groupByClassification(const ObjectDetections& detections) {
    SceneContext ctx;
    for (const auto& detection : detections) {
        switch (detection.classification) {
        case LabelClassification::Person: {
            ctx.persons.push_back(detection.box);
            break;
        }
        case LabelClassification::Stove: {
            ctx.stoves.push_back(detection.box);
            break;
        }
        case LabelClassification::Fire: {
            ctx.fires.push_back(detection.box);
            break;
        }
        case LabelClassification::Pot: {
            ctx.pots.push_back(detection.box);
            break;
        }
        default: {
            break;
        }
        }
    }
    return ctx;
}

std::optional<std::size_t>
findBestMatchCluster(const BoundingBox& target, const std::vector<StoveCluster>& clusters, const float minScore) {
    float maxScore = minScore;
    std::optional<std::size_t> bestIndex;
    for (std::size_t i = 0; i < clusters.size(); ++i) {
        const float score = calculateOverlapRatio(target, clusters[i].stove()); // NOLINT
        if (score >= maxScore) {
            maxScore = score;
            bestIndex = i;
        }
    }
    return bestIndex;
}

void associateWithStoves(
    std::vector<StoveCluster>& stoveClusters,
    const SceneContext& ctx,
    const OverlapThresholds thresholds) {
    if (stoveClusters.empty()) {
        return;
    }

    for (const auto& fire : ctx.fires) {
        if (const auto clusterIndex = findBestMatchCluster(fire, stoveClusters, thresholds.fireOnStove)) {
            stoveClusters[*clusterIndex].addFire(fire); // NOLINT
        }
    }

    for (const auto& pot : ctx.pots) {
        if (const auto clusterIndex = findBestMatchCluster(pot, stoveClusters, thresholds.potOnStove)) {
            stoveClusters[*clusterIndex].addPot(pot); // NOLINT
        }
    }
}

std::vector<StoveCluster> createStoveClusters(const std::vector<BoundingBox>& stoves) {
    std::vector<StoveCluster> stoveClusters;
    stoveClusters.reserve(stoves.size());

    for (const auto& stove : stoves) {
        stoveClusters.emplace_back(stove);
    }
    return stoveClusters;
}

} // namespace

SceneState map(const ObjectDetections& detections, const OverlapThresholds thresholds) {
    SceneState scene{};

    const auto sceneContext = groupByClassification(detections);
    if (!sceneContext.persons.empty()) {
        scene.personState = PersonState::Present;
    }

    auto stoveClusters = createStoveClusters(sceneContext.stoves);
    associateWithStoves(stoveClusters, sceneContext, thresholds);

    for (const auto& stoveCluster : stoveClusters) {
        if (stoveCluster.isActive()) {
            scene.stoveState = StoveState::On;
        }

        if (stoveCluster.hasPot()) {
            scene.potOnStove = true;
        }

        if (stoveCluster.hasPotOnFire(thresholds.fireToPot)) {
            scene.potOnFire = true;
        }
    }

    return scene;
}
} // namespace SceneMapper
