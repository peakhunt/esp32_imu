#include <math.h>
#include "imu_gyro_calibration.h"
#include "sensor_calib.h"

/*
   As for gyro, all we can do is zero calibration. That is,
   when the board stands still, we should get zero values for all the axises from gyro.
   But in reality, those values will be slightly bigger/smaller than zero due to various
   errors. So all we gotta do is calculate the offset values for zero position and save them.
   Later after calibration, those offset values will be simply subtracted from real sample
   values to produce more correct value.
   Offset can be calculated simply by off = (sum of all samples) / num_samples
*/

static uint32_t             _sample_count;

static float                _sum[3];
static int16_t              _offsets[3];

void
imu_gyro_calibration_init(void)
{
  _sample_count = 0;
  _sum[0] = 
  _sum[1] = 
  _sum[2] = 0.0f;

  _offsets[0] = 
  _offsets[1] = 
  _offsets[2] =  0;
}

void
imu_gyro_calibration_update(int16_t gx, int16_t gy, int16_t gz)
{
  _sum[0] += gx;
  _sum[1] += gy;
  _sum[2] += gz;

  _sample_count++;
}

void
imu_gyro_calibration_finish(int16_t offsets[3])
{
  _offsets[0] = (int16_t)(_sum[0] / _sample_count);
  _offsets[1] = (int16_t)(_sum[1] / _sample_count);
  _offsets[2] = (int16_t)(_sum[2] / _sample_count);
}
