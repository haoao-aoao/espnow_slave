#ifndef __MDATA_H__
#define __MDATA_H__

#include "config.h"

#define DATA_FLAG 0xFE

typedef enum {
    DATA_EVT_RECEIVE = 0,     /*!< 接收数据事件 */
} data_evt_t;

typedef struct {
    uint8_t evt;         /*!< 事件类型 */
    uint8_t source;      /*!< 数据来源 */
    uint8_t len;         /*!< 数据长度 */
    uint8_t data[256];    /*!< 接收数据，最大256字节 */
} data_event_data_t;

extern void data_evt_send(uint8_t source, uint8_t *data, uint8_t len);
extern void data_task_init(void);

#endif