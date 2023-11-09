
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "MQTTClient.h"


#define PAYLOAD "hello here is me" 
#define TOPIC "ziggy"
#define QOS 0


MQTTClient_deliveryToken deliveredToken;

// function declaration
void connectionLost(void *context, char *reason);
int messageArrived(void *context, char *topicName, int topicLength, MQTTClient_message *message);
void messageDelivered(void *context, MQTTClient_deliveryToken token);



int main()
{
	const char *url = "localhost:1883";
	const char *clientid = "ziggy";
	
	MQTTClient client;
	MQTTClient_connectOptions connectionOptions = MQTTClient_connectOptions_initializer;
    MQTTClient_message publishMessage = MQTTClient_message_initializer;
	MQTTClient_deliveryToken deliveryToken;

	int rc;
	rc = MQTTClient_create(&client, url, clientid, MQTTCLIENT_PERSISTENCE_NONE, NULL);


	// Creating an MQTT client
	if(rc != MQTTCLIENT_SUCCESS){
		printf("Failed to create client, return code %d\n", rc);
		rc = EXIT_FAILURE;
	}
	else{
		printf("woopsi maceroni, I am connected \n");
		rc = EXIT_SUCCESS;
	}

	//Setting callback funcitons: 
	if (rc = MQTTClient_setCallbacks(client, NULL, connectionLost, messageArrived, messageDelivered) != MQTTCLIENT_SUCCESS)
	{
		printf("Callbacks not set\n");
		rc = EXIT_FAILURE;
	}
	else {
		printf("Callbacks set \n");
	}


	if ((rc = MQTTClient_connect(client, &connectionOptions)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }
	else {
		printf("Connected to MQTT");
	}

	//? somehow doesnt work without char*
	publishMessage.payload = (char*) PAYLOAD;
	publishMessage.payloadlen = (int)strlen(PAYLOAD);
	publishMessage.qos = 0;
	publishMessage.retained = 0;
	deliveryToken = 0;

	if(rc = MQTTClient_publishMessage(client, TOPIC, &publishMessage, &deliveryToken) != MQTTCLIENT_SUCCESS){
		printf("Not sent");
		rc = EXIT_FAILURE;
	} 
	else {
		printf ("Waiting for topic %s \n" "in topic %s for cleint with ID %s \n", PAYLOAD, TOPIC, clientid);
		while (deliveredToken != deliveryToken){
			sleep(1000L);
		}
		
	}
	
    if ((rc = MQTTClient_disconnect(client, 10000)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to disconnect, return code %d\n", rc);
        rc = EXIT_FAILURE;
    }

destroy_exit:
    MQTTClient_destroy(&client);
 
exit:
    return rc;
}


// functions

void connectionLost(void *context, char *reason){
	printf("ConnectionLost\n");
	printf("Reason: %s\n", reason);
}

int messageArrived(void *context, char *topicName, int topicLength, MQTTClient_message *message){
	printf("Message arrived in topic: %s\n", topicName);
	printf("Message %*s \n", message->payloadlen, (char*)message->payload);
	//set the space free again
	MQTTClient_freeMessage(&message);
	MQTTClient_free(topicName);
	return 1;
}

void messageDelivered(void *context, MQTTClient_deliveryToken token){
	printf("Message delivered with token: %d\n", token);
	// ? dont get why 
	deliveredToken = token;
}