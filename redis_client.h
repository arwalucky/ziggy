#include <iostream>
#include <algorithm>
#include "event_handling/event_manager.hpp"
#include "MQTTClient.h"
#include "anchorList.hpp"

#include <sw/redis++/redis++.h>
using namespace sw::redis;

#define MQTT_CLIENT_H

class Database : public EventListener
{
private:
    sw::redis::Redis *redis;

public:
    Database(sw::redis::Redis &redis);
    
    void handleEvent(EventType event, void *message);
    bool checkAnchorList(MQTTClient_message *data);
};

