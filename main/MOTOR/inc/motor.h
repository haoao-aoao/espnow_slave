#ifndef _MOTOR_H_
#define _MOTOR_H_

#include "config.h"


/**
 * @brief 初始化舵机控制
 * @param pin 舵机控制引脚
 * @param channel LEDC通道
 * @param frequency PWM频率
 * @return esp_err_t
 */
esp_err_t motor_init(int pin, ledc_channel_t channel, uint32_t frequency);

/**
 * @brief 控制舵机角度
 * @param channel LEDC通道
 * @param angle 角度值 (0-180)
 * @return esp_err_t
 */
esp_err_t motor_set_angle(ledc_channel_t channel, uint8_t angle);

/**
 * @brief 停止舵机控制
 * @param channel LEDC通道
 * @return esp_err_t
 */
esp_err_t motor_deinit(ledc_channel_t channel);

#endif /* _MOTOR_H_ */