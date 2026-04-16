#include "mled.h"

static QueueHandle_t mled_evt_queue = NULL;

void mled_d4_on(void)
{
    gpio_set_level(LED_GPIO_D4, 1);
}

void mled_d4_off(void)
{
    gpio_set_level(LED_GPIO_D4, 0);
}

void mled_d5_on(void)
{
    gpio_set_level(LED_GPIO_D5, 1);
}

void mled_d5_off(void)
{
    gpio_set_level(LED_GPIO_D5, 0);
}

void mled_io_init(void)
{
    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = (1ULL<<LED_GPIO_D4) | (1ULL<<LED_GPIO_D5) | (1ULL<<TEST_GPIO);
    //disable pull-down mode
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    //disable pull-up mode
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    //configure GPIO with the given settings
    gpio_config(&io_conf);
}

static void mled_handle(void* arg)
{
    uint32_t io_num, cnt = 0;
    for (;;) 
    {
        // if (xQueueReceive(mled_evt_queue, &io_num, portMAX_DELAY)) {
        //     printf("GPIO[%"PRIu32"] intr, val: %d\n", io_num, gpio_get_level(io_num));
        // }
        if(cnt % 2 == 0)
        {
            mled_d4_on();
            mled_d5_off();
        }
        else
        {
            mled_d4_off();
            mled_d5_on();
        }

        cnt++;
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void mled_task_init(void)
{
    mled_io_init();

    gpio_set_level(TEST_GPIO, 1);

    mled_evt_queue = xQueueCreate(10, sizeof(uint32_t));

    xTaskCreate(mled_handle, "mled_task", 2048, NULL, 10, NULL);
}