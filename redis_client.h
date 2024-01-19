#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <iostream>
#include <algorithm>
#include "MQTTClient.h"
#include "anchorList.hpp"

#include <sw/redis++/redis++.h>
using namespace sw::redis;

class Database 
{
private:
public:
    static sw::redis::Redis redis;

    Database();
    static void setJSON(std::string key, json message);
    static json getJSON(std::string key);


};

#endif