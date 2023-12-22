# Get MQTT client





# Get Redis + client

https://github.com/sewenew/redis-plus-plus

## Redis-plus-plus is based on Hiredis: 

``sudo apt-get install libhiredis-dev``


## Installing redis-plus-plus
`` 
git clone https://github.com/sewenew/redis-plus-plus.git

cd redis-plus-plus

mkdir build

cd build

cmake ..

make

make install

cd ..
``




How the sytem works:: 


Start RedisJSON in a docker container
Start MQTT broker at 1883
