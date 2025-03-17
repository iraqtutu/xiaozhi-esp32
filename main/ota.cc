#include "ota.h"
#include "system_info.h"
#include "board.h"
#include "settings.h"
// #include "settings_schema.h"

#include <cJSON.h>
#include <esp_log.h>
#include <esp_partition.h>
#include <esp_ota_ops.h>
#include <esp_app_format.h>
#include <esp_system.h>
#include <esp_http_client.h>
#include <esp_https_ota.h>

#include <cstring>
#include <vector>
#include <sstream>
#include <algorithm>

#include <esp_netif.h>
#include <esp_wifi.h>
#include "esp_netif_ip_addr.h"
#include "lwip/ip6_addr.h"
#include "lwip/inet.h"
#include "esp_event.h"

#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>

#define TAG "Ota"


Ota::Ota() {
    // 不在构造函数中调用 Init()，而是在网络初始化后再调用
    netif = NULL;
}

Ota::~Ota() {
}

void Ota::SetCheckVersionUrl(std::string check_version_url) {
    check_version_url_ = check_version_url;
}

void Ota::SetHeader(const std::string& key, const std::string& value) {
    headers_[key] = value;
}

void Ota::SetPostData(const std::string& post_data) {
    post_data_ = post_data;
}

bool Ota::CheckVersion() {
    current_version_ = esp_app_get_description()->version;
    ESP_LOGI(TAG, "Current version: %s", current_version_.c_str());

    if (check_version_url_.length() < 10) {
        ESP_LOGE(TAG, "Check version URL is not properly set");
        return false;
    }

    ESP_LOGI(TAG, "Checking for new version at: %s", check_version_url_.c_str());
    
    // 添加直接URL测试功能
    // ESP_LOGI(TAG, "首先进行URL连通性测试...");
    // if (TestUrlConnectivity(check_version_url_)) {
    //     ESP_LOGI(TAG, "URL连通性测试成功，继续检查版本");
    // } else {
    //     ESP_LOGE(TAG, "URL连通性测试失败，可能无法成功检查版本");
    //     // 继续尝试检查版本
    // }
    
    // 添加重试逻辑
    const int max_retries = 3;
    for (int retry = 0; retry < max_retries; retry++) {
        if (retry > 0) {
            ESP_LOGI(TAG, "重试检查版本 (%d/%d)...", retry + 1, max_retries);
            // 每次重试前等待5秒
            vTaskDelay(pdMS_TO_TICKS(5000));
        }
        
        auto http = Board::GetInstance().CreateHttp();
        
        for (const auto& header : headers_) {
            http->SetHeader(header.first, header.second);
        }

        http->SetHeader("Content-Type", "application/json");
        http->SetHeader("Connection", "close"); // 确保关闭连接，避免keep-alive问题
        
        std::string method = "POST";
        ESP_LOGI(TAG, "使用%s方法请求，Post data长度: %zu字节", method.c_str(), post_data_.length());
        
        if (!http->Open(method, check_version_url_, post_data_)) {
            ESP_LOGE(TAG, "尝试 %d/%d: HTTP连接失败", retry + 1, max_retries);
            delete http;
            
            if (retry == max_retries - 1) {
                ESP_LOGE(TAG, "所有重试都失败，请检查网络连接和服务器地址");
                return false;
            }
            continue; // 尝试下一次重试
        }
        
        // 如果连接成功，获取响应并处理
        auto response = http->GetBody();
        http->Close();
        delete http;
        
        if (response.empty()) {
            ESP_LOGE(TAG, "服务器返回空响应");
            continue; // 尝试下一次重试
        }
        
        ESP_LOGI(TAG, "服务器响应长度: %zu字节", response.length());
        
        // 解析JSON响应
        cJSON *root = cJSON_Parse(response.c_str());
        ESP_LOGI(TAG, "Response: %s", response.length() > 100 ? 
                 (response.substr(0, 100) + "...").c_str() : response.c_str());
                 
        if (root == NULL) {
            ESP_LOGE(TAG, "解析JSON响应失败");
            if (retry == max_retries - 1) {
                return false;
            }
            continue;
        }

        has_activation_code_ = false;
        cJSON *activation = cJSON_GetObjectItem(root, "activation");
        if (activation != NULL) {
            cJSON* message = cJSON_GetObjectItem(activation, "message");
            if (message != NULL) {
                activation_message_ = message->valuestring;
            }
            cJSON* code = cJSON_GetObjectItem(activation, "code");
            if (code != NULL) {
                activation_code_ = code->valuestring;
            }
            has_activation_code_ = true;
        }

        has_mqtt_config_ = false;
        cJSON *mqtt = cJSON_GetObjectItem(root, "mqtt");
        if (mqtt != NULL) {
            Settings settings("mqtt", true);
            cJSON *item = NULL;
            cJSON_ArrayForEach(item, mqtt) {
                if (item->type == cJSON_String) {
                    if (settings.GetString(item->string) != item->valuestring) {
                        settings.SetString(item->string, item->valuestring);
                    }
                }
            }
            has_mqtt_config_ = true;
        }

        has_server_time_ = false;
        cJSON *server_time = cJSON_GetObjectItem(root, "server_time");
        if (server_time != NULL) {
            cJSON *timestamp = cJSON_GetObjectItem(server_time, "timestamp");
            cJSON *timezone_offset = cJSON_GetObjectItem(server_time, "timezone_offset");
            
            if (timestamp != NULL) {
                // 设置系统时间
                struct timeval tv;
                double ts = timestamp->valuedouble;
                
                // 如果有时区偏移，计算本地时间
                if (timezone_offset != NULL) {
                    ts += (timezone_offset->valueint * 60 * 1000); // 转换分钟为毫秒
                }
                
                tv.tv_sec = (time_t)(ts / 1000);  // 转换毫秒为秒
                tv.tv_usec = (suseconds_t)((long long)ts % 1000) * 1000;  // 剩余的毫秒转换为微秒
                settimeofday(&tv, NULL);
                has_server_time_ = true;
            }
        }

        cJSON *firmware = cJSON_GetObjectItem(root, "firmware");
        if (firmware == NULL) {
            ESP_LOGE(TAG, "Failed to get firmware object");
            cJSON_Delete(root);
            return false;
        }
        cJSON *version = cJSON_GetObjectItem(firmware, "version");
        if (version == NULL) {
            ESP_LOGE(TAG, "Failed to get version object");
            cJSON_Delete(root);
            return false;
        }
        cJSON *url = cJSON_GetObjectItem(firmware, "url");
        if (url == NULL) {
            ESP_LOGE(TAG, "Failed to get url object");
            cJSON_Delete(root);
            return false;
        }

        firmware_version_ = version->valuestring;
        firmware_url_ = url->valuestring;
        cJSON_Delete(root);

        // Check if the version is newer, for example, 0.1.0 is newer than 0.0.1
        has_new_version_ = IsNewVersionAvailable(current_version_, firmware_version_);
        if (has_new_version_) {
            ESP_LOGI(TAG, "New version available: %s", firmware_version_.c_str());
        } else {
            ESP_LOGI(TAG, "Current is the latest version");
        }
        return true;
    }
    return false;
}

