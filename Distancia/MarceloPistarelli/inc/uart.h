/* Copyright 2017, Marcelo Pistarelli.

 */


#ifndef _UART_H_
#define _UART_H_

/*==================[inclusions]=============================================*/
#include "sapi.h"

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

#define TX_BUFFER_SIZE 1024
#define RX_BUFFER_SIZE 1024

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

void UART_Init(uartMap_t uart);
void UART_Update(void);
void UART_write_String_To_TX_Buffer(char* STR_PTR);
uint32_t UART_read_String_From_RX_Buffer(char* STR_PTR);
uint32_t UART_read_Char_From_RX_Buffer(char* STR_PTR);
uint32_t UART_read_line_lowecase(char * charArray, uint32_t longitudMax);

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif /* #ifndef _UART_H_ */
