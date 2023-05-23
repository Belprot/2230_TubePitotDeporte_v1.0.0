/*******************************************************************************
 System Interrupts File

  File Name:
    system_interrupt.c

  Summary:
    Raw ISR definitions.

  Description:
    This file contains a definitions of the raw ISRs required to support the
    interrupt sub-system.

  Summary:
    This file contains source code for the interrupt vector functions in the
    system.

  Description:
    This file contains source code for the interrupt vector functions in the
    system.  It implements the system and part specific vector "stub" functions
    from which the individual "Tasks" functions are called for any modules
    executing interrupt-driven in the MPLAB Harmony system.

  Remarks:
    This file requires access to the systemObjects global data structure that
    contains the object handles to all MPLAB Harmony module objects executing
    interrupt-driven in the system.  These handles are passed into the individual
    module "Tasks" functions to identify the instance of the module to maintain.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2011-2014 released Microchip Technology Inc.  All rights reserved.

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

#include "system/common/sys_common.h"
#include "system_definitions.h"
#include "app.h"
#include "RN4678_driver.h"


// *****************************************************************************
// *****************************************************************************
// Section: System Interrupt Vector Functions
// *****************************************************************************
// *****************************************************************************
void __ISR(_UART_1_VECTOR, ipl5AUTO) _IntHandlerDrvUsartInstance0(void){
    
    USART_ERROR usartStatus;
    bool  TxBuffFull;
    int8_t c;
    int8_t TXsize;
            
    // Is this an RX interrupt ?
    if(PLIB_INT_SourceFlagGet(INT_ID_0, INT_SOURCE_USART_1_RECEIVE) &&
                PLIB_INT_SourceIsEnabled(INT_ID_0, INT_SOURCE_USART_1_RECEIVE)){

        // Oui Test si erreur parité ou overrun
        usartStatus = PLIB_USART_ErrorsGet(USART_ID_1);

        if ((usartStatus & (USART_ERROR_PARITY | USART_ERROR_FRAMING | 
                USART_ERROR_RECEIVER_OVERRUN)) == 0){

            // Traitement RX à faire ICI
            // Lecture des caractères depuis le buffer HW -> fifo SW
			//  (pour savoir s'il y a une data dans le buffer HW RX : PLIB_USART_ReceiverDataIsAvailable())
			//  (Lecture via fonction PLIB_USART_ReceiverByteReceive())

            // Transfert dans le FIFO de tous les chars reçus
            // 1 Si ONE_CHAR, 4 si HALF_FULL et 6 3B4FULL
            while(PLIB_USART_ReceiverDataIsAvailable(USART_ID_1)){
                
                c = PLIB_USART_ReceiverByteReceive(USART_ID_1);
                putCharInFifo(&usartFifoRx, c);
            }
            // buffer is empty, clear interrupt flag
            PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_RECEIVE);
       
            // buffer is empty, clear interrupt flag
            PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_RECEIVE);
        }else{
            // Suppression des erreurs
            // La lecture des erreurs les efface sauf pour overrun
            if((usartStatus & USART_ERROR_RECEIVER_OVERRUN) == 
                    USART_ERROR_RECEIVER_OVERRUN){
                
                PLIB_USART_ReceiverOverrunErrorClear(USART_ID_1);
            }
        }

        
        // Traitement controle de flux reception à faire ICI
        // Gerer sortie RS232_RTS en fonction de place dispo dans fifo reception
        // ... 
    }
    
    
    // Is this an TX interrupt ?
    if (PLIB_INT_SourceFlagGet(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT) &&
                 PLIB_INT_SourceIsEnabled(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT) ) {
        // Traitement TX à faire ICI
        
        TXsize = getReadSize(&usartFifoTx);
       // i_cts = input(RS232_CTS);
       // On vérifie 3 conditions :
       // Si CTS = 0 (autorisation d'émettre)
       // Si il y a un caractèreà émettre
       // Si le txreg est bien disponible
        
       // Il est possible de déposer un caractère
       // tant que le tampon n'est pas plein
       TxBuffFull = PLIB_USART_TransmitterBufferIsFull(USART_ID_1);
       
        if (/*(i_cts == 0) && */(TXsize > 0) && (TxBuffFull == false)){
            do {
                getCharFromFifo(&usartFifoTx, &c);
                PLIB_USART_TransmitterByteSend(USART_ID_1, c);
                /*i_cts = RS232_CTS;*/
                TXsize = getReadSize (&usartFifoTx);
                TxBuffFull = PLIB_USART_TransmitterBufferIsFull(USART_ID_1);
            } while (/*(i_cts == 0) && */( TXsize > 0 ) &&TxBuffFull == false);
        }
		
        // disable TX interrupt (pour éviter une interrupt. inutile si plus rien à transmettre)
        PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
        
        // Clear the TX interrupt Flag (Seulement apres TX) 
        PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
    }
}



 



void __ISR(_TIMER_1_VECTOR, ipl1AUTO) IntHandlerDrvTmrInstance0(void){
    
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_1);
    TIMER1_Callback_Function();
}
void __ISR(_TIMER_2_VECTOR, ipl2AUTO) IntHandlerDrvTmrInstance1(void)
{
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_2);
    TIMER2_Callback_Function();
}
void __ISR(_TIMER_5_VECTOR, ipl1AUTO) IntHandlerDrvTmrInstance2(void)
{
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_5);
    TIMER5_Callback_Function();
}
void __ISR(_TIMER_3_VECTOR, ipl1AUTO) IntHandlerDrvTmrInstance3(void)
{
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_3);
    appData.usCounter32++;
}
 /*******************************************************************************
 End of File
*/
