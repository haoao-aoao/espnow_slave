#include "motor.h"

/**
 * @brief 初始化舵机控制
 * @param pin 舵机控制引脚
 * @param channel LEDC通道
 * @param frequency PWM频率
 * @return esp_err_t
 */
esp_err_t motor_init(int pin, ledc_channel_t channel, uint32_t frequency)
{
    // 配置LEDC定时器
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_12_BIT,  // 12位分辨率
        .freq_hz = frequency,                  // PWM频率
        .speed_mode = LEDC_LOW_SPEED_MODE,     // 低速模式
        .timer_num = LEDC_TIMER_0,             // 定时器0
        .clk_cfg = LEDC_AUTO_CLK,              // 自动时钟源
    };
    
    // 配置LEDC通道
    ledc_channel_config_t ledc_channel = {
        .channel = channel,
        .duty = 0,
        .gpio_num = pin,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .hpoint = 0,
        .timer_sel = LEDC_TIMER_0,
        .flags = {
            .output_invert = false
        }
    };
    
    // 应用定时器配置
    esp_err_t err = ledc_timer_config(&ledc_timer);
    if (err != ESP_OK) {
        return err;
    }
    
    // 应用通道配置
    err = ledc_channel_config(&ledc_channel);
    if (err != ESP_OK) {
        return err;
    }
    
    return ESP_OK;
}

/**
 * @brief 控制舵机角度
 * @param channel LEDC通道
 * @param angle 角度值 (0-180)
 * @return esp_err_t
 */
esp_err_t motor_set_angle(ledc_channel_t channel, uint8_t angle)
{
    // 限制角度范围
    if (angle > 180) {
        angle = 180;
    }
    
    // 计算占空比
    // SG90舵机的PWM信号周期为20ms，高电平时间为0.5ms-2.5ms
    // 对应角度0-180度
    uint32_t duty = (angle * 4095 / 180) * 2.5 / 20;
    duty = (duty < 102) ? 102 : duty;  // 最小占空比（0.5ms）
    duty = (duty > 512) ? 512 : duty;  // 最大占空比（2.5ms）
    
    // 设置占空比
    return ledc_set_duty(LEDC_LOW_SPEED_MODE, channel, duty);
}

/**
 * @brief 停止舵机控制
 * @param channel LEDC通道
 * @return esp_err_t
 */
esp_err_t motor_deinit(ledc_channel_t channel)
{
    // 停止LEDC通道
    return ledc_stop(LEDC_LOW_SPEED_MODE, channel, 0);
}