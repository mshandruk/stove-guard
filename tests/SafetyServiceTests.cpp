#include <chrono>
#include <cstddef>
#include <vector>

#include <gtest/gtest.h>

#include "Clock.h"
#include "Frame.h"
#include "FrameTimer.h"
#include "Notifier.h"
#include "SafetyService.h"
#include "SceneState.h"
#include "StoveMonitor.h"

using namespace std::chrono;

class FakeNotifier : public Notifier {
  public:
    using Events = std::vector<Event>;

    void notify(const Event event) override {
        events_.push_back(event);
    }

    [[nodiscard]] const Events& events() const noexcept {
        return events_;
    }

    [[nodiscard]] std::size_t callCount() const noexcept {
        return events_.size();
    }

  private:
    Events events_;
};

class FakeClock final : public Clock {
  public:
    FakeClock()
            : current_{steady_clock::now()} {
    }

    [[nodiscard]]
    steady_clock::time_point getTime() const override {
        return current_;
    }

    void advance(const seconds seconds) {
        current_ += seconds;
    }

  private:
    steady_clock::time_point current_{Duration::zero()};
};

class SafetyServiceTest : public testing::Test {
  protected:
    FakeNotifier notifier;
    FakeClock clock;
    Duration alarmThreshold = seconds{15};
    FrameTimer frameTimer{clock};
    SafetyService safetyService{alarmThreshold, notifier, frameTimer};
    Frame frame{};
};

TEST_F(SafetyServiceTest, DangerousEntered_WhenStoveOnAndNoPerson) {
    constexpr SceneState scene{StoveState::On, PersonState::Absent};
    EXPECT_EQ(safetyService.handle(scene), Event::DangerousEntered);
}

TEST_F(SafetyServiceTest, ReturnsNone_WhenStoveOnAndPersonPresent) {
    constexpr SceneState scene{StoveState::On, PersonState::Present};
    EXPECT_EQ(safetyService.handle(scene), Event::None);
}

TEST_F(SafetyServiceTest, ReturnsNone_WhenStoveOffAndNoPerson) {
    constexpr SceneState scene{StoveState::Off, PersonState::Absent};
    EXPECT_EQ(safetyService.handle(scene), Event::None);
}

TEST_F(SafetyServiceTest, AlarmStarted_WhenNoPersonForAlarmThreshold) {
    constexpr SceneState scene{StoveState::On, PersonState::Absent};
    ASSERT_EQ(safetyService.handle(scene), Event::DangerousEntered);

    clock.advance(alarmThreshold);
    EXPECT_EQ(safetyService.handle(scene), Event::AlarmStarted);
}

TEST_F(SafetyServiceTest, AlarmCanceled_WhenPersonAppearsAfterAlarm) {
    auto scene = SceneState{StoveState::On, PersonState::Absent};
    ASSERT_EQ(safetyService.handle(scene), Event::DangerousEntered);

    clock.advance(alarmThreshold);
    ASSERT_EQ(safetyService.handle(scene), Event::AlarmStarted);

    clock.advance(alarmThreshold + seconds{1});
    scene = SceneState{StoveState::On, PersonState::Present};
    EXPECT_EQ(safetyService.handle(scene), Event::AlarmCleared);
}

TEST_F(SafetyServiceTest, AlarmNotStarted_WhenPersonAppearsBeforeAlarm) {
    auto scene = SceneState{StoveState::On, PersonState::Absent};
    ASSERT_EQ(safetyService.handle(scene), Event::DangerousEntered);

    clock.advance(alarmThreshold - seconds{1});
    ASSERT_EQ(safetyService.handle(scene), Event::None);

    clock.advance(alarmThreshold);
    scene = SceneState{StoveState::On, PersonState::Present};
    EXPECT_EQ(safetyService.handle(scene), Event::DangerousCleared);
}

TEST_F(SafetyServiceTest, AlarmRestarted_WhenPersonLeavesAfterClearingAlarm) {
    auto scene = SceneState{StoveState::On, PersonState::Absent};
    ASSERT_EQ(safetyService.handle(scene), Event::DangerousEntered);

    clock.advance(alarmThreshold);
    ASSERT_EQ(safetyService.handle(scene), Event::AlarmStarted);

    scene = SceneState{StoveState::On, PersonState::Present};
    ASSERT_EQ(safetyService.handle(scene), Event::AlarmCleared);

    clock.advance(seconds{2});
    scene = SceneState{StoveState::On, PersonState::Absent};
    ASSERT_EQ(safetyService.handle(scene), Event::DangerousEntered);

    clock.advance(alarmThreshold);
    EXPECT_EQ(safetyService.handle(scene), Event::AlarmStarted);
}

TEST_F(SafetyServiceTest, Notify_WhenEvent) {
    constexpr auto scene = SceneState{StoveState::On, PersonState::Absent};

    ASSERT_EQ(safetyService.handle(scene), Event::DangerousEntered);
    ASSERT_EQ(notifier.events().at(0), Event::DangerousEntered);

    clock.advance(alarmThreshold);
    ASSERT_EQ(safetyService.handle(scene), Event::AlarmStarted);
    EXPECT_EQ(notifier.events().at(1), Event::AlarmStarted);

    ASSERT_EQ(notifier.callCount(), 2);
    clock.advance(seconds{1});
    ASSERT_EQ(safetyService.handle(scene), Event::None);
    EXPECT_EQ(notifier.callCount(), 2);
}
