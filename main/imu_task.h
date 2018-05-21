#ifndef __IMU_TASK_DEF_H__
#define __IMU_TASK_DEF_H__

#include "common_def.h"
#include "mpu9250.h"

struct imu_sensor_data_t
{
  int16_t   accel_raw[3];
  int16_t   gyro_raw[3];
  int16_t   mag_raw[3];
  int16_t   temp_raw;

  float     accel[3];             // in g
  float     gyro[3];              // in degrees per sec  (not radian)
  float     mag[3];               // in uT
  float     temp;                 // in celcius
};

typedef struct
{
  mpu9250_t                   mpu9250;
  struct imu_sensor_data_t    sensor_data;
} imu_t;

extern void imu_task_init(void);
extern void imu_task_get_raw_values(struct imu_sensor_data_t* data);

#endif /* !__IMU_TASK_DEF_H__ */
