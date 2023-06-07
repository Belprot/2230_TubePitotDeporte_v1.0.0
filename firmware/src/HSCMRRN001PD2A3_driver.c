/* 
 * File:   HSCMRRN001PD2A3_driver.c
 * Author: M.Ricchieri
 *
 * Created on 12. avril 2023
 */


//----------------------------------------------------------------------------// Includes
#include "HSCMRRN001PD2A3_driver.h"
#include "math.h"
#include "Mc32_I2cUtilCCS.h"
#include "peripheral\i2c\plib_i2c.h"


//----------------------------------------------------------------------------// Constants
#define HSCMRRN001PD2A3_ADDR 0x51

#define RHO_AIR 1.2


//----------------------------------------------------------------------------// readRawDiffPress
// Read the raw compensed differential pressure from the HSCMRRN001PD2A3 sensor
int16_t readRawDiffPress(){
    
    int16_t rawDiffPress;
    uint8_t MSB;
    uint8_t LSB;
    
    // I2C communication with the sensor
    i2c_start();
    i2c_write(HSCMRRN001PD2A3_ADDR);
    MSB = i2c_read(1);
    LSB = i2c_read(1);
    // Reads 2 unused bytes to avoid bug // needs to be clarified !
    i2c_read(1);
    i2c_read(0); // No ACK
    i2c_stop();
    
    // Data formatting 
    rawDiffPress = MSB;
    rawDiffPress = rawDiffPress << 8;
    rawDiffPress = rawDiffPress | LSB;
    rawDiffPress = rawDiffPress - 8192;
    // Safety to avoid negative speeds
    if(rawDiffPress < 0) rawDiffPress = 0;
    
    return rawDiffPress;
}


//----------------------------------------------------------------------------// convertRawToVelocity
// Convert raw compensed differential pressure to velocity (km/h))
void convertRawToVelocity(int16_t rawDiffPress, SENS_DATA *pSensData){
    
    // Multiplied by 3.6 to obtain velocity in km/h instead of m/s
    pSensData->velocity = 3.6 * (sqrtf((2*(float)rawDiffPress)/(RHO_AIR)));
}