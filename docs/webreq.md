## 版本检查(伴随MQTT服务器查询)
POST:https://api.tenclass.net/xiaozhi/ota/

<details>
<summary>点击展开请求数据</summary>

```json
{
    "flash_size": 16777216,
    "minimum_free_heap_size": 8319916,
    "mac_address": "d8:3b:da:4e:00:9c",
    "chip_model_name": "esp32s3",
    "chip_info": {
        "model": 9,
        "cores": 2,
        "revision": 2,
        "features": 18
    },
    "application": {
        "name": "xiaozhi",
        "version": "1.0.1",
        "compile_time": "Feb  2 2025T00:07:45Z",
        "idf_version": "v5.4",
        "elf_sha256": "91c01b66abc63509b1fdadbbb1c00c1020de6af90c364ca0f7260cd61f30c130"
    },
    "partition_table": [
        {
            "label": "nvs",
            "type": 1,
            "subtype": 2,
            "address": 36864,
            "size": 16384
        },
        {
            "label": "otadata",
            "type": 1,
            "subtype": 0,
            "address": 53248,
            "size": 8192
        },
        {
            "label": "phy_init",
            "type": 1,
            "subtype": 1,
            "address": 61440,
            "size": 4096
        },
        {
            "label": "model",
            "type": 1,
            "subtype": 130,
            "address": 65536,
            "size": 983040
        },
        {
            "label": "ota_0",
            "type": 0,
            "subtype": 16,
            "address": 1048576,
            "size": 6291456
        },
        {
            "label": "ota_1",
            "type": 0,
            "subtype": 17,
            "address": 7340032,
            "size": 6291456
        }
    ],
    "ota": {
        "label": "ota_0"
    },
    "board": {
        "type": "lichuang-dev",
        "ssid": "chenyuyang",
        "rssi": -61,
        "channel": 2,
        "ip": "192.168.31.226",
        "mac": "d8:3b:da:4e:00:9c"
    }
}
```
</details>

<details>
<summary>返回服务器最新版本以及MQTT服务器信息,点击展开响应数据</summary>

```json
 {
    "mqtt": {
        "endpoint": "post-cn-apg3xckag01.mqtt.aliyuncs.com",
        "client_id": "GID_test@@@d8_3b_da_4e_00_9c",
        "username": "Signature|LTAI5tF8J3CrdWmRiuTjxHbF|post-cn-apg3xckag01",
        "password": "BSRV6VwHfYNkxAAEmhXLbf7pHZ4=",
        "publish_topic": "device-server",
        "subscribe_topic": "devices/d8_3b_da_4e_00_9c"
    },
    "server_time": {
        "timestamp": 1738915106357,
        "timezone": "Asia/Shanghai",
        "timezone_offset": -480
    },
    "firmware": {
        "version": "1.0.1",
        "url": ""
    }
}
```
</details>

