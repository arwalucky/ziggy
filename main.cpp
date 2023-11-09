
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "mqtt_client.h"



int main()
{

	const char *url = "localhost:1883";
	const char *clientid = "ziggy";
	const char *payload = "test";
	const char *topic = "ziggy";

	MQTT_Client mqttClient(url, clientid); 
	mqttClient.connect();
	mqttClient.publish(payload, topic, clientid);


exit:
    return 0;
}

