
/*
 * File:    I2C_ICM42670P_Function.c
 * Author:  M.Ricchieri
 * 
 * Created on 10. mai 2023
 * 
 * This code uses the Mc32_I2cUtilCCS.h
 */


//----------------------------------------------------------------------------// Includes
#include "stdint.h"
#include "Mc32_I2cUtilCCS.h"
#include "imu/inv_imu_driver.h"
#include "imu/inv_imu_transport.h"
#include "I2C_ICM42670P_Functions.h"


//----------------------------------------------------------------------------// Constants
#define ICM42670P_INIT_VALUE 0
#define I2C_BUFFER_LEN 10
#define ICM42670P_I2C_BUS_WRITE_ARRAY_INDEX 1


// \Brief : Those functions are used by the ICM42670 IMU to communicate with
//          the chip in I2C. The first one is used to read and the second to 
//          write in the chip. Those functions are pointed by the ICM42670 
//          driver.

//----------------------------------------------------------------------------// ICM42670P_I2C_bus_write
int ICM42670P_I2C_bus_write(struct inv_imu_serif *serif, uint8_t reg,
        const uint8_t *buf, uint32_t len){
    
    int cursor = 0;
    
    i2c_start();
    i2c_write(ICM42670P_ADDR_W);
    i2c_write(reg);
    
    for (cursor = 0; cursor < len; cursor++){
        
        i2c_write(*(buf+cursor));
    }
    i2c_stop();
    
    return 0;
}


//----------------------------------------------------------------------------// ICM42670P_I2C_bus_read
int ICM42670P_I2C_bus_read(struct inv_imu_serif *serif, uint8_t reg, 
        uint8_t *buf, uint32_t len){
    
    uint8_t array[len];
    int cursor = 0;
    
    i2c_start();
    i2c_write(ICM42670P_ADDR_W);
    i2c_write(reg);
    i2c_start();
    i2c_write(ICM42670P_ADDR_R);
    
    for (cursor = 0; cursor < len; cursor++){
        
        if((cursor+1) < len)
            // With ACK
            array[cursor] = i2c_read(1);
        else
            // Without ACK
            array[cursor] = i2c_read(0);
        
        buf[cursor] = array[cursor];
        //*(buf+cursor) = array[cursor];
    }
    i2c_stop();
    
    return 0;
}