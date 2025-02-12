I (5840) Ota: Check version URL: https://api.tenclass.net/xiaozhi/ota/
I (5850) Ota: Post data: {"flash_size":16777216,"minimum_free_heap_size":8267352,"mac_address":"d8:3b:da:4e:00:9c","chip_model_name":"esp32s3","chip_info":{"model":9,"cores":2,"revision":2,"features":18},"application":{"name":"xiaozhi","version":"1.1.1","compile_time":"Feb 10 2025T18:37:06Z","idf_version":"v5.4","elf_sha256":"63b425c953105a3b74a75a3e8b6177ca895f5457403b9681ff2ca06c01f6c516"},"partition_table": [{"label":"nvs","type":1,"subtype":2,"address":36864,"size":16384},{"label":"otadata","type":1,"subtype":0,"address":53248,"size":8192},{"label":"phy_init","type":1,"subtype":1,"address":61440,"size":4096},{"label":"model","type":1,"subtype":130,"address":65536,"size":983040},{"label":"ota_0","type":0,"subtype":16,"address":1048576,"size":6291456},{"label":"ota_1","type":0,"subtype":17,"address":7340032,"size":6291456}],"ota":{"label":"ota_0"},"board":{"type":"lichuang-dev","ssid":"chenyuyang","rssi":-70,"channel":2,"ip":"192.168.31.226","mac":"d8:3b:da:4e:00:9c"}}
I (5930) EspHttp: Opening HTTP connection to https://api.tenclass.net/xiaozhi/ota/
I (5940) AFE_VC: mode: 1, (Nov  5 2024 16:02:49)

I (5950) MODEL_LOADER: The storage free size is 20608 KB
I (5950) AudioProcessor: Audio communication task started, feed size: 160 fetch size: 512        
I (5950) MODEL_LOADER: The partition size is 960 KB
I (5960) MODEL_LOADER: Successfully load srmodels
I (5970) WakeWordDetect: Model 0: wn9_nihaoxiaozhi_tts
I (5980) AFE_SR: afe interface for speech recognition
I (5980) AFE_SR: AFE version: SR_V220727
I (5980) AFE_SR: Initial auido front-end, total channel: 2, mic num: 1, ref num: 1
I (5990) AFE_SR: aec_init: 1, se_init: 1, vad_init: 1

I (6000) AFE_SR: wakenet_init: 1

MC Quantized wakenet9: wakenet9l_tts1h8_你好小智_3_0.631_0.635, tigger:v3, mode:0, p:0, (Nov  5 2024 16:02:49)
I (6100) esp-x509-crt-bundle: Certificate validated
I (6200) AFE_SR: wake num: 1, mode: 1, (Nov  5 2024 16:02:49)

I (6200) Application: STATE: idle
I (6200) WakeWordDetect: Audio detection task started, feed size: 512 fetch size: 512
I (6200) LcdDisplay: Setting LCD backlight: 0%
I (6210) main_task: Returned from app_main()
I (6640) Ota: Response: {"mqtt":{"endpoint":"post-cn-apg3xckag01.mqtt.aliyuncs.com","client_id":"GID_test@@@d8_3b_da_4e_00_9c","username":"Signature|LTAI5tF8J3CrdWmRiuTjxHbF|post-cn-apg3xckag01","password":"BSRV6VwHfYNkxAAEmhXLbf7pHZ4=","publish_topic":"device-server","subscribe_topic":"devices/d8_3b_da_4e_00_9c"},"server_time":{"timestamp":1739274574014,"timezone":"Asia/Shanghai","timezone_offset":-480},"firmware":{"version":"1.1.1","url":""}}
I (6670) Ota: Current is the latest version
I (6670) Ota: Running partition: ota_0
I (11120) I2S_IF: Pending out channel for in channel running
I (11120) AudioCodec: Set output enable to false
I (23520) Application: STATE: connecting
I (23520) LcdDisplay: Setting LCD backlight: 100%
I (23530) MQTT: 打开音频通道

I (23530) MQTT: 发送 hello 消息申请udp通道
I (23530) MQTT: 发送消息: {"type":"hello","version": 3,"transport":"udp","audio_params":{"format":"opus", "sample_rate":16000, "channels":1, "frame_duration":60}}
I (23840) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"hello","version":3,"transport":"udp","udp":{"server":"120.24.160.13","port":8884,"encryption":"aes-128-ctr","key":"58f2100de14a63e4a796d4b3ddf67c7f","nonce":"01000000d83bda4e009c57b600000000"},"audio_params":{"format":"opus","sample_rate":24000,"channels":1,"frame_duration":60},"session_id":"62d41463"}
I (23860) wifi:Set ps type: 0, coexist: 0

