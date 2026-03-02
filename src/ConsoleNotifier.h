#ifndef STOVEGUARD_CONSOLENOTIFIER_H
#define STOVEGUARD_CONSOLENOTIFIER_H
#include "Notifier.h"
#include "StoveMonitor.h"

class ConsoleNotifier final : public Notifier {
  public:
    void notify(Event event) override;
};

#endif // STOVEGUARD_CONSOLENOTIFIER_H
