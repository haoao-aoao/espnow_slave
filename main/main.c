#include "config.h"
#include "mled.h"
#include "battery.h"
#include "motor.h"
#include "ble_app.h"
#include "mdata.h"
#include "device.h"



void app_main(void)
{
    setvbuf(stdin, NULL, _IONBF, 0);

    printf("app start, version: %s\r\n", SW_VERSION);
    printf("Minimum free heap size: %"PRIu32" bytes\r\n", esp_get_minimum_free_heap_size());

    power_on_init();
    mled_task_init();
    // battery_task_init(1000 * 10);
    data_task_init();
    motor_task_init();
    ble_app_init();
}
