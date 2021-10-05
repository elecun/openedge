{
    "md5":"603239CAF2F774498037F944F5592F52",
    "info":{
        "taskname":"agw.manage.task",
        "version":"0.0.1",
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
            "name":"DCRIO01",
            "product":"MOXA ioLogik E1212",
            "ip":"192.168.99.163",
            "modbus_tcp":{
                "di_address":48,
                "do_address":0,
                "port":502
            },
            "di":[
                    {"name":"DI0", "pin":0},
                    {"name":"DI1", "pin":1},
                    {"name":"DI2", "pin":2},
                    {"name":"DI3", "pin":3},
                    {"name":"DI4", "pin":4},
                    {"name":"DI5", "pin":5},
                    {"name":"DI6", "pin":6},
                    {"name":"DI7", "pin":7}
                ],
            "do":[
                    {"name":"DO0", "pin":0},
                    {"name":"DO1", "pin":1},
                    {"name":"DO2", "pin":2},
                    {"name":"DO3", "pin":3},
                    {"name":"DO4", "pin":4},
                    {"name":"DO5", "pin":5},
                    {"name":"DO6", "pin":6},
                    {"name":"DO7", "pin":7}
            ]

        },
        "mqtt":{
            "broker":"192.168.3.105",
            "port":1883,
            "pub_topic":"aop/uvlc/1/io",
            "pub_qos":2,
            "sub_topic":["aop/sys/control", "aop/uvlc/1/io/control"],
            "keep_alive":60
        }
    }
}