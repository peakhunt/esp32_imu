#ifndef __IMU_TASK_DEF_H__
#define __IMU_TASK_DEF_H__

#include "common_def.h"

typedef struct
{
  int16_t   accel_raw[3];
  int16_t   gyro_raw[3];
  int16_t   mag_raw[3];
  int16_t   temp_raw;

  float     accel[3];             // in g
  float     gyro[3];              // in degrees per sec  (not radian)
  float     mag[3];               // in uT
  float     temp;                 // in celcius
} imu_t;

extern void imu_task_init(void);
extern void imu_task_get_raw_values(imu_t* imu);

#endif /* !__IMU_TASK_DEF_H__ */
