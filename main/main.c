#include "config.h"
#include "mled.h"
#include "battery.h"

void app_main(void)
{
    printf("app start\r\n");
    printf("Minimum free heap size: %"PRIu32" bytes\r\n", esp_get_minimum_free_heap_size());

    mled_task_init();
    battery_task_init(1000 * 10);
}
