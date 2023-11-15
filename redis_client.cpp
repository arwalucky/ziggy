#include "redis_client.h"

Redis_Client::Redis_Client(sw::redis::Redis &redis) : EventListener()
{

    // registers itself as a listener
    EventManager::getInstance().registerListener(MQTT_CONNECTED, this);
    EventManager::getInstance().registerListener(NEW_ANCHOR, this);
    EventManager::getInstance().registerListener(MQTT_MESSAGE_RECEIVED, this);

    this->redis = &redis;
};

void Redis_Client::handleEvent(EventType event, void *message)
{
    auto data = static_cast<MQTTClient_message *>(message);

    switch (event)
    {
    case MQTT_MESSAGE_RECEIVED:
        this->redis->publish("channel1", (char *)data->payload);
        break;

    case NEW_ANCHOR:
        if (checkAnchorList(data))
        {
            EventManager::getInstance().dispatchEvent(ANCHOR_FOUND, message);
            break;
        }
        else
        {
            EventManager::getInstance().dispatchEvent(ANCHOR_REGISTERED, message);
            break;
        }
    default:
        break;
    }
};

bool Redis_Client::checkAnchorList(MQTTClient_message *data)
{
    std::vector<std::string> vec = {};
    redis->lrange("anchorList", 0, -1, std::back_inserter(vec));

    std::string str = (char *)data->payload;
    if (std::find(vec.begin(), vec.end(), str) != vec.end())
    {
        return true;
    }
    else
    {
        this->redis->rpush("anchorList", str);
        return false;
    }

    vec.clear();
    return this;
}