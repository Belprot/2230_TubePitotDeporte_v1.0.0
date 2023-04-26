/* 
 * File:   HSCMRRN001PD2A3_driver.c
 * Author: ricch
 *
 * Created on 12. avril 2023, 16:38
 */



//----------------------------------------------------------------------------// Includes
#include "HSCMRRN001PD2A3_driver.h"
#include "app.h"
#include "math.h"
#include "Mc32_I2cUtilCCS.h"
#include "peripheral\i2c\plib_i2c.h"
#include "peripheral\osc\plib_osc.h"


//----------------------------------------------------------------------------// Constants
#define HSCMRRN001PD2A3_ADDR 0x51
#define RHO_AIR 1.2


//----------------------------------------------------------------------------// Global variables / arrays


//----------------------------------------------------------------------------// Functions

// Get the raw compensed differential pressure of the HSCMRRN001PD2A3 sensor
float getVelocity_HSCMRRN001PD2A3(){
    
    uint8_t MSB;
    uint8_t LSB;
    int16_t rawDiffPressure;
    float velocity;
    
    i2c_start();
    i2c_write(HSCMRRN001PD2A3_ADDR);
    MSB = i2c_read(1);
    LSB = i2c_read(1);
    i2c_read(1);
    i2c_read(0);
    i2c_stop();
    
    rawDiffPressure = MSB;
    rawDiffPressure = rawDiffPressure << 8;
    rawDiffPressure = rawDiffPressure | LSB;
    rawDiffPressure = rawDiffPressure - 8192;
    if(rawDiffPressure < 0) rawDiffPressure = 0;
    
    
    velocity = sqrtf((2*(float)rawDiffPressure)/(RHO_AIR));
    
    return velocity;
}

















// Get the raw compensed differential pressure of the HSCMRRN001PD2A3 sensor
int16_t getCompDiffPress_HSCMRRN001PD2A3(){
    
    int16_t rawDiffPressure;
    uint8_t MSB;
    uint8_t LSB;
    
    i2c_start();
    i2c_write(HSCMRRN001PD2A3_ADDR);
    MSB = i2c_read(1);
    LSB = i2c_read(1);
    i2c_read(1);
    i2c_read(0);
    i2c_stop();
    
    rawDiffPressure = MSB;
    rawDiffPressure = rawDiffPressure << 8;
    rawDiffPressure = rawDiffPressure | LSB;
    rawDiffPressure = rawDiffPressure - 8192;
    if(rawDiffPressure < 0) rawDiffPressure = 0;
    
    //float velocity;
    //velocity = sqrtf((2*(float)rawDiffPressure)/(RHO_AIR));
    
    return rawDiffPressure;
}



// Convert raw compensed differential pressure to velocity
float convertRawToVelocity(int16_t rawDiffPressure){
    
    float velocity;
    
    velocity = sqrtf((2*(float)rawDiffPressure)/(RHO_AIR));
    
    return velocity;
}

