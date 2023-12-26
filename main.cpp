
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <sqlite3.h>
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

int messageArrived(void *context, char *topicName, int topicLength, MQTTClient_message *message);
void messageDelivered(void *context, MQTTClient_deliveryToken token);
void connectionLost(void *context, char *reason);
void checkAndAcknowledgeAnchor(MQTTClient_message *data);

int main()
{
	Database();

	// get previously stored anchors from redis
	// would be useful for when the server is restarted
	// if none found upload an empty list
	json anchors = Database::getJSON("anchors");
	if (anchors.size() > 0)
	{
		for (auto &[key, value] : anchors.items())
		{
			AnchorList(value["id"], value["x"], value["y"], value["timestamp"]);
		}
	}
	else
		Database::setJSON("anchors", json::object().dump());


	std::cout << "test" << std::endl;
	// MQTT settings
	MQTT();
	MQTT::setCallbacks(connectionLost, messageArrived, messageDelivered);
	MQTT::subscribe("#");
	// TODO: get registration data from Anchor with its location, timestamp and id and send it to redis

	return 0;
}

void connectionLost(void *context, char *reason)
{
	printf("ConnectionLost\n");
	printf("Reason: %s\n", reason);
}

void messageDelivered(void *context, MQTTClient_deliveryToken token)
{
	printf("Message delivered with token: %d\n", token);
	deliveredToken = token;
}

int messageArrived(void *context, char *topicName, int topicLength, MQTTClient_message *message)
{

	if (strcmp(topicName, (char *)"register/Anchor") == 0)
	{
		std::cout << (char *)message->payload << std::endl;
		checkAndAcknowledgeAnchor(message);
	}
	else if (strcmp(topicName, (char *)"position/Anchor") == 0)
	{

		std::string s = (char *)message->payload;

		std::string timestamp = parseData(s, "TIMESTAMP");
		std::string anchor_id = parseData(s, "ANCHORID");
		std::string tag_id = parseData(s, "TAGID");
		std::string distance = parseData(s, "DISTANCE");

		// check if anchor is in list add or update

		if (!(TagList::isInList(tag_id)))
		{
			std::cout << "Tag not in list" << std::endl;
			TagList(tag_id, json::array(), timestamp);

			//immediate tag update
			Database::redis.publish("newTag", tag_id);
			// in case tendi live is started after a tag was registered by Ziggy

			json tags = TagList::getTagList();

			for (auto &[key, value] : tags.items())
			{
				std::cout << value["id"] << std::endl;
			}
			Database::setJSON("tags", tags.dump());
			
			SQLite3::insertTag(tag_id.c_str(), timestamp.c_str());

		}
		else
		{
			TagList::addAnchor(tag_id, anchor_id, distance);

			std::string data = tag_id;

			json temp = TagList::getTagList();
			for (auto &[key, value] : temp.items())
			{
				if (value["id"] == tag_id)
				{
					for (auto &[key1, value1] : value["anchors"].items())
					{
						data += ";" + value1["anchorID"].dump() + "=" + value1["distance"].dump();
					}
				}
			}
			data.erase(
				remove(data.begin(), data.end(), '\"'),
				data.end());
			std::cout << data << std::endl;

			Database::redis.publish("newRange", data);
			SQLite3::addRange(tag_id.c_str(), anchor_id.c_str(), distance.c_str(), timestamp.c_str());
		}
	}
	else if (strcmp(topicName, (char *)"register/Tag") == 0)
	{
		std::cout << "tag registration" << std::endl;
	}

	MQTTClient_freeMessage(&message);
	MQTTClient_free(topicName);
	return 1;
}

void checkAndAcknowledgeAnchor(MQTTClient_message *data)
{
	std::string s = (char *)data->payload;

	std::string anchor_id = parseData(s, "ANCHORID");
	std::string coor_x = parseData(s, "COOR_X");
	float x  = std::stof(coor_x);
	std::string coor_y = parseData(s, "COOR_Y");
	float y  = std::stof(coor_y);
	std::string timestamp = parseData(s, "TIMESTAMP");

	json anchors = AnchorList::getAnchorList();
	bool found = false;

	for (auto &[key, value] : anchors.items())
	{
		if (value["id"] == anchor_id)
		{
			found = true;
		}
	}
	if (found)
	{
		MQTT::publish("accepted", "register/AnchorACK", 1);
		return;
	}

	// TODO: make dynamic
	AnchorList(anchor_id, x, y, timestamp);

	anchors = AnchorList::getAnchorList();
	Database::setJSON("anchors", anchors.dump());
	SQLite3::insertAnchor(anchor_id.c_str(), coor_x.c_str(), coor_y.c_str(), timestamp.c_str());

	MQTT::publish("registered", "register/AnchorACK", 1);

	return;
}