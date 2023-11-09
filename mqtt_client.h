#include "MQTTClient.h"

#define MQTT_CLIENT_H

inline MQTTClient_deliveryToken deliveredToken;

class MQTT_Client
{
private:
    MQTTClient client;
    MQTTClient_connectOptions connectionOptions = MQTTClient_connectOptions_initializer;
    MQTTClient_message publishMessage = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
public:
    MQTT_Client(const char *url, const char *clientid);
    void connect();
    void publish(const char *payload, const char *topic, const char* clientid);
    void subscribe(const char * topic);

};

static void connectionLost(void *context, char *reason);
static int messageArrived(void *context, char *topicName, int topicLength, MQTTClient_message *message);
static void messageDelivered(void *context, MQTTClient_deliveryToken token);
