
/* 
 * File:   adc_driver.c
 * Author: M.Ricchieri
 *
 * Created on 31. mai 2023, 08:55
 * 
 * Inspired by the "Mc32DriverAdc.c" file
 */


//----------------------------------------------------------------------------// Includes
#include "voltageADC_driver.h"


//----------------------------------------------------------------------------// Constants
#define ADC_RESOLUTION  1024
#define ADC_VREF        3.3
    
//---------------------------------------------------//
// Help to select the SCAN_MASK value
//
// AN10   AN9    AN8    AN7    AN6    AN5    AN4    AN3    AN2    AN1    AN0
//  |      |      |      |      |      |      |      |      |      |      |
//  1      1      0      0      0      0      0      0      0      0      0
//
// Value in CONFIGSCAN = 0b0011 0000 0000 = 0x600
//
#define SCAN_MASK  0x0600
//---------------------------------------------------//


//----------------------------------------------------------------------------// initAdc
void initAdc(void){
    
    // Mask configuration
    PLIB_ADC_InputScanMaskAdd(ADC_ID_1, SCAN_MASK);
    // Data return configuration
    PLIB_ADC_ResultFormatSelect(ADC_ID_1, ADC_RESULT_FORMAT_INTEGER_16BIT);
    // Alternate buffer selection
    PLIB_ADC_ResultBufferModeSelect(ADC_ID_1, ADC_BUFFER_MODE_TWO_8WORD_BUFFERS);
    // Multiplex mode selection
    PLIB_ADC_SamplingModeSelect(ADC_ID_1, ADC_SAMPLING_MODE_MUXA);
    PLIB_ADC_ConversionTriggerSourceSelect(ADC_ID_1, 
            ADC_CONVERSION_TRIGGER_INTERNAL_COUNT);
    // Reference selection
    PLIB_ADC_VoltageReferenceSelect(ADC_ID_1, ADC_REFERENCE_VDD_TO_AVSS );
    PLIB_ADC_SampleAcquisitionTimeSet(ADC_ID_1, 0x1F);
    PLIB_ADC_ConversionClockSet(ADC_ID_1, SYS_CLK_FREQ, 32);
    // Configuration of number of readings (depends on the number of inputs)
    PLIB_ADC_SamplesPerInterruptSelect(ADC_ID_1, ADC_2SAMPLES_PER_INTERRUPT);
    PLIB_ADC_MuxAInputScanEnable(ADC_ID_1);
    // Enable the ADC module
    PLIB_ADC_Enable(ADC_ID_1);
}


//----------------------------------------------------------------------------// readRawAdc
void readRawAdc(RAW_ADC *pRawAdc){
    
    ADC_RESULT_BUF_STATUS BufStatus;
    
    // Stop sample/convert
    PLIB_ADC_SampleAutoStartDisable(ADC_ID_1);
    
    // Treatment with alternating buffer
    BufStatus = PLIB_ADC_ResultBufferStatusGet(ADC_ID_1);
    
    if (BufStatus == ADC_FILLING_BUF_0TO7){
        
        pRawAdc->AN9_V_GEN  = PLIB_ADC_ResultGetByIndex(ADC_ID_1, 0);
        pRawAdc->AN10_V_BAT = PLIB_ADC_ResultGetByIndex(ADC_ID_1, 1);
    }else{
        
        pRawAdc->AN9_V_GEN  = PLIB_ADC_ResultGetByIndex(ADC_ID_1, 8);
        pRawAdc->AN10_V_BAT = PLIB_ADC_ResultGetByIndex(ADC_ID_1, 9);
    }
    
    // Auto start sampling
    PLIB_ADC_SampleAutoStartEnable(ADC_ID_1);
}


//----------------------------------------------------------------------------// convertRawToVoltage
void convertRawToVoltage(RAW_ADC *pRawAdc, SENS_DATA *pSensData){
    
    // Converts RAW data of the battery voltage into a real decimal value
    // The number 2 is present because of the hardware bridge divider
    pSensData->batVoltage = (2 *(pRawAdc->AN10_V_BAT * 
            (ADC_VREF / ADC_RESOLUTION)));
    
    // Converts RAW data of the generator voltage into a real decimal value
    // The number 5 is present because of the hardware bridge divider
    pSensData->genVoltage = (5 *(pRawAdc->AN9_V_GEN * 
            (ADC_VREF / ADC_RESOLUTION)));
}
