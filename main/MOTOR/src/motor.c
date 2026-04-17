#include "motor.h"

const static char *TAG = "MOTOR_TASK";

static motor_obj_t motor = {
    .on_angle = MOTOR_DEFAULT_ON_ANGLE,
    .off_angle = MOTOR_DEFAULT_OFF_ANGLE
};
static TaskHandle_t motor_task_handle = NULL;
static QueueHandle_t motor_evt_queue = NULL;

static uint32_t motor_evt_send(motor_event_data_t evt)
{
    return xQueueSend(motor_evt_queue, &evt, portMAX_DELAY);
}

void motor_evt_set_angle(uint8_t on_angle, uint8_t off_angle)
{
    motor_event_data_t evt = {
        .evt = MOTOR_EVT_SET_ANGLE,
        .on_angle = on_angle,
        .off_angle = off_angle
    };

    motor_evt_send(evt);
}

void motor_evt_move_to_angle(uint8_t angle)
{
    motor_event_data_t evt = {
        .evt = MOTOR_EVT_MOVE_TO_ANGLE,
        .angle = angle
    };

    motor_evt_send(evt);
}

void motor_evt_on(void)
{
    motor_event_data_t evt = {
        .evt = MOTOR_EVT_MOVE_ON
    };

    motor_evt_send(evt);
}

void motor_evt_off(void)
{
    motor_event_data_t evt = {
        .evt = MOTOR_EVT_MOVE_OFF
    };

    motor_evt_send(evt);
}

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
    esp_err_t err = ledc_set_duty(LEDC_LOW_SPEED_MODE, channel, duty);
    if (err != ESP_OK) {
        return err;
    }
    
    // 应用占空比更改
    return ledc_update_duty(LEDC_LOW_SPEED_MODE, channel);
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

static void motor_task(void *pvParameters)
{
    motor_event_data_t evt;

    while (1) 
    {
        if (xQueueReceive(motor_evt_queue, (motor_event_data_t *)&evt, portMAX_DELAY) == pdTRUE) 
        {
            switch (evt.evt)
            {
                case MOTOR_EVT_SET_ANGLE:
                {
                    motor.on_angle = evt.on_angle;
                    motor.off_angle = evt.off_angle;
                }
                break;

                case MOTOR_EVT_MOVE_TO_ANGLE:
                {
                    motor_set_angle(LEDC_CHANNEL_0, evt.angle);
                }
                break;

                case MOTOR_EVT_MOVE_ON:
                {
                    motor_set_angle(LEDC_CHANNEL_0, motor.on_angle);
                }
                break;

                case MOTOR_EVT_MOVE_OFF:
                {
                    motor_set_angle(LEDC_CHANNEL_0, motor.off_angle);
                }
                break;

            default:
                break;
            }
        }
    }
}

esp_err_t motor_task_init(void)
{
    motor_init(MOTOR_PWM_PIN, LEDC_CHANNEL_0, 50);
    motor_evt_queue = xQueueCreate(10, sizeof(motor_event_data_t));

    xTaskCreate(motor_task, "motor_task", 2048, NULL, 10, &motor_task_handle);

    return ESP_OK;
}