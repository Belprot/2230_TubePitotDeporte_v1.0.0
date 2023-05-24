
// Inspired by the GesFifoTh32.c from C. Huber


#include "usart_FIFO.h"


USART_FIFO usartFifoRx;
USART_FIFO usartFifoTx;

int8_t a_fifoRx[FIFO_RX_SIZE];
int8_t a_fifoTx[FIFO_TX_SIZE];

//----------------------------------------------------------------------------// initFifo
// Initialization with fill value

void initFifo(USART_FIFO *pFifo, int32_t fifoSize, int8_t *pHead, 
        int8_t initVal){
    
   int32_t i;
   int8_t *pFif;
   
   pFifo->fifoSize  = fifoSize;
   pFifo->pHead     = pHead; 
   pFifo->pTail     = pHead + (fifoSize - 1);
   pFifo->pWrite    = pHead;
   pFifo->pRead     = pHead;
   
   pFif = pHead;
   
    for (i=0; i < fifoSize; i++){
       
        *pFif = initVal;
        pFif++;
    }
}



//----------------------------------------------------------------------------// GetWriteSpace
// Return the available writing space

int32_t getWriteSpace(USART_FIFO *pFifo){
   
    int32_t writeSize;

    //  Determines the number of characters that can be deposited
    writeSize = pFifo->pRead - pFifo->pWrite - 1;
    
    if (writeSize < 0) {
        
        writeSize = writeSize + pFifo->fifoSize;
    }
    return writeSize;
}



//----------------------------------------------------------------------------// GetReadSize
// Returns the number of characters to read

int32_t getReadSize(USART_FIFO *pFifo){
    
   int32_t readSize;

    readSize = pFifo->pWrite - pFifo->pRead;
    if (readSize < 0){
        
        readSize = readSize +  pFifo->fifoSize;
    }

   return readSize;
}



//----------------------------------------------------------------------------// PutCharInFifo
// Put a character in the FIFO and return 0 if if all is good or 1 if the FIFO
// is full

uint8_t putCharInFifo(USART_FIFO *pFifo, int8_t charToPut){
    
    uint8_t writeStatus;

    // test si fifo est FULL
    if (getWriteSpace(pFifo) == 0) {
        writeStatus = 1; // fifo FULL
    }
    else {
        // Writes the char in FIFO
        *(pFifo->pWrite) = charToPut;

        // Increments the writing pointer
        pFifo->pWrite++;
        
        // Rebound management 
        if (pFifo->pWrite > pFifo->pTail){

            pFifo->pWrite = pFifo->pHead;
        }

        writeStatus = 0; // GOOD
    }
    return writeStatus;
}



//----------------------------------------------------------------------------// getCharFromFifo
// Character read is returned as reference and it returns 0 if GOOD or 1 if
// the FIFO is empty

uint8_t getCharFromFifo(USART_FIFO *pFifo, int8_t *charRead){
    
    int32_t readSize;
    uint8_t readStatus;

    // Determines the number of characters that can be read
    readSize = getReadSize(pFifo);

    // Checks if the FIFO is empty
    if(readSize == 0){
       
        readStatus = 1; // fifo EMPTY
        *charRead = 0;  // charRead = NULL
    }
    else {
        // Reads the char in FIFO
        *charRead = *(pFifo->pRead);

        // Increments the reading pointer
        pFifo->pRead++;
        
        // Rebound management 
        if (pFifo->pRead > pFifo->pTail){
          
            pFifo->pRead = pFifo->pHead;
        }
        readStatus = 0; // GOOD
    }
    return readStatus;
} 



//----------------------------------------------------------------------------// putStringInFifo
// Puts a string in the FIFO buffer

void putStringInFifo(USART_FIFO *pFifo, size_t arraySize, char* pArrayToSave){
    
    int i;
    
    for (i = 0; i < arraySize; i++) {
        
        putCharInFifo(pFifo, pArrayToSave[i]);
    }
}



//----------------------------------------------------------------------------//
//

int32_t getStringFromFifo(USART_FIFO *pFifo, int8_t* pArrayToModify){
    
    int32_t arraySize;
    int i = 0;
    
    arraySize = getReadSize(pFifo);
    
    getCharFromFifo(pFifo, &pArrayToModify[i]);
    
    for(i = 1; i < (arraySize); i++){
        
        getCharFromFifo(pFifo, &pArrayToModify[i]);
    }
    
    return arraySize;
}