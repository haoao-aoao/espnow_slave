#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdio.h>
#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "nvs_flash.h"
#include "nvs.h"

#define SW_VERSION "1.0.0"

#define SLAVE       0
#define MASTER      1
#define DEVICE_TYPE     SLAVE

#define BLUETOOTH   0
#define ESPNOW      1
#define WIRELESS_TYPE   BLUETOOTH


#endif