#include "mpu6050.h"
#include "i2c.h"
#include <stdlib.h>
struct mpu6050{
  uint8_t devAddr;
  uint8_t buffer[14];
};
#define DEFAULT_ADDRESS 0b1000100

/* default constructor,
 * address = 0b1000100
 */
struct mpu6050 *mpu_create(void){
  return mpu_create_with_address(DEFAULT_ADDRESS);
}
struct mpu6050 *mpu_create_with_address(uint8_t address){
  struct mpu6050 *dev = malloc(sizeof (struct mpu6050));
  dev->devAddr = address;
  return dev;
}

static uint8_t getDeviceId(){
  char address;
  i2cReadByte(MPU6050_ADDRESS_AD0_LOW,MPU6050_RA_WHO_AM_I,&address);
  return address;
}

/*initialize device*/
void mpu_initialize(struct mpu6050  *dev){
  i2cSetupPins();
  setClockSource(MPU6050_CLOCK_PLL_XGYRO);
  setFullScaleGyroRange(MPU_GYRO_FS_250);
  setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
  setSleepEnabled(FALSE);
}

char mpu_test_connection(struct mpu6050 *dev){
  return getDeviceId() == 0x34;
}



