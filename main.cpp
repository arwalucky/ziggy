
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
#include "sqlite3.hpp" 



int main()
{
	Database();

	// get previously stored anchors from redis
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
	else 
	{
		Database::redis.command<void>("JSON.SET", "anchors", ".", json::object().dump());
	}

	MQTT_Client();
	MQTT_Client::subscribe("#");

	// TODO: get registration data from Anchor with its location, timestamp and id and send it to redis

	return 0;
}
