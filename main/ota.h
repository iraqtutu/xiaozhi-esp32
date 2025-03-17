#ifndef _OTA_H
#define _OTA_H

#include <functional>
#include <string>
#include <map>
#include "esp_event.h"
#include "esp_netif.h"

class Ota {
public:
    Ota();
    ~Ota();

    void SetCheckVersionUrl(std::string check_version_url);
    void SetHeader(const std::string& key, const std::string& value);
    void SetPostData(const std::string& post_data);
    bool CheckVersion();
    bool HasNewVersion() { return has_new_version_; }
    bool HasMqttConfig() { return has_mqtt_config_; }
    bool HasActivationCode() { return has_activation_code_; }
    bool HasServerTime() { return has_server_time_; }
    void StartUpgrade(std::function<void(int progress, size_t speed)> callback);
    void MarkCurrentVersionValid();
    void print_ipv6_address();
    bool Init();
    void handle_ip6_event(esp_event_base_t event_base, int32_t event_id, void* event_data);

    const std::string& GetFirmwareVersion() const { return firmware_version_; }
    const std::string& GetCurrentVersion() const { return current_version_; }
    const std::string& GetActivationMessage() const { return activation_message_; }
    const std::string& GetActivationCode() const { return activation_code_; }
    
    // 网络接口（需要从任务中访问）
    esp_netif_t* netif;

    // 添加测试URL连通性的函数声明
    bool TestUrlConnectivity(const std::string& url);

    // 添加使用IP地址直接访问的函数声明
    bool CheckVersionWithIP(const std::string& ip_address);

private:
    std::string check_version_url_;
    std::string activation_message_;
    std::string activation_code_;
    bool has_new_version_ = false;
    bool has_mqtt_config_ = false;
    bool has_server_time_ = false;
    bool has_activation_code_ = false;
    std::string current_version_;
    std::string firmware_version_;
    std::string firmware_url_;
    std::string post_data_;
    std::map<std::string, std::string> headers_;

    void Upgrade(const std::string& firmware_url);
    std::function<void(int progress, size_t speed)> upgrade_callback_;
    std::vector<int> ParseVersion(const std::string& version);
    bool IsNewVersionAvailable(const std::string& currentVersion, const std::string& newVersion);
};

#endif // _OTA_H
