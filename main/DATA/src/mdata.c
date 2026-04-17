#include "mdata.h"
#include "motor.h"

const static char *TAG = "DATA_TASK";
static TaskHandle_t data_task_handle = NULL;
static QueueHandle_t data_evt_queue = NULL;

/**
 * @brief 计算数据的累加和
 * @param data 数据指针
 * @param len 数据长度（包含校验和字节）
 * @return 累加和结果
 */
static uint8_t calculate_checksum(uint8_t *data, uint8_t len)
{
    uint8_t sum = 0;

    for (uint8_t i = 0; i < len; i++) {
        sum += data[i];
    }
    return sum & 0xFF;
}

void data_evt_send(uint8_t source, uint8_t *data, uint8_t len)
{
    data_event_data_t evt = {
        .evt = DATA_EVT_RECEIVE,
        .source = source,
        .len = len
    };
   
    memcpy(evt.data, data, len);
    xQueueSend(data_evt_queue, &evt, portMAX_DELAY);
}

static int data_hanle(uint8_t *data, uint8_t len)
{
    int ret = -1;
    uint8_t flag1, flag2, checksum;
    uint16_t adv_interval;
    uint8_t led_enable, led_action, floor, switch_no, motor_on_angle, motor_off_angle, motor_onoff;

    if(data || len > 0)
    {
        flag1 = data[0];
        flag2 = data[1];
        checksum = data[len - 1];

        if(flag1 != DATA_FLAG || flag2 != DATA_FLAG)
        {
            ESP_LOGE(TAG, "invalid data flag");
            return ret;
        }

        if(calculate_checksum(data+2, len-2-1) != checksum)
        {
            ESP_LOGE(TAG, "invalid checksum 0x%02X, expected 0x%02X", checksum, calculate_checksum(data+2, len-2-1));
            return ret;
        }

        if(data[2] != 0xFF && data[3] != 0xFF)
        {
            adv_interval = data[2] << 8 | data[3];
            ESP_LOGI(TAG, "adv_interval: %d", adv_interval);
        }
        
        if( data[4] != 0xFF )
        {
            led_enable = data[4];
            ESP_LOGI(TAG, "led_enable: %d", led_enable);
        }

        if( data[5] != 0xFF )
        {
            led_action = data[5];
            ESP_LOGI(TAG, "led_action: %d", led_action);
        }
        
        if( data[6] != 0xFF )
        {
            floor = data[6];
            ESP_LOGI(TAG, "floor: %d", floor);
        }
        
        if( data[7] != 0xFF )
        {
            switch_no = data[7];
            ESP_LOGI(TAG, "switch_no: %d", switch_no);
        }

        if( data[8] != 0xFF )
        {
            motor_on_angle = data[8];
            ESP_LOGI(TAG, "motor_on_angle: %d", motor_on_angle);
        }

        if( data[9] != 0xFF )
        {
            motor_off_angle = data[9];
            ESP_LOGI(TAG, "motor_off_angle: %d", motor_off_angle);  
        }

        if( data[10] != 0xFF )
        {
            motor_onoff = data[10];
            ESP_LOGI(TAG, "motor_onoff: %d", motor_onoff);
            if(motor_onoff)
            {
                motor_evt_on();
            }
            else
            {
                motor_evt_off();
            }
        }
    }

    return ret;
}


static void data_task(void *param)
{
    ESP_LOGI(TAG, "data task has been started!");
    data_event_data_t evt;

    while (1)
    {
        if (xQueueReceive(data_evt_queue, (data_event_data_t *)&evt, portMAX_DELAY) == pdTRUE) 
        {
            switch (evt.evt)
            {
                case DATA_EVT_RECEIVE:
                {
                    ESP_LOGI(TAG, "received data from source %d", evt.source);
                    data_hanle(evt.data, evt.len);
                }
                break;

            default:
                break;
            }
        }
    }
    
    /* Clean up at exit */
    vTaskDelete(NULL);
}

void data_task_init(void)
{
    data_evt_queue = xQueueCreate(10, sizeof(data_event_data_t));
    xTaskCreate(data_task, "data_task", 2048, NULL, 10, &data_task_handle);
}