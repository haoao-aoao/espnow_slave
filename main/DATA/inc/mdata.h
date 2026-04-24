#ifndef __MDATA_H__
#define __MDATA_H__

#include "config.h"

#define MDATA_FLAG 0xFE
#define MDATA_PROTOCOL_VERSION 0x01

typedef enum {
    DATA_EVT_RECEIVE = 0,     /*!< 接收数据事件 */
} data_evt_t;

typedef struct {
    uint8_t evt;         /*!< 事件类型 */
    uint8_t source;      /*!< 数据来源 */
    uint8_t len;         /*!< 数据长度 */
    uint8_t data[256];    /*!< 接收数据，最大256字节 */
} data_event_data_t;


#pragma pack(push, 1)
typedef struct {
    uint8_t flag;                    /*!< 特殊标识: 0xFE */
    uint8_t protocol_version;        /*!< 协议版本 */
    uint16_t adv_interval;           /*!< 广播间隔: 单位 0.625 ms */
    uint8_t battery_level;           /*!< 电量: 百分比 */
    uint8_t floor_number;            /*!< 楼层数 */
    uint8_t switch_index;            /*!< 开关序号 */
    uint8_t device_status;           /*!< 设备状态: 
                                      * bit0: 出厂状态 (0:初始/已安装)
                                      * bit1: 舵机开关状态 (0:关闭, 1:开启)
                                      * bit2: 红外状态 (0:关闭, 1:开启)
                                      * bit3: 指示灯开关状态 (0:关闭, 1:开启)
                                      */
    uint8_t servo_open_angle;        /*!< 舵机开角度: 0~180 */
    uint8_t servo_close_angle;       /*!< 舵机关角度: 0~180 */
    uint8_t indicator_task;          /*!< 指示灯任务 */
} manu_data_t;
#pragma pack(pop)


extern manu_data_t *get_manufacturer_data(void);
extern void data_evt_send(uint8_t source, uint8_t *data, uint8_t len);
extern void data_task_init(void);

#endif