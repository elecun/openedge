
# MOXA IO Logik E1212 Device Component

## Specification
* Analog Input channels : 0~7

## MQTT Protocol
* AI Read
```
{ "ai":{"AI0":<float number>, ...}}
```


## Configuration
* 변경된 AI 데이터가 있을때만 publish할 경우
```
"method" : "on_ai_change"
```
* 모든 데이터 주기적으로 publish 할 경우
```
"method":"on_update"
```