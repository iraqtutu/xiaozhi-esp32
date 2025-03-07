#include "mqtt_protocol.h"
#include "board.h"
#include "application.h"
#include "settings.h"
#include "system_info.h" 

#include <esp_log.h>
#include <ml307_mqtt.h>
#include <ml307_udp.h>
#include <cstring>
#include <arpa/inet.h>

#define TAG "MQTT"

MqttProtocol::MqttProtocol() {
    event_group_handle_ = xEventGroupCreate();
}

MqttProtocol::~MqttProtocol() {
    ESP_LOGI(TAG, "MqttProtocol deinit");
    if (udp_ != nullptr) {
        delete udp_;
    }
    if (mqtt_ != nullptr) {
        delete mqtt_;
    }
    vEventGroupDelete(event_group_handle_);
}

void MqttProtocol::Start() {
    StartMqttClient();
}

bool MqttProtocol::StartMqttClient() {
    if (mqtt_ != nullptr) {
        ESP_LOGW(TAG, "Mqtt client already started");
        delete mqtt_;
    }

    Settings settings("mqtt", false);
    endpoint_ = settings.GetString("endpoint");
    client_id_ = settings.GetString("client_id");
    username_ = settings.GetString("username");
    password_ = settings.GetString("password");
    publish_topic_ = settings.GetString("publish_topic");

    if (endpoint_.empty()) {
        ESP_LOGE(TAG, "MQTT endpoint is not specified");
        return false;
    }

    mqtt_ = Board::GetInstance().CreateMqtt();
    mqtt_->SetKeepAlive(90);

    mqtt_->OnDisconnected([this]() {
        ESP_LOGI(TAG, "Disconnected from endpoint");
    });

    mqtt_->OnMessage([this](const std::string& topic, const std::string& payload) {
        message_counter_++;
        ESP_LOGI(TAG, "--%d--收到消息，标题： %s，内容：%s", message_counter_, topic.c_str(), payload.c_str());
        cJSON* root = cJSON_Parse(payload.c_str());
        if (root == nullptr) {
            ESP_LOGE(TAG, "Failed to parse json message %s", payload.c_str());
            return;
        }
        cJSON* type = cJSON_GetObjectItem(root, "type");
        if (type == nullptr) {
            ESP_LOGE(TAG, "Message type is not specified");
            cJSON_Delete(root);
            return;
        }

        if (strcmp(type->valuestring, "hello") == 0) {
            ParseServerHello(root);
        } else if (strcmp(type->valuestring, "goodbye") == 0) {
            auto session_id = cJSON_GetObjectItem(root, "session_id");
            if (session_id == nullptr || session_id_ == session_id->valuestring) {
                Application::GetInstance().Schedule([this]() {
                    CloseAudioChannel();
                });
            }
        } else if (on_incoming_json_ != nullptr) {
            on_incoming_json_(root);
        }
        cJSON_Delete(root);
    });

    ESP_LOGI(TAG, "Connecting to endpoint %s", endpoint_.c_str());
    if (!mqtt_->Connect(endpoint_, 8883, client_id_, username_, password_)) {
        ESP_LOGE(TAG, "Failed to connect to endpoint");
        if (on_network_error_ != nullptr) {
            on_network_error_("无法连接服务");
        }
        return false;
    }

    ESP_LOGI(TAG, "Connected to endpoint");
    return true;
}

void MqttProtocol::SendText(const std::string& text) {
    message_counter_++;
    ESP_LOGI(TAG, "--%d--发送消息: %s", message_counter_, text.c_str());
    if (publish_topic_.empty()) {
        return;
    }
    if (!mqtt_->Publish(publish_topic_, text)) {
        ESP_LOGE(TAG, "Failed to publish message");
        if (on_network_error_ != nullptr) {
            on_network_error_("发送失败，请检查网络");
        }
    }
}

