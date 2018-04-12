/*
 * uart.h
 *
 *  Created on: Apr 11, 2018
 *      Author: lchico
 */

#ifndef EJERCICIOS4_LUISCHICO_INC_UART_H_
#define EJERCICIOS4_LUISCHICO_INC_UART_H_

#define UPDATE_RXTX_TIME_PERIOD 10


char Get_Char_From_Buffer (char * ch);
void UpdateRxTx(void);

void Write_String_To_Buffer( char* STR_PTR );
void Write_Char_To_Buffer ( char Data );


#endif /* EJERCICIOS4_LUISCHICO_INC_UART_H_ */
