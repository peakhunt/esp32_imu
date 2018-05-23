#ifndef __IMU_MAG_CALIBRATION_DEF_H__
#define __IMU_MAG_CALIBRATION_DEF_H__

#include "common_def.h"

extern void imu_mag_calibration_init(void);
extern void imu_mag_calibration_update(int16_t mx, int16_t my, int16_t mz);
extern void imu_mag_calibration_finish(int16_t offsets[3]);

#endif /* !__IMU_MAG_CALIBRATION_DEF_H__ */
