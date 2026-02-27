#ifndef STOVEGUARD_NOTIFIER_H
#define STOVEGUARD_NOTIFIER_H

#include "StoveMonitor.h"

class Notifier {
  public:
    Notifier(const Notifier&) = delete;
    Notifier& operator=(const Notifier&) = delete;
    Notifier(Notifier&&) = default;
    Notifier& operator=(Notifier&&) = default;
    Notifier() = default;
    virtual ~Notifier() = default;

    virtual void notify(Event event) = 0;
};

#endif // STOVEGUARD_NOTIFIER_H
