
#include "Mc32_I2cUtilCCS.h"

#define ICM42670P_ADDR_W 0b11010000 // W
#define ICM42670P_ADDR_R 0b11010001 // R

#define PWR_MGMT0 0x1F
#define TEMP_CONFIG0 0x22


void test(){
    
    uint8_t MSB, LSB;
    int16_t temp;
    float tempf;
    
    i2c_start();
    i2c_write(ICM42670P_ADDR_W);
    i2c_write(0x09); 
    i2c_reStart(); 
    i2c_write(ICM42670P_ADDR_R);
    MSB = i2c_read(1);
    LSB = i2c_read(0);
    i2c_stop();
    
    temp = MSB;
    temp = temp << 8;
    temp = temp | LSB;
    
    tempf = ((float)temp/128.0);
}

void init(){
    
    i2c_start();
    i2c_write(ICM42670P_ADDR_W);
    i2c_write(PWR_MGMT0);
    i2c_write(0b10011111);
    i2c_stop();
    i2c_start();
    i2c_write(ICM42670P_ADDR_W);
    i2c_write(TEMP_CONFIG0);
    i2c_write(0b01000000);
    i2c_stop();
}