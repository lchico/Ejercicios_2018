/*
 * uart.c
 *
 *  Created on: Apr 11, 2018
 *      Author: lchico
 */

#include "sapi.h"

#define BUFFER_SIZE_RX 10
#define BUFFER_SIZE_TX 10

uint8_t RXIndex_escritura=0;
uint8_t RXIndex_lectura=0;
uint8_t TXIndex_escritura=0;
uint8_t TXIndex_lectura=0;


char RX_buffer[BUFFER_SIZE_RX];
char TX_buffer[BUFFER_SIZE_TX];

/* Función consumidora */
char Get_Char_From_Buffer (char * ch){
	// Hay nuevo dato en el buffer?
	if (RXIndex_lectura <RXIndex_escritura)	{
		*ch = RX_buffer [ RXIndex_lectura ];
		RXIndex_lectura ++;
		return 1; // Hay nuevo dato
	}else {
		RXIndex_lectura=0;
		RXIndex_escritura=0;
		return 0; // No Hay
	}
}

/* Send and receive data from RX and TX buffer  */
void UpdateRxTx(void) {
	char dato;
	if(TXIndex_lectura < TXIndex_escritura){ // Hay byte en el buffer Tx para	transmitir?
		uartWriteByte( UART_USB, TX_buffer [ TXIndex_lectura ] );
		TXIndex_lectura++;
	}else{ 							// No hay datos disponibles para enviar
		TXIndex_lectura = 0;
		TXIndex_escritura = 0;
	}
	// se ha recibido algún byte?
	if ( uartReadByte( UART_USB, &dato)  != FALSE ){
		// Byte recibido. Escribir byte en buffer de entrada
		if (RXIndex_escritura < BUFFER_SIZE_RX) {
			RX_buffer[RXIndex_escritura] = dato; // Guardar dato en buffer
			RXIndex_escritura++; // Inc sin desbordar buffer
		} else {
			/* Write buffer is full, have to wait free space */
		}
	}
}

/* Funciones productoras */
void Write_String_To_Buffer( char* STR_PTR ){
	char i = 0;
	while ( STR_PTR [ i ] != '\0'){
		Write_Char_To_Buffer ( STR_PTR [ i ] );
		i++;
	}
}

void Write_Char_To_Buffer ( char Data ){
	// Write to the buffer *only* if there is space
	if (TXIndex_escritura < BUFFER_SIZE_TX){
		TX_buffer[TXIndex_escritura ] = Data;
		TXIndex_escritura++;
	}else{
		/* Write buffer is full, have to wait free space */
	}
}
