# plc.daq.task

## Note
 * Task는 사전에 모든 서브모듈이나 인터페이스가 결정되어 있음. 일종의 Application.
 * 하지만, 서브모듈을 특정하지는 않도록 함. (확장성)
 * 하나의 Task는 여러개의 Service(서브모듈)를 가질 수 있음.
 * plc.daq.task : PLC로부터 데이터를 주기적으로 가져오는 Realtime Task (rt_task::runnable I/F)
 * data가 수집되면서  subscribe하는 task가 있으면 data를 공유함. (Shared Memmory)

## Submodule (required)
 * plc.general.service
   * 일반적인 plc 인터페이스 서비스를 가짐.
   * 해당 plc와는 특정하지 않은 bus 인터페이스를 가지도록 함.
     * LS산전 PLC의 경우, Modbus TCP나 XGT전용 프로토콜을 사용함.(IEEE 802.3 표준 기반)