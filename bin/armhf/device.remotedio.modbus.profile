{
    "md5":"603239CAF2F774498037F944F5592F52",
    "info":{
        "taskname":"device.remotedio.modbus",
        "version":"0.0.1",
        "cpu_affinity":1,
        "cycle_ns":100000000,
        "policy":{
            "check_jitter":true,
            "check_overrun":true,
            "fault_level":0
        }
    },
    "configurations":{
        "mqtt":{
            "broker":"168.126.66.23",
            "port":1883,
            "pub_topic":"aop/uvlc/1",
            "pub_qos":2,
            "sub_topic":["aop/sys/sysctrl", "aop/uvlc/1/control", "aop/uvlc/1/io"],
            "keep_alive":60,
            "method":"on_update"
        }
    }
}