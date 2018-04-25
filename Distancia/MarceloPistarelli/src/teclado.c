/*============================================================================
 * Autor: Marcelo Pistarelli
 * Licencia:
 * Fecha:
 *===========================================================================*/

/*==================[inclusiones]============================================*/

#include "teclado.h"
#include "main.h"
#include "uart.h"
#include <string.h>
#include "sapi.h"        // <= Biblioteca sAPI

/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/

#define LONGITUD_COMANDO_CAMBIO_CLAVE 12	// longitud de comando de cambio de clave

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

static teclado_init_t teclado_config;	// estructura de configuración de teclado, contiene los callbacks
char password_introducida[LONGITUD_PASSWORD];	// password introducida extraída de UART
char password_introducida_cambio_old[LONGITUD_PASSWORD];// password actual para cambio de clave introducida extraída de UART
char password_introducida_cambio_new[LONGITUD_PASSWORD];// password nueva para cambio de clave introducida extraída de UART

/*==================[declaraciones de funciones externas]====================*/

void teclado_init(teclado_init_t init_teclado_param)
{
	teclado_config = init_teclado_param;	// configuración de callbacks del teclado
}

void actualizarTeclado()
{

	char receivedLine[LONGITUD_MAX_LINEA_COMANDO_UART]; //array para la extracción de línea recibida
	bool_t comandoInvalido = false;	// flag indicador de comando invalido
	uint32_t longitudLineaLeida = 0;
	longitudLineaLeida = UART_read_line_lowecase(receivedLine, LONGITUD_MAX_LINEA_COMANDO_UART); // lectura de comando presente en buffer de entrada

	if (longitudLineaLeida > 0) // si la longitud del comando es superior a 0 se interpreta
	{

		if (strncmp(receivedLine, "sp:", 3) == 0)	// detección de comando de cambio de contraseña
		{
			if (longitudLineaLeida == LONGITUD_COMANDO_CAMBIO_CLAVE)  // validación de longitud de comando
			{
				strncpy(password_introducida_cambio_old, &receivedLine[3], LONGITUD_PASSWORD); // extracción de contraseña actual
				strncpy(password_introducida_cambio_new, &receivedLine[8], LONGITUD_PASSWORD); // extracción de contraseña nueva
				if (teclado_config.cambio_password_callBack != NULL)
				{
					teclado_config.cambio_password_callBack(password_introducida_cambio_old, // callback para cambio de contraseña
							password_introducida_cambio_new);
				}
			}
			else
			{
				comandoInvalido = true;
			}

		}
		else if (longitudLineaLeida == LONGITUD_PASSWORD) // validación de longitud de contraseña
		{

			strncpy(password_introducida, receivedLine, LONGITUD_PASSWORD); // extracción de contraseña introducida
			if (teclado_config.password_callBack != NULL)
			{
				teclado_config.password_callBack(password_introducida);	// callback para introducción de contraseña
			}

		}
		else
		{
			comandoInvalido = true;
		}

		if (comandoInvalido && teclado_config.password_callBack != NULL)
		{
			teclado_config.password_callBack(password_introducida);
		}
	}

}

