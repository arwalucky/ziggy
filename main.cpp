
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <sqlite3.h>
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
	Database();
	
	//get previously stored anchors from redis 
	// would be useful for when the server is restarted
	auto anchors = Database::redis.command<OptionalString>("JSON.GET", "anchors");
	if (anchors)
	{
		json anchorsTemp = json::parse(*anchors);
		for (auto &[key, value] : anchorsTemp.items())
		{
			AnchorList(value["id"], value["x"], value["y"], value["timestamp"]);
		}
	}

	AnchorList("AAAA1", 2, 9, 0);
	AnchorList("AAAA2", 7, 7, 0);
	AnchorList("AAAA3", 15, 4.5, 0);
	AnchorList("AAAA4", 11, 8.33, 0);
	AnchorList("AAAA5", 6, 3, 0);
	AnchorList("AAAA6", 13, 1, 0);
	AnchorList("AAAA7", 12, 11, 0);
	AnchorList("AAAA8", 2, 4, 0);


	json temp = json::array();
	temp = AnchorList::getAnchorList();
	Database::redis.command<void>("JSON.SET", "anchors", ".", temp.dump());

	Database::redis.publish("newAnchor", "test");

	Database::redis.publish("newTag", "tagID");
	Database::redis.publish("newTag", "tagID1");

	while(true)
	{
		Database::redis.publish("newRange", "tagID;AAAA1=2;AAAA2=3;AAAA3=4;AAAA4=5;AAAA5=6;AAAA6=7;AAAA7=8");
		Database::redis.publish("newRange", "tagID1;AAAA1=2;AAAA2=3;AAAA5=6;AAAA6=7");
		usleep(100000);
	}
	MQTT_Client();

	MQTT_Client::subscribe("#");
	// TODO: get registration data from Anchor with its location, timestamp and id and send it to redis

	return 0;
}
