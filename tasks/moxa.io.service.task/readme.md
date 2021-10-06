
# MOXA IO Logik E1212 Device Component

## Specification
* Digital Input channels : 0~7
* Diguial In/Out channels : 0~7 (내부 Jumper로 In 또는 Out 변경 가능함)

## MQTT Protocol
* DI & DO Read
```
{ "di":{"DI0":false, ...}, "do":{"DO0":false, ...}}
```

* DO Write
> unsigned short로 각 비트마다 포트가 할당되어 있음. (DO0=1, DO1=2, DO2=4,...)
```
{ "DO":1}
```
> OR 연산을 포함하는 DO값 설정. 기존 값에 OR연산을 취하여 할당함.
```
{ "DO":1, "operator":"OR"}
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