
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

float r1 = std::rand() % 350;
float r2 = std::rand() % 200;
float r3 = std::rand() % 130;
float r4 = std::rand() % 150;
float r5 = std::rand() % 100;

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

	SQLite3::insertAnchor("32523", "3", "5", "1703869755");
	SQLite3::insertAnchor("32423", "4", "5", "1703869755");
	SQLite3::insertAnchor("23452", "2", "7", "1703869755");
	SQLite3::insertAnchor("23445", "3", "0", "1703869755");
	SQLite3::insertAnchor("23432", "5", "6", "1703869755");
	SQLite3::insertAnchor("32452", "1", "9", "1703869755");

	SQLite3::SQLselect("*", "ANCHORS", NULL);

	std::cout << "---------------------" << std::endl;
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
		if (message->payloadlen < 1)
		{
			return 1;
		}
		checkAndAcknowledgeAnchor(message);
	}
	else if (strcmp(topicName, (char *)"position/Anchor") == 0)
	{
		std::string s = (char *)message->payload;

		std::string timestamp = parseData(s, "TIMESTAMP");
		std::string anchor_id = parseData(s, "ANCHORID");
		std::string tag_id = parseData(s, "TAGID");
		std::string distance = parseData(s, "DISTANCE");

		if (!(TagList::isInList(tag_id)))
		{
			std::cout << "Tag not in list" << std::endl;
			TagList(tag_id, json::array(), timestamp);

			// immediate tag update
			Database::redis.publish("newTag", tag_id);

			// in case tendi live is started after a tag was registered by Ziggy
			json tags = TagList::getTagList();
			Database::setJSON("tags", tags.dump());

			SQLite3::insertTag(tag_id.c_str(), timestamp.c_str());
			SQLite3::SQLselect("*", "tags", NULL);
		}
		else
		{
			TagList::updateRange(tag_id, anchor_id, distance);
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

			Database::redis.publish("newRange", data);

			r1 = r1 + 1;
			std::string s1 = std::to_string(r1);
			r2 = r2 + 1;
			std::string s2 = std::to_string(r2);
			r3 = r3 + 1;
			std::string s3 = std::to_string(r3);
			r4 = r4 + 1;
			std::string s4 = std::to_string(r4);
			r5 = r5 + 1;
			std::string s5 = std::to_string(r5);

			// one tag`s, 5 ranges

			SQLite3::addRange(tag_id.c_str(), anchor_id.c_str(), distance.c_str(), timestamp.c_str());
			SQLite3::addRange(tag_id.c_str(), "32523", s1.c_str(), timestamp.c_str());
			SQLite3::addRange(tag_id.c_str(), "32423", s2.c_str(), timestamp.c_str());
			SQLite3::addRange(tag_id.c_str(), "23445", s3.c_str(), timestamp.c_str());
			SQLite3::addRange(tag_id.c_str(), "23432", s4.c_str(), timestamp.c_str());
			SQLite3::addRange(tag_id.c_str(), "32452", s5.c_str(), timestamp.c_str());

			std::cout << "working" << std::endl;
		}
	}
	else if (strcmp(topicName, (char *)"deregister/Tag") == 0)
	{

		std::cout << "deregister/Tag" << std::endl;
		std::string s = (char *)message->payload;
		std::string timestamp = parseData(s, "TIMESTAMP");
		std::string anchor_id = parseData(s, "ANCHORID");
		std::string tag_id = parseData(s, "TAGID");

		TagList::removeAnchor(tag_id, anchor_id);

		json temp = TagList::getAnchorListForTag(tag_id);
		if (!temp.size())
		{
			Database::redis.publish("removeTag", tag_id);
		}
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
	float x = std::stof(coor_x);
	std::string coor_y = parseData(s, "COOR_Y");
	float y = std::stof(coor_y);
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
		SQLite3::insertAnchor(anchor_id.c_str(), coor_x.c_str(), coor_y.c_str(), timestamp.c_str());
		return;
	}

	// TODO: make dynamic
	AnchorList(anchor_id, x, y, timestamp);

	anchors = AnchorList::getAnchorList();
	Database::setJSON("anchors", anchors.dump());
	SQLite3::insertAnchor(anchor_id.c_str(), coor_x.c_str(), coor_y.c_str(), timestamp.c_str());

	MQTT::publish("registered", "register/AnchorACK", 1);
	SQLite3::insertAnchor(anchor_id.c_str(), coor_x.c_str(), coor_y.c_str(), timestamp.c_str());

	std::cout << "Anchor registered" << std::endl;

	return;
}