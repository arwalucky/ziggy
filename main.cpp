
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

	// Create an Redis object, which is movable but NOT copyable.
	auto redis = Redis("tcp://127.0.0.1:6379");


	AnchorList("1234", 1.0, 2.0, 1234);
	AnchorList("5678", 3.0, 4.0, 5678);	
	AnchorList("9012", 5.0, 6.0, 9012);

// gets json format list of anchors
	json j22 = AnchorList::getAnchorList();

		for (json::iterator it = j22.begin(); it != j22.end(); ++it)
		{
			std::cout << *it << '\n';
		}
// sends to redis
	redis.command<void>("JSON.SET", "doc", ".", j22.dump());


// // gets from redis and prints
// 	auto res = redis.command<OptionalString>("JSON.GET", "doc");
// 	if (res)
// 	{
// 		json j = json::parse(*res);
// 		for (json::iterator it = j.begin(); it != j.end(); ++it)
// 		{
// 			std::cout << *it << '\n';
// 		}
// 	}
	
	//TODO: get registration data from Anchor with its location, timestamp and id and send it to redis

	// Create a subscriber object.
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
