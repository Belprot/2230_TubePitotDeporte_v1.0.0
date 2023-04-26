
#include "Mc32_I2cUtilCCS.h"

#define ICM42670P_ADDR_W 0b11010000 // W
#define ICM42670P_ADDR_R 0b11010001 // R

#define PWR_MGMT0 0x1F
#define TEMP_CONFIG0 0x22



void test(){
    
    uint8_t gyro_data_x0;
    uint8_t gyro_data_x1;
    uint16_t gyro_data_x;
    float f_gyro_data_x;
    char a_gyro_data_x[20];
    
    
    i2c_start();
    i2c_write(ICM42670P_ADDR_W);
    i2c_write(0x0B); 
    i2c_reStart(); 
    i2c_write(ICM42670P_ADDR_R);
    gyro_data_x1 = i2c_read(1);
    gyro_data_x0 = i2c_read(0);
    i2c_stop();
    
    gyro_data_x = gyro_data_x1;
    gyro_data_x = gyro_data_x << 8;
    gyro_data_x = gyro_data_x | gyro_data_x0;
    
    // Reset of the array
    int i;
    for (i = 0; i < sizeof(a_gyro_data_x); i++){

        a_gyro_data_x[i] = 0;
    }
        
    f_gyro_data_x = (float)gyro_data_x * 0.00137331197;
    
    
    // Converts float in a char array (only integer part)
    sprintf(a_gyro_data_x, "X=%f\r", f_gyro_data_x);
    sendData_RN4678(&a_gyro_data_x[0]);
}




void init_ICM42670P(){
    
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