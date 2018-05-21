#ifndef __MADGWICK_DEF_H__
#define __MADGWICK_DEF_H__

typedef struct
{
  float sampleFreq;
  float invSampleFreq;
  float beta;
  float q0, q1, q2, q3;
} Madgwick;

extern void madgwick_init(Madgwick* madgwick, float sample_freq);
extern void madgwick_updateIMU(Madgwick* madgwick,
                               float gx, float gy, float gz,
                               float ax, float ay, float az);
extern void madgwick_update(Madgwick* madgwick,
                            float gx, float gy, float gz,
                            float ax, float ay, float az,
								            float mx, float my, float mz);
extern void madgwick_get_roll_pitch_yaw(Madgwick* madgwick, float data[3], float mg);
extern void madgwick_get_quaternion(Madgwick* madgwick, float data[4]);

#endif //!__MADGWICK_DEF_H__
