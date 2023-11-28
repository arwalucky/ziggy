
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include "colours.h"
#include "mqtt_client.h"
#include "redis_client.h"
#include "anchorList.hpp"
#include <sw/redis++/redis++.h>
using namespace sw::redis;
#include <fstream>
#include "scripts/json.hpp"
using json = nlohmann::json;
using namespace nlohmann::literals;

int main()
{

	auto redis = Redis("tcp://127.0.0.1:6379");

	// gets from redis and saves locally
	auto anchors = redis.command<OptionalString>("JSON.GET", "anchors");
	if (anchors)
	{
		json anchorsTemp = json::parse(*anchors);
		for (auto &[key, value] : anchorsTemp.items())
		{
			AnchorList(value["id"], value["x"], value["y"], value["timestamp"]);
		}
	}

	// TODO: get registration data from Anchor with its location, timestamp and id and send it to redis

	// Create a subscriber object.
	auto sub = redis.subscriber();

	const char *url = "localhost:1883";
	const char *clientid = "ziggy";
	const char *payload = "test";
	const char *topic = "#";

	Database redis1(redis);

	MQTT_Client mqttClient(url, clientid, redis);
	mqttClient.connect();
	mqttClient.publish(payload, "ziggy", clientid);
	mqttClient.subscribe("#");

exit:
	return 0;
}
