/*============================================================================
 * Autor:	Rodrigo Tirapegui
 * Licencia:
 * Fecha:	22/3/2018
 *===========================================================================*/

#ifndef _CONF_BUTTONS_H_
#define _CONF_BUTTONS_H_

/*==================[inclusiones]============================================*/

#include "sapi_peripheral_map.h"

/*==================[c++]====================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/*==================[tipos de datos declarados por el usuario]===============*/

/* Definir a continuaci�n el listado de los botones existentes en el BOARD utilizado.
 * La definici�n debe realizarse usando el tag BUTTONS_BOARD_GPIO_MAP
 */
#define BUTTONS_BOARD_GPIO_MAP	{			\
									         TEC1	\
									      ,  TEC2  \
                                 };

/* Definir a continuaci�n la cantidad de botones que contiene el BOARD usado */
#define BUTTONS_MAX  2

/*==================[declaraciones de datos externos]========================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[c++]====================================================*/
#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif /* _CONF_BUTTONS_H_ */
