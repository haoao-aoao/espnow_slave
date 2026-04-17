idf_component_register(SRCS "main.c" "LED/src/mled.c" "MOTOR/src/motor.c" "BATTERY/src/battery.c" "BLE/src/ble_app.c" "BLE/src/gatt_svc.c" "BLE/src/gap.c" "BLE/src/heart_rate_mock.c" "DATA/src/mdata.c"
                    PRIV_REQUIRES 
                    nvs_flash 
                    esp_event 
                    esp_netif 
                    esp_wifi
                    esp_driver_gpio
                    esp_driver_ledc
                    esp_adc
                    bt
                    INCLUDE_DIRS "." "LED/inc" "MOTOR/inc" "BATTERY/inc" "BLE/inc" "DATA/inc")
