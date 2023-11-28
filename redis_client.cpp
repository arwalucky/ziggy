#include "redis_client.h"
#include <time.h>
Database::Database(sw::redis::Redis &redis) : EventListener()
{
    // registers itself as a listener
    EventManager::getInstance().registerListener(MQTT_CONNECTED, this);
    EventManager::getInstance().registerListener(NEW_ANCHOR, this);
    EventManager::getInstance().registerListener(MQTT_MESSAGE_RECEIVED, this);

    this->redis = &redis;
};

void Database::handleEvent(EventType event, void *message)
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

bool Database::checkAnchorList(MQTTClient_message *data)
{
    std::string str = (char *)data->payload;

    json anchors = AnchorList::getAnchorList();

    for (auto &[key, value] : anchors.items())
    {
        if (value["id"] == str)
        {
            return true;
        }
    }

    //TODO: make dynamic
    AnchorList(str, rand()%15, rand()%13, rand()%1000);
    anchors = AnchorList::getAnchorList();
    this->redis->command<void>("JSON.SET", "anchors", ".", anchors.dump());

    return false;
}