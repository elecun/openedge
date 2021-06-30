{
    "md5":"603239CAF2F774498037F944F5592F52",
    "info":{
        "taskname":"dx3000.control.task",
        "version":"0.0.1",
        "cpu_affinity":1,
        "cycle_ns":1000000000,
        "policy":{
            "check_jitter":true,
            "check_overrun":true,
            "fault_level":0
        }
    },
    "configurations":{
        "mqtt":{
            "broker":"192.168.100.71",
            "port":1883,
            "pub_topic":"aop/uvlc/motor",
            "pub_qos":2,
            "sub_topic":["pcan.mqtt.task/sysctrl", "aop/uvlc/control"],
            "keep_alive":60
        },
        "dx3000":{
            "access":"/dev/ttyUSB0",
            "baudrate":9600,
            "slave_id":1,
            "rpm":100
        }
    }
}