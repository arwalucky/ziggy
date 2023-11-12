#include "MQTTClient.h"
#include "event_handling/event_manager.hpp"

#include "colours.h"
// #include "redis_client.h"
#include <sw/redis++/redis++.h>
using namespace sw::redis;

#define MQTT_CLIENT_H

inline MQTTClient_deliveryToken deliveredToken;

class MQTT_Client
{
private:
    MQTTClient client;
    MQTTClient_connectOptions connectionOptions = MQTTClient_connectOptions_initializer;
    MQTTClient_message publishMessage = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    sw::redis::Redis *redis;

public:
    MQTT_Client(const char *url, const char *clientid, sw::redis::Redis &redis);
    void connect();
    void publish(const char *payload, const char *topic, const char *clientid);
    void subscribe(const char *topic);
    void forwardMessage(const char *message);
};
// QTT_Client *test;
void connectionLost(void *context, char *reason);
int messageArrived(void *context, char *topicName, int topicLength, MQTTClient_message *message);
void messageDelivered(void *context, MQTTClient_deliveryToken token);
