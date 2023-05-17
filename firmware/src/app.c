/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It 
    implements the logic of the application's state machine and it may call 
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "app.h"
#include "Mc32_I2cUtilCCS.h"

#include "HSCMRRN001PD2A3_driver.h"

#include "RN4678_driver.h"



#define FAST 1
#define SLOW 0


// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

//----------------------------------------------------------------------------// Global data
APP_DATA appData;
bool bluethoothIsReady = 0;
struct inv_imu_device   myImuDevice;
struct inv_imu_serif    myImuSertif;



// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

//----------------------------------------------------------------------------// TIMER1 callback function
void TIMER1_Callback_Function(){
    
    if(bluethoothIsReady == 0){
        bluethoothIsReady = initialize_RN4678();
    }
    else{
        
    }
}


//----------------------------------------------------------------------------// TIMER2 callback function
void TIMER2_Callback_Function(){
    
    
//    uint8_t who_am_i;
//    inv_imu_get_who_am_i(&myImuDevice, &who_am_i);
            
    //uint16_t velocity;
    //char a_velocity[9];
    //char a_frameToSend[30] = "ABC_ABC_ABC_ABC_123_123_123\r";
    
//    GYRO_CONFIG0_FS_SEL_t  gyro_fsr_bitfield;
//    gyro_fsr_bitfield = (3 << GYRO_CONFIG0_GYRO_UI_FS_SEL_POS);
//    inv_imu_get_gyro_fsr(&myImuDevice, &gyro_fsr_bitfield);
    
    
    
//    if(bluethoothIsReady == 1){
//        
//        // Reads the differential pressures sensor
//        velocity = getVelocity_HSCMRRN001PD2A3();
//        // Converts the velocity in m/s in km/h
//        velocity = (float)velocity * 3.6;
//        // Blocks values under 15km/h
//        if(velocity <= 15) velocity = 0;
//        
//        // Reset of the velocity array
//        int i;
//        for (i = 0; i < sizeof(a_velocity); i++){
//            
//            a_velocity[i] = 0;
//        }
//        
//        // Converts float in a char array (only integer part)
//        sprintf(a_velocity, "S=%03dkm/h\r", velocity);
//        
//        // IMU
//        //test();
//        
//        // Sends data to the Bluetooth module
//        sendData_RN4678(&a_velocity[0]);
//        
    
        get_imu_data();
        SIGN_LEDToggle();
//    }
}

//----------------------------------------------------------------------------// TIMER5 callback function
void TIMER5_Callback_Function(){
    
    // 64 bits counter
    // 2^32 = 4294967296
    appData.usCounter64 += 4294967296;
}

void imu_callback(inv_imu_sensor_event_t *event){
	
    char a_gyro[10];
    char a_accel[50];
    float gyroX, gyroY, gyroZ;
    float accelX, accelY, accelZ;
    
    gyroX = (event->gyro[0])/250; // 250 dps
    gyroY = (event->gyro[1])/250; // 250 dps
    gyroZ = (event->gyro[2])/250; // 250 dps
    
    accelX = (float)(event->accel[0])/8192.0; // 8192 bits
    accelY = (float)(event->accel[1])/8192.0; // 8192 bits
    accelZ = (float)(event->accel[2])/8192.0; // 8192 bits per g
    
    // Converts float in a char array (only integer part)
//    sprintf(a_gyro, "GX=%5d\r", gyroX);
//    sendData_RN4678(&a_gyro[0]);
//    sprintf(a_gyro, "GY=%5d\r", gyroY);
//    sendData_RN4678(&a_gyro[0]);
//    sprintf(a_gyro, "GZ=%5d\r", gyroZ);
//    sendData_RN4678(&a_gyro[0]);
    
    sprintf(a_accel, "AX=%+1.02f AY=%+1.02f AZ=%+1.02f \r", accelX, accelY, accelZ);
    sendData_RN4678(&a_accel[0]);
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

void APP_UpdateState(APP_STATES NewState){
    
    appData.state = NewState;
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;

    RESET_BLEOff();
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

// Initialize serial interface between MCU and IMU 
int initImuInterface(struct inv_imu_serif *icm_serif){
    
    // No need
	icm_serif->context    = 0;
    // Points to the function
	icm_serif->read_reg   = ICM42670P_I2C_bus_read;
    // Points to the function
	icm_serif->write_reg  = ICM42670P_I2C_bus_write;
	icm_serif->max_read   = 255; /* maximum number of bytes allowed per serial read */
	icm_serif->max_write  = 255; /* maximum number of bytes allowed per serial write */
    // Set the communication interface
	icm_serif->serif_type = SERIF_TYPE;
    
	return 1;
}


void APP_Tasks ( void ){
    
    int rc = 0;
    
    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            // Initialization of the I2C communication
            i2c_init(SLOW);
            // Starts TIMERs
            DRV_TMR0_Start();
            DRV_TMR1_Start();
            DRV_TMR2_Start();
            
            
            // Initializes the ICM42670 interface
            rc |= initImuInterface(&myImuSertif);
            // Resets and prepares the chip for the configuration
            rc |= setupImuDevice(&myImuSertif);
            // Configures ICM42670 parameters
            rc |= configureImuDevice();
            
            
            // State machine update
            APP_UpdateState(APP_STATE_SERVICE_TASKS);
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {
        
            break;
        }

        default:
        {
            break;
        }
    }
}



/*******************************************************************************
 End of File
 */


