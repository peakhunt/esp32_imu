#ifndef __IMU_GYRO_CALIBRATION_DEF_H__
#define __IMU_GYRO_CALIBRATION_DEF_H__

#include "common_def.h"

extern void imu_gyro_calibration_init(void);
extern void imu_gyro_calibration_update(int16_t gx, int16_t gy, int16_t gz);
extern void imu_gyro_calibration_finish(int16_t offsets[3]);

#endif /* !__IMU_GYRO_CALIBRATION_DEF_H__ */