I (23870) MQTT: 发送消息: {"session_id":"62d41463","type":"iot","descriptors":[{"name":"Speaker","description":"当前 AI 机器人的扬声器","properties":{"volume":{"description":"当前音量值","type":"number"}},"methods":{"SetVolume":{"description":"设置音量","parameters":{"volume":{"description":"0到100之间的整数","type":"number"}}}}}]}


I (23900) Application: 编码并发送唤醒音频数据: 0
I (23900) MQTT: UDP发送音频数据: 113
I (23910) MQTT: UDP发送音频数据: 119
I (23910) MQTT: UDP发送音频数据: 123
I (23910) MQTT: UDP发送音频数据: 119
I (23920) MQTT: UDP发送音频数据: 119
I (23920) MQTT: UDP发送音频数据: 139
I (23920) MQTT: UDP发送音频数据: 131
I (23930) MQTT: UDP发送音频数据: 131
I (23930) MQTT: UDP发送音频数据: 125
I (23940) MQTT: UDP发送音频数据: 131
I (23940) MQTT: UDP发送音频数据: 117
I (23940) MQTT: UDP发送音频数据: 125
I (23950) MQTT: UDP发送音频数据: 105
I (23950) MQTT: UDP发送音频数据: 105
I (23960) MQTT: UDP发送音频数据: 128
I (23960) MQTT: UDP发送音频数据: 132
I (23960) MQTT: UDP发送音频数据: 120
I (23970) MQTT: UDP发送音频数据: 114
I (23970) MQTT: UDP发送音频数据: 132
I (23970) MQTT: UDP发送音频数据: 133
I (23980) MQTT: UDP发送音频数据: 137
I (23980) MQTT: UDP发送音频数据: 128
I (23990) MQTT: UDP发送音频数据: 118
I (23990) MQTT: UDP发送音频数据: 123
I (23990) MQTT: UDP发送音频数据: 132
I (24000) MQTT: UDP发送音频数据: 129
I (24000) WakeWordDetect: Encode wake word opus 7 packets in 469 ms
I (24000) MQTT: UDP发送音频数据: 120
I (24010) MQTT: UDP发送音频数据: 117
I (24010) MQTT: UDP发送音频数据: 123
I (24020) MQTT: UDP发送音频数据: 116
I (24020) MQTT: UDP发送音频数据: 134
I (24030) MQTT: UDP发送音频数据: 99
I (24030) MQTT: UDP发送音频数据: 100
I (24030) MQTT: 发送消息: {"session_id":"62d41463","type":"listen","state":"detect","text":"你好 小智"}
I (24040) Application: STATE: listening
I (24050) I2S_IF: Mode 0 need extend bits 64 to 32
I (24050) I2S_IF: channel mode 0 bits:16/32 channel:2 mask:1
I (24060) I2S_IF: STD Mode 1 bits:16/32 channel:2 sample_rate:24000 mask:1
I (24080) Adev_Codec: Open codec device OK
I (24080) AudioCodec: Set output enable to true
I (24080) MQTT: 发送消息: {"session_id":"62d41463","type":"iot","states":[{"name":"Speaker","state":{"volume":50}}]}



