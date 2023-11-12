#include "redis_client.h"

Redis_Client::Redis_Client(sw::redis::Redis &redis) : EventListener()
{

    //registers itself as a listener
    EventManager::getInstance().registerListener(MQTT_CONNECTED, this);
    EventManager::getInstance().registerListener(MQTT_MESSAGE_RECEIVED, this);


    this->redis = &redis;
};

void Redis_Client::handleEvent(EventType event, void *message)
{
    switch (event)
    {
    case MQTT_MESSAGE_RECEIVED:
        printf("wieeh %p \n", (const char*) message);
        //printf("Value:  %d\n", *message );
        this->redis->publish("test", "tespp");
        break;

    default:
        break;
    }
};
