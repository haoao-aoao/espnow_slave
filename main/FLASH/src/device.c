#include "device.h"

#define TAG "DEVICE"

static device_parm_t device_parm = {0};

static uint8_t CalculateChecksum(const void *data, uint32_t length)
{
  const uint8_t *ptr = (const uint8_t *)data;
  uint32_t checksum = 0;
  
  for (uint32_t i = 0; i < length; i++)
  {
    checksum += ptr[i];
  }
  
  return (uint8_t)(checksum & 0xFF);
}

uint8_t get_device_device_type(void)
{
    return device_parm.device_type;
}

void set_device_device_type(uint8_t device_type)
{
    device_parm.device_type = device_type;
}

uint8_t get_device_device_state(void)
{
    return device_parm.device_state;
}

void set_device_device_state(uint8_t device_state)
{
    device_parm.device_state = device_state;
}

uint8_t get_device_wireless_type(void)
{
    return device_parm.wireless_type;
}

void set_device_wireless_type(uint8_t wireless_type)
{
    device_parm.wireless_type = wireless_type;
}

uint16_t get_device_adv_interval(void)
{
    return device_parm.adv_interval;
}

void set_device_adv_interval(uint16_t adv_interval) 
{
    device_parm.adv_interval = adv_interval;
}

uint8_t get_device_led_enable(void)
{
    return device_parm.led_enable;
}   

void set_device_led_enable(uint8_t led_enable)
{
    device_parm.led_enable = led_enable;
}

uint8_t get_device_led_evt(void)
{
    return device_parm.led_evt;
}

void set_device_led_evt(uint8_t led_evt)
{
    device_parm.led_evt = led_evt;
}

uint8_t get_device_floor(void)
{
    return device_parm.floor;
}

void set_device_floor(uint8_t floor)
{
    device_parm.floor = floor;
}

uint8_t get_device_switch_no(void)
{
    return device_parm.switch_no;
}

void set_device_switch_no(uint8_t switch_no)
{
    device_parm.switch_no = switch_no;
}

uint8_t get_device_motor_on_angle(void)
{
    return device_parm.motor_on_angle;
}

void set_device_motor_on_angle(uint8_t motor_on_angle)
{
    device_parm.motor_on_angle = motor_on_angle;
}

uint8_t get_device_motor_off_angle(void)
{
    return device_parm.motor_off_angle;
}

void set_device_motor_off_angle(uint8_t motor_off_angle)
{
    device_parm.motor_off_angle = motor_off_angle;
}

uint8_t get_device_motor_onOff(void)
{
    return device_parm.motor_onOff;
}

void set_device_motor_onOff(uint8_t motor_onOff)
{
    device_parm.motor_onOff = motor_onOff;
}

static void set_default_device_parm(void)
{
    device_parm.device_type = DEVICE_TYPE;
    device_parm.device_state = FACTORY_SETTING;
    device_parm.wireless_type = WIRELESS_TYPE;
    device_parm.adv_interval = 1000;
    device_parm.led_enable = 0;
    device_parm.led_evt = 0;
    device_parm.floor = 1;
    device_parm.switch_no = 1;
    device_parm.motor_on_angle = 90;
    device_parm.motor_off_angle = 0;
    device_parm.motor_onOff = 0;
}

static void flash_init(void)
{
    esp_err_t ret;
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "failed to initialize nvs flash, error code: %d ", ret);
        return;
    }
}

static void save_device_parm(void)
{
    nvs_handle_t handle;
    nvs_open("device", NVS_READWRITE, &handle);

    device_parm.check_sum = CalculateChecksum(&device_parm + 1, sizeof(device_parm_t) - 1);

    nvs_set_blob(handle, "device_parm", &device_parm, sizeof(device_parm_t));
    nvs_commit(handle);
    nvs_close(handle);
}

static void load_device_parm(void)
{
    nvs_handle_t handle;
    nvs_open("device", NVS_READONLY, &handle);

    size_t required_size = sizeof(device_parm_t);
    nvs_get_blob(handle, "device_parm",  &device_parm, &required_size);
    nvs_close(handle);
}

static void init_device_parm(void)
{
    uint8_t device_state = get_device_device_state();
    if (device_state == FACTORY_SETTING)
    {
        // set_default_device_parm();
    }
    else if (device_state == NORMAL_MODE)
    {
        // load_device_parm();
    }
    else
    {
        set_default_device_parm();
        save_device_parm();
    }

    ESP_LOGI(TAG, "================ device_parm ====================");
    ESP_LOGI(TAG, "device_type: %d", get_device_device_type());
    ESP_LOGI(TAG, "device_state: %d", get_device_device_state());
    ESP_LOGI(TAG, "wireless_type: %d", get_device_wireless_type());
    ESP_LOGI(TAG, "adv_interval: %d", get_device_adv_interval());
    ESP_LOGI(TAG, "led_enable: %d", get_device_led_enable());
    ESP_LOGI(TAG, "led_evt: %d", get_device_led_evt());
    ESP_LOGI(TAG, "floor: %d", get_device_floor());
    ESP_LOGI(TAG, "switch_no: %d", get_device_switch_no());
    ESP_LOGI(TAG, "motor_on_angle: %d", get_device_motor_on_angle());
    ESP_LOGI(TAG, "motor_off_angle: %d", get_device_motor_off_angle());
    ESP_LOGI(TAG, "motor_onOff: %d", get_device_motor_onOff());
    ESP_LOGI(TAG, "==================== END ========================");
}

void update_device_parm(void)
{
    save_device_parm();
}

void power_on_init(void)
{
    flash_init();
    load_device_parm();
    init_device_parm();
}