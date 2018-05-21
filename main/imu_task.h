#ifndef __IMU_TASK_DEF_H__
#define __IMU_TASK_DEF_H__

#include "common_def.h"

typedef struct
{
  int16_t   accel_raw[3];
  int16_t   gyro_raw[3];
  int16_t   mag_raw[3];
  int16_t   temp_raw;
} imu_t;

extern void imu_task_init(void);
extern void imu_task_get_raw_values(int16_t a[3], int16_t g[3], int16_t* temp);

#endif /* !__IMU_TASK_DEF_H__ */
