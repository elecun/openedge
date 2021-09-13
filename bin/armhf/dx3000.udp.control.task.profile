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
            "broker":"192.168.3.105",
            "port":1883,
            "pub_topic":"aop/uvlc/motor",
            "pub_qos":2,
            "sub_topic":["aop/sys/control", "aop/uvlc/motor/control"],
            "keep_alive":60
        },
        "dx3000":{
            "default_rpm":1000,
            "port":4002,
            "target":"192.168.100.166",
            "slave_id":1
        }
    }
}