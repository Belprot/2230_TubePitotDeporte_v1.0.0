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

#ifdef	__cplusplus
extern "C" {
#endif
    
    
    
    
    
    
bool init_RN4678(void);
void clearAnswerArray_RN4678();
bool sendCMD_RN4678(char* pArrayToSend, char* pArrayExpected);
void sendData_RN4678(char* pArrayToSend);
void performAction(const char* word);
bool searchWord(char* message, const char* word);
void readStatus();

    
    
    
    

#ifdef	__cplusplus
}
#endif

#endif	/* RN4678_DRIVER_H */

