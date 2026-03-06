#include "ConsoleNotifier.h"

#include <iostream>

#include "StoveMonitor.h"

void ConsoleNotifier::notify(const Event event) {
    std::cout << "[ConsoleNotifier] ";
    switch (event) {
    case Event::AlarmStarted:
        std::cout << "AlarmStarted" << '\n';
        break;
    case Event::AlarmCleared:
        std::cout << "AlarmCleared" << '\n';
        break;
    case Event::DangerousEntered:
        std::cout << "DangerousEntered" << '\n';
        break;
    case Event::DangerousCleared:
        std::cout << "DangerousCleared" << '\n';
        break;
    case Event::None:
        return;
    }
    std::cout << '\n';
}
