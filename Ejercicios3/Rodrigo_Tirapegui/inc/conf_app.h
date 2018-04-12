/*============================================================================
 * Autor:	Rodrigo Tirapegui
 * Licencia:
 * Fecha:	22/3/2018
 *===========================================================================*/

#ifndef _CONF_APP_H_
#define _CONF_APP_H_

/*==================[inclusiones]============================================*/

#include "sapi_peripheral_map.h"

/*==================[c++]====================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/*==================[tipos de datos declarados por el usuario]===============*/

/* Definir a continuaci�n el listado de los periodos de parpadeo que deben
 * usarse al pulsar la tecla TEC1 (en milisegundos)
 * La definici�n debe realizarse usando el tag APP_BLINK_PERIODS_MS
 */
#define APP_BLINK_PERIODS_MS	{			\
                                    50	   \
                                 ,  100   \
									      ,  1000  \
								         };

/*==================[declaraciones de datos externos]========================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[c++]====================================================*/
#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif /* _CONF_APP_H_ */
