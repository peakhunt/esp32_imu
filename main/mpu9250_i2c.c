#include <stdio.h>
#include "driver/i2c.h"
#include "mpu9250_i2c.h"

#define WRITE_BIT                          I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT                           I2C_MASTER_READ  /*!< I2C master read */
#define ACK_CHECK_EN                       0x1              /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS                      0x0              /*!< I2C master will not check ack from slave */
#define ACK_VAL                            0x0              /*!< I2C ack value */
#define NACK_VAL                           0x1              /*!< I2C nack value */

void
mpu9250_i2c_init(void)
{
  i2c_config_t            conf;

  conf.mode               = I2C_MODE_MASTER;
  conf.sda_io_num         = MPU9250_I2C_MASTER_SDA_IO;
  conf.sda_pullup_en      = GPIO_PULLUP_DISABLE;
  conf.scl_io_num         = MPU9250_I2C_MASTER_SCL_IO;
  conf.scl_pullup_en      = GPIO_PULLUP_DISABLE;
  conf.master.clk_speed   = MPU9250_I2C_CLK_FREQ;

  i2c_param_config(MPU9250_I2C_NUM, &conf);

  i2c_driver_install(MPU9250_I2C_NUM, conf.mode,
      MPU9250_I2C_RX_BUFFER_DISABLE,
      MPU9250_I2C_TX_BUFFER_DISABLE, 0);

}

bool
mpu9250_i2c_read(uint8_t addr, uint8_t* data, uint32_t size)
{
  i2c_cmd_handle_t    cmd = i2c_cmd_link_create();

  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (addr << 1) | READ_BIT, ACK_CHECK_EN);
  if(size > 1)
  {
    i2c_master_read(cmd, data, size - 1, ACK_VAL);
  }
  i2c_master_read_byte(cmd, &data[size-1], NACK_VAL);
  i2c_master_stop(cmd);

  esp_err_t ret = i2c_master_cmd_begin(MPU9250_I2C_NUM, cmd, 1000 / portTICK_RATE_MS);
  i2c_cmd_link_delete(cmd);

  if(ret == ESP_OK)
  {
    return TRUE;
  }
  return FALSE;
}

bool
mpu9250_i2c_write(uint8_t addr, uint8_t* data, uint32_t size)
{
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();

  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, ( addr << 1 ) | WRITE_BIT, ACK_CHECK_EN);
  i2c_master_write(cmd, data, size, ACK_CHECK_EN);
  i2c_master_stop(cmd);

  esp_err_t ret = i2c_master_cmd_begin(MPU9250_I2C_NUM, cmd, 1000 / portTICK_RATE_MS);
  i2c_cmd_link_delete(cmd);

  if(ret == ESP_OK)
  {
    return TRUE;
  }
  return FALSE;
}
