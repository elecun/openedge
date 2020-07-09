# OpenEdge
Open Source Application Gateway Middleware for Edge Computing

# Features

# Pre-requisites
If you build this source on your host (e.g. Ubuntu), you probably should install libraries

* 

# Supporting Tasks & Services
* aop10t.pilot.task
  - This RT Task would be used for pilot plant of AOP (10ton/day scale)
  - It depends on 2 services : lsis.fenet.connector.service, mqtt.publisher.service

# Dependency
This Software depends on other open sources

* spdlog : fast c++ logging library (https://github.com/gabime/spdlog.git)


# Build
Clone the Openedge source from github repository.
```
$ git clone https://github.com/elecun/openedge.git
$ cd ./openedge
$ make ARCH=arm
```

build all the source codes.
```
$ make all
```

If you only want to build the openedge runner,
```
$ make edge
```

for only tasks
```
$ make tasks
```

for only services
```
$ make services
```
