#ifndef KILLITEMICONEVENT_H
#define KILLITEMICONEVENT_H

#include "../EventBus/Event.h"
#include "../EventBus/EventBus.h"

class KillItemIconEvent: public Event {
    public:
        inline KillItemIconEvent() = default;
};

#endif