void MqttProtocol::SendAudio(const std::vector<uint8_t>& data) {
    std::lock_guard<std::mutex> lock(channel_mutex_);
    if (udp_ == nullptr) {
        return;
    }
    ESP_LOGI(TAG, "UDP发送音频数据: %zu,aes_nonce_: %s", data.size(), aes_nonce_.c_str());
    // for (int i = 0; i < std::min(100, (int)data.size()); i++) {
    //     printf("%02x ", data[i]);
    //     if ((i + 1) % 8 == 0) printf("\n");
    // }
    // printf("\n");
    std::string nonce(aes_nonce_);
    *(uint16_t*)&nonce[2] = htons(data.size());
    *(uint32_t*)&nonce[12] = htonl(++local_sequence_);

    std::string encrypted;
    encrypted.resize(aes_nonce_.size() + data.size());
    memcpy(encrypted.data(), nonce.data(), nonce.size());
    // 声明了一个空的 std::string 对象 encrypted，
    // 使用 resize 方法将其大小调整为 aes_nonce_ 的长度加上 data 的长度
    // 然后使用 memcpy 函数将 nonce 的内容复制到 encrypted 的开头
    // 最后，将 data 的内容复制到 encrypted 的末尾

    size_t nc_off = 0;
    uint8_t stream_block[16] = {0};
    ESP_LOGI(TAG, "加密用nonce: %s", nonce.c_str());
    if (mbedtls_aes_crypt_ctr(&aes_ctx_, data.size(), &nc_off, (uint8_t*)nonce.c_str(), stream_block,
        (uint8_t*)data.data(), (uint8_t*)&encrypted[nonce.size()]) != 0) {
        ESP_LOGE(TAG, "Failed to encrypt audio data");
        return;
    }
    // ESP_LOGI(TAG, "加密后的数据:");
    // for (int j = 0; j < std::min(100, (int)encrypted.size()); j++) {
    //     printf("%02x ", encrypted[j]);
    //     if ((j + 1) % 8 == 0) printf("\n");
    // }
    // printf("\n");
    udp_->Send(encrypted);
}

void MqttProtocol::CloseAudioChannel() {
    ESP_LOGI(TAG, "关闭音频通道");
    {
        std::lock_guard<std::mutex> lock(channel_mutex_);
        if (udp_ != nullptr) {
            delete udp_;
            udp_ = nullptr;
        }
    }

    std::string message = "{";
    message += "\"session_id\":\"" + session_id_ + "\",";
    message += "\"type\":\"goodbye\"";
    message += "}";
    SendText(message);

    if (on_audio_channel_closed_ != nullptr) {
        on_audio_channel_closed_();
    }
}

bool MqttProtocol::OpenAudioChannel() {
    ESP_LOGI(TAG, "打开音频通道");
    if (mqtt_ == nullptr || !mqtt_->IsConnected()) {
        ESP_LOGI(TAG, "MQTT is not connected, try to connect now");
        if (!StartMqttClient()) {
            return false;
        }
    }

    session_id_ = "";
    xEventGroupClearBits(event_group_handle_, MQTT_PROTOCOL_SERVER_HELLO_EVENT);

    // 发送 hello 消息申请 UDP 通道
    ESP_LOGI(TAG, "发送 hello 消息申请udp通道");
    std::string message = "{";
    message += "\"type\":\"hello\",";
    message += "\"version\": 3,";
    message += "\"device_id\":\"" + SystemInfo::GetMacAddress() + "\",";
    message += "\"transport\":\"udp\",";
    message += "\"audio_params\":{";
    message += "\"format\":\"opus\", \"sample_rate\":16000, \"channels\":1, \"frame_duration\":" + std::to_string(OPUS_FRAME_DURATION_MS);
    message += "}}";
    SendText(message);

    // 等待服务器响应
    EventBits_t bits = xEventGroupWaitBits(event_group_handle_, MQTT_PROTOCOL_SERVER_HELLO_EVENT, pdTRUE, pdFALSE, pdMS_TO_TICKS(10000));
    if (!(bits & MQTT_PROTOCOL_SERVER_HELLO_EVENT)) {
        ESP_LOGE(TAG, "Failed to receive server hello");
        if (on_network_error_ != nullptr) {
            on_network_error_("等待响应超时");
        }
        return false;
    }

    std::lock_guard<std::mutex> lock(channel_mutex_);
    if (udp_ != nullptr) {
        delete udp_;
    }
    udp_ = Board::GetInstance().CreateUdp();
    udp_->OnMessage([this](const std::string& data) {
        ESP_LOGI("UDP", "收到音频数据: %zu", data.size());
        if (data.size() < sizeof(aes_nonce_)) {
            ESP_LOGE(TAG, "Invalid audio packet size: %zu", data.size());
            return;
        }
        if (data[0] != 0x01) {
            ESP_LOGE(TAG, "Invalid audio packet type: %x", data[0]);
            return;
        }
        uint32_t sequence = ntohl(*(uint32_t*)&data[12]);
        if (sequence < remote_sequence_) {
            ESP_LOGW(TAG, "Received audio packet with old sequence: %lu, expected: %lu", sequence, remote_sequence_);
            return;
        }
        if (sequence != remote_sequence_ + 1) {
            ESP_LOGW(TAG, "Received audio packet with wrong sequence: %lu, expected: %lu", sequence, remote_sequence_ + 1);
        }
        // 远程数据包解密逻辑
        std::vector<uint8_t> decrypted;
        size_t decrypted_size = data.size() - aes_nonce_.size();
        size_t nc_off = 0;
        uint8_t stream_block[16] = {0};
        decrypted.resize(decrypted_size);
        auto nonce = (uint8_t*)data.data();
        auto encrypted = (uint8_t*)data.data() + aes_nonce_.size();
        ESP_LOGI(TAG, "解密用nonce: %s", nonce);
        int ret = mbedtls_aes_crypt_ctr(&aes_ctx_, decrypted_size, &nc_off, nonce, stream_block, encrypted, (uint8_t*)decrypted.data());
        if (ret != 0) {
            ESP_LOGE(TAG, "Failed to decrypt audio data, ret: %d", ret);
            return;
        }
        if (on_incoming_audio_ != nullptr) {
            on_incoming_audio_(std::move(decrypted));
        }
        remote_sequence_ = sequence;
    });

    udp_->Connect(udp_server_, udp_port_);

    if (on_audio_channel_opened_ != nullptr) {
        on_audio_channel_opened_();
    }
    return true;
}

