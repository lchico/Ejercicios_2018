/*============================================================================
 * Autor:	Rodrigo Tirapegui
 * Licencia:
 * Fecha:	22/3/2018
 *===========================================================================*/

#ifndef _CONF_LEDS_H_
#define _CONF_LEDS_H_

/*==================[inclusiones]============================================*/

#include "sapi_peripheral_map.h"

/*==================[c++]====================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/*==================[tipos de datos declarados por el usuario]===============*/

/* Definir a continuaci�n el listado de los leds existentes en el BOARD utilizado.
 * La definici�n debe realizarse usando el tag LEDS_BOARD_GPIO_MAP
 */
#define LEDS_BOARD_GPIO_MAP	{			   \
                                    LEDB	\
									      ,  LEDR  \
									      ,	LEDG  \
									      ,  LED3  \
									      ,  LED2  \
									      ,  LED1  \
								         };

/*==================[declaraciones de datos externos]========================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[c++]====================================================*/
#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif /* _CONF_LEDS_H_ */
