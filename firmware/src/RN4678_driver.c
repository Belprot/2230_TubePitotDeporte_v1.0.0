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
#define CMD_BT_CLASSIC_ONLY "SG,2\r"
#define CMD_PREFIX_SUFIX    "SO,<,>\r"
#define CMD_REBOOT_DEVICE   "R,1\r"

// Answers
#define CMD_MODE_ANSWER     "CMD> "
#define CMD_POS_ANSWER      "AOK\r\nCMD> "
#define CMD_NEG_ANSWER      "ERR\r\nCMD> "
#define CMD_REBOOT_ANSWER   "<REBOOT>"

// Device name
#define DEVICE_NAME         "SN,TubePitotDeporte_v1.0.0\r"

//----------------------------------------------------------------------------// Global variables / arrays
char a_answerCMD[20];
bool isANewAnswer;

//----------------------------------------------------------------------------// Functions

//.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:// Initialization function
bool init_RN4678(void){
    
    int8_t c;
    int i = 0;
    uint16_t readSize = 0;
    char* pArrayExpected = CMD_MODE_ANSWER;
    bool initIsDone = 0;
    
    
    //Reset the module
    RESET_BLEOff();

    inv_imu_sleep_us(200000);
    RESET_BLEOn();

    inv_imu_sleep_us(200000);
    
    putCharInFifo(&usartFifoTx, '$');
    putCharInFifo(&usartFifoTx, '$');
    putCharInFifo(&usartFifoTx, '$');
    putCharInFifo(&usartFifoTx, '\r');
    // autorise l'emission déclenche INTERRUPT
    PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
    
    
    
    
    readSize = getReadSize(&usartFifoRx);
    getCharFromFifo(&usartFifoRx, &c);
    
    
    for(i=0; i < (readSize); i++){

        getCharFromFifo(&usartFifoRx, &a_fifoRx[i]);
    }
    
    if(strstr((char*)a_fifoRx, (char*)CMD_MODE_ANSWER) != NULL){
     
        initIsDone = 1;
    }
    else initIsDone = 0;
    
     // 0 <<<<------
    
//        // Reset the module
//        RESET_BLEOff();
//
//        inv_imu_sleep_us(200000);
//        RESET_BLEOn();
//
//        inv_imu_sleep_us(200000);
//        
//        // Initilialization process
//        initIsDone =  sendCMD_RN4678(CMD_MODE_ENTER, CMD_MODE_ANSWER);
//        initIsDone *= sendCMD_RN4678(DEVICE_NAME, CMD_POS_ANSWER);
//        initIsDone *= sendCMD_RN4678(CMD_BT_CLASSIC_ONLY, CMD_POS_ANSWER);
//        initIsDone *= sendCMD_RN4678(CMD_PREFIX_SUFIX, CMD_POS_ANSWER);
//        initIsDone *= sendCMD_RN4678(CMD_REBOOT_DEVICE, CMD_REBOOT_ANSWER);
//        
//        inv_imu_sleep_us(200000);
    
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

//.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.://
void readStatus(char *pArrayStatus){
    
    int cursor = 0;
    
    while(PLIB_USART_ReceiverDataIsAvailable(USART_ID_1)){
        // Reads and saves the characters received in an array
        pArrayStatus[cursor] = PLIB_USART_ReceiverByteReceive(USART_ID_1);
        // Increments the cursor value
        cursor++;
    }
}

//.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.://
void performAction(const char* word){
    if (strcmp(word, "CONNECT") == 0){
        
        isBluetoothConnected = true;
        
    } else if (strcmp(word, "DISCONN") == 0){
        
        isBluetoothConnected = false;
        
    } else {
        // Ajoutez ici le code pour les autres actions ou traitements
    }
}


//.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.:.://
bool searchWord(char* message, const char* word){
    
    const char* pch = strstr(message, word);
    if (pch != NULL) {
        performAction(word);
        return true;
    }
    return false;
}