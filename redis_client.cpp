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
    auto data = static_cast<MQTTClient_message *>(message);

    switch (event)
    {
    case MQTT_MESSAGE_RECEIVED:
        this->redis->publish("test", (char *)data->payload);
        break;

    default:
        break;
    }
};
