#include "mqtt_client.h"

MQTTClient MQTT_Client::client = NULL;
MQTTClient_connectOptions MQTT_Client::connectionOptions = MQTTClient_connectOptions_initializer;
MQTTClient_message MQTT_Client::publishMessage = MQTTClient_message_initializer;
MQTTClient_deliveryToken MQTT_Client::token;

const char *MQTT_Client::clientid = "";

json MQTT_Client::data = json::array();

// TODO: change rc to sth that makes sense
MQTT_Client::MQTT_Client(const char *url, const char *clientid)
{
    MQTT_Client::clientid = clientid;   

    int rc;
    // Creating an MQTT client
    if ((rc = MQTTClient_create(&client, url, clientid, MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to create client, return code %d\n", rc);
    }
    else
    {
        printf("woopsi maceroni, I am connected \n");
    }

    // Setting callback funcitons:
    if ((rc = MQTTClient_setCallbacks(client, NULL, connectionLost, messageArrived, messageDelivered)) != MQTTCLIENT_SUCCESS)
    {
        printf("Callbacks not set\n");
    }
    else
    {
        printf("Callbacks set \n");
    }

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

void MQTT_Client::subscribeMany(char *const *topics)
{
    // int rc;
    // int length = topics.length();
    // if ((rc = MQTTClient_subscribeMany(client, length ,topics, 0) != MQTTCLIENT_SUCCESS))
    // {
    //     printf("Failed to subscribe. Error code: %i \n", rc);
    // }
    // else
    // {

    // }
}

void MQTT_Client::forwardMessage(const char *message)
{
}

//? dont understand https://stackoverflow.com/questions/50826532/pahomqtt-assign-events/50827163#50827163

void connectionLost(void *context, char *reason)
{
    printf("ConnectionLost\n");
    printf("Reason: %s\n", reason);
}

int messageArrived(void *context, char *topicName, int topicLength, MQTTClient_message *message)
{

    if (strcmp(topicName, (char *)"register") == 0)
    {
        printf("%s \n", topicName);
        printf("%*s \n", message->payloadlen, (char *)message->payload);
        MQTT_Client::checkAndAcknowledgeAnchor(message);
    }
    else if (strcmp(topicName, "ziggy") == 0)
    {
        printf("%s\n", topicName);
    }
    else if (strcmp(topicName, "newTag") == 0)
    {
        std::string s = (char *)message->payload;
        std::string var0 = parseData(s, "TIMESTAMP");
        std::string var1 = parseData(s, "ANCHORID");
        std::string var2 = parseData(s, "TAGID");
        std::string var3 = parseData(s, "DISTANCE");

        if (TagList::isInList(var2))
        {
            TagList::addAnchor(var2, var1);
        }
        else
        {
            TagList(var2, json::array());
            TagList::addAnchor(var2, var1);
        }
    }
    else if (strcmp(topicName, "tagData") == 0)
    {
        std::string s = (char *)message->payload;
        // std::string var0 = parseData(s, "TIMESTAMP");
        std::string var1 = parseData(s, "ANCHORID");
        std::string var2 = parseData(s, "TAGID");
        std::string var3 = parseData(s, "DISTANCE");
        MQTT_Client::packageData(var1, var2, var3);
    }
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void MQTT_Client::packageData(std::string anchorID, std::string tagID, std::string distance)
{

    json temp = json::array();

    temp.push_back({{"anchorID", anchorID},
                    {"tagID", tagID},
                    {"coordinates", {{"x", distance}, {"y", distance}}}});

    if (MQTT_Client::data.size() >= 10)
    {
        std::cout << "nough" << std::endl;
    }
    else
    {	
        MQTT_Client::data.push_back(temp);
        Database::redis.publish("test","test");
    }
}

void messageDelivered(void *context, MQTTClient_deliveryToken token)
{
    printf("Message delivered with token: %d\n", token);
    deliveredToken = token;
}

void MQTT_Client::checkAndAcknowledgeAnchor(MQTTClient_message *data)
{
    std::string str = (char *)data->payload;
    json anchors = AnchorList::getAnchorList();
    bool found = false;

    for (auto &[key, value] : anchors.items())
    {
        if (value["id"] == str)
        {
            found = true;
        }
    }

    if (found) {
        publish("accepted", "register/ACK", MQTT_Client::clientid);
        return;
    }

    //TODO: make dynamic
    AnchorList(str, rand()%15, rand()%13, rand()%1000);
    anchors = AnchorList::getAnchorList();
    Database::redis.command<void>("JSON.SET", "anchors", ".", anchors.dump());
    publish("registered", "register/ACK",  MQTT_Client::clientid);
     
    return;
}