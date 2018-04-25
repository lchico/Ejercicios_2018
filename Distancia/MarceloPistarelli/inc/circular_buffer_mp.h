/* Copyright 2017, Marcelo Pistarelli.

 */

#ifndef _CIRCULAR_BUFFER_MP_H_
#define _CIRCULAR_BUFFER_MP_H_

/*==================[inclusions]=============================================*/
#include "sapi.h"

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C"
{
#endif

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

typedef struct
{
	uint8_t * 	buffer;
	uint32_t 	cant_datos;
	uint32_t 	capacidad;
	uint32_t 	indice_escritura;
	uint32_t 	indice_lectura;
} circular_buffer_t;

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

void circ_buffer_init(circular_buffer_t * circularBuffer, uint32_t longitud);
uint32_t readAll_circ_buffer(circular_buffer_t * circularBuffer, uint8_t * datos);
bool_t readByte_circ_buffer(circular_buffer_t * circularBuffer, uint8_t * dato);
uint32_t writeAll_circ_buffer(circular_buffer_t * circularBuffer, uint8_t * datos, uint32_t longitud);
bool_t writeByte_circ_buffer(circular_buffer_t * circularBuffer, uint8_t dato);

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif /* #ifndef _CIRCULAR_BUFFER_MP_H_ */
