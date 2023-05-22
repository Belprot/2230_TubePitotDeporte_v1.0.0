/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
//DOM-IGNORE-END

#ifndef _APP_H
#define _APP_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "system_config.h"
#include "system_definitions.h"

#include "ICM42670P_driver.h"
#include "imu/inv_imu_driver.h"
#include "imu/inv_imu_transport.h"
#include "Invn/EmbUtils/RingBuffer.h"
#include "I2C_ICM42670P_Functions.h"
#include "inv_imu_personnal_functions.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END 

// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************

/* 
 * Select communication link between SmartMotion and IMU 
 */
#define SERIF_TYPE UI_I2C

/*
 * Set power mode flag
 * Set this flag to run example in low-noise mode.
 * Reset this flag to run example in low-power mode.
 * Note: low-noise mode is not available with sensor data frequencies less than 12.5Hz.
 */
#define USE_LOW_NOISE_MODE 1
    
/*
 * Select Fifo resolution Mode (default is low resolution mode)
 * Low resolution mode: 16 bits data format
 * High resolution mode: 20 bits data format
 * Warning: Enabling High Res mode will force FSR to 16g and 2000dps
 */
#define USE_HIGH_RES_MODE 0

/*
 * Select to use FIFO or to read data from registers
 */
#define USE_FIFO 0

/*
 * Print raw data or scaled data
 * 0 : print raw accel, gyro and temp data
 * 1 : print scaled accel, gyro and temp data in g, dps and degree Celsius
 */
#define SCALED_DATA_G_DPS 1

    
    
#define FAST 1
#define SLOW 0
    
// *****************************************************************************
/* Application states

  Summary:
    Application states enumeration

  Description:
    This enumeration defines the valid application states.  These states
    determine the behavior of the application at various times.
*/
    
    

typedef enum
{
	/* Application's state machine's initial state. */
	APP_STATE_INIT=0,
	APP_STATE_SERVICE_TASKS,
    APP_STATE_WAIT,

	/* TODO: Define states used by the application state machine. */

} APP_STATES;


// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    Application strings and buffers are be defined outside this structure.
 */

typedef struct{
    
    /* The application's current state */
    APP_STATES state;
    uint32_t usCounter32;
    uint64_t usCounter64;
    /* TODO: Define any additional data used by the application. */

} APP_DATA;

typedef struct{
    
    uint16_t velocity;
    float gyroX, gyroY, gyroZ;
    float accelX, accelY, accelZ;
    float batVoltage, genVoltage;
    
} SENS_DATA;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Routines
// *****************************************************************************
// *****************************************************************************
/* These routines are called by drivers when certain events occur.
*/
	
// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************









// Extern variables and structures
extern APP_DATA     appData;
extern SENS_DATA    sensData;
extern bool         isBluetoothConnected;

extern struct inv_imu_device   myImuDevice;
extern struct inv_imu_serif    myImuSertif;

// Basic functions prototypes
void APP_Initialize (void);
void APP_Tasks( void );
void APP_UpdateState(APP_STATES NewState);
void clearArray(size_t arraySize, char *pArrayToClear);

// Callback functions prototypes
void TIMER1_Callback_Function(void);
void TIMER2_Callback_Function(void);
void TIMER5_Callback_Function(void);
void USART1_Callback_Function(void);
void imu_callback(inv_imu_sensor_event_t *event);


// IMU useful functions prototypes
int         initImuInterface(struct inv_imu_serif *icm_serif);
uint64_t    inv_imu_get_time_us(void);




#endif /* _APP_H */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

/*******************************************************************************
 End of File
 */

