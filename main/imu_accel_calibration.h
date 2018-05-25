#ifndef __IMU_ACCEL_CALIBRATION_DEF_H__
#define __IMU_ACCEL_CALIBRATION_DEF_H__

#include "common_def.h"

extern void imu_accel_calibration_init(void);
extern void imu_accel_calibration_update(int16_t ax, int16_t ay, int16_t az);
extern void imu_accel_calibration_finish(int16_t offsets[3], int16_t gains[3]);

#endif /* !__IMU_ACCEL_CALIBRATION_DEF_H__ */
