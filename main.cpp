#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "colours.h"

#include <sw/redis++/redis++.h>
#include "mqtt_client.h"

using namespace sw::redis;  


int main()
{
    // Create an Redis object, which is movable but NOT copyable.
    auto redis = Redis("tcp://127.0.0.1:6379");
    redis.set("key1", "stard");

	auto val = redis.get("key1");
	if (val) {
		std::cout << *val << std::endl;
	}

	const char *url = "localhost:1883";
	const char *clientid = "ziggy";
	const char *payload = "test";
	const char *topic = "ziggy";

	MQTT_Client mqttClient(url, clientid); 
	mqttClient.connect();
	mqttClient.publish(payload, topic, clientid);
	mqttClient.subscribe(topic);

exit:
    return 0;
}

