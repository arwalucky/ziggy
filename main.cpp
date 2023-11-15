
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "colours.h"
#include "mqtt_client.h"
#include "redis_client.h"
#include <sw/redis++/redis++.h>
using namespace sw::redis;

int main()
{

	// Create an Redis object, which is movable but NOT copyable.
	auto redis = Redis("tcp://127.0.0.1:6379");

	std::vector<std::string> vec = {};
	redis.lrange("anchorList", 0, -1, std::back_inserter(vec));

	if (!vec.empty())
	{
		redis.rpush("anchorList", "");
	}
	else
	{
		printf("anchorList found");
	}
	auto sub = redis.subscriber();

	const char *url = "localhost:1883";
	const char *clientid = "ziggy";
	const char *payload = "test";
	const char *topic = "#";

	Redis_Client redis1(redis);

	MQTT_Client mqttClient(url, clientid, redis);
	mqttClient.connect();
	mqttClient.publish(payload, "ziggy", clientid);
	mqttClient.subscribe("#");

exit:
	return 0;
}
