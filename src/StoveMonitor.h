#ifndef STOVEGUARD_STOVEMONITOR_H
#define STOVEGUARD_STOVEMONITOR_H

#include <chrono>
#include <cstdint>

enum class StoveState : uint8_t { On, Off };
enum class PersonState : uint8_t { Present, Absent };

enum class Event : uint8_t {
    None,
    DangerousEntered,
    DangerousCleared,
    AlarmStarted,
    AlarmCleared,
};

constexpr std::chrono::steady_clock::duration ALARM_THRESHOLD = std::chrono::seconds(15);

class StoveMonitor {
  public:
    [[nodiscard]] Event
    process(StoveState stoveState, PersonState personState, std::chrono::steady_clock::time_point currentTime);

  private:
    enum class SystemState : uint8_t {
        Safe,
        Dangerous,
        Alarmed,
    };

    SystemState systemState_ = SystemState::Safe;
    std::chrono::steady_clock::time_point dangerStartTime_;

    [[nodiscard]] static bool isDangerous(StoveState stoveState, PersonState personState);

    [[nodiscard]] bool isTimerExpired(std::chrono::steady_clock::time_point currentTime) const;
};
#endif // STOVEGUARD_STOVEMONITOR_H
