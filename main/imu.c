#include <stdio.h>
#include <string.h>
#include "imu.h"
#include "mag_calibration.h"
#include "gyro_calibration.h"
#include "accel_calibration.h"

/*
   board orientation (not flipped)

          | O|=================| /|\
          | G|       U(+z)     |  | +x
          | T|       top       |  |
          | T|=================|  |
                                  |
          <------------- y+  

              gyro

                clock wise rotate around Z : + (need to be inverted)
                clock wise rotate around Y : + (ok)
                counter clock wise rotate around X : + (ok)

              accel 
                against positive axis: +G
                

  TTGO accel/gyro align 

                    /\
                    | +y
                    |
            T|=================|
            T|       U(+z)     |
            G|      bottom     |
            O|=================|
               -------------> +x

               flip and rotate

               -------------> +x
            O|=================|
            G|       D(+z)     |
            T|      top        |
            T|=================|
                    |
                    | +y
                   \|/

          x = -y
          y = -x
          z = -z


  TTGO mag align
                    /\
                    | +x
                    |
            T|=================|
            T|       D(+z)     |
            G|      bottom     |
            O|=================|
               -------------> +y

               flip and rotate

               -------------> +y
            O|=================|
            G|       U(+z)     |
            T|      top        |
            T|=================|
                    |
                    | +x
                   \|/

         x = -x
         y = -y
         z =  z

  Finally NEU setup. Madgwick expects NEU!
              
              North
                /\
                |   +x
                |
                |
             ----------
                TTGO
             ----------
                top
                                         +y
                         -----------------> East

                U(Z+)
             ----------


      so
            x = y
            y = x
            z = z

*/
////////////////////////////////////////////////////////////////////////////////
//
// utilities
//
////////////////////////////////////////////////////////////////////////////////
static inline void
alignReading(int16_t* values, imu_board_align_t align)
{
  const int16_t   x = values[0],
                  y = values[1],
                  z = values[2];

  switch(align)
  {
  case imu_board_align_cw_0:
    values[0] = x;
    values[1] = y;
    values[2] = z;
    break;

  case imu_board_align_cw_90:
    values[0] = y;
    values[1] = -x;
    values[2] = z;
    break;

  case imu_board_align_cw_180:
    values[0] = -x;
    values[1] = -y;
    values[2] = z;
    break;

  case imu_board_align_cw_270:
    values[0] = -y;
    values[1] = x;
    values[2] = z;
    break;

  case imu_board_align_cw_0_flip:
    values[0] = -x;
    values[1] = y;
    values[2] = -z;
    break;

  case imu_board_align_cw_90_flip:
    values[0] = y;
    values[1] = x;
    values[2] = -z;
    break;

  case imu_board_align_cw_180_flip:
    values[0] = x;
    values[1] = -y;
    values[2] = -z;
    break;

  case imu_board_align_cw_270_flip:
    values[0] = -y;
    values[1] = -x;
    values[2] = -z;
    break;
  }
}

static void
imu_apply_calibration(imu_t* imu)
{
  imu->adjusted.accel[0] = (imu->raw.accel[0] - imu->cal.accel_off[0]) * imu->cal.accel_scale[0] / 4096;
  imu->adjusted.accel[1] = (imu->raw.accel[1] - imu->cal.accel_off[1]) * imu->cal.accel_scale[0] / 4096;
  imu->adjusted.accel[2] = (imu->raw.accel[2] - imu->cal.accel_off[2]) * imu->cal.accel_scale[0] / 4096;

  imu->adjusted.gyro[0] = (imu->raw.gyro[0] - imu->cal.gyro_off[0]);
  imu->adjusted.gyro[1] = (imu->raw.gyro[1] - imu->cal.gyro_off[1]);
  imu->adjusted.gyro[2] = (imu->raw.gyro[2] - imu->cal.gyro_off[2]);

  imu->adjusted.mag[0] = (imu->raw.mag[0] - imu->cal.mag_bias[0]);
  imu->adjusted.mag[1] = (imu->raw.mag[1] - imu->cal.mag_bias[1]);
  imu->adjusted.mag[2] = (imu->raw.mag[2] - imu->cal.mag_bias[2]);

  imu->adjusted.temp = imu->raw.temp;
}

static void
imu_apply_board_orientation(imu_t* imu)
{
  alignReading(imu->adjusted.accel, imu->accel_align);
  alignReading(imu->adjusted.gyro, imu->gyro_align);
  alignReading(imu->adjusted.mag, imu->mag_align);
}

static void
imu_calc_sensor_value(imu_t* imu)
{
  imu->data.accel[0] = imu->adjusted.accel[0] * imu->lsb.accel_lsb;
  imu->data.accel[1] = imu->adjusted.accel[1] * imu->lsb.accel_lsb;
  imu->data.accel[2] = imu->adjusted.accel[2] * imu->lsb.accel_lsb;

  imu->data.gyro[0] = imu->adjusted.gyro[0] * imu->lsb.gyro_lsb;
  imu->data.gyro[1] = imu->adjusted.gyro[1] * imu->lsb.gyro_lsb;
  imu->data.gyro[2] = imu->adjusted.gyro[2] * imu->lsb.gyro_lsb;

  imu->data.mag[0] = imu->adjusted.mag[0] * imu->lsb.mag_lsb;
  imu->data.mag[1] = imu->adjusted.mag[1] * imu->lsb.mag_lsb;
  imu->data.mag[2] = imu->adjusted.mag[2] * imu->lsb.mag_lsb;

  imu->data.temp = ((float) imu->adjusted.temp) / 333.87f + 21.0f;
}

static void
imu_apply_neu_align(imu_t* imu)
{
  //
  //   
  // my board sensor setup
  //
  //     y
  //    |
  //    |
  //    |------- x
  //   Z(U)

  //
  // madgwick/mahony filter expets NEU
  //
  //    
  //     x
  //    |
  //    |
  //    |------- y
  //   Z(U)

  float temp;

  temp = imu->data.gyro[0];
  imu->data.gyro[0] = imu->data.gyro[1];
  imu->data.gyro[1] = temp;

  temp = imu->data.accel[0];
  imu->data.accel[0] = imu->data.accel[1];
  imu->data.accel[1] = temp;

  temp = imu->data.mag[0];
  imu->data.mag[0] = imu->data.mag[1];
  imu->data.mag[1] = temp;
}

static void
imu_update_normal(imu_t* imu)
{
  imu_apply_calibration(imu);

  imu_apply_board_orientation(imu);

  imu_calc_sensor_value(imu);

  imu_apply_neu_align(imu);

  //
  // remember
  // a) accel/mag is uniless. you can push in values in any unit
  // b) gyro must be in degree per second
  // c) madgwick expects data in NED coordinate.
  //    If your roll/pitch/raw values are strange, suspect this.
  //
#if USE_MADGWICK == 1
  //
  // NED or NEU or whatever
  //
  // I want TTGO mark to be north
  //
  // So
  // north(x) = x
  // east(y)  = y
  // D(or U)  = z
  // read comments in the beginning if you are not sure.
  // this is still confusing to me
  //
  madgwick_update(&imu->filter,
      imu->data.gyro[0],  imu->data.gyro[1],   imu->data.gyro[2],
      imu->data.accel[0], imu->data.accel[1],  imu->data.accel[2],
      imu->data.mag[0],   imu->data.mag[1],    imu->data.mag[2]);

  madgwick_get_roll_pitch_yaw(&imu->filter,
      imu->data.orientation,
      imu->cal.mag_declination);
#else
  mahony_update(&imu->filter,
      imu->data.gyro[0],  imu->data.gyro[1],   imu->data.gyro[2],
      imu->data.accel[0], imu->data.accel[1],  imu->data.accel[2],
      imu->data.mag[0],   imu->data.mag[1],    imu->data.mag[2]);

  mahony_get_roll_pitch_yaw(&imu->filter,
      imu->data.orientation,
      imu->cal.mag_declination);
#endif
}

////////////////////////////////////////////////////////////////////////////////
//
// public interfaces
//
////////////////////////////////////////////////////////////////////////////////
void
imu_init(imu_t* imu)
{
  memset(imu, 0, sizeof(imu_t));

  imu->mode = imu_mode_normal;

  imu->cal.accel_scale[0] = 
  imu->cal.accel_scale[1] = 
  imu->cal.accel_scale[2] = 4096;

  // FIXME 
  // update_rate
  // sensor align
  //
  imu->accel_align  = imu_board_align_cw_270_flip;
  imu->gyro_align   = imu_board_align_cw_270_flip;
  imu->mag_align    = imu_board_align_cw_180;

  imu->update_rate  = 200;

#if USE_MADGWICK == 1
  madgwick_init(&imu->filter, imu->update_rate);
#else
  mahony_init(&imu->filter, imu->update_rate);
#endif
}

void
imu_update(imu_t* imu)
{
  // raw sensor data is already read in .raw

  switch(imu->mode)
  {
  case imu_mode_normal:
    imu_update_normal(imu);
    break;

  case imu_mode_accel_calibrating:
    accel_calibration_update(imu->raw.accel[0], imu->raw.accel[1], imu->raw.accel[2]);
    break;

  case imu_mode_gyro_calibrating:
    gyro_calibration_update(imu->raw.gyro[0], imu->raw.gyro[1], imu->raw.gyro[2]);
    break;

  case imu_mode_mag_calibrating:
    mag_calibration_update(imu->raw.mag[0], imu->raw.mag[1], imu->raw.mag[2]);
    break;
  }
}

void
imu_mag_calibration_start(imu_t* imu)
{
  imu->mode = imu_mode_mag_calibrating;
  mag_calibration_init();
}

void
imu_mag_calibration_finish(imu_t* imu)
{
  imu->mode = imu_mode_normal;
  mag_calibration_finish(imu->cal.mag_bias);
}

void
imu_gyro_calibration_start(imu_t* imu)
{
  imu->mode = imu_mode_gyro_calibrating;
  gyro_calibration_init();
}

void
imu_gyro_calibration_finish(imu_t* imu)
{
  imu->mode = imu_mode_normal;
  gyro_calibration_finish(imu->cal.gyro_off);
}

void
imu_accel_calibration_init(imu_t* imu)
{
  accel_calibration_init();
}

void
imu_accel_calibration_step_start(imu_t* imu)
{
  imu->mode = imu_mode_accel_calibrating;
}

void
imu_accel_calibration_step_stop(imu_t* imu)
{
  imu->mode = imu_mode_normal;
}

void
imu_accel_calibration_finish(imu_t* imu)
{
  imu->mode = imu_mode_normal;
  accel_calibration_finish(imu->cal.accel_off, imu->cal.accel_scale);
}
