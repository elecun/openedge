{
    "md5":"603239CAF2F774498037F944F5592F52",
    "info":{
        "taskname":"device.dx3000.motor",
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
        "device":{
            "gear_ratio":3.6,
            "default_rpm":100,
            "gateway_ip":"192.168.3.166",
            "gateway_port":4001,
            "slave_id":1
        },
        "mqtt":{
            "broker":"192.168.3.130",
            "port":1883,
            "pub_topic":"aop/uvlc/1/motor",
            "pub_qos":2,
            "sub_topic":["aop/uvlc/1/motor/control"],
            "keep_alive":60
        }
    }
}