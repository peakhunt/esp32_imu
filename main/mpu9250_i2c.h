#ifndef __MPU9250_I2C_DEF_H__
#define __MPU9250_I2C_DEF_H__

#include "common_def.h"

#define MPU9250_I2C_NUM                 I2C_NUM_1
#define MPU9250_I2C_MASTER_SDA_IO       19
#define MPU9250_I2C_MASTER_SCL_IO       18
#define MPU9250_I2C_CLK_FREQ            400000

#define MPU9250_I2C_RX_BUFFER_DISABLE   0
#define MPU9250_I2C_TX_BUFFER_DISABLE   0

extern void mpu9250_i2c_init(void);
extern bool mpu9250_i2c_read(uint8_t addr, uint8_t* data, uint32_t size);
extern bool mpu9250_i2c_write(uint8_t addr, uint8_t* data, uint32_t size);

#endif /* !__MPU9250_I2C_DEF_H__ */
