/*==================[inlcusiones]============================================*/

#include "circular_buffer_mp.h"

/*==================[macros and definitions]=================================*/

/*==================[definiciones de datos internos]=========================*/

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/

void circ_buffer_init(circular_buffer_t * circularBuffer, uint32_t longitud) // inicialización y reserva de espacio en memoria de buffer
{
	circularBuffer->buffer = malloc(longitud);
	circularBuffer->cant_datos = 0;
	circularBuffer->indice_escritura = 0;
	circularBuffer->indice_lectura = 0;
	circularBuffer->capacidad = longitud;
}

uint32_t readAll_circ_buffer(circular_buffer_t * circularBuffer, uint8_t * datos)
{
	uint32_t bytes = 0; // cantidad de bytes leidos correctamente

	while (circularBuffer->cant_datos > 0)
	{
		datos[bytes] = circularBuffer->buffer[circularBuffer->indice_lectura];
		circularBuffer->cant_datos--;
		bytes++;
		circularBuffer->indice_lectura = (circularBuffer->indice_lectura + 1) % circularBuffer->capacidad;
	}

	return bytes;

}

bool_t readByte_circ_buffer(circular_buffer_t * circularBuffer, uint8_t * dato)
{
	bool_t readOK = false; // byte leido correctamente

	if (circularBuffer->cant_datos > 0)
	{
		*dato = circularBuffer->buffer[circularBuffer->indice_lectura];
		circularBuffer->cant_datos--;
		readOK = true;
		circularBuffer->indice_lectura = (circularBuffer->indice_lectura + 1) % circularBuffer->capacidad;
	}

	return readOK;

}

uint32_t writeAll_circ_buffer(circular_buffer_t * circularBuffer, uint8_t * datos, uint32_t longitud)
{
	uint32_t bytes = 0; // bytes escritos correctamente

	while (circularBuffer->cant_datos < circularBuffer->capacidad)
	{
		circularBuffer->buffer[circularBuffer->indice_escritura] = datos[bytes];
		circularBuffer->cant_datos++;
		bytes++;
		circularBuffer->indice_escritura = (circularBuffer->indice_escritura + 1) % circularBuffer->capacidad;
	}

	return bytes;

}

bool_t writeByte_circ_buffer(circular_buffer_t * circularBuffer, uint8_t dato)
{
	bool_t writeOK = false; // byte escrito correctamente

	if (circularBuffer->cant_datos < circularBuffer->capacidad)
	{
		circularBuffer->buffer[circularBuffer->indice_escritura] = dato;
		circularBuffer->cant_datos++;
		writeOK = true;
		circularBuffer->indice_escritura = (circularBuffer->indice_escritura + 1) % circularBuffer->capacidad;
	}

	return writeOK;

}

