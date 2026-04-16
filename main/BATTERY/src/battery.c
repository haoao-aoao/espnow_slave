#include "battery.h"

const static char *TAG = "BATTERY_TASK";

#define ADC_CHANNEL ADC_CHANNEL_0  // GPIO0 对应 ADC1_CHANNEL_0
#define ADC_ATTENUATION ADC_ATTEN_DB_12
#define SAMPLE_COUNT 10  // 采样次数，用于滤波

// 电池电压参数
#define BATTERY_MIN_VOLTAGE 3000  // 电池最低电压
#define BATTERY_MAX_VOLTAGE 4200  // 电池最高电压

static adc_oneshot_unit_handle_t adc0_handle;
static adc_cali_handle_t adc0_cali_chan0_handle = NULL;
static TaskHandle_t battery_task_handle = NULL;
static uint8_t battery_percentage = 0;
static int battery_voltage = 0;
static int adc_raw = 0, voltage = 0;
static bool do_calibration1_chan0 = false;

/**
 * @brief 采集电池电压
 * @return float 电池电压（单位：V）
 */
static int battery_read_voltage(void)
{
    uint32_t adc_reading = 0;
    
    // 多次采样并取平均值
    for (int i = 0; i < SAMPLE_COUNT; i++) {
        adc_oneshot_read(adc0_handle, ADC_CHANNEL, &adc_raw);
        adc_reading += adc_raw;
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    adc_reading /= SAMPLE_COUNT;
    
    // 转换为电压值
    if (do_calibration1_chan0) {
        adc_cali_raw_to_voltage(adc0_cali_chan0_handle, adc_reading, &voltage);
    }
    else {
        ESP_LOGE(TAG, "no ready calibration");
    }
    
    // 假设使用了电压 divider，这里需要根据实际电路调整
    // 例如：如果使用了 10kΩ 和 10kΩ 的分压电阻，需要乘以 2
    // voltage *= 2.0f;
    
    return voltage;
}

/**
 * @brief 计算电池电量百分比
 * @param voltage 电池电压
 * @return uint8_t 电池电量百分比
 */
static uint8_t battery_calculate_percentage(int voltage)
{
    if (voltage <= BATTERY_MIN_VOLTAGE) {
        return 0;
    } else if (voltage >= BATTERY_MAX_VOLTAGE) {
        return 100;
    } else {
        // 线性计算电量百分比
        return (uint8_t)((voltage - BATTERY_MIN_VOLTAGE) / (BATTERY_MAX_VOLTAGE - BATTERY_MIN_VOLTAGE) * 100);
    }
}

/**
 * @brief 电池采集任务
 * @param pvParameters 任务参数
 */
static void battery_task(void *pvParameters)
{
    uint32_t interval_ms = *(uint32_t *)pvParameters;
    
    while (1) {
        // 读取电池电压
        int voltage = battery_read_voltage();
        
        // 计算电量百分比
        uint8_t percentage = battery_calculate_percentage(voltage);
        
        // 更新全局变量
        battery_voltage = voltage;
        battery_percentage = percentage;
        
        // 打印日志
        ESP_LOGI(TAG, "Battery voltage: %dmV, Percentage: %d%%", voltage, percentage);
        
        // 延时到下一次采集
        vTaskDelay(pdMS_TO_TICKS(interval_ms));
    }
}

static bool example_adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle)
{
    adc_cali_handle_t handle = NULL;
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    if (!calibrated) {
        ESP_LOGI(TAG, "calibration scheme version is %s", "Curve Fitting");
        adc_cali_curve_fitting_config_t cali_config = {
            .unit_id = unit,
            .chan = channel,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif

#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    if (!calibrated) {
        ESP_LOGI(TAG, "calibration scheme version is %s", "Line Fitting");
        adc_cali_line_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }
#endif

    *out_handle = handle;
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Calibration Success");
    } else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated) {
        ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
    } else {
        ESP_LOGE(TAG, "Invalid arg or no memory");
    }

    return calibrated;
}

/**
 * @brief 初始化电池电量采集
 * @return esp_err_t
 */
esp_err_t battery_init(void)
{
        //-------------ADC0 Init---------------//
    
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc0_handle));

    //-------------ADC1 Config---------------//
    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTENUATION,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc0_handle, ADC_CHANNEL, &config));

    //-------------ADC1 Calibration Init---------------//
    do_calibration1_chan0 = example_adc_calibration_init(ADC_UNIT_1, ADC_CHANNEL, ADC_ATTENUATION, &adc0_cali_chan0_handle);
    
    return ESP_OK;
}

/**
 * @brief 开始电池电量采集任务
 * @param interval_ms 采集间隔（毫秒）
 * @return esp_err_t
 */
esp_err_t battery_task_init(uint32_t interval_ms)
{
    if (battery_task_handle != NULL) {
        return ESP_OK; // 任务已经在运行
    }

    battery_init();
    // 创建电池采集任务
    xTaskCreate(battery_task, "battery_task", 2048, &interval_ms, 5, &battery_task_handle);
    
    return ESP_OK;
}

/**
 * @brief 获取当前电池电量百分比
 * @return uint8_t 电池电量百分比（0-100）
 */
uint8_t battery_get_percentage(void)
{
    return battery_percentage;
}

/**
 * @brief 获取当前电池电压
 * @return float 电池电压（单位：V）
 */
float battery_get_voltage(void)
{
    return battery_voltage;
}

/**
 * @brief 停止电池电量采集任务
 * @return esp_err_t
 */
esp_err_t battery_stop_task(void)
{
    if (battery_task_handle != NULL) {
        vTaskDelete(battery_task_handle);
        battery_task_handle = NULL;
    }
    
    return ESP_OK;
}