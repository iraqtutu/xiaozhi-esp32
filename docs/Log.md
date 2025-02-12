开始聆听时清空播放队列
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

I (13750) MQTT: 打开音频通道
## 发送 hello 消息申请udp通道
### 发送 hello 消息
I (13750) MQTT: 发送消息: {"type":"hello","version": 3,"transport":"udp","audio_params":{"format":"opus", "sample_rate":16000, "channels":1, "frame_duration":60}}

### 收到服务器消息
I (13860) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"hello","version":3,"transport":"udp","udp":{"server":"120.24.160.13","port":8884,"encryption":"aes-128-ctr","key":"5653c10a06a484d8255416b42aa22570","nonce":"01000000d83bda4e009c1c3a00000000"},"audio_params":{"format":"opus","sample_rate":24000,"channels":1,"frame_duration":60},"session_id":"997a6577"}
I (13890) wifi:Set ps type: 0, coexist: 0

### 发送 iot 消息
I (13890) MQTT: 发送消息: {"session_id":"997a6577","type":"iot","descriptors":[{"name":"Speaker","description":"当前 AI 机器人的扬声器","properties":{"volume":{"description":"当前音量值","type":"number"}},"methods":{"SetVolume":{"description":"设置音量","parameters":{"volume":{"description":"0到100之间的整数","type":"number"}}}}}]}

## 唤醒
### 编码并发送唤醒音频数据
I (13920) Application: 编码并发送唤醒音频数据: 0
I (13920) MQTT: UDP发送音频数据: 69
I (14190) MQTT: UDP发送音频数据: 70
I (14200) MQTT: UDP发送音频数据: 63
I (14200) WakeWordDetect: Encode wake word opus 0 packets in 455 ms

### <span style="color: brown">发送 listen 消息</span>
I (14200) MQTT: 发送消息: {"session_id":"997a6577","type":"listen","state":"detect","text":" 你好小智"}
### 发送 iot 音量消息
I (14250) MQTT: 发送消息: {"session_id":"997a6577","type":"iot","states":[{"name":"Speaker","state":{"volume":50}}]}

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

### <span style="color: red">发送 listen -start消息</span>
I (17560) MQTT: 发送消息: {"session_id":"997a6577","type":"listen","state":"start","mode":"auto"}
I (17560) Application: STATE: listening

### UDP 发送音频数据
I (17670) MQTT: UDP发送音频数据: 95
I (17670) MQTT: UDP发送音频数据: 89
I (20700) MQTT: UDP发送音频数据: 1
I (20760) MQTT: UDP发送音频数据: 1
I (20820) MQTT: UDP发送音频数据: 1

### 收到 stt 消息
I (20830) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"stt","text":"现在几点了？","session_id":"997a6577"}

### 收到 tts -start消息
I (20840) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"tts","state":"start","session_id":"997a6577"}
I (20890) MQTT: UDP发送音频数据: 1
I (20890) Application: STATE: speaking

### 收到 llm 消息
I (20910) MQTT: UDP发送音频数据: 83
I (20920) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"llm","text":"😉","emotion":"winking","session_id":"997a6577"}

### 收到 tts -sentence_start消息
I (21320) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"tts","state":"sentence_start","text":"现在是 15 点 41 分，下午茶的时间啦。","session_id":"997a6577"}

### 收到 UDP音频数据
I (21510) UDP: 收到音频数据: 158
I (21520) UDP: 收到音频数据: 179
I (24350) UDP: 收到音频数据: 93
I (24350) UDP: 收到音频数据: 93
I (24350) UDP: 收到音频数据: 93

### 收到 tts -sentence_end消息
I (24370) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"tts","state":"sentence_end","text":"现在是 15 点 41 分，下午茶的时间啦。","session_id":"997a6577"}

### 收到 tts -stop消息
I (24380) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"tts","state":"stop","session_id":"997a6577"}

## 一次完整的对话[中间]
### <span style="color: red">发送 listen -start消息</span>
I (25280) MQTT: 发送消息: {"session_id":"997a6577","type":"listen","state":"start","mode":"auto"}
I (25280) Application: STATE: listening
I (25370) MQTT: UDP发送音频数据: 69
I (27470) MQTT: UDP发送音频数据: 82
I (27530) MQTT: UDP发送音频数据: 1
I (27590) MQTT: UDP发送音频数据: 1

### 收到 stt 消息[服务器翻译发送的语音]
I (27600) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"stt","text":"再见。","session_id":"997a6577"}


### 收到 tts -start消息[服务器通知客户端开始接收反馈]
I (27610) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"tts","state":"start","session_id":"997a6577"}
<!-- I (27670) MQTT: UDP发送音频数据: 1
I (27670) Application: STATE: speaking
I (27680) MQTT: UDP发送音频数据: 1 -->

### 收到 llm 消息[服务器的心情]
I (27730) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"llm","text":"😊","emotion":"happy","session_id":"997a6577"}

### 收到 tts -sentence_start消息[通知接受音频数据]
I (27880) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"tts","state":"sentence_start","text":"好的，再见！","session_id":"997a6577"}

### 收到 UDP音频数据
I (28090) UDP: 收到音频数据: 152
I (28790) UDP: 收到音频数据: 93
I (28790) UDP: 收到音频数据: 93

### 收到 tts -sentence_end消息[通知客户端当前语句结束]
I (28820) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"tts","state":"sentence_end","text":"好的，再见！","session_id":"997a6577"}



### 收到 tts -sentence_start消息[通知接受音频数据]
I (28830) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"tts","state":"sentence_start","text":"有事随时来找我哦。","session_id":"997a6577"}

### 收到 UDP音频数据
I (29090) UDP: 收到音频数据: 152
I (30890) UDP: 收到音频数据: 93
I (30890) UDP: 收到音频数据: 93
I (30890) UDP: 收到音频数据: 93

### 收到 goodbye 消息
I (30900) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"goodbye","session_id":"997a6577"}

### 收到 tts -sentence_end消息
I (30930) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"tts","state":"sentence_end","text":"有事随时来找我哦。","session_id":"997a6577"}

### 收到 tts -stop消息
I (30930) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"tts","state":"stop","session_id":"997a6577"}
I (30960) MQTT: 关闭音频通道
I (30960) MQTT: 发送 goodbye 消息关闭音频通道

### 发送 goodbye 消息
I (30960) MQTT: 发送消息: {"session_id":"997a6577","type":"goodbye"}
I (30960) wifi:Set ps type: 1, coexist: 0

### 发送 listen -start消息
I (31480) MQTT: 发送消息: {"session_id":"997a6577","type":"listen","state":"start","mode":"auto"}
I (31480) Application: STATE: listening
I (31500) Application: STATE: idle
I (31500) LcdDisplay: Setting LCD backlight: 0%
I (42520) I2S_IF: Pending out channel for in channel running
I (42520) AudioCodec: Set output enable to false