void Ota::MarkCurrentVersionValid() {
    auto partition = esp_ota_get_running_partition();
    if (strcmp(partition->label, "factory") == 0) {
        ESP_LOGI(TAG, "Running from factory partition, skipping");
        return;
    }

    ESP_LOGI(TAG, "Running partition: %s", partition->label);
    esp_ota_img_states_t state;
    if (esp_ota_get_state_partition(partition, &state) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get state of partition");
        return;
    }

    if (state == ESP_OTA_IMG_PENDING_VERIFY) {
        ESP_LOGI(TAG, "Marking firmware as valid");
        esp_ota_mark_app_valid_cancel_rollback();
    }
}

void Ota::Upgrade(const std::string& firmware_url) {
    ESP_LOGI(TAG, "Upgrading firmware from %s", firmware_url.c_str());
    esp_ota_handle_t update_handle = 0;
    auto update_partition = esp_ota_get_next_update_partition(NULL);
    if (update_partition == NULL) {
        ESP_LOGE(TAG, "Failed to get update partition");
        return;
    }

    ESP_LOGI(TAG, "Writing to partition %s at offset 0x%lx", update_partition->label, update_partition->address);
    bool image_header_checked = false;
    std::string image_header;

    auto http = Board::GetInstance().CreateHttp();
    if (!http->Open("GET", firmware_url)) {
        ESP_LOGE(TAG, "Failed to open HTTP connection");
        delete http;
        return;
    }

    size_t content_length = http->GetBodyLength();
    if (content_length == 0) {
        ESP_LOGE(TAG, "Failed to get content length");
        delete http;
        return;
    }

    char buffer[512];
    size_t total_read = 0, recent_read = 0;
    auto last_calc_time = esp_timer_get_time();
    while (true) {
        int ret = http->Read(buffer, sizeof(buffer));
        if (ret < 0) {
            ESP_LOGE(TAG, "Failed to read HTTP data: %s", esp_err_to_name(ret));
            delete http;
            return;
        }

        // Calculate speed and progress every second
        recent_read += ret;
        total_read += ret;
        if (esp_timer_get_time() - last_calc_time >= 1000000 || ret == 0) {
            size_t progress = total_read * 100 / content_length;
            ESP_LOGI(TAG, "Progress: %zu%% (%zu/%zu), Speed: %zuB/s", progress, total_read, content_length, recent_read);
            if (upgrade_callback_) {
                upgrade_callback_(progress, recent_read);
            }
            last_calc_time = esp_timer_get_time();
            recent_read = 0;
        }

        if (ret == 0) {
            break;
        }

        if (!image_header_checked) {
            image_header.append(buffer, ret);
            if (image_header.size() >= sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t) + sizeof(esp_app_desc_t)) {
                esp_app_desc_t new_app_info;
                memcpy(&new_app_info, image_header.data() + sizeof(esp_image_header_t) + sizeof(esp_image_segment_header_t), sizeof(esp_app_desc_t));
                ESP_LOGI(TAG, "New firmware version: %s", new_app_info.version);

                auto current_version = esp_app_get_description()->version;
                if (memcmp(new_app_info.version, current_version, sizeof(new_app_info.version)) == 0) {
                    ESP_LOGE(TAG, "Firmware version is the same, skipping upgrade");
                    delete http;
                    return;
                }

                if (esp_ota_begin(update_partition, OTA_WITH_SEQUENTIAL_WRITES, &update_handle)) {
                    esp_ota_abort(update_handle);
                    delete http;
                    ESP_LOGE(TAG, "Failed to begin OTA");
                    return;
                }

                image_header_checked = true;
                std::string().swap(image_header);
            }
        }
        auto err = esp_ota_write(update_handle, buffer, ret);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to write OTA data: %s", esp_err_to_name(err));
            esp_ota_abort(update_handle);
            delete http;
            return;
        }
    }
    delete http;

    esp_err_t err = esp_ota_end(update_handle);
    if (err != ESP_OK) {
        if (err == ESP_ERR_OTA_VALIDATE_FAILED) {
            ESP_LOGE(TAG, "Image validation failed, image is corrupted");
        } else {
            ESP_LOGE(TAG, "Failed to end OTA: %s", esp_err_to_name(err));
        }
        return;
    }

    err = esp_ota_set_boot_partition(update_partition);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set boot partition: %s", esp_err_to_name(err));
        return;
    }

    ESP_LOGI(TAG, "Firmware upgrade successful, rebooting in 3 seconds...");
    vTaskDelay(pdMS_TO_TICKS(3000));
    esp_restart();
}

void Ota::StartUpgrade(std::function<void(int progress, size_t speed)> callback) {
    upgrade_callback_ = callback;
    Upgrade(firmware_url_);
}

std::vector<int> Ota::ParseVersion(const std::string& version) {
    std::vector<int> versionNumbers;
    std::stringstream ss(version);
    std::string segment;
    
    while (std::getline(ss, segment, '.')) {
        versionNumbers.push_back(std::stoi(segment));
    }
    
    return versionNumbers;
}

bool Ota::IsNewVersionAvailable(const std::string& currentVersion, const std::string& newVersion) {
    std::vector<int> current = ParseVersion(currentVersion);
    std::vector<int> newer = ParseVersion(newVersion);
    
    for (size_t i = 0; i < std::min(current.size(), newer.size()); ++i) {
        if (newer[i] > current[i]) {
            return true;
        } else if (newer[i] < current[i]) {
            return false;
        }
    }
    
    return newer.size() > current.size();
}

void Ota::print_ipv6_address() {
    // 获取WiFi网络接口
    esp_netif_t* netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
    if (netif == NULL) {
        ESP_LOGE(TAG, "未找到WiFi网络接口");
        return;
    }
    
    ESP_LOGI(TAG, "尝试获取IPv6地址信息:");
    
    // 检查链路本地地址
    esp_ip6_addr_t addr;
    if (esp_netif_get_ip6_linklocal(netif, &addr) == ESP_OK) {
        char ip_str[48];
        ip6addr_ntoa_r((const ip6_addr_t*)&addr, ip_str, sizeof(ip_str));
        ESP_LOGI(TAG, "  链路本地地址: %s", ip_str);
    } else {
        ESP_LOGW(TAG, "  无法获取链路本地地址");
    }
    
    // 尝试获取所有IPv6地址
    esp_ip6_addr_t ip6_addrs[CONFIG_LWIP_IPV6_NUM_ADDRESSES];
    int num_addrs = esp_netif_get_all_ip6(netif, ip6_addrs);
    
    if (num_addrs > 0) {
        ESP_LOGI(TAG, "找到 %d 个IPv6地址:", num_addrs);
        
        for (int i = 0; i < num_addrs; i++) {
            // 跳过空地址
            if (ip6_addrs[i].zone == 0 || ip6_addr_isany_val(ip6_addrs[i])) {
                continue;
            }
            
            char ip_str[48];
            ip6addr_ntoa_r((const ip6_addr_t*)&ip6_addrs[i], ip_str, sizeof(ip_str));
            
            // 仅使用lwIP函数检查地址类型
            ip6_addr_t* lwip_addr = (ip6_addr_t*)&ip6_addrs[i];
            const char* type = "未知";
            
            if (ip6_addr_islinklocal(lwip_addr)) {
                type = "链路本地";
            } else if (ip6_addr_isglobal(lwip_addr)) {
                type = "全局";
            } else if (ip6_addr_isuniquelocal(lwip_addr)) {
                type = "唯一本地";
            }
            
            ESP_LOGI(TAG, "  IPv6地址 #%d: %s (类型: %s)", i, ip_str, type);
        }
    } else {
        ESP_LOGW(TAG, "没有找到IPv6地址");
    }
    
    // 测试IPv6站点连接性
    // ESP_LOGI(TAG, "测试IPv6连接性:");
  
    // if (TestUrlConnectivity("http://ipv6.baidu.com")) {
    //     ESP_LOGI(TAG, "  连接到ipv6.baidu.com成功");
    // } else {
    //     ESP_LOGW(TAG, "  连接到ipv6.baidu.com失败");
    // }
}

