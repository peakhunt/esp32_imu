#ifndef __MPU_9250_DEF_H__
#define __MPU_9250_DEF_H__

////////////////////////////////////////////////////////////////////////////////
//
// XXX refer to page 38 of MPU9250 Product specification
// for accel/gyro/mag orientation
//
//
//      chip orientation mark: top left
//
//      accel/gyro
//      X : left positive, counter clock
//      y : forward positive, counter clock
//      Z : up positive, counter clock
//
//      mag
//      x : forward positive
//      y : left positive
//      z : down positive
//
////////////////////////////////////////////////////////////////////////////////

#define MPU9250_I2C_ADDR        0x68      // or 110 100x. 400Khz interface


#endif /* !__MPU_9250_DEF_H__ */
