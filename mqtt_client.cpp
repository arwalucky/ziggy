#include "mqtt_client.h"

// TODO: change rc to sth that makes sense
MQTT_Client::MQTT_Client(const char *url, const char *clientid, sw::redis::Redis &redis) : EventListener()
{

    this->redis = &redis;
    this->clientid = clientid;
    int rc;
    // Creating an MQTT client
    if (rc = MQTTClient_create(&client, url, clientid, MQTTCLIENT_PERSISTENCE_NONE, NULL) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to create client, return code %d\n", rc);
    }
    else
    {
        printf("woopsi maceroni, I am connected \n");
    }

    // Setting callback funcitons:
    if (rc = MQTTClient_setCallbacks(client, NULL, connectionLost, messageArrived, messageDelivered) != MQTTCLIENT_SUCCESS)
    {
        printf("Callbacks not set\n");
    }
    else
    {
        printf("Callbacks set \n");
    }
    EventManager::getInstance().dispatchEvent(MQTT_CONNECTED, NULL);


    EventManager::getInstance().registerListener(ANCHOR_FOUND, this);
    EventManager::getInstance().registerListener(ANCHOR_REGISTERED, this);
}

void MQTT_Client::connect()
{
    int rc;
    if ((rc = MQTTClient_connect(client, &connectionOptions)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        rc = EXIT_FAILURE;
    }
    else
    {
        printf("Connected to MQTT \n");
    }
};

void MQTT_Client::publish(const char *payload, const char *topic, const char *clientid)
{

    // TODO : check if relevant
    //? somehow doesnt work without char*
    publishMessage.payload = (char *)payload;
    publishMessage.payloadlen = (int)strlen(payload);
    publishMessage.qos = 0;
    publishMessage.retained = 0;
    deliveredToken = 0;

    int rc;
    if (rc = MQTTClient_publishMessage(client, topic, &publishMessage, &token) != MQTTCLIENT_SUCCESS)
    {
        printf("Couldnt publish message, error code: %i \n", rc);
    }
    else
    {
        printf("Waiting for topic %s in topic %s \n", payload, topic);
    }
}

void MQTT_Client::subscribe(const char *topic)
{
    int rc;
    if ((rc = MQTTClient_subscribe(client, topic, 0) != MQTTCLIENT_SUCCESS))
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
void MQTT_Client::forwardMessage(const char *message)
{
    printf("here");
    redis->publish("key", message);
}

//? dont understand https://stackoverflow.com/questions/50826532/pahomqtt-assign-events/50827163#50827163

void connectionLost(void *context, char *reason)
{
    printf("ConnectionLost\n");
    printf("Reason: %s\n", reason);
}

int messageArrived(void *context, char *topicName, int topicLength, MQTTClient_message *message)
{
    // sstring topic = static_cast<std::string *> (topicName);

    if (strcmp(topicName, (char *)"register") == 0)
    {
        printf("%s \n", topicName);
        printf("%*s \n", message->payloadlen, (char *)message->payload);
        EventManager::getInstance().dispatchEvent(NEW_ANCHOR, message);
    }
    else if (strcmp(topicName, "ziggy") == 0)
    {
        printf("%s\n", topicName);
        EventManager::getInstance().dispatchEvent(MQTT_MESSAGE_RECEIVED, message);
    }

    // set the space free again
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void messageDelivered(void *context, MQTTClient_deliveryToken token)
{
    printf("Message delivered with token: %d\n", token);
    deliveredToken = token;
}

void MQTT_Client::handleEvent(EventType event, void *message)
{
    auto data = static_cast<MQTTClient_message *>(message);

    switch (event)
    {
    case ANCHOR_FOUND:
        publish("accepted", "register/ACK", this->clientid);
        break;

    case ANCHOR_REGISTERED:
        publish("registered", "register/ACK", this->clientid);

        break;

    default:
        break;
    }
};