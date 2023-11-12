CC=g++

INC_DIR = event_handling

main: main.cpp 
	g++  main.cpp mqtt_client.cpp colours.cpp event_handling/event_manager.cpp event_handling/event_listener.cpp redis_client.cpp -lpaho-mqtt3c  -lredis++   -lhiredis -pthread