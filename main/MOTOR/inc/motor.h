#ifndef _MOTOR_H_
#define _MOTOR_H_

#include "config.h"

#define MOTOR_PWM_PIN     GPIO_NUM_9

#define MOTOR_DEFAULT_ON_ANGLE     60
#define MOTOR_DEFAULT_OFF_ANGLE    0

typedef enum {
    MOTOR_EVT_SET_ANGLE = 0,     /*!< 设置舵机角度事件 */
    MOTOR_EVT_MOVE_TO_ANGLE,     /*!< 舵机移动到指定角度事件 */
    MOTOR_EVT_MOVE_ON,           /*!< 舵机移动事件 */
    MOTOR_EVT_MOVE_OFF,          /*!< 停止舵机事件 */
} motor_evt_t;

typedef struct {
    uint8_t evt;         /*!< 事件类型 */
    uint8_t angle;       /*!< 目标角度值，仅在相关事件中使用 */
    uint8_t on_angle;    /*!< 开启角度值，仅在相关事件中使用 */
    uint8_t off_angle;   /*!< 关闭角度值，仅在相关事件中使用 */
} motor_event_data_t;

typedef struct {
    uint8_t on_angle;        /*!< 开启角度值，仅在相关事件中使用 */
    uint8_t off_angle;       /*!< 关闭角度值，仅在相关事件中使用 */
} motor_obj_t;

void motor_evt_set_angle(uint8_t on_angle, uint8_t off_angle);
void motor_evt_move_to_angle(uint8_t angle);
void motor_evt_on(void);
void motor_evt_off(void);

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

/**
 * @brief 初始化舵机任务
 * @return esp_err_t
 */
esp_err_t motor_task_init(void);

#endif /* _MOTOR_H_ */