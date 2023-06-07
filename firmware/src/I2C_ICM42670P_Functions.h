
/* 
 * File:    I2C_ICM42670P_Functions.h
 * Author:  M.Ricchieri
 *
 * Created on 10. mai 2023
 * 
 * This code uses the Mc32_I2cUtilCCS.h
 */


//----------------------------------------------------------------------------// Includes
#include "app.h"


#ifndef I2C_ICM42670P_FUNCTIONS_H
#define	I2C_ICM42670P_FUNCTIONS_H

//----------------------------------------------------------------------------// Constants
#define ICM42670P_ADDR_W 0b11010000 // W
#define ICM42670P_ADDR_R 0b11010001 // R


#ifdef	__cplusplus
extern "C" {
#endif
    
//----------------------------------------------------------------------------// Fonctions
int ICM42670P_I2C_bus_write(struct inv_imu_serif *serif, uint8_t reg, 
        const uint8_t *buf, uint32_t len);
int ICM42670P_I2C_bus_read(struct inv_imu_serif *serif, uint8_t reg, 
        uint8_t *buf, uint32_t len);

#ifdef	__cplusplus
}
#endif

#endif	/* I2C_ICM42670P_FUNCTIONS_H */