void MqttProtocol::ParseServerHello(const cJSON* root) {
    auto transport = cJSON_GetObjectItem(root, "transport");
    if (transport == nullptr || strcmp(transport->valuestring, "udp") != 0) {
        ESP_LOGE(TAG, "Unsupported transport: %s", transport->valuestring);
        return;
    }

    auto session_id = cJSON_GetObjectItem(root, "session_id");
    if (session_id != nullptr) {
        session_id_ = session_id->valuestring;
        ESP_LOGI(TAG, "Session ID: %s", session_id_.c_str());
    }

    // Get sample rate from hello message
    auto audio_params = cJSON_GetObjectItem(root, "audio_params");
    if (audio_params != NULL) {
        auto sample_rate = cJSON_GetObjectItem(audio_params, "sample_rate");
        if (sample_rate != NULL) {
            server_sample_rate_ = sample_rate->valueint;
        }
    }

    auto udp = cJSON_GetObjectItem(root, "udp");
    if (udp == nullptr) {
        ESP_LOGE(TAG, "UDP is not specified");
        return;
    }
    udp_server_ = cJSON_GetObjectItem(udp, "server")->valuestring;
    udp_port_ = cJSON_GetObjectItem(udp, "port")->valueint;
    auto key = cJSON_GetObjectItem(udp, "key")->valuestring;
    auto nonce = cJSON_GetObjectItem(udp, "nonce")->valuestring;
    ESP_LOGI(TAG, "UDP server: %s, port: %d, nonce: %s", udp_server_.c_str(), udp_port_, nonce);
    // auto encryption = cJSON_GetObjectItem(udp, "encryption")->valuestring;
    // ESP_LOGI(TAG, "UDP server: %s, port: %d, encryption: %s", udp_server_.c_str(), udp_port_, encryption);
    ESP_LOGI(TAG, "返回的nonce: %s", nonce);
    aes_nonce_ = DecodeHexString(nonce);
    ESP_LOGI(TAG, "解密后的aes_nonce_: %s", aes_nonce_.c_str());
    mbedtls_aes_init(&aes_ctx_);
    ESP_LOGI(TAG, "aes key: %s", key);
    ESP_LOGI(TAG, "aes_nonce_: %s", aes_nonce_.c_str());
    mbedtls_aes_setkey_enc(&aes_ctx_, (const unsigned char*)DecodeHexString(key).c_str(), 128);
    local_sequence_ = 0;
    remote_sequence_ = 0;
    xEventGroupSetBits(event_group_handle_, MQTT_PROTOCOL_SERVER_HELLO_EVENT);
}

static const char hex_chars[] = "0123456789ABCDEF";
// 辅助函数，将单个十六进制字符转换为对应的数值
static inline uint8_t CharToHex(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return 0;  // 对于无效输入，返回0
}

std::string MqttProtocol::DecodeHexString(const std::string& hex_string) {
    std::string decoded;
    decoded.reserve(hex_string.size() / 2);
    for (size_t i = 0; i < hex_string.size(); i += 2) {
        char byte = (CharToHex(hex_string[i]) << 4) | CharToHex(hex_string[i + 1]);
        decoded.push_back(byte);
    }
    return decoded;
}

bool MqttProtocol::IsAudioChannelOpened() const {
    return udp_ != nullptr;
}
