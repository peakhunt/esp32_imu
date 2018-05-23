#ifndef __IMU_TASK_DEF_H__
#define __IMU_TASK_DEF_H__

#include "common_def.h"

#include "imu.h"
#include "mpu9250.h"

extern void imu_task_init(void);

extern void imu_task_get_raw_and_data(imu_sensor_data_t* raw, imu_data_t* data);
extern uint32_t imu_task_get_loop_cnt(void);

#endif /* !__IMU_TASK_DEF_H__ */
