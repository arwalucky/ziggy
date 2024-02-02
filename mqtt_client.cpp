#include "mqtt_client.h"
#include <iostream>
#include <vector>
MQTTClient MQTT::client = NULL;
MQTTClient_connectOptions MQTT::connectionOptions = MQTTClient_connectOptions_initializer;
MQTTClient_message MQTT::publishMessage = MQTTClient_message_initializer;
MQTTClient_deliveryToken MQTT::token;

const char *MQTT::clientid = "ziggy";
const char *MQTT::url = "localhost:1883";

json MQTT::data = json::array();

MQTT::MQTT()
{
    int rc;
    // Creating an MQTT client
    if ((rc = MQTTClient_create(&client, url, clientid, MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to create client, return code %d\n", rc);
    }
}

void MQTT::setCallbacks(MQTTClient_connectionLost *cl, MQTTClient_messageArrived *ma, MQTTClient_deliveryComplete *dc)
{
    if(MQTTClient_setCallbacks(client, NULL, cl, ma, dc) != MQTTCLIENT_SUCCESS)
    {
        printf("Callbacks not set\n");
    }

    if (MQTTClient_connect(client, &connectionOptions) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect");
    }
}

void MQTT::publish(const char *payload, const char *topic, int qos)
{
    publishMessage.payload = (char *)payload;
    publishMessage.payloadlen = (int)strlen(payload);

    publishMessage.qos = qos;
    publishMessage.retained = 0;
    deliveredToken = 0;

    int rc;
    if (rc = MQTTClient_publishMessage(client, topic, &publishMessage, &token) != MQTTCLIENT_SUCCESS)
    {
        printf("Couldnt publish message, error code: %i \n", rc);
    }

    //TODO: remove eventually
    else
    {
        printf("Published %s in topic %s \n", payload, topic);
    }
}

void MQTT::subscribe(const char *topic)
{

    int rc;
    if ((rc = MQTTClient_subscribe(client, topic, 1) != MQTTCLIENT_SUCCESS))
    {
        printf("Failed to subscribe. Error code: %i \n", rc);
    }
    else
    {
        int message;
        do
        {
            message = getchar();
        } while (message != 'Q' && message != 'q');
        if (rc = MQTTClient_unsubscribe(client, topic) != MQTTCLIENT_SUCCESS)
        {
            printf("Couldn't unsubscribe \n");
        }
    }
}