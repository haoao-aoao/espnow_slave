#ifndef __DEVICE_H__
#define __DEVICE_H__

#include "config.h"



/**
 * @brief 定义设备模式
 *
 */
typedef enum
{
  FACTORY_SETTING                     = 0x10, // 出厂设置
  NORMAL_MODE                         = 0x11, // 正常模式
} device_state_t;

typedef struct
{
    uint8_t check_sum;
    uint8_t device_type; // SLAVE or MASTER
    uint8_t device_state; // 0: idle, 1: working
    uint8_t wireless_type; // BLUETOOTH or ESPNOW
    uint16_t adv_interval; // advertising interval in ms, only for BLUETOOTH
    uint8_t led_enable; // whether to enable LED control, only for BLUETOOTH
    uint8_t led_evt; // LED event, only for BLUETOOTH
    uint8_t floor; // floor number
    uint8_t switch_no; // switch number, only for SLAVE
    uint8_t motor_on_angle; // motor on angle, only for BLUETOOTH
    uint8_t motor_off_angle; // motor off angle, only for BLUETOOTH
    uint8_t motor_onOff; // whether to turn on or off the motor, only for BLUETOOTH
    uint8_t reserved[2]; // reserved for future use
} device_parm_t;

extern uint8_t get_device_device_type(void);
extern void set_device_device_type(uint8_t device_type);

extern uint8_t get_device_device_state(void);
extern void set_device_device_state(uint8_t device_state);

extern uint8_t get_device_wireless_type(void);
extern void set_device_wireless_type(uint8_t wireless_type);

extern uint16_t get_device_adv_interval(void);
extern void set_device_adv_interval(uint16_t adv_interval);

extern uint8_t get_device_led_enable(void);
extern void set_device_led_enable(uint8_t led_enable);

extern uint8_t get_device_led_evt(void);
extern void set_device_led_evt(uint8_t led_evt);

extern uint8_t get_device_floor(void);
extern void set_device_floor(uint8_t floor);

extern uint8_t get_device_switch_no(void);
extern void set_device_switch_no(uint8_t switch_no);

extern uint8_t get_device_motor_on_angle(void);
extern void set_device_motor_on_angle(uint8_t motor_on_angle);

extern uint8_t get_device_motor_off_angle(void);
extern void set_device_motor_off_angle(uint8_t motor_off_angle);

extern uint8_t get_device_motor_onOff(void);
extern void set_device_motor_onOff(uint8_t motor_onOff);

extern void update_device_parm(void);
extern void power_on_init(void);

#endif /* __DEVICE_H__ */