/*============================================================================
 * Autor:R. Oliva
 * Proyecto: Proy6 - TP5 CESE ProguP 18.4.2018
 * Licencia:
 * Fecha: 18.4.18
 *===========================================================================*/


/*
 * Practice only..
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*==================[inlcusiones]============================================*/

#include "sapi.h"								// <= Biblioteca sAPI
#include "cooperativeOs_scheduler.h"	   //se usa para testear los botones
#include "SerieRxTx.h"

/*==================[macros and definitions]=================================*/

// ------ Private data type declarations ---------------------------


/*==================[declaraciones de funciones internas]====================*/



/*==================[definiciones de datos internos]=======================
 *
 */
uint8_t RX_buffer [RX_BUFFER_LENGTH];
uint8_t Tran_buffer[TRAN_BUFFER_LENGTH];

// Punteros de buffers
static uint8_t RXIndex_lectura = 0;
static uint8_t RXIndex_escritura = 0;
static uint8_t TXindice_lectura = 0;
static uint8_t TXindice_escritura = 0;
static uint8_t Error_code = 0;


/*==================[declaraciones de funciones externas]====================*/

void SerieRxTxBufUpdTask(void){
	  {

	   uint8_t dato;


	   // Hay datos para mandar?
	   if (TXindice_lectura < TXindice_escritura)
	      {
		   //  UART_USB, Sapi
		   //  void uartWriteByte( uartMap_t uart, uint8_t byte );
		   uartWriteByte(UART_USB, Tran_buffer[TXindice_lectura]);
 	       TXindice_lectura++;
	      }
	   else
	      {
	      // No data to send - just reset the buffer index
		   TXindice_lectura = 0;
		   TXindice_escritura = 0;
	      }

	   // Si se recibe algo..(Sapi)
	   // bool_t uartReadByte( uartMap_t uart, uint8_t* receivedByte );

	   if ( uartReadByte(UART_USB, &dato )  !=  FALSE)
	     {       // Byte recibido. Escribir byte en buffer de entrada
	      if (RXIndex_escritura < RX_BUFFER_LENGTH)
	      {
	      RX_buffer [ RXIndex_escritura ] = dato; // Guardar dato en buffer
	      RXIndex_escritura ++; // Inc sin desbordar buffer
	      }
	      else
	      Error_code = ERROR_UART_FULL_BUFF;
	      }
	   }

}

/* Funciones anexas de manejo serial.. */

void SerieRxTxBufInit(void){
	RXIndex_lectura = 0;
	RXIndex_escritura = 0;
	TXindice_lectura = 0;
	TXindice_escritura = 0;
	Error_code = 0;
}
/*------------------------------------------------------------------*-

  SerieRxTx_Write_Char_To_Buffer()

  Guarda un caracter para transmision en el buffer TX

-*------------------------------------------------------------------*/
void SerieRxTx_Write_Char_To_Buffer(uint8_t txdato)
   {
   // Write to the buffer *only* if there is space
   if (TXindice_escritura < TRAN_BUFFER_LENGTH)
      {
      Tran_buffer[TXindice_escritura] = txdato;
      TXindice_escritura++;
      }
   else
      {
      //Buffer lleno
      Error_code = ERROR_USART_WRITE_CHAR;
      }
   }

/*------------------------------------------------------------------*-

  SerieRxTx_Write_String_To_Buffer()

  Copia un string terminado en '0' al buffer..

-*------------------------------------------------------------------*/
void SerieRxTx_Write_String_To_Buffer(char* str)
   {
     while (*str != 0)
      {
      SerieRxTx_Write_Char_To_Buffer((uint8_t)*str);
      str++;
      }
   }

/*------------------------------------------------------------------*-

 SerieRxTx_Get_Char_From_Buffer()

 Toma (si hay) un caracter del Buffer en software..
 si no hay nada, devuelve 0..


-*------------------------------------------------------------------*/
uint8_t SerieRxTx_Get_Char_From_Buffer(uint8_t *ch)
{
   /* Hay algo ? */
   if (RXIndex_lectura < RXIndex_escritura)
   {
     *ch = RX_buffer [ RXIndex_lectura ];
     RXIndex_lectura ++;
     return 1; // Hay nuevo dato
   }
   else {
           RXIndex_lectura=0;
           RXIndex_escritura=0;
     return 0; // No Hay
     }
}


/*==================[definiciones de funciones internas]=====================*/


/*------------------------------------------------------------------*-
  ---- END OF FILE -------------------------------------------------
-*------------------------------------------------------------------*/
