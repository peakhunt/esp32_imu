#include "mpu9250.h"
#include "mpu9250_i2c.h"
#include "imu_task.h"

#include "esp_log.h"
#include "sdkconfig.h"

const static char* TAG = "mpu9250";

static const float _accel_lsbs[] =
{
  1.0f / MPU9250_ACCE_SENS_2,
  1.0f / MPU9250_ACCE_SENS_4,
  1.0f / MPU9250_ACCE_SENS_8,
  1.0f / MPU9250_ACCE_SENS_16,
};

static const float _gyro_lsbs[] = 
{
  1.0f / MPU9250_GYRO_SENS_250,
  1.0f / MPU9250_GYRO_SENS_500,
  1.0f / MPU9250_GYRO_SENS_1000,
  1.0f / MPU9250_GYRO_SENS_2000,
};

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

static inline uint8_t
ak8963_read_reg(uint8_t reg)
{
  uint8_t ret;

  if(mpu9250_i2c_write(AK8963_I2C_ADDR, &reg, 1) == FALSE)
  {
    ESP_LOGE(TAG, "ak8963_read_reg: failed to mpu9250_i2c_write");
  }

  if(mpu9250_i2c_read(AK8963_I2C_ADDR, &ret, 1) == FALSE)
  {
    ESP_LOGE(TAG, "ak8963_read_reg: failed to mpu9250_i2c_read");
  }
  return ret;
}

static inline void
ak8963_write_reg(uint8_t reg, uint8_t data)
{ 
  uint8_t buffer[2];

  buffer[0] = reg;
  buffer[1] = data;

  if(mpu9250_i2c_write(AK8963_I2C_ADDR, buffer, 2) == FALSE)
  {
    ESP_LOGE(TAG, "ak8963_write_reg: failed to mpu9250_i2c_write");
  }
}

static inline void
ak8963_read_data(uint8_t reg, uint8_t* data, uint8_t len)
{ 
  if(mpu9250_i2c_write(AK8963_I2C_ADDR, &reg, 1) == FALSE)
  {
    ESP_LOGE(TAG, "ak8963_read_data: failed to mpu9250_i2c_write");
  }

  if(mpu9250_i2c_read(AK8963_I2C_ADDR, data, len) == FALSE)
  {
    ESP_LOGE(TAG, "ak8963_read_data: failed to mpu9250_i2c_read");
  }
} 

static void
ak8963_init(void)
{
  uint8_t   v;

  v = ak8963_read_reg(AK8963_WIA);
  ESP_LOGI(TAG, "ak8963 chip ID: %x", v);

  // put ak8963 in mode 2 for 100Hz measurement
  // also set 16 bit output
  v = ((1 << 4) |  0x06);
  ak8963_write_reg(AK8963_CNTL1, v);

  v = ak8963_read_reg(AK8963_CNTL1);
  ESP_LOGI(TAG, "ak8963 control reg: %x", v);
}

static void
ak8963_read_all(struct imu_sensor_data_t* imu)
{
  uint8_t   data[7];

  ak8963_read_data(AK8963_HXL, data, 7);

  imu->mag_raw[0] = (int16_t)(data[1] << 8 | data[0]);
  imu->mag_raw[1] = (int16_t)(data[3] << 8 | data[2]);
  imu->mag_raw[2] = (int16_t)(data[5] << 8 | data[4]);

  imu->mag[0] = imu->mag_raw[0] * AK8963_MAG_LSB;
  imu->mag[1] = imu->mag_raw[1] * AK8963_MAG_LSB;
  imu->mag[2] = imu->mag_raw[2] * AK8963_MAG_LSB;
}

////////////////////////////////////////////////////////////////////////////////
//
// public utilities
//
////////////////////////////////////////////////////////////////////////////////
void
mpu9250_init(mpu9250_t* mpu9250,
    MPU9250_Accelerometer_t accel_sensitivity,
    MPU9250_Gyroscope_t gyro_sensitivity)
{
  mpu9250_i2c_init();

  mpu9250->accel_config   = accel_sensitivity;
  mpu9250->gyro_config    = gyro_sensitivity;

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

  mpu9250->accel_lsb = _accel_lsbs[accel_sensitivity];
  mpu9250->gyro_lsb  = _gyro_lsbs[gyro_sensitivity];
  
  // enable bypass mode to access AK8963
  mpu9250_write_reg(55, 0x02);

  ak8963_init();
}

bool
mpu9250_read_all(mpu9250_t* mpu9250, struct imu_sensor_data_t* imu)
{
  uint8_t data[14];

  // read full raw data
  mpu9250_read_data(MPU9250_ACCEL_XOUT_H, data, 14);

  imu->accel_raw[0] = (int16_t)(data[0] << 8 | data[1]);
  imu->accel_raw[1] = (int16_t)(data[2] << 8 | data[3]);
  imu->accel_raw[2] = (int16_t)(data[4] << 8 | data[5]);

  imu->accel[0] = imu->accel_raw[0] * mpu9250->accel_lsb;
  imu->accel[1] = imu->accel_raw[1] * mpu9250->accel_lsb;
  imu->accel[2] = imu->accel_raw[2] * mpu9250->accel_lsb;


  imu->temp_raw     = (data[6] << 8 | data[7]);
  imu->temp = (imu->temp_raw / 340333.87f + 21.0f);

  imu->gyro_raw[0]  = (int16_t)(data[8] << 8 | data[9]);
  imu->gyro_raw[1]  = (int16_t)(data[10] << 8 | data[11]);
  imu->gyro_raw[2]  = (int16_t)(data[12] << 8 | data[13]);

  imu->gyro[0]  = imu->gyro_raw[0] * mpu9250->gyro_lsb;
  imu->gyro[1]  = imu->gyro_raw[1] * mpu9250->gyro_lsb;
  imu->gyro[2]  = imu->gyro_raw[2] * mpu9250->gyro_lsb;

  ak8963_read_all(imu);

  return TRUE;
}
