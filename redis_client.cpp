#include "redis_client.h"

sw::redis::Redis Database::redis = Redis("tcp://localhost:6379");


Database::Database()
{};
