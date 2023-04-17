//----------------------------------------------------------------------------// RN4678_driver.c
/*
 * File:   RN4678_driver.c
 * Author: ricch
 * Inspired by the code of S. Giuseppe
 * Created on 12. avril 2023, 16:38
 */

//----------------------------------------------------------------------------// Includes
#include <stdbool.h>
#include <stdint.h>
#include "RN4678_driver.h"
#include "app.h"


//----------------------------------------------------------------------------// Constants
// Commands
#define CMD_MODE_ENTER      "$$$\r"
#define CMD_MODE_EXIT       "---\r"
#define CMD_BLE_DISCOV_EN   "Q,0\r"
#define CMD_BLE_ONLY        "SG,1\r"
#define CMD_REBOOT_DEVICE   "R,1\r"

// Answers
#define CMD_MODE_ANSWER     "CMD> "
#define CMD_POS_ANSWER      "AOK\r\nCMD> "
#define CMD_NEG_ANSWER      "ERR\r\nCMD> "
#define CMD_REBOOT_ANSWER   "%REBOOT%"

// Device name
#define DEVICE_NAME_A       "SN,TubePi"
#define DEVICE_NAME_B       "totDeport" 
#define DEVICE_NAME_C       "e_v2.0.0\r"

#define DEVICE_NAME         "SN,TubePitotDeporte_v1.0.0\r"

//----------------------------------------------------------------------------// Global variables / arrays
char a_answerCMD[20];
bool isANewAnswer;

//----------------------------------------------------------------------------// Functions

//.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:// Initialization function
bool initialize_RN4678(void){
    
    bool initIsDone = 0;
    static uint32_t _100us_counter = 0;
        
    switch(_100us_counter){
        
        case 0:
            // Reset the module
            RESET_BLEOff();
        break;
        case 5000:
            RESET_BLEOn();
        break;
        case 10000:
            initIsDone = sendCMD_RN4678(CMD_MODE_ENTER, CMD_MODE_ANSWER);
            initIsDone *= sendCMD_RN4678(DEVICE_NAME, CMD_POS_ANSWER);
            initIsDone *= sendCMD_RN4678(CMD_BLE_ONLY, CMD_POS_ANSWER);
            initIsDone *= sendCMD_RN4678(CMD_REBOOT_DEVICE, CMD_REBOOT_ANSWER);
        break;
    }
    // 10'000 corresponds to 1 second
    if(_100us_counter < 10000){
        
        _100us_counter++;
    }
    return initIsDone;
}

//.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:// Sending command function (USART))
bool sendCMD_RN4678(char* pArrayToSend, char* pArrayExpected){
    
    uint8_t cursor = 0;
    bool result = 0;
    int i = 0;
    
    // Does until character '\r' is sent
    do{
        // Wait for the Transmit buffer to be empty.
        if(!PLIB_USART_TransmitterBufferIsFull(USART_ID_1)){
            
            // Sends all data of the array
            PLIB_USART_TransmitterByteSend(USART_ID_1, pArrayToSend[i]);
            i++;
        }
    }while(pArrayToSend[i-1] != '\r');
    
    // Clear USART buffer before reading expected data
    while(PLIB_USART_ReceiverDataIsAvailable(USART_ID_1)) 
        PLIB_USART_ReceiverByteReceive(USART_ID_1);
    // Clear the answer array
    clearAnswerArray_RN4678();
    
    // Sets the cursor on case 0 of the array
    cursor = 0;
    do{
        // While data are available in the buffer
        while(PLIB_USART_ReceiverDataIsAvailable(USART_ID_1)){
            // Reads and saves the characters received in an array
            a_answerCMD[cursor] = PLIB_USART_ReceiverByteReceive(USART_ID_1);
            // Increments the cursor value
            cursor++;
        }
        // If data read are the same as expected
        if (strstr(a_answerCMD, pArrayExpected) != NULL) result = true;
        else result = false;
    }while(!result);
    
    return result;
}


//.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.://
void clearAnswerArray_RN4678(){
    
    int cursor;
    for (cursor = 0; cursor < sizeof(a_answerCMD); cursor++) {
        
        a_answerCMD[cursor] = NULL;
    }
}




//.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.://
void sendData_RN4678(char* pArrayToSend){
    
    int cursor = 0;
    
    // Does until character '\r' is sent
    do{
        // Wait for the Transmit buffer to be empty.
        if(!PLIB_USART_TransmitterBufferIsFull(USART_ID_1)){
            
            // Sends all data of the array
            PLIB_USART_TransmitterByteSend(USART_ID_1, pArrayToSend[cursor]);
            cursor++;
        }
    }while(pArrayToSend[cursor-1] != '\r');
}
