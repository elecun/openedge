# OpenEdge
Open Source Application Gateway Middleware for Edge Computing

# Features
* Component-based Architecture (towards microservice architecture)
* Runs on Realtime Operating System(Realtime Linux)

# Pre-requisites
If you build this source on your host (e.g. Ubuntu), you probably should install libraries

* $ sudo apt-get install make gcc-8-arm-linux-gnueabihf g++-8-arm-linux-gnueabihf libczmq-dev sqlite3

# Supporting Tasks & Services
* aop10t.pilot.task
  - This RT Task would be used for pilot plant of AOP (10ton/day scale)
  - It depends on 2 services : lsis.fenet.connector.service, mqtt.publisher.service

* lsis.fenet.connector.service
  - this service would be required for aop10t.pilot.task to connect with LSIS PLC using its dedicated communication protocol


# Dependency
This software will be required other open source libraries for some tasks and services.

* spdlog : fast c++ logging library (https://github.com/gabime/spdlog.git)
* libsockpp : modern c++ socket library (https://github.com/fpagliughi/sockpp) - optional (required for some components)
* libbson : parse BSON document, it is required for MongoDB C Driver
* libmosquitto, libmosquittopp : communication with MQTT
* libmongoc : Mongo DB C driver
* libczmq-dev : Zero MQ C Binding Library (version 4.2.0)


# Build
Clone the Openedge source from github repository.
```
$ git clone https://github.com/elecun/openedge.git
$ cd ./openedge

# for ARM architecture
$ make ARCH=armhf

# for X86_64 architecture
$ make ARCH=x86_64
```

If you only want to build the openedge runner,
```
$ make edge -j
```

for only tasks
```
$ make tasks -j
```

for only services
```
$ make services -j
```


# Configurations for AOP Project