#pragma once

#include <chrono>
#include <cstdint>

enum class StoveState : uint8_t { On, Off };
enum class PersonState : uint8_t { Present, Absent };

using Duration = std::chrono::steady_clock::duration;

enum class Event : uint8_t {
    None,
    DangerousEntered,
    DangerousCleared,
    AlarmStarted,
    AlarmCleared,
};

class StoveMonitor {
  public:
    explicit StoveMonitor(Duration alarmThreshold);

    [[nodiscard]] Event process(StoveState stoveState, PersonState personState, Duration delta);

  private:
    Duration alarmThreshold_;
    enum class SystemState : uint8_t {
        Safe,
        Dangerous,
        Alarmed,
    };

    SystemState systemState_ = SystemState::Safe;
    Duration dangerousDuration_ = Duration::zero();

    [[nodiscard]] static bool isDangerous(StoveState stoveState, PersonState personState);

    [[nodiscard]] bool isTimerExpired() const noexcept;
};
