#include "ConsoleNotifier.h"

#include <iostream>

#include "StoveMonitor.h"

void ConsoleNotifier::notify(Event event) {
    std::cout << "[ConsoleNotifier] ";
    switch (event) {
    case Event::AlarmStarted:
        std::cout << "\nAlarmStarted";
        break;
    case Event::AlarmCleared:
        std::cout << "\nAlarmCleared";
        break;
    case Event::DangerousEntered:
        std::cout << "\nDangerousEntered";
        break;
    case Event::DangerousCleared:
        std::cout << "\nDangerousCleared";
        break;
    case Event::None:
        return;
    }
    std::cout << '\n';
}
