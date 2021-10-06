
# DX3000 Motor Controller with UDP

## Specification
* Port 1 : RS485 (2-wire)
* Port 2 : RS485 (2-wire)

## MQTT Protocol
* Motor Stop
> 모터 구동 정지. emergency Stop아니고, 감속 속도설정 값에 따라 모터 정지
```
{ "command": "stop"}
```

* Motor Move(Direction : CW)
> 시계방향으로 회전
```
{ "command": "move_cw"}
```

* Motor Move(Direction : CW)
```
{ "command": "move_ccw"}
```


## Configuration
* 변경된 DI 데이터가 있을때만 publish할 경우
```
"method" : "on_di_change"
```
* 모든 데이터 주기적으로 publish 할 경우
```
"method":"on_update"
```