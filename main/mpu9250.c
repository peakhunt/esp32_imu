#include "mpu9250.h"
#include "mpu9250_i2c.h"

#include "esp_log.h"
#include "sdkconfig.h"

const static char* TAG = "mpu9250";

////////////////////////////////////////////////////////////////////////////////
//
// private utilities
//
////////////////////////////////////////////////////////////////////////////////
static inline void
mpu9250_write_reg(uint8_t reg, uint8_t data)
{ 
  uint8_t buffer[2];

  buffer[0] = reg;
  buffer[1] = data;

  if(mpu9250_i2c_write(MPU9250_I2C_ADDR, buffer, 2) == FALSE)
  {
    ESP_LOGE(TAG, "mpu9250_write_reg: failed to mpu9250_i2c_write");
  }
}

static inline void
mpu9250_write_reg16(uint8_t reg, uint16_t data)
{ 
  uint8_t buffer[3];

  buffer[0] = reg;
  buffer[1] = (data >> 8 ) & 0xff;
  buffer[2] = data & 0xff;

  if(mpu9250_i2c_write(MPU9250_I2C_ADDR, buffer, 3) == FALSE)
  {
    ESP_LOGE(TAG, "mpu9250_write_reg16: failed to mpu9250_i2c_write");
  }
}

static inline uint8_t
mpu9250_read_reg(uint8_t reg)
{
  uint8_t ret;

  if(mpu9250_i2c_write(MPU9250_I2C_ADDR, &reg, 1) == FALSE)
  {
    ESP_LOGE(TAG, "mpu9250_read_reg: failed to mpu9250_i2c_write");
  }


  if(mpu9250_i2c_read(MPU9250_I2C_ADDR, &ret, 1) == FALSE)
  {
    ESP_LOGE(TAG, "mpu9250_read_reg: failed to mpu9250_i2c_read");
  }

  return ret;
}

static inline void
mpu9250_read_data(uint8_t reg, uint8_t* data, uint8_t len)
{ 
  if(mpu9250_i2c_write(MPU9250_I2C_ADDR, &reg, 1) == FALSE)
  {
    ESP_LOGE(TAG, "mpu9250_read_data: failed to mpu9250_i2c_write");
  }

  if(mpu9250_i2c_read(MPU9250_I2C_ADDR, data, len) == FALSE)
  {
    ESP_LOGE(TAG, "mpu9250_read_data: failed to mpu9250_i2c_read");
  }
} 

////////////////////////////////////////////////////////////////////////////////
//
// public utilities
//
////////////////////////////////////////////////////////////////////////////////
void
mpu9250_init(MPU9250_Accelerometer_t accel_sensitivity, MPU9250_Gyroscope_t gyro_sensitivity)
{
  mpu9250_i2c_init();

  uint8_t temp;

  /* Wakeup MPU6050 */
  mpu9250_write_reg(MPU9250_PWR_MGMT_1, 0x00);

  /* Config accelerometer */
  temp = mpu9250_read_reg(MPU9250_ACCEL_CONFIG);
  temp = (temp & 0xE7) | (uint8_t)accel_sensitivity << 3;
  mpu9250_write_reg(MPU9250_ACCEL_CONFIG, temp);

  /* Config gyroscope */
  temp = mpu9250_read_reg(MPU9250_GYRO_CONFIG);
  temp = (temp & 0xE7) | (uint8_t)gyro_sensitivity << 3;
  mpu9250_write_reg(MPU9250_GYRO_CONFIG, temp);
}
