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
    static const char *url;
    static json data;

public:

    MQTT_Client();
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

    static void subscribeMany(char* const topic, int qos);
    /* @brief Checks if the anchor is already found,  if not, adds it to the system and database and sends acknowledgment
     * @param data The anchor to be checked
    */
    static void checkAndAcknowledgeAnchor(MQTTClient_message *data);


};

/* @brief Callback function for when the connection is lost
 * 
 * @param context The context of the connection
 * @param reason The reason for the connection loss
*/
void connectionLost(void *context, char *reason);

/* @brief Callback function for when a message arrives
 * 
 * @param context The context of the connection
 * @param topicName The name of the topic
 * @param topicLength The length of the topic
 * @param message The message that arrived
*/
int messageArrived(void *context, char *topicName, int topicLength, MQTTClient_message *message);

/* @brief Callback function for when a message is delivered
 * 
 * @param context The context of the connection
 * @param token The token of the message
*/
void messageDelivered(void *context, MQTTClient_deliveryToken token);


#endif