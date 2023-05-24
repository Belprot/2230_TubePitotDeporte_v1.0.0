/* 
 * File:   RN4678_driver.h
 * Author: ricch
 *
 * Created on 12. avril 2023, 16:38
 */

#ifndef RN4678_DRIVER_H
#define	RN4678_DRIVER_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifdef	__cplusplus
extern "C" {
#endif
    
    
    
    
    
    
bool init_RN4678(void);
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

