#ifndef __MLED_H__
#define __MLED_H__

#include "config.h"

#define LED_GPIO_D4     GPIO_NUM_12
#define LED_GPIO_D5     GPIO_NUM_13
#define TEST_GPIO       GPIO_NUM_1

extern void mled_d4_on(void);
extern void mled_d4_off(void);
extern void mled_d5_on(void);
extern void mled_d5_off(void);
extern void mled_task_init(void);

#endif