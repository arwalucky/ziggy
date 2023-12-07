#include "MQTTClient.h"
#include "event_handling/event_manager.hpp"
#include "scripts/auxiliary_functions.hpp"
#include "tag_list.hpp"

#include "colours.h"


#define MQTT_CLIENT_H

inline MQTTClient_deliveryToken deliveredToken;

class MQTT_Client : public EventListener
{
private:
	const char *clientid;

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
    void handleEvent(EventType event, void *message);
};

void connectionLost(void *context, char *reason);
int messageArrived(void *context, char *topicName, int topicLength, MQTTClient_message *message);
void messageDelivered(void *context, MQTTClient_deliveryToken token);
