#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <iostream>
#include <algorithm>
#include "MQTTClient.h"
#include "anchorList.hpp"

#include <sw/redis++/redis++.h>
using namespace sw::redis;

#define DATABASE_H

class Database 
{
private:
public:
    static sw::redis::Redis redis;

    Database();
    static void setData(std::string key, std::string message);
    static json getData();
};

#endif