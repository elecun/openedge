{
    "md5":"603239CAF2F774498037F944F5592F52",
    "info":{
        "taskname":"agw.manage.task",
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
            "name":"MOXA ioLogik e1212",
            "ip":"192.168.100.164",
            "modbus_tcp":{
                "di_address"
            },
            "di":[
                    {"name":"proximity_1", "pin":2},
                    {"name":"proximity_2", "pin":3},
                    {"name":"mc_wipe_once", "pin":4},
                    {"name":"mc_wipe_forward", "pin":5},
                    {"name":"mc_wipe_reverse", "pin":6},
                    {"name":"mc_wipe_stop", "pin":7}
                ],
            "do":[
                    {"name":"working", "pin":0},
                    {"name":"over_current", "pin":1}
            ]

        },
        "network":{
            "dataport":4002
        }
    }
}