# OEWare
OpenEdge Ware works as Service Launcher \
This software aims to manage the RT task(components or packages) on target application gateway device.

# Features
* 하위에 연결된 장치(Edge Node)들로부터 자신의 프로파일이 전달되면, 서버로부터 적합한 Service Component(Package)를 Request하여 받아오고, OEWare에 등록하고 서비스 상태를 관리함. 서비스 중재나 실행의 역할은 하지 않으며, 오로지 컴포넌트들을 관리함.

# Functions
## on Gateway
- device management : 하위에 연결된 장치들을 관리
- service request : 연결된 장치에서 요청하는 서비스들을 서버에 요구하여 설치
- task monitoring : 구동되는 서비스(task)들이 realtime이 필요하다면 task의 time 
monitoring
- system status monitoring : cpu, memory, working process...

## on Server
- 

# Todo
1. define task interface, create dummy task(thread) example
2. MQTT-IPC
3. taks container(repository)

# Program Options

* -c, --config  : with configuration file
```
$ oeware --config config.json
```