I (24150) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"tts","state":"start","session_id":"62d41463"}
I (24160) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"stt","text":"小智","session_id":"62d41463"}
I (24190) MQTT: UDP发送音频数据: 106
I (24190) Application: STATE: speaking
I (24220) MQTT: UDP发送音频数据: 145
I (24460) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"llm","text":"😊","emotion":"happy","session_id":"62d41463"}
I (24940) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"tts","state":"sentence_start","text":"你好呀，找我有什么事儿吗？","session_id":"62d41463"}
I (25150) UDP: 收到音频数据: 160
I (25150) UDP: 收到音频数据: 154
I (25160) UDP: 收到音频数据: 252
I (25200) UDP: 收到音频数据: 259
I (25200) UDP: 收到音频数据: 227
I (25210) UDP: 收到音频数据: 212
I (25210) UDP: 收到音频数据: 207
I (25210) UDP: 收到音频数据: 186
I (25260) UDP: 收到音频数据: 204
I (25260) UDP: 收到音频数据: 199
I (25260) UDP: 收到音频数据: 235
I (25270) UDP: 收到音频数据: 184
I (25300) UDP: 收到音频数据: 177
I (25300) UDP: 收到音频数据: 186
I (25300) UDP: 收到音频数据: 198
I (25300) UDP: 收到音频数据: 240
I (25340) UDP: 收到音频数据: 211
I (25340) UDP: 收到音频数据: 208
I (25350) UDP: 收到音频数据: 198
I (25350) UDP: 收到音频数据: 228
I (25440) UDP: 收到音频数据: 256
I (25440) UDP: 收到音频数据: 232
I (25440) UDP: 收到音频数据: 223
I (25440) UDP: 收到音频数据: 234
I (25560) UDP: 收到音频数据: 219
I (25560) UDP: 收到音频数据: 198
I (25570) UDP: 收到音频数据: 202
I (25570) UDP: 收到音频数据: 221
I (25800) UDP: 收到音频数据: 210
I (25800) UDP: 收到音频数据: 219
I (25800) UDP: 收到音频数据: 202
I (25810) UDP: 收到音频数据: 176
I (26040) UDP: 收到音频数据: 160
I (26100) UDP: 收到音频数据: 177
I (26100) UDP: 收到音频数据: 124
I (26100) UDP: 收到音频数据: 101
I (26100) UDP: 收到音频数据: 93
I (26110) UDP: 收到音频数据: 93
I (26130) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"tts","state":"sentence_end","text":"你好呀，找我有什么事儿吗？","session_id":"62d41463"}
I (26140) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"tts","state":"stop","session_id":"62d41463"}





I (27280) MQTT: 发送消息: {"session_id":"62d41463","type":"listen","state":"start","mode":"auto"}
I (27280) Application: STATE: listening
I (27380) MQTT: UDP发送音频数据: 107
I (29650) MQTT: UDP发送音频数据: 89
I (29710) MQTT: UDP发送音频数据: 88
I (29780) MQTT: UDP发送音频数据: 92
I (29830) MQTT: UDP发送音频数据: 1
I (29880) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"stt","text":"几点了？","session_id":"62d41463"}
I (29890) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"tts","state":"start","session_id":"62d41463"}
I (29900) MQTT: UDP发送音频数据: 1
I (29900) Application: STATE: speaking
I (29930) MQTT: UDP发送音频数据: 1
I (29970) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"llm","text":"😊","emotion":"happy","session_id":"62d41463"}
I (30640) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"tts","state":"sentence_start","text":"现在是 19 点 49 分，快到晚上 8 点了。有事儿吗？","session_id":"62d41463"}
I (30840) UDP: 收到音频数据: 149
I (30840) UDP: 收到音频数据: 183
I (30840) UDP: 收到音频数据: 224
I (30870) UDP: 收到音频数据: 231
I (30870) UDP: 收到音频数据: 226
I (30870) UDP: 收到音频数据: 180
I (30870) UDP: 收到音频数据: 237
I (30920) UDP: 收到音频数据: 213
I (30920) UDP: 收到音频数据: 217
I (30920) UDP: 收到音频数据: 238
I (30920) UDP: 收到音频数据: 210
I (30920) UDP: 收到音频数据: 214
I (30950) UDP: 收到音频数据: 219
I (30950) UDP: 收到音频数据: 243
I (30950) UDP: 收到音频数据: 206
I (30950) UDP: 收到音频数据: 220
I (30990) UDP: 收到音频数据: 246
I (30990) UDP: 收到音频数据: 218
I (30990) UDP: 收到音频数据: 229
I (30990) UDP: 收到音频数据: 226
I (31050) UDP: 收到音频数据: 217
I (31050) UDP: 收到音频数据: 216
I (31050) UDP: 收到音频数据: 243
I (31050) UDP: 收到音频数据: 227
I (31280) UDP: 收到音频数据: 234
I (31280) UDP: 收到音频数据: 228
I (31280) UDP: 收到音频数据: 227
I (31280) UDP: 收到音频数据: 196
I (31520) UDP: 收到音频数据: 196
I (31520) UDP: 收到音频数据: 210
I (31520) UDP: 收到音频数据: 207
I (31520) UDP: 收到音频数据: 203
I (31760) UDP: 收到音频数据: 176
I (31820) UDP: 收到音频数据: 180
I (31820) UDP: 收到音频数据: 163
I (31820) UDP: 收到音频数据: 168
I (31830) UDP: 收到音频数据: 230
I (32060) UDP: 收到音频数据: 243
I (32060) UDP: 收到音频数据: 253
I (32060) UDP: 收到音频数据: 198
I (32060) UDP: 收到音频数据: 222
I (32300) UDP: 收到音频数据: 229
I (32310) UDP: 收到音频数据: 234
I (32310) UDP: 收到音频数据: 239
I (32310) UDP: 收到音频数据: 224
I (32540) UDP: 收到音频数据: 223
I (32540) UDP: 收到音频数据: 202
I (32540) UDP: 收到音频数据: 191
I (32540) UDP: 收到音频数据: 226
I (32780) UDP: 收到音频数据: 235
I (32780) UDP: 收到音频数据: 237
I (32790) UDP: 收到音频数据: 222
I (32790) UDP: 收到音频数据: 216
I (33020) UDP: 收到音频数据: 222
I (33020) UDP: 收到音频数据: 197
I (33020) UDP: 收到音频数据: 175
I (33020) UDP: 收到音频数据: 162
I (33020) UDP: 收到音频数据: 164
I (33320) UDP: 收到音频数据: 170
I (33320) UDP: 收到音频数据: 223
I (33320) UDP: 收到音频数据: 232
I (33330) UDP: 收到音频数据: 257
I (33560) UDP: 收到音频数据: 216
I (33560) UDP: 收到音频数据: 201
I (33560) UDP: 收到音频数据: 215
I (33560) UDP: 收到音频数据: 221
I (33800) UDP: 收到音频数据: 202
I (33800) UDP: 收到音频数据: 225
I (33800) UDP: 收到音频数据: 196
I (33800) UDP: 收到音频数据: 169
I (34040) UDP: 收到音频数据: 155
I (34100) UDP: 收到音频数据: 145
I (34100) UDP: 收到音频数据: 109
I (34100) UDP: 收到音频数据: 93
I (34100) UDP: 收到音频数据: 93
I (34100) UDP: 收到音频数据: 93
I (34130) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"tts","state":"sentence_end","text":"现在是 19 点 49 分，快到晚上 8 点了。有事儿吗？","session_id":"62d41463"}
I (34150) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"tts","state":"stop","session_id":"62d41463"}


