#include "event_manager.hpp"

EventManager& EventManager::getInstance() {
    static EventManager instance;
    return instance;
}

EventManager::~EventManager() {
    // Clear event listeners
    for (auto mapIterator : eventListeners) {
        for (auto listener : mapIterator.second) {
            delete listener;
        }
    }
    eventListeners.clear();
}

void EventManager::registerListener(EventType type, EventListener* listener) {
    eventListeners[type].push_back(listener);
}

void EventManager::removeListener(EventType type, EventListener* listener) {
    for (auto it = eventListeners[type].begin(); it != eventListeners[type].end(); it++) {
        if (*it == listener) {
            eventListeners[type].erase(it);
            break;
        }
    }
}

void EventManager::dispatchEvent(EventType type, void* data) {
    for (auto listener : eventListeners[type]) {
        listener->handleEvent(type, data);
    }
}