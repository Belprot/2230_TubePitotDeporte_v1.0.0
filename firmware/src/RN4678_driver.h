
/* 
 * File:    RN4678_driver.h
 * Author:  M.Ricchieri
 *
 * Created on 12. avril 2023
 * 
 * This code uses USART with FIFO
 */

#ifndef RN4678_DRIVER_H
#define	RN4678_DRIVER_H

//----------------------------------------------------------------------------// Includes
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifdef	__cplusplus
extern "C" {
#endif
    
    
//----------------------------------------------------------------------------// Functions
bool init_RN4678(void);
bool turnOffDiscoverBT(void);
bool sendCMD_RN4678(char* pArrayToSend, size_t arraySize, char* pArrayExpected, 
        size_t answereSize);

void sendData_RN4678(int8_t* pArrayToSend);
void performAction(const char* word);
bool searchWord(char* message, const char* word);
void readStatus();
void clearInt8Array(size_t arraySize, int8_t* arrayToClear);
void getUsartData();
    

#ifdef	__cplusplus
}
#endif

#endif	/* RN4678_DRIVER_H */

