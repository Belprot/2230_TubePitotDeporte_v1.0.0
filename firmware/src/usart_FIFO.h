/* 
 * File:   usart_FIFO.h
 * Author: mevricchier
 *
 * Created on 23. mai 2023, 17:38
 */

#ifndef USART_FIFO_H
#define	USART_FIFO_H

#ifdef	__cplusplus
extern "C" {
#endif


#include "app.h"

    
    
    
    
#define FIFO_RX_SIZE 40
#define FIFO_TX_SIZE 40

    
//----------------------------------------------------------------------------// Structure
typedef struct {
    
    int32_t fifoSize;   // Size of the FIFO
    int8_t *pHead;       // Head of the FIFO
    int8_t *pTail;       // Tail of the FIFO
    int8_t *pWrite;     // Writing pointer
    int8_t *pRead;      // Reading pointer
    
} USART_FIFO;


//----------------------------------------------------------------------------// Function
void initFifo(USART_FIFO *pFifo, int32_t fifoSize, int8_t *pHead, 
        int8_t initVal);

int32_t getWriteSpace(USART_FIFO *pFifo);
int32_t getReadSize(USART_FIFO *pFifo);

uint8_t putCharInFifo(USART_FIFO *pFifo, int8_t charToPut);
uint8_t getCharFromFifo(USART_FIFO *pFifo, int8_t *charRead);

void putStringInFifo(USART_FIFO *pFifo, size_t arraySize, char* pArrayToSave);
int32_t getStringFromFifo(USART_FIFO *pFifo, int8_t* pArrayToModify);

extern USART_FIFO usartFifoRx;
extern USART_FIFO usartFifoTx;
    
extern int8_t a_fifoRx[FIFO_RX_SIZE];
extern int8_t a_fifoTx[FIFO_TX_SIZE];


#ifdef	__cplusplus
}
#endif

#endif	/* USART_FIFO_H */

