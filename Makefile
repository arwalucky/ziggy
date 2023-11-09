main: main.cpp 
	g++ -std=c++17  main.cpp mqtt_client.cpp colours.cpp -lpaho-mqtt3c  -lredis++   -lhiredis -pthread