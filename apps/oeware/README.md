# OEWare
OpenEdge Ware works as Service Launcher \
This software aims to manage the RT task(components or packages) on target application gateway device.

# Features
* 하위에 연결된 장치(Edge Node)들로부터 자신의 프로파일이 전달되면, 서버로부터 적합한 Service Component(Package)를 Request하여 받아오고, OEWare에 등록하고 서비스 상태를 관리함. 서비스 중재나 실행의 역할은 하지 않으며, 오로지 컴포넌트들을 관리함.

# Program Options

* -c, --config  : with configuration file
```
$ oeware --config config.json
```
