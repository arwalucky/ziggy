#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "mqtt_client.h"

MQTT_Client::MQTT_Client(const char *url, const char *clientid)
{

    int rc;
    // Creating an MQTT client
    if (rc = MQTTClient_create(&client, url, clientid, MQTTCLIENT_PERSISTENCE_NONE, NULL) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to create client, return code %d\n", rc);
        // rc = EXIT_FAILURE;
    }
    else
    {
        printf("woopsi maceroni, I am connected \n");
        // rc = EXIT_SUCCESS;
    }


    // Setting callback funcitons:
    if (rc = MQTTClient_setCallbacks(client, NULL, connectionLost, messageArrived, messageDelivered) != MQTTCLIENT_SUCCESS)
    {
        printf("Callbacks not set\n");
        // rc = EXIT_FAILURE;
    }
    else
    {
        printf("Callbacks set \n");
        // rc = EXIT_SUCCESS;
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
	else {
		printf("Connected to MQTT \n");
	}

};

void MQTT_Client::publish(const char* payload, const char* topic, const char* clientid)
{	    
    
    //? somehow doesnt work without char*
	publishMessage.payload = (char*) payload;
	publishMessage.payloadlen = (int)strlen(payload);
	publishMessage.qos = 0;
	publishMessage.retained = 0;
	deliveredToken = 0;

    int rc;
    if (rc = MQTTClient_publishMessage(client, topic, &publishMessage, &token) != MQTTCLIENT_SUCCESS)
    {
        printf("Couldnt publish message, error code: %i", rc);
    }
    else
    {
        printf("Waiting for topic %s \n in topic %s", payload, topic);
        while (deliveredToken != token)
        {
            sleep(1000L);
        }
    }
}

//? dont understand https://stackoverflow.com/questions/50826532/pahomqtt-assign-events/50827163#50827163

static void connectionLost(void *context, char *reason)
{
    printf("ConnectionLost\n");
    printf("Reason: %s\n", reason);
}

static int messageArrived(void *context, char *topicName, int topicLength, MQTTClient_message *message)
{
    printf("Message arrived in topic: %s\n", topicName);
    printf("Message %*s \n", message->payloadlen, (char *)message->payload);
    // set the space free again
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

static void messageDelivered(void *context, MQTTClient_deliveryToken token)
{
    printf("Message delivered with token: %d\n", token);
    // ? dont get why
    deliveredToken = token;
}