I (35300) MQTT: 发送消息: {"session_id":"62d41463","type":"listen","state":"start","mode":"auto"}
I (35300) Application: STATE: listening
I (35400) MQTT: UDP发送音频数据: 125
I (35430) MQTT: UDP发送音频数据: 137
I (35510) MQTT: UDP发送音频数据: 109
I (35570) MQTT: UDP发送音频数据: 99
I (35630) MQTT: UDP发送音频数据: 103
I (35690) MQTT: UDP发送音频数据: 87
I (35750) MQTT: UDP发送音频数据: 78
I (35810) MQTT: UDP发送音频数据: 81
I (35880) MQTT: UDP发送音频数据: 94
I (35930) MQTT: UDP发送音频数据: 122
I (36000) MQTT: UDP发送音频数据: 137
I (36050) MQTT: UDP发送音频数据: 133
I (36120) MQTT: UDP发送音频数据: 135
I (36170) MQTT: UDP发送音频数据: 102
I (36210) MQTT: UDP发送音频数据: 119
I (36300) MQTT: UDP发送音频数据: 133
I (36360) MQTT: UDP发送音频数据: 128
I (36420) MQTT: UDP发送音频数据: 130
I (36470) MQTT: UDP发送音频数据: 150
I (36540) MQTT: UDP发送音频数据: 123
I (36590) MQTT: UDP发送音频数据: 116
I (36650) MQTT: UDP发送音频数据: 119
I (36710) MQTT: UDP发送音频数据: 101
I (36780) MQTT: UDP发送音频数据: 111
I (36800) MQTT: UDP发送音频数据: 104
I (36900) MQTT: UDP发送音频数据: 122
I (36960) MQTT: UDP发送音频数据: 121
I (37010) MQTT: UDP发送音频数据: 112
I (37070) MQTT: UDP发送音频数据: 110
I (37130) MQTT: UDP发送音频数据: 111
I (37200) MQTT: UDP发送音频数据: 84
I (37260) MQTT: UDP发送音频数据: 91
I (37320) MQTT: UDP发送音频数据: 99
I (37380) MQTT: UDP发送音频数据: 141
I (37410) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"stt","text":"再见。","session_id":"62d41463"}
I (37430) MQTT: UDP发送音频数据: 119
I (37440) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"tts","state":"start","session_id":"62d41463"}
I (37480) Application: STATE: speaking
I (37480) MQTT: UDP发送音频数据: 127
I (37540) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"llm","text":"😊","emotion":"happy","session_id":"62d41463"}
I (37870) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"tts","state":"sentence_start","text":"好的，再见，有什么需要帮忙的随时找我哦。","session_id":"62d41463"}
I (38110) UDP: 收到音频数据: 159
I (38110) UDP: 收到音频数据: 181
I (38110) UDP: 收到音频数据: 216
I (38110) UDP: 收到音频数据: 235
I (38110) UDP: 收到音频数据: 230
I (38110) UDP: 收到音频数据: 235
I (38120) UDP: 收到音频数据: 195
I (38140) UDP: 收到音频数据: 227
I (38140) UDP: 收到音频数据: 208
I (38140) UDP: 收到音频数据: 175
I (38140) UDP: 收到音频数据: 172
I (38140) UDP: 收到音频数据: 160
I (38190) UDP: 收到音频数据: 194
I (38190) UDP: 收到音频数据: 213
I (38190) UDP: 收到音频数据: 209
I (38190) UDP: 收到音频数据: 258
I (38350) UDP: 收到音频数据: 234
I (38350) UDP: 收到音频数据: 214
I (38350) UDP: 收到音频数据: 243
I (38350) UDP: 收到音频数据: 198
I (38570) UDP: 收到音频数据: 170
I (38570) UDP: 收到音频数据: 169
I (38580) UDP: 收到音频数据: 183
I (38580) UDP: 收到音频数据: 240
I (38810) UDP: 收到音频数据: 216
I (38810) UDP: 收到音频数据: 236
I (38810) UDP: 收到音频数据: 231
I (38810) UDP: 收到音频数据: 211
I (39060) UDP: 收到音频数据: 244
I (39060) UDP: 收到音频数据: 217
I (39060) UDP: 收到音频数据: 216
I (39060) UDP: 收到音频数据: 216
I (39330) UDP: 收到音频数据: 195
I (39360) UDP: 收到音频数据: 249
I (39360) UDP: 收到音频数据: 192
I (39360) UDP: 收到音频数据: 211
I (39360) UDP: 收到音频数据: 223
I (39610) UDP: 收到音频数据: 222
I (39610) UDP: 收到音频数据: 205
I (39610) UDP: 收到音频数据: 231
I (39610) UDP: 收到音频数据: 239
I (39840) UDP: 收到音频数据: 228
I (39840) UDP: 收到音频数据: 217
I (39840) UDP: 收到音频数据: 226
I (39840) UDP: 收到音频数据: 222
I (40080) UDP: 收到音频数据: 242
I (40090) UDP: 收到音频数据: 226
I (40090) UDP: 收到音频数据: 214
I (40090) UDP: 收到音频数据: 204
I (40310) UDP: 收到音频数据: 207
I (40350) UDP: 收到音频数据: 237
I (40360) UDP: 收到音频数据: 236
I (40360) UDP: 收到音频数据: 218
I (40560) UDP: 收到音频数据: 241
I (40570) UDP: 收到音频数据: 185
I (40570) UDP: 收到音频数据: 172
I (40570) UDP: 收到音频数据: 169
I (40790) UDP: 收到音频数据: 152
I (40800) UDP: 收到音频数据: 111
I (40800) UDP: 收到音频数据: 93
I (40800) UDP: 收到音频数据: 93
I (40800) UDP: 收到音频数据: 93
I (40820) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"goodbye","session_id":"62d41463"}
I (40830) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"tts","state":"sentence_end","text":"好的，再见，有什么需要帮忙的随时找我哦。","session_id":"62d41463"}  
I (40850) MQTT: 收到消息，标题： devices/p2p/GID_test@@@d8_3b_da_4e_00_9c，内容：{"type":"tts","state":"stop","session_id":"62d41463"}
I (40880) MQTT: 关闭音频通道
I (40880) MQTT: 发送 goodbye 消息关闭音频通道
I (40880) MQTT: 发送消息: {"session_id":"62d41463","type":"goodbye"}
I (40890) wifi:Set ps type: 1, coexist: 0

I (41670) MQTT: 发送消息: {"session_id":"62d41463","type":"listen","state":"start","mode":"auto"}
I (41670) Application: STATE: listening
I (41700) Application: STATE: idle
I (41700) LcdDisplay: Setting LCD backlight: 0%
I (52710) I2S_IF: Pending out channel for in channel running
I (52710) AudioCodec: Set output enable to false