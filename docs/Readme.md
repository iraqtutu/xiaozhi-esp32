    要使用十方的小智服务，请修改配置将OTA地址修改为
    default "https://api.tenclass.net/xiaozhi/ota/"
    //default "wss://api.tenclass.net/xiaozhi/v1/"
    修改idf_component.yml
        78/esp-ml307: "~1.7.2"




    使用东方信通的服务，将OTA地址修改为
    default "http://115.190.88.174/ota"

    iraqtutu/esp-ml307:
        git: https://github.com/iraqtutu/esp-ml307.git
        version: main  # 或其他分支名