// 添加 IPv6 事件处理函数
void Ota::handle_ip6_event(esp_event_base_t event_base, int32_t event_id, void* event_data) {
    ip_event_got_ip6_t* event = (ip_event_got_ip6_t*)event_data;
    char ipv6_str[48];
    
    ip6addr_ntoa_r((const ip6_addr_t*)&event->ip6_info.ip.addr, ipv6_str, sizeof(ipv6_str));
    ESP_LOGI(TAG, "Got IPv6 address: %s", ipv6_str);
    
    // 立即打印所有 IPv6 地址
    // print_ipv6_address();
}

// 在初始化函数中添加 IPv6 事件监听
bool Ota::Init() {
    // 检查WiFi连接状态
    wifi_ap_record_t ap_info;
    if (esp_wifi_sta_get_ap_info(&ap_info) != ESP_OK) {
        ESP_LOGE(TAG, "WiFi未连接，无法获取IPv6地址");
        return false;
    }
    
    ESP_LOGI(TAG, "WiFi已连接到: %s", ap_info.ssid);
    
    // 确保IPv6已在WiFi接口上启用
    // 不需要检查标志，直接假设IPv6已启用
    ESP_LOGI(TAG, "假设IPv6已在WiFi接口上启用（无法直接检查）");
    
    // 尝试多种方式获取 WiFi STA 接口
    for (int retry = 0; retry < 5; retry++) {
        // 方法1：使用 ifkey
        netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
        
        // 方法2：如果方法1失败，尝试获取默认 STA 接口
        if (netif == NULL) {
            netif = esp_netif_get_default_netif();
        }
        
        // 方法3：遍历所有接口查找 WiFi STA
        if (netif == NULL) {
            // 尝试获取 WiFi STA 接口
            for (esp_netif_t* tmp = NULL; (tmp = esp_netif_next_unsafe(tmp)) != NULL; ) {
                esp_netif_dhcp_status_t status;
                if (esp_netif_dhcpc_get_status(tmp, &status) == ESP_OK) {
                    // 查看是否是 WiFi 接口
                    char ifname[16];
                    if (esp_netif_get_netif_impl_name(tmp, ifname) == ESP_OK) {
                        ESP_LOGI(TAG, "找到网络接口: %s", ifname);
                        if (strstr(ifname, "sta") != NULL || strstr(ifname, "STA") != NULL) {
                            netif = tmp;
                            ESP_LOGI(TAG, "找到 WiFi STA 接口: %s", ifname);
                            break;
                        }
                    }
                }
            }
        }
        
        if (netif != NULL) {
            ESP_LOGI(TAG, "成功获取网络接口");
            break;
        }
        
        ESP_LOGW(TAG, "WiFi STA 接口未就绪，等待 1 秒后重试 (%d/5)", retry + 1);
        vTaskDelay(pdMS_TO_TICKS(1000));  // 等待 1 秒再次尝试
    }

    if (netif == NULL) {
        ESP_LOGE(TAG, "Failed to get WiFi STA interface after multiple retries");
        return false;
    }
    
    ESP_LOGI(TAG, "Network interface initialized successfully");

    // 注册 IPv6 事件处理
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_GOT_IP6, 
        [](void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
            static_cast<Ota*>(arg)->handle_ip6_event(event_base, event_id, event_data);
        }, this));

    // 确保启用 IPv6 自动配置
    esp_err_t err = esp_netif_create_ip6_linklocal(netif);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "创建链路本地IPv6地址失败: %s", esp_err_to_name(err));
    } else {
        ESP_LOGI(TAG, "成功创建链路本地IPv6地址");
    }
    
    // 强制重新创建IPv6地址
    ESP_LOGI(TAG, "尝试强制重新创建IPv6地址");
    // 使用ESP-IDF 5.x兼容的API
    esp_netif_action_connected(netif, NULL, 0, NULL);
    
    // 打印当前网络接口状态
    ESP_LOGI(TAG, "当前网络接口状态：");
    esp_netif_ip_info_t ip_info;
    esp_netif_get_ip_info(netif, &ip_info);
    ESP_LOGI(TAG, "IPv4 地址: " IPSTR, IP2STR(&ip_info.ip));
    
    // 创建一个定时任务，每隔5秒检查一次IPv6状态
    ESP_LOGI(TAG, "创建IPv6监控任务");
    xTaskCreate([](void* arg) {
        Ota* ota = static_cast<Ota*>(arg);
        for (int i = 0; i < 10; i++) {  // 尝试10次，每次间隔5秒
            vTaskDelay(pdMS_TO_TICKS(5000));
            
            // 重新创建IPv6链路本地地址
            esp_err_t err = esp_netif_create_ip6_linklocal(ota->netif);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "重新创建链路本地IPv6地址失败: %s", esp_err_to_name(err));
            }
            
            // 打印当前IPv6地址
            ota->print_ipv6_address();
            
            // 检查是否有IPv6地址
            esp_ip6_addr_t ip6_addrs[CONFIG_LWIP_IPV6_NUM_ADDRESSES];
            int num_addrs = esp_netif_get_all_ip6(ota->netif, ip6_addrs);
            if (num_addrs > 0) {
                ESP_LOGI(TAG, "成功获取%d个IPv6地址，停止监控任务", num_addrs);
                break;  // 如果有IPv6地址了，就停止任务
            }
        }
        vTaskDelete(NULL);
    }, "ipv6_monitor", 4096, this, 1, NULL);
    
    
    // 等待一段时间，让SLAAC有机会获取地址
    vTaskDelay(pdMS_TO_TICKS(3000));

    // 检查是否已获取全局IPv6地址
    bool has_global = false;
    esp_ip6_addr_t ip6_addrs[CONFIG_LWIP_IPV6_NUM_ADDRESSES];
    int num_addrs = esp_netif_get_all_ip6(netif, ip6_addrs);
    
    for (int i = 0; i < num_addrs; ++i) {
        if (ip6_addrs[i].zone != 0 && !ip6_addr_isany_val(ip6_addrs[i])) {
            uint8_t prefix = (ip6_addrs[i].addr[0] >> 8) & 0xff;
            if (prefix != 0xfe) {
                has_global = true;
                break;
            }
        }
    }
    
    // 如果没有全局地址，尝试手动创建一个临时的ULA地址用于测试
    if (!has_global) {
        ESP_LOGW(TAG, "未获取到全局IPv6地址，尝试创建一个唯一本地地址(ULA)");
        esp_ip6_addr_t ula_addr;
        
        // 使用ULA前缀fd00::/8
        ula_addr.addr[0] = 0xfd000000;
        // 使用MAC地址最后几位作为标识符
        uint8_t mac[6];
        esp_wifi_get_mac(WIFI_IF_STA, mac);
        ula_addr.addr[1] = ((uint32_t)mac[0] << 24) | ((uint32_t)mac[1] << 16) | ((uint32_t)mac[2] << 8) | mac[3];
        ula_addr.addr[2] = ((uint32_t)mac[4] << 24) | ((uint32_t)mac[5] << 16);
        ula_addr.addr[3] = 0x00000001;  // 主机部分
        ula_addr.zone = 0;
        
        // 使用正确的API函数
        esp_netif_ip6_info_t ip6_info;
        memcpy(&ip6_info.ip, &ula_addr, sizeof(esp_ip6_addr_t));
        if (esp_netif_add_ip6_address(netif, ip6_info.ip, ESP_IP6_ADDR_IS_GLOBAL) == ESP_OK) {
            ESP_LOGI(TAG, "成功创建ULA地址，但注意这仅在本地网络有效");
        } else {
            ESP_LOGE(TAG, "创建ULA地址失败");
        }
    }

    // 打印当前 IPv6 地址
    // print_ipv6_address();

    return true;
}

// 添加TestUrlConnectivity函数实现
bool Ota::TestUrlConnectivity(const std::string& url) {
    ESP_LOGI(TAG, "测试URL连通性: %s", url.c_str());
    
    // 检查是否使用HTTPS
    bool is_https = (url.rfind("https://", 0) == 0);
    
    // 解析URL获取主机名
    std::string host;
    size_t host_start = url.find("://");
    if (host_start == std::string::npos) {
        ESP_LOGE(TAG, "URL格式错误");
        return false;
    }
    
    host_start += 3;
    size_t host_end = url.find("/", host_start);
    if (host_end == std::string::npos) {
        host = url.substr(host_start);
    } else {
        host = url.substr(host_start, host_end - host_start);
    }
    
    // 查找端口号
    size_t port_pos = host.find(":");
    std::string port_str;
    if (port_pos != std::string::npos) {
        port_str = host.substr(port_pos + 1);
        host = host.substr(0, port_pos);
    } else {
        port_str = is_https ? "443" : "80";
    }
    
    ESP_LOGI(TAG, "主机名: %s, 端口: %s", host.c_str(), port_str.c_str());
    
    // 进行DNS解析
    struct addrinfo hints = {}, *res = NULL;
    hints.ai_family = AF_INET6;  // 优先使用IPv6
    hints.ai_socktype = SOCK_STREAM;
    
    int err = getaddrinfo(host.c_str(), port_str.c_str(), &hints, &res);
    if (err != 0 || res == NULL) {
        ESP_LOGE(TAG, "IPv6 DNS解析失败: %s (错误: %d)", host.c_str(), err);
        
        // 尝试IPv4解析
        hints.ai_family = AF_INET;
        ESP_LOGI(TAG, "尝试使用IPv4解析: %s", host.c_str());
        err = getaddrinfo(host.c_str(), port_str.c_str(), &hints, &res);
        if (err != 0 || res == NULL) {
            ESP_LOGE(TAG, "IPv4 DNS解析也失败: %s (错误: %d)", host.c_str(), err);
            return false;
        }
    }
    
    ESP_LOGI(TAG, "DNS解析成功，尝试连接...");
    
    // 尝试连接
    int sock = -1;
    bool connected = false;
    
    // 先尝试IPv6连接
    for (struct addrinfo* p = res; p != NULL; p = p->ai_next) {
        if (p->ai_family != AF_INET6) continue; // 跳过非IPv6地址
        
        char addr_str[128];
        void* addr = &((struct sockaddr_in6*)p->ai_addr)->sin6_addr;
        inet_ntop(p->ai_family, addr, addr_str, sizeof(addr_str));
        ESP_LOGI(TAG, "尝试连接IPv6地址: %s", addr_str);
        
        sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sock < 0) {
            ESP_LOGE(TAG, "创建IPv6 socket失败");
            continue;
        }
        
        // 设置超时时间
        struct timeval timeout;
        timeout.tv_sec = 5;  // 5秒超时
        timeout.tv_usec = 0;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
        
        if (connect(sock, p->ai_addr, p->ai_addrlen) == 0) {
            ESP_LOGI(TAG, "IPv6连接成功: %s", addr_str);
            close(sock);
            connected = true;
            break;
        }
        
        ESP_LOGE(TAG, "IPv6连接失败: %s (错误: %d)", addr_str, errno);
        close(sock);
    }
    
    // 如果IPv6连接失败，尝试IPv4
    if (!connected) {
        ESP_LOGI(TAG, "尝试使用IPv4连接");
        for (struct addrinfo* p = res; p != NULL; p = p->ai_next) {
            if (p->ai_family != AF_INET) continue; // 跳过非IPv4地址
            
            char addr_str[128];
            void* addr = &((struct sockaddr_in*)p->ai_addr)->sin_addr;
            inet_ntop(p->ai_family, addr, addr_str, sizeof(addr_str));
            ESP_LOGI(TAG, "尝试连接IPv4地址: %s", addr_str);
            
            sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
            if (sock < 0) {
                ESP_LOGE(TAG, "创建IPv4 socket失败");
                continue;
            }
            
            // 设置超时时间
            struct timeval timeout;
            timeout.tv_sec = 5;  // 5秒超时
            timeout.tv_usec = 0;
            setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
            setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
            
            if (connect(sock, p->ai_addr, p->ai_addrlen) == 0) {
                ESP_LOGI(TAG, "IPv4连接成功: %s", addr_str);
                close(sock);
                connected = true;
                break;
            }
            
            ESP_LOGE(TAG, "IPv4连接失败: %s (错误: %d)", addr_str, errno);
            close(sock);
        }
    }
    
    freeaddrinfo(res);
    
    if (!connected) {
        ESP_LOGE(TAG, "所有地址连接尝试都失败");
        return false;
    }
    
    return true;
}

