/* 
 * File:   HSCMRRN001PD2A3_driver.h
 * Author: M.Ricchieri
 *
 * Created on 12. avril 2023
 */

#ifndef HSCMRRN001PD2A3_DRIVER
#define	HSCMRRN001PD2A3_DRIVER


//----------------------------------------------------------------------------// Includes
#include "app.h"

#ifdef	__cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------// Functions
int16_t readRawDiffPress();
void convertRawToVelocity(int16_t rawDiffPress, SENS_DATA *pSensData);

#ifdef	__cplusplus
}
#endif

#endif	/* HSCMRRN001PD2A3_DRIVER */




