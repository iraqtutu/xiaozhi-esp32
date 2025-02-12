## 一、版本检查(伴随MQTT服务器查询)
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


## 二、打开通道
### 1、向MQTT服务器发送hello申请udp通道
客户端MQTT发送:
<details>
<summary>点击展开客户端发送数据</summary>

```json
{
    "type": "hello",
    "version": 3,
    "transport": "udp",
    "audio_params": {
        "format": "opus",
        "sample_rate": 16000,
        "channels": 1,
        "frame_duration": 60
    }
}
```

</details>

### 2、等待服务器的Hello回复(UDP通道信息)

### 3、接收到MQTT消息(UDP通道信息)
接受的服务器返回数据：
<details>
<summary>点击展开服务器返回数据</summary>
标题：devices/p2p/GID_test@@@d8_3b_da_4e_00_9c
内容：udp信息及session_id

```json
{
    "type": "hello",
    "version": 3,
    "transport": "udp",
    "udp": {
        "server": "120.24.160.13",
        "port": 8884,
        "encryption": "aes-128-ctr",
        "key": "f19f3eeaa601d614130f4384342d994a",
        "nonce": "01000000d83bda4e009c928200000000"
    },
    "audio_params": {
        "format": "opus",
        "sample_rate": 24000,
        "channels": 1,
        "frame_duration": 60
    },
    "session_id": "555e2743"
}
```

</details>

## 完整的一次对话
### 1、MQTT: 客户端发送消息注册Iot Speaker
<details>
<summary>点击展开客户端发送数据</summary>

```json
{
    "session_id": "555e2743",
    "type": "iot",
    "descriptors": [
        {
            "name": "Speaker",
            "description": "当前 AI 机器人的扬声器",
            "properties": {
                "volume": {
                    "description": "当前音量值",
                    "type": "number"
                }
            },
            "methods": {
                "SetVolume": {
                    "description": "设置音量",
                    "parameters": {
                        "volume": {
                            "description": "0到100之间 的整数",
                            "type": "number"
                        }
                    }
                }
            }
        }
    ]
}
```
</details>



#IF 语音唤醒(非必需)
I (1628290) MQTT: UDP发送音频数据: 105
I (1628430) MQTT: 发送消息: {"session_id":"2cfa0edf","type":"listen","state":"detect","text":"你好小智"}
I (1628470) MQTT: 发送消息: {"session_id":"2cfa0edf","type":"iot","states":[{"name":"Speaker","state":{"volume":50}}]}
I (1628540) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"stt","text":"小智","session_id":"2cfa0edf"}
#ENDIF

如果设备状态变化，则发送消息给服务器
I (1628470) MQTT: 发送消息: {"session_id":"2cfa0edf","type":"iot","states":[{"name":"Speaker","state":{"volume":50}}]}
I (41830) MQTT: 发送消息: {"session_id":"113b467d","type":"listen","state":"start","mode":"auto"}

I (41880) MQTT: 发送消息: {"session_id":"113b467d","type":"iot","states":[{"name":"Speaker","state":{"volume":50}}]}
I (41970) MQTT: UDP发送音频数据: 85
I (44670) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"stt","text":"再见。","session_id":"113b467d"}


I (44670) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"tts","state":"start","session_id":"113b467d"}


I (45000) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"llm","text":"😊","emotion":"happy","session_id":"113b467d"}     
I (45180) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"tts","state":"sentence_start","text":"好的，再见！","session_id":"113b467d"}
I (45340) UDP: 收到音频数据: 157
I (46060) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"tts","state":"sentence_end","text":"好的，再见！","session_id":"113b467d"}

I (46070) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"tts","state":"sentence_start","text":"希望你今天过得愉快。","session_id":"113b467d"}
I (46340) UDP: 收到音频数据: 174
I (47930) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"tts","state":"sentence_end","text":"希望你今天过得愉快。","session_id":"113b467d"}


I (47940) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"tts","state":"stop","session_id":"113b467d"}


I (47950) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"goodbye","session_id":"113b467d"}


I (48790) MQTT: 发送消息: {"session_id":"113b467d","type":"listen","state":"start","mode":"auto"}

### 4、UDP: 发送音频数据

### 5、MQTT: 发送唤醒词消息
<details>
<summary>点击展开客户端发送数据</summary>

```json
{
    "session_id": "ce222a20",
    "type": "listen",
    "state": "detect",
    "text": "你好小智"
}
```
</details>

### 6、MQTT: 发送客户端状态
<details>
<summary>点击展开客户端发送数据</summary>

```json
{
    "session_id": "ce222a20",
    "type": "iot",
    "states": [
        {
            "name": "Speaker",
            "state": {
                "volume": 70
            }
        }
    ]
}
```
</details>

### 收到 stt 消息
I (14320) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"stt","text":"小智","session_id":"997a6577"}

### 收到 tts -start消息
I (14330) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"tts","state":"start","session_id":"997a6577"}
I (14360) MQTT: UDP发送音频数据: 85
I (14370) Application: STATE: speaking
I (14390) MQTT: UDP发送音频数据: 91

### 收到 llm 消息
I (14720) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"llm","text":"😊","emotion":"happy","session_id":"997a6577"}

### 收到 tts -sentence_start消息
I (15050) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"tts","state":"sentence_start","text":"你好呀，找我有什么事吗？","session_id":"997a6577"}     

### 收到 UDP音频数据
I (15240) UDP: 收到音频数据: 159
I (15240) UDP: 收到音频数据: 170
I (16390) UDP: 收到音频数据: 93
I (16390) UDP: 收到音频数据: 93
I (16390) UDP: 收到音频数据: 93

### 收到 tts -sentence_end消息
I (16430) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"tts","state":"sentence_end","text":"你好呀，找我有什么事吗？","session_id":"997a6577"}

### 收到 tts -stop消息
I (16440) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"tts","state":"stop","session_id":"997a6577"}