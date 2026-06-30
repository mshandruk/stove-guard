#pragma once

#include <cstddef>
#include <stdexcept>

template <typename State>
class StateStabilizer {
  public:
    StateStabilizer(State stableState, std::size_t threshold);
    [[nodiscard]] State updateState(State newState) noexcept;

  private:
    State stableState_;
    std::size_t threshold_;
    std::size_t candidateCount_ = 0;
};

template <typename State>
StateStabilizer<State>::StateStabilizer(const State stableState, const std::size_t threshold)
        : stableState_{stableState},
          threshold_{threshold} {
    if (threshold_ <= 0) {
        throw std::invalid_argument("Threshold value must be greater than zero");
    }
}

template <typename State>
State StateStabilizer<State>::updateState(const State newState) noexcept {
    if (newState == stableState_) {
        candidateCount_ = 0;
        return stableState_;
    }

    ++candidateCount_;

    if (candidateCount_ >= threshold_) {
        stableState_ = newState;
        candidateCount_ = 0;
    }

    return stableState_;
}