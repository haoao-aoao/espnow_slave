#include "config.h"
#include "mled.h"
#include "battery.h"
#include "motor.h"
#include "ble_app.h"
#include "mdata.h"



void app_main(void)
{
    printf("app start, version: %s\r\n", SW_VERSION);
    printf("Minimum free heap size: %"PRIu32" bytes\r\n", esp_get_minimum_free_heap_size());

    ble_app_init();

    mled_task_init();
    battery_task_init(1000 * 10);
    motor_task_init();
    data_task_init();
}
