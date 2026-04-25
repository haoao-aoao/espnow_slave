#include "mdata.h"
#include "motor.h"
#include "device.h"
#include "gap.h"

const static char *TAG = "DATA_TASK";
static TaskHandle_t data_task_handle = NULL;
static QueueHandle_t data_evt_queue = NULL;
static manu_data_t manufacturer_data = {0};

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

void update_manufacturer_data(void)
{
    uint8_t device_status = (get_device_device_state() << 0) | 
                            (get_device_motor_onOff() << 1) | 
                            (get_device_led_enable() << 3);

    manufacturer_data.flag = MDATA_FLAG;
    manufacturer_data.protocol_version = MDATA_PROTOCOL_VERSION;
    manufacturer_data.adv_interval = get_device_adv_interval();
    manufacturer_data.battery_level = 0;
    manufacturer_data.floor_number = get_device_floor();
    manufacturer_data.switch_index = get_device_switch_no();
    manufacturer_data.device_status = device_status;
    manufacturer_data.servo_open_angle = get_device_motor_on_angle();
    manufacturer_data.servo_close_angle = get_device_motor_off_angle();
    manufacturer_data.indicator_task = get_device_led_evt();
}

manu_data_t *get_manufacturer_data(void)
{
    update_manufacturer_data();

    return &manufacturer_data;
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

        if(flag1 != MDATA_FLAG || flag2 != MDATA_FLAG)
        {
            ESP_LOGE(TAG, "invalid data flag");
            return ret;
        }

        if(calculate_checksum(data+2, len-2-1) != checksum)
        {
            ESP_LOGE(TAG, "invalid checksum 0x%02X, expected 0x%02X", checksum, calculate_checksum(data+2, len-2-1));
            return ret;
        }

        if(get_device_device_state() == FACTORY_SETTING)
        {
            set_device_device_state(NORMAL_MODE);
        }

        if(data[2] != 0xFF && data[3] != 0xFF)
        {
            adv_interval = data[2] << 8 | data[3];
            ESP_LOGI(TAG, "adv_interval: %d", adv_interval);
            set_device_adv_interval(adv_interval);
        }
        
        if( data[4] != 0xFF )
        {
            led_enable = data[4];
            ESP_LOGI(TAG, "led_enable: %d", led_enable);
            set_device_led_enable(led_enable);
        }

        if( data[5] != 0xFF )
        {
            led_action = data[5];
            ESP_LOGI(TAG, "led_action: %d", led_action);
            set_device_led_evt(led_action);
        }
        
        if( data[6] != 0xFF )
        {
            floor = data[6];
            ESP_LOGI(TAG, "floor: %d", floor);
            set_device_floor(floor);
        }
        
        if( data[7] != 0xFF )
        {
            switch_no = data[7];
            ESP_LOGI(TAG, "switch_no: %d", switch_no);
            set_device_switch_no(switch_no);
        }

        if( data[8] != 0xFF )
        {
            motor_on_angle = data[8];
            ESP_LOGI(TAG, "motor_on_angle: %d", motor_on_angle);
            motor_evt_set_angle(motor_on_angle, 0xFF);
            set_device_motor_on_angle(motor_on_angle);
        }

        if( data[9] != 0xFF )
        {
            motor_off_angle = data[9];
            ESP_LOGI(TAG, "motor_off_angle: %d", motor_off_angle);
            motor_evt_set_angle(0xFF, motor_off_angle);
            set_device_motor_off_angle(motor_off_angle);
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
            set_device_motor_onOff(motor_onoff);
        }

        update_device_parm();
        update_advertising_data();
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

static void input_task(void *arg)
{
    int c;
    while (1)
    {
        // 直接用 getchar() 读 USB‑Serial 输入（阻塞）
        c = getchar();
        if (c != EOF)
        {
            printf("收到输入: %c (0x%02x)\n", c, c);

            switch (c)
            {
                case '1': motor_evt_on(); break;
                case '0': motor_evt_off(); break;
                
                default:
                {
                    ESP_LOGE(TAG, "invalid input %c", c);
                }
                break;
            }
        }
        vTaskDelay(10);
    }
}

void data_task_init(void)
{
    data_evt_queue = xQueueCreate(10, sizeof(data_event_data_t));
    xTaskCreate(data_task, "data_task", 2048, NULL, 10, &data_task_handle);

    xTaskCreate(input_task, "debug_input_task", 2048, NULL, 5, NULL);
}