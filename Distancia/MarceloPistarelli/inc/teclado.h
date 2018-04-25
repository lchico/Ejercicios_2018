/* Copyright 2018, Marcelo Pistarelli.

 */

#ifndef _TECLADO_H_
#define _TECLADO_H_

/*==================[inclusions]=============================================*/
#include "sapi.h"

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C"
{
#endif

/*==================[macros]=================================================*/

#define LONGITUD_MAX_LINEA_COMANDO_UART	20

/*==================[typedef]================================================*/

typedef struct
{
	void (*cambio_password_callBack)(char * old_pass, char * new_pass);
	void (*password_callBack)(char * pass);
	void (*comando_invalido)(void);
} teclado_init_t;

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif /* #ifndef _TECLADO_H_ */

