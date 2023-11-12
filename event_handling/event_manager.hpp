#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include "event_listener.hpp"
#include <map>
#include <vector>


class EventManager {
    public:
        static EventManager& getInstance();
        ~EventManager();

        // Register an event listener
        void registerListener(EventType type, EventListener* listener);

        // Remove an event listener
        void removeListener(EventType type, EventListener* listener);

        // Dispatch an event
        void dispatchEvent(EventType type, void* data);

    private:
        EventManager(){};
        EventManager(EventManager const&) = delete;
        void operator=(EventManager const&) = delete;

        std::map<EventType, std::vector<EventListener*>> eventListeners;
};

#endif