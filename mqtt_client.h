#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H



#include "MQTTClient.h"
#include "scripts/auxiliary_functions.hpp"
#include "tag_list.hpp"
#include "redis_client.h"
#include "colours.h"
#include "scripts/json.hpp"
using json = nlohmann::json;
using namespace nlohmann::literals;

#define MQTT_CLIENT_H

inline MQTTClient_deliveryToken deliveredToken;

class MQTT_Client 
{
private:
	static const char *clientid;

    static json data;

public:
    static MQTTClient client;

    static char* const  subscriptions;
    static MQTTClient_connectOptions connectionOptions;
    static MQTTClient_message publishMessage;
    static MQTTClient_deliveryToken token;


    MQTT_Client(const char *url, const char *clientid);
    static void connect();
    static void publish(const char *payload, const char *topic, const char *clientid);
    static void subscribe(const char *topic);
    static void subscribeMany(char* const* topics);
    void forwardMessage(const char *message);


    static void checkAndAcknowledgeAnchor(MQTTClient_message *data);

    static void packageData(std::string anchorID, std::string tagID, std::string distance);

};

void connectionLost(void *context, char *reason);
int messageArrived(void *context, char *topicName, int topicLength, MQTTClient_message *message);
void messageDelivered(void *context, MQTTClient_deliveryToken token);


#endif