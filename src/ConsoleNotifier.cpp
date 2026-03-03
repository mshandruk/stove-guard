#include "ConsoleNotifier.h"

#include <iostream>

#include "StoveMonitor.h"

void ConsoleNotifier::notify(const Event event) {
    std::cout << "[ConsoleNotifier]" << '\n';
    switch (event) {
    case Event::AlarmStarted:
        std::cout << "AlarmStarted";
        break;
    case Event::AlarmCleared:
        std::cout << "AlarmCleared";
        break;
    case Event::DangerousEntered:
        std::cout << "DangerousEntered";
        break;
    case Event::DangerousCleared:
        std::cout << "DangerousCleared";
        break;
    case Event::None:
        return;
    }
    std::cout << '\n';
}
