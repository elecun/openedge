{
    "md5":"603239CAF2F774498037F944F5592F52",
    "info":{
        "taskname":"aop.uvlc.logic",
        "version":"0.0.1",
        "cpu_affinity":1,
        "cycle_ns":200000000,
        "policy":{
            "check_jitter":true,
            "check_overrun":true,
            "fault_level":0
        }
    },
    "configurations":{
        "iomap":{
            "l_proximity_in":"DI2",
            "r_proximity_in":"DI3",
            "wipe_forward_in":"DI4",
            "wipe_backward_in":"DI5",
            "wipe_stop_in":"DI6",
            "emergency_in":"DI7",
            "wipe_forward_out":"DO5",
            "wipe_backward_out":"DO6",
            "wipe_stop_out":"DO7",
            "emergency_out":"DO4"
        },
        "mqtt":{
            "broker":"192.168.3.8",
            "port":1883,
            "pub_topic":"aop/uvlc/1/motor/control",
            "pub_qos":2,
            "sub_topic":["aop/uvlc/1/control", "aop/uvlc/1/dio"],
            "keep_alive":60
        }
    }
}