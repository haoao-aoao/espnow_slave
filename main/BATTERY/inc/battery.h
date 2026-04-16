#ifndef _BATTERY_H_
#define _BATTERY_H_

#include "config.h"

/**
 * @brief 初始化电池电量采集
 * @return esp_err_t
 */
esp_err_t battery_init(void);

/**
 * @brief 开始电池电量采集任务
 * @param interval_ms 采集间隔（毫秒）
 * @return esp_err_t
 */
esp_err_t battery_task_init(uint32_t interval_ms);

/**
 * @brief 获取当前电池电量百分比
 * @return uint8_t 电池电量百分比（0-100）
 */
uint8_t battery_get_percentage(void);

/**
 * @brief 获取当前电池电压
 * @return float 电池电压（单位：V）
 */
float battery_get_voltage(void);

/**
 * @brief 停止电池电量采集任务
 * @return esp_err_t
 */
esp_err_t battery_stop_task(void);

#endif /* _BATTERY_H_ */