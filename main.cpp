
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

MQTTClient_deliveryToken deliveredToken;

int messageArrived(void *context, char *topicName, int topicLength, MQTTClient_message *message);
void messageDelivered(void *context, MQTTClient_deliveryToken token);
void connectionLost(void *context, char *reason);

void handleRegisterAnchor(MQTTClient_message *message);
void handlePositionAnchor(MQTTClient_message *message);
void handleDeregisterTag(MQTTClient_message *message);

std::string parseData(std::string str, std::string FLAG);


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
	{
		Database::setJSON("anchors", json::object().dump());
		std::cout << "No anchors found" << std::endl;
	}

	// MQTT settings
	MQTT();
	MQTT::setCallbacks(connectionLost, messageArrived, messageDelivered);
	MQTT::subscribe("#");
	// TODO: get registration data from Anchor with its location, timestamp and id and send it to redis

	return 0;
}

void connectionLost(void *context, char *reason)
{
	printf("Connection Lost\n");
	printf("Reason: %s\n", reason);
}

void messageDelivered(void *context, MQTTClient_deliveryToken token)
{
	deliveredToken = token;
}

std::string parseData(std::string str, std::string FLAG)
{
	std::string s = str;
	std::string delimiter = ";";

	int counter = 0;
	std::string str1;

	if (FLAG == "TIMESTAMP")
	{
		counter = 3;
	}
	else if (FLAG == "DISTANCE" || FLAG == "COOR_Y")
	{
		counter = 2;
	}
	else if (FLAG == "TAGID" || FLAG == "COOR_X")
	{
		counter = 1;
	}
	else if (FLAG == "ANCHORID")
	{
		counter = 0;
	}
	else
	{
		std::cout << "FLAG not found" << std::endl;
		return 0;
	}

	size_t pos = 0;
	std::string token;
	while ((pos = s.find(delimiter)) != std::string::npos)
	{
		if (counter == 0)
		{
			str1 = s.substr(0, pos);
			s.erase(0, pos + delimiter.length());
		}
		s.erase(0, pos + delimiter.length());
		counter--;
	}

	return str1;
}

int messageArrived(void *context, char *topicName, int topicLength, MQTTClient_message *message)
{
	if (std::string_view{topicName, 16} == "register/Anchor/")
	{
		handleRegisterAnchor(message);
	}
	else if (std::string_view{topicName, 15} == "position/Anchor")
	{
		handlePositionAnchor(message);
	}
	else if (std::string_view{topicName, 14} == "deregister/Tag")
	{
		handleDeregisterTag(message);
	}

	MQTTClient_freeMessage(&message);
	MQTTClient_free(topicName);
	return 1;
}

// Example of breaking up the function
void handleRegisterAnchor(MQTTClient_message *message)
{

	if (message->payloadlen < 1)
	{
		std::cout << "Retained message removed" << std::endl;
		return;
	}

	std::string s = (char *)message->payload;
	std::string anchor_id = parseData(s, "ANCHORID");
	std::string coor_x = parseData(s, "COOR_X");
	std::string coor_y = parseData(s, "COOR_Y");
	std::string timestamp_str = parseData(s, "TIMESTAMP");

	float x, y;
	int timestamp;

	try
	{
		x = std::stof(coor_x);
		y = std::stof(coor_y);
		timestamp = std::stof(timestamp_str);
	}
	catch (const std::invalid_argument &ia)
	{
		std::cerr << "Invalid argument: " << ia.what() << '\n';
		return;
	}
	catch (const std::out_of_range &oor)
	{
		std::cerr << "Out of Range error: " << oor.what() << '\n';
		return;
	}

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
		MQTT::publish("accepted", "register/AnchorACK", 0);
		return;
	}

	AnchorList(anchor_id, x, y, timestamp);

	anchors = AnchorList::getAnchorList();
	Database::setJSON("anchors", anchors.dump());
	Database::redis.publish("newAnchor", anchor_id);
	SQLite3::insertAnchor(anchor_id.c_str(), coor_x.c_str(), coor_y.c_str(), timestamp_str.c_str());

	MQTT::publish("accepted", "register/AnchorACK", 0);
	SQLite3::insertAnchor(anchor_id.c_str(), coor_x.c_str(), coor_y.c_str(), timestamp_str.c_str());

	std::cout << "Anchor registered" << std::endl;

	return;
}

void handlePositionAnchor(MQTTClient_message *message)
{
	std::string s = (char *)message->payload;

	std::string timestamp = parseData(s, "TIMESTAMP");
	std::string anchor_id = parseData(s, "ANCHORID");
	std::string tag_id = parseData(s, "TAGID");
	std::string distance = parseData(s, "DISTANCE");

	if (!(TagList::isInList(tag_id)))
	{
		std::cout << "New tag added" << std::endl;
		TagList(tag_id, json::array(), timestamp);

		// immediate tag update
		Database::redis.publish("newTag", tag_id);

		// in case tendi live is started after a tag was registered by Ziggy
		json tags = TagList::getTagList();
		Database::setJSON("tags", tags.dump());

		SQLite3::insertTag(tag_id.c_str(), timestamp.c_str());
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
	}
}

void handleDeregisterTag(MQTTClient_message *message)
{


	if (message->payloadlen < 1)
	{
		std::cout << "Retained message removed" << std::endl;
		return;
	}
	std::cout << "deregister/Tag" << std::endl;
	std::string s = (char *)message->payload;
	std::string timestamp = parseData(s, "TIMESTAMP");
	std::string anchor_id = parseData(s, "ANCHORID");
	std::string tag_id = parseData(s, "TAGID");

	std::string topic = "deregister/Tag/" + anchor_id + tag_id;
	MQTT::publish("", topic.c_str(), 1);

	TagList::removeAnchor(tag_id, anchor_id);

	json temp = TagList::getAnchorListForTag(tag_id);
	if (!temp.size())
	{
		Database::redis.publish("removeTag", tag_id);
	}
}
