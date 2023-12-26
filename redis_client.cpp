#include "redis_client.h"

sw::redis::Redis Database::redis = Redis("tcp://localhost:6379");


Database::Database()
{};

void Database::setJSON(std::string key, json message)
{
	redis.command<void>("JSON.SET", key, ".", message);
}

json Database::getJSON(std::string key)
{
    auto response = redis.command<OptionalString>("JSON.GET", key);
    if (!response)
    {
        std::cout << "No data found" << std::endl;
        return json::object();
    }
    json data = json::parse(*response);
    return data;
}