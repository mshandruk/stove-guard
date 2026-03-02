#include "ConsoleNotifier.h"

#include <iostream>

#include "StoveMonitor.h"

void ConsoleNotifier::notify(const Event event) {
    switch (event) {
    case Event::AlarmStarted: {
        std::cout << "[ConsoleNotifier] " << "AlarmStarted" << '\n';
        break;
    }
    case Event::AlarmCleared: {
        std::cout << "[ConsoleNotifier] " << "AlarmCleared" << '\n';
        break;
    }

    case Event::DangerousEntered: {
        std::cout << "[ConsoleNotifier] " << "DangerousEntered" << '\n';
        break;
    }
    case Event::DangerousCleared: {
        std::cout << "[ConsoleNotifier] " << "DangerousCleared" << '\n';
        break;
    }

    default: {
        break;
    }
    }
}
