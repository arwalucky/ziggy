#ifndef EVENT_LISTENER_H
#define EVENT_LISTENER_H

#include "event_enum_type.hpp"

class EventListener {
    public:
        EventListener();

        virtual void handleEvent(EventType type, void* data) = 0;
};

#endif