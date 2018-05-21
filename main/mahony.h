#ifndef __MAHONY_DEF_H__
#define __MAHONY_DEF_H__

typedef struct
{
  float twoKp;                                        // 2 * proportional gain (Kp)
  float twoKi;                                        // 2 * integral gain (Ki)
  float q0, q1, q2, q3;                               // quaternion of sensor frame relative to auxiliary frame
  float integralFBx, integralFBy, integralFBz;        // integral error terms scaled by Ki
  float invSampleFreq;
} Mahony;

extern void mahony_init(Mahony* mahony, float sampleFrequency);
extern void mahony_updateIMU(Mahony* mahony, float gx, float gy, float gz,
                             float ax, float ay, float az);
extern void mahony_update(Mahony* mahony,
							float gx, float gy, float gz,
							float ax, float ay, float az,
							float mx, float my, float mz);
extern void mahony_get_roll_pitch_yaw(Mahony* mahony, float data[3], float md);
extern void mahony_get_quaternion(Mahony* mahony, float data[4]);

#endif //!__MAHONY_DEF_H__
