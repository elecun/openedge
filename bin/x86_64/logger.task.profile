{
    "md5":"603239CAF2F774498037F944F5592F52",
    "info":{
        "taskname":"logger.task",
        "version":"0.0.1",
        "cpu_affinity":1,
        "cycle_ns":10000000,
        "policy":{
            "check_jitter":true,
            "check_overrun":true,
            "fault_level":0
        }
    },
    "prepherals":{
        "port":"/dev/i2c-2",
        "sensor-1":{
            "use":"INA3221",
            "address":"0x40"
        },
        "sensor-2":{
            "use":"INA3221",
            "address":"0x41"
        }
    }
}