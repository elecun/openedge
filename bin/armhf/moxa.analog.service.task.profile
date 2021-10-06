{
    "md5":"603239CAF2F774498037F944F5592F52",
    "info":{
        "taskname":"moxa.analog.service.task",
        "version":"0.1.0",
        "cpu_affinity":1,
        "cycle_ns":300000000,
        "policy":{
            "check_jitter":true,
            "check_overrun":true,
            "fault_level":0
        }
    },
    "configurations":{
        "device":{
            "name":"DCAI01",
            "product":"MOXA ioLogik E1240",
            "ip":"192.168.99.165",
            "modbus_tcp":{
                "ai_address":8,
                "port":502
            },
            "ai":[
                    {"name":"AI0", "pin":0},
                    {"name":"AI1", "pin":1},
                    {"name":"AI2", "pin":2},
                    {"name":"AI3", "pin":3},
                    {"name":"AI4", "pin":4},
                    {"name":"AI5", "pin":5},
                    {"name":"AI6", "pin":6},
                    {"name":"AI7", "pin":7}
                ]

        },
        "mqtt":{
            "broker":"168.126.66.23",
            "port":1883,
            "pub_topic":"aop/uvlc/1/ai",
            "pub_qos":2,
            "sub_topic":["aop/sys/control"],
            "keep_alive":60,
            "method":"on_ai_change"
        }
    }
}