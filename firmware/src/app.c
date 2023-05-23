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





// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

//----------------------------------------------------------------------------// Global data
APP_DATA        appData;
SENS_DATA       sensData;

bool isBluethoothModuleInit     = false;
bool isBluetoothConnected       = false;

struct inv_imu_device   myImuDevice;
struct inv_imu_serif    myImuSertif;



// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

//----------------------------------------------------------------------------// TIMER1 callback function
void TIMER1_Callback_Function(){ //156Hz
    
//    if(bluethoothModuleIsInit == false){
//        bluethoothModuleIsInit = init_RN4678();
//    }
//    else{
//        
//    }
    
    char a_status[100];
    int cursor = 0;
    bool result = 0;

    const char* word =  "<RFCOMM_OPEN>";
    const char* word2 = "<RFCOMM_CLOSE>";


    clearArray(sizeof(a_status), &a_status[0]);
    
    // Wait for the Transmit buffer to be empty.
    if(PLIB_USART_ReceiverDataIsAvailable(USART_ID_1)){
        
        do{
            while(PLIB_USART_ReceiverDataIsAvailable(USART_ID_1)){
                // Reads and saves the characters received in an array
                a_status[cursor] = PLIB_USART_ReceiverByteReceive(USART_ID_1);
                // Increments the cursor value
                cursor++;
            }
        }while(a_status[cursor - 1] != '>');
        
        cursor = 0;
    }
        
//    if(strcmp(&a_status[0], word) == 0) isBluetoothConnected = 1;
//    if(strcmp(&a_status[0], word2) == 0) isBluetoothConnected = 0;
    
    
//    do{
//        // While data are available in the buffer
//        while(PLIB_USART_ReceiverDataIsAvailable(USART_ID_1)){
//            // Reads and saves the characters received in an array
//            a_status[cursor] = PLIB_USART_ReceiverByteReceive(USART_ID_1);
//            
//            if(a_status[cursor] == '>') result = 1;
//            // Increments the cursor value
//            cursor++;
//            
//        }
//        // If data read are the same as expected
//    }while(!result);
    
//    if(isNewData){
//        
//        if(strcmp(&a_status[0], word) == 0) isBluetoothConnected = 1;
//        if(strcmp(&a_status[0], word2) == 0) isBluetoothConnected = 0;
//    }
    //searchWord(a_status, word);
}


//----------------------------------------------------------------------------// TIMER2 callback function
void TIMER2_Callback_Function(){ // 20Hz
    
    char    a_dataToSend[100];
    
    if(isBluetoothConnected == true){
        
        // Reads the differential pressures sensor
        sensData.velocity = getVelocity_HSCMRRN001PD2A3();
        // Converts the velocity in m/s in km/h
        sensData.velocity = (float)sensData.velocity * 3.6;
        // Blocks values under 15km/h to avoid wrong values
        if(sensData.velocity <= 15) sensData.velocity = 0;
        
        
        // Gets new IMU data
        get_imu_data();
        
        // Clears the array before saving new values
        clearArray(sizeof(a_dataToSend), &a_dataToSend[0]);
        // Converts float values in a char array
        sprintf(a_dataToSend, "S=%03dkm/h " 
                              "GX=%+3.02f GY=%+3.02f GZ=%+3.02f "
                              "AX=%+1.02f AY=%+1.02f AZ=%+1.02f\r",
            sensData.velocity,
            sensData.gyroX,     sensData.gyroY,     sensData.gyroZ,
            sensData.accelX,    sensData.accelY,    sensData.accelZ);
    
        // Send data through USART
        sendData_RN4678(&a_dataToSend[0]);
        
        SIGN_LEDToggle();
    }
    else SIGN_LEDOn();
}

//----------------------------------------------------------------------------// TIMER5 callback function
void TIMER5_Callback_Function(void){
    
    // 64 bits counter
    // 2^32 = 4294967296
    appData.usCounter64 += 4294967296;
}


//----------------------------------------------------------------------------// IMU callback function
void imu_callback(inv_imu_sensor_event_t *event){
    
    // Transforms 16bits values into degrees and saves them in the sensor data 
    // structure
    sensData.gyroX = (event->gyro[0])/250; // 250 dps
    sensData.gyroY = (event->gyro[1])/250; // 250 dps
    sensData.gyroZ = (event->gyro[2])/250; // 250 dps
    
    // Reads and transforms 16bits values into 
    // Transforms 16bits values into g acceleration and saves them in the sensor 
    // data structure
    sensData.accelX = (float)(event->accel[0])/8192.0; // 8192 bits per g
    sensData.accelY = (float)(event->accel[1])/8192.0; // 8192 bits per g
    sensData.accelZ = (float)(event->accel[2])/8192.0 + 0.075; // 8192 bits per g + offset
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

//----------------------------------------------------------------------------// APP_Tasks
void APP_Tasks ( void ){
    
    int rc = 0;
    
    /* Check the application's current state. */
    switch(appData.state){
        
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            // Initialization of the I2C communication
            i2c_init(SLOW);
            
            // Initialization of the Bluetooth module
            isBluethoothModuleInit = init_RN4678();
            
            // Starts TIMERs
            DRV_TMR0_Start();
            DRV_TMR1_Start();
            DRV_TMR2_Start();
            
            // Initialization of the ICM42670 interface
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
        
        case APP_STATE_WAIT:
        {
            
            break;
        }
        default:
        {
            break;
        }
    }
}

//----------------------------------------------------------------------------// clearArray
void clearArray(size_t arraySize, char *pArrayToClear){
    
    int i;
    for (i = 0; i < arraySize; i++){

        pArrayToClear[i] = NULL;
    }
}

/*******************************************************************************
 End of File
 */

