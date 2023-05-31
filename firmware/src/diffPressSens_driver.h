/* 
 * File:   HSCMRRN001PD2A3_driver.h
 * Author: ricch
 *
 * Created on 12. avril 2023, 16:38
 */

#ifndef HSCMRRN001PD2A3_DRIVER
#define	HSCMRRN001PD2A3_DRIVER

#include "app.h"
#include <stdbool.h>
#include <stdint.h>


#ifdef	__cplusplus
extern "C" {
#endif

    

// Functions
int16_t readRawDiffPress();
void convertRawToVelocity(int16_t rawDiffPress, SENS_DATA *pSensData);


//int16_t getCompDiffPress();
//void getVelocity(SENS_DATA *pSensData);


#ifdef	__cplusplus
}
#endif

#endif	/* HSCMRRN001PD2A3_DRIVER */




