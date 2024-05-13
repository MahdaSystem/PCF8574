# PCF8574 Library
Driver for PCF8574 and PCF8574A 8-bit I/O expander.


## Hardware Support
It is easy to port this library to any platform. But now it is ready for use in:
- ESP32 (esp-idf)


## How To Use
1. Add `PCF8574.h` and `PCF8574.c` files to your project.  It is optional to use `PCF8574_platform.h` and `PCF8574_platform.c` files (open and config `PCF8574_platform.h` file).
2. Initialize platform-dependent part of handler.
4. Call `PCF8574_Init()`.
5. Call other functions and enjoy.


## Example
<details>
<summary>Using PCF8574_platform files</summary>

```c
#include <stdio.h>
#include "PCF8574.h"
#include "PCF8574_platform.h"

int main(void)
{
  PCF8574_Handler_t Handler = {0};
  uint8_t OutData = 0x00;
  uint8_t InData = 0x00;

  PCF8574_Platform_Init(&Handler);
  PCF8574_Init(&Handler, PCF8574_DEVICE_PCF8574A, 0);
  PCF8574_SetDir(&Handler, 0x01);

  while (1)
  {
    OutData ^= 0x01;
    PCF8574_Write(&Handler, OutData);

    PCF8574_Read(&Handler, &InData);
    printf("Read Data: 0x%02X\r\n\r\n",
           InData);
  }

  PCF8574_DeInit(&Handler);
  return 0;
}
```
</details>


<details>
<summary>Without using PCF8574_platform files (esp-idf)</summary>

```c
#include <stdio.h>
#include <stdint.h>
#include "sdkconfig.h"
#include "esp_system.h"
#include "esp_err.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "PCF8574.h"

#define PCF8574_I2C_NUM   I2C_NUM_1
#define PCF8574_I2C_RATE  100000
#define PCF8574_SCL_GPIO  GPIO_NUM_13
#define PCF8574_SDA_GPIO  GPIO_NUM_14

int8_t
PCF8574_Platform_Init(void)
{
  i2c_config_t conf = {0};
  conf.mode = I2C_MODE_MASTER;
  conf.sda_io_num = PCF8574_SDA_GPIO;
  conf.sda_pullup_en = GPIO_PULLUP_DISABLE;
  conf.scl_io_num = PCF8574_SCL_GPIO;
  conf.scl_pullup_en = GPIO_PULLUP_DISABLE;
  conf.master.clk_speed = PCF8574_I2C_RATE;
  if (i2c_param_config(PCF8574_I2C_NUM, &conf) != ESP_OK)
    return -1;
  if (i2c_driver_install(PCF8574_I2C_NUM, conf.mode, 0, 0, 0) != ESP_OK)
    return -1;
  return 0;
}

int8_t
PCF8574_Platform_DeInit(void)
{
  i2c_driver_delete(PCF8574_I2C_NUM);
  gpio_reset_pin(PCF8574_SDA_GPIO);
  gpio_reset_pin(PCF8574_SCL_GPIO);
  return 0;
}

int8_t
PCF8574_Platform_Send(uint8_t Address, uint8_t *Data, uint8_t DataLen)
{
  i2c_cmd_handle_t PCF8574_i2c_cmd_handle = {0};
  Address <<= 1;
  Address &= 0xFE;

  PCF8574_i2c_cmd_handle = i2c_cmd_link_create();
  i2c_master_start(PCF8574_i2c_cmd_handle);
  i2c_master_write(PCF8574_i2c_cmd_handle, &Address, 1, 1);
  i2c_master_write(PCF8574_i2c_cmd_handle, Data, DataLen, 1);
  i2c_master_stop(PCF8574_i2c_cmd_handle);
  if (i2c_master_cmd_begin(PCF8574_I2C_NUM, PCF8574_i2c_cmd_handle, 1000 / portTICK_PERIOD_MS) != ESP_OK)
  {
    i2c_cmd_link_delete(PCF8574_i2c_cmd_handle);
    return -1;
  }
  i2c_cmd_link_delete(PCF8574_i2c_cmd_handle);
  return 0;
}

int8_t
PCF8574_Platform_Receive(uint8_t Address, uint8_t *Data, uint8_t DataLen)
{
  i2c_cmd_handle_t PCF8574_i2c_cmd_handle = {0};
  Address <<= 1;
  Address |= 0x01;

  PCF8574_i2c_cmd_handle = i2c_cmd_link_create();
  i2c_master_start(PCF8574_i2c_cmd_handle);
  i2c_master_write(PCF8574_i2c_cmd_handle, &Address, 1, 1);
  i2c_master_read(PCF8574_i2c_cmd_handle, Data, DataLen, I2C_MASTER_LAST_NACK);
  i2c_master_stop(PCF8574_i2c_cmd_handle);
  if (i2c_master_cmd_begin(PCF8574_I2C_NUM, PCF8574_i2c_cmd_handle, 1000 / portTICK_PERIOD_MS) != ESP_OK)
  {
    i2c_cmd_link_delete(PCF8574_i2c_cmd_handle);
    return -1;
  }
  i2c_cmd_link_delete(PCF8574_i2c_cmd_handle);
  return 0;
}


void app_main(void)
{
  PCF8574_Handler_t Handler = {0};
  uint8_t OutData = 0x00;
  uint8_t InData = 0x00;

  PCF8574_PLATFORM_LINK_INIT(Handler, PCF8574_Platform_Init);
  PCF8574_PLATFORM_LINK_DEINIT(Handler, PCF8574_Platform_DeInit);
  PCF8574_PLATFORM_LINK_SEND(Handler, PCF8574_Platform_Send);
  PCF8574_PLATFORM_LINK_RECEIVE(Handler, PCF8574_Platform_Receive);

  PCF8574_Init(&Handler, PCF8574_DEVICE_PCF8574A, 0);
  PCF8574_SetDir(&Handler, 0x01);

  while (1)
  {
    OutData ^= 0x01;
    PCF8574_Write(&Handler, OutData);

    PCF8574_Read(&Handler, &InData);
    printf("Read Data: 0x%02X\r\n\r\n",
           InData);

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }

  PCF8574_DeInit(&Handler);
}
```
</details>