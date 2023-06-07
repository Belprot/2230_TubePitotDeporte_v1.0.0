/* 
 * File:   adc_driver.h
 * Author: M.Ricchieri
 *
 * Created on 31. mai 2023, 08:55
 */

#ifndef ADC_DRIVER_H
#define	ADC_DRIVER_H

#ifdef	__cplusplus
extern "C" {
#endif
    
//----------------------------------------------------------------------------// Includes
#include "app.h"
#include "peripheral/adc/plib_adc.h"

    
//----------------------------------------------------------------------------// Functions
void initAdc(void);
void readRawAdc(RAW_ADC *pRawAdc);
void convertRawToVoltage(RAW_ADC *pRawAdc, SENS_DATA *pSensData);

#ifdef	__cplusplus
}
#endif

#endif	/* ADC_DRIVER_H */