// 提供直接使用服务器IP地址的功能
bool Ota::CheckVersionWithIP(const std::string& ip_address) {
    if (ip_address.empty()) {
        ESP_LOGE(TAG, "IP地址为空");
        return false;
    }
    
    ESP_LOGI(TAG, "使用直接IP地址检查版本: %s", ip_address.c_str());
    
    // 保存原始URL以备后用
    std::string original_url = check_version_url_;
    
    // 提取原始URL中的路径部分
    std::string path = "/";
    size_t host_start = original_url.find("://");
    if (host_start != std::string::npos) {
        host_start += 3;
        size_t path_start = original_url.find("/", host_start);
        if (path_start != std::string::npos) {
            path = original_url.substr(path_start);
        }
    }
    
    // 判断原URL的协议类型
    bool is_https = original_url.find("https://") == 0;
    
    // 检查是否是IPv6地址（包含冒号）
    bool is_ipv6 = (ip_address.find(':') != std::string::npos);
    
    // 构建新的URL
    std::string new_url = is_https ? "https://" : "http://";
    
    // 如果是IPv6地址，需要用方括号包围
    if (is_ipv6) {
        // 检查地址是否已经有方括号
        if (ip_address[0] != '[') {
            new_url += "[" + ip_address + "]";
        } else {
            new_url += ip_address; // 已经有方括号，直接使用
        }
    } else {
        new_url += ip_address; // IPv4地址
    }
    
    new_url += path;
    
    ESP_LOGI(TAG, "生成的IP直接访问URL: %s (IPv6: %d)", new_url.c_str(), is_ipv6);
    
    // 临时设置新URL
    check_version_url_ = new_url;
    
    // 使用新URL进行检查
    bool result = CheckVersion();
    
    // 恢复原始URL
    check_version_url_ = original_url;
    
    return result;
}
