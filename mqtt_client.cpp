#include "mqtt_client.h"
#include <iostream>
#include <vector>
MQTTClient MQTT_Client::client = NULL;
MQTTClient_connectOptions MQTT_Client::connectionOptions = MQTTClient_connectOptions_initializer;
MQTTClient_message MQTT_Client::publishMessage = MQTTClient_message_initializer;
MQTTClient_deliveryToken MQTT_Client::token;

const char *MQTT_Client::clientid = "ziggy";
const char *MQTT_Client::url = "localhost:1883";

json MQTT_Client::data = json::array();

// TODO: change rc to sth that makes sense
MQTT_Client::MQTT_Client()
{
    int rc;

    // Creating an MQTT client
    if ((rc = MQTTClient_create(&client, url, clientid, MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to create client, return code %d\n", rc);
    }

    // Setting callback funcitons:
    if ((rc = MQTTClient_setCallbacks(client, NULL, connectionLost, messageArrived, messageDelivered)) != MQTTCLIENT_SUCCESS)
    {
        printf("Callbacks not set\n");
    }
    if ((rc = MQTTClient_connect(client, &connectionOptions)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        rc = EXIT_FAILURE;
    }
}

void MQTT_Client::publish(const char *payload, const char *topic, int qos)
{
    publishMessage.payload = (char *)payload;
    publishMessage.payloadlen = (int)strlen(payload);

    // TODO: eventually take arguments for these
    publishMessage.qos = qos;
    publishMessage.retained = 0;
    deliveredToken = 0;

    int rc;
    if (rc = MQTTClient_publishMessage(client, topic, &publishMessage, &token) != MQTTCLIENT_SUCCESS)
    {
        printf("Couldnt publish message, error code: %i \n", rc);
    }
    else
    {
        printf("Published %s in topic %s \n", payload, topic);
    }
}

void MQTT_Client::subscribe(const char *topic)
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


int messageArrived(void *context, char *topicName, int topicLength, MQTTClient_message *message)
{

    if (strcmp(topicName, (char *)"register/Anchor") == 0)
    {
        std::cout << (char *)message->payload << std::endl;
        MQTT_Client::checkAndAcknowledgeAnchor(message);
    }
    else if (strcmp(topicName, (char *)"position/Anchor") == 0)
    {

        std::string s = (char *)message->payload;

        // parse details

        // std::string var0 = parseData(s, "TIMESTAMP");
        std::string anchor_id = parseData(s, "ANCHORID");
        std::string tag_id = parseData(s, "TAGID");
        std::string distance = parseData(s, "DISTANCE");

        // check if anchor is in list add or update

        if (!(TagList::isInList(tag_id)))
        {
std::cout << "Tag not in list" << std::endl;
            // TagList::addAnchor(tag_id, anchor_id, distance);
            Database::redis.publish("newTag", tag_id);
        }
        else
        {
            TagList::addAnchor(tag_id, anchor_id, distance);

            std::string data = tag_id;

            json temp = TagList::getTagList();
            for (auto &[key, value] : temp.items())
            {
                if (value["id"] == tag_id)
                {
                    for (auto &[key1, value1] : value["anchors"].items())
                    {
                        data += ";" + value1["anchorID"].dump() + "=" + value1["distance"].dump();
                    }
                }
            }
            data.erase(
                remove(data.begin(), data.end(), '\"'),
                data.end());
            std::cout << data << std::endl;

            Database::redis.publish("newRange", data);
        }
    }
    else if (strcmp(topicName, (char *)"register/Tag") == 0)
    {
        std::cout << "tag registration" << std::endl;
    }

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
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
    if (found)
    {
        publish("accepted", "register/AnchorACK", 1);
        return;
    }

    // TODO: make dynamic
    AnchorList(str, rand() % 15, rand() % 13, rand() % 1000);
    anchors = AnchorList::getAnchorList();
    Database::redis.command<void>("JSON.SET", "anchors", ".", anchors.dump());

    publish("registered", "register/AnchorACK", 1);

    return;
}

void connectionLost(void *context, char *reason)
{
    printf("ConnectionLost\n");
    printf("Reason: %s\n", reason);
}

void messageDelivered(void *context, MQTTClient_deliveryToken token)
{
    printf("Message delivered with token: %d\n", token);
    deliveredToken = token;
}
