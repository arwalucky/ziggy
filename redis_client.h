#include "event_handling/event_manager.hpp"

#include <sw/redis++/redis++.h>
using namespace sw::redis;

#define MQTT_CLIENT_H

class Redis_Client : public EventListener
{
private:
    sw::redis::Redis *redis;

public:
    Redis_Client(sw::redis::Redis &redis);
    
    void handleEvent(EventType event, void *message);
};

