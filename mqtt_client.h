#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H



#include "MQTTClient.h"
#include "scripts/auxiliary_functions.hpp"
#include "tag_list.hpp"
#include "redis_client.h"
#include "scripts/json.hpp"
using json = nlohmann::json;
using namespace nlohmann::literals;

#define MQTT_CLIENT_H

inline MQTTClient_deliveryToken deliveredToken;

class MQTT 
{
private:
	static const char *clientid;
    static const char *url;
    static json data;

public:

    MQTT();
    static MQTTClient client;
    static MQTTClient_connectOptions connectionOptions;
    static MQTTClient_message publishMessage;
    static MQTTClient_deliveryToken token;

    /* @brief Publishes a message to a topic
     * 
     * @param payload The message to be published
     * @param topic The topic to publish to
    */
    static void publish(const char *payload, const char *topic, int qos);

    /* @brief Subscribes to a topic
     * 
     * @param topic The topic to subscribe to
    */
    static void subscribe(const char *topic);

    static void setCallbacks(MQTTClient_connectionLost *cl, MQTTClient_messageArrived *ma, MQTTClient_deliveryComplete *dc);

};

#endif