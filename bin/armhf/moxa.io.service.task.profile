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
            "name":"DCRIO02",
            "product":"MOXA ioLogik E1212",
            "ip":"192.168.100.164",
            "modbus_tcp":{
                "di_address":48,
                "do_address":0,
                "port":502
            },
            "di":[
                    {"name":"emergency_reset", "pin":1},
                    {"name":"proximity_1", "pin":2},
                    {"name":"proximity_2", "pin":3},
                    {"name":"mc_wipe_once", "pin":4},
                    {"name":"mc_wipe_forward", "pin":5},
                    {"name":"mc_wipe_reverse", "pin":6},
                    {"name":"mc_wipe_stop", "pin":7}
                ],
            "do":[
                    {"name":"emergency", "pin":0},
                    {"name":"working", "pin":1},
                    {"name":"over_current", "pin":2}
            ]

        },
        "mqtt":{
            "broker":"192.168.3.105",
            "port":1883,
            "pub_topic":"aop/uvlc/2/io",
            "pub_qos":2,
            "sub_topic":["aop/sys/control", "aop/uvlc/io/control"],
            "keep_alive":60
        }
    }
}