/* Copyright 2017, Marcelo Pistarelli.

 */


#ifndef _UTILS_H_
#define _UTILS_H_

/*==================[inclusions]=============================================*/
#include "sapi.h"

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

#define _length(_Array) (sizeof(_Array) / sizeof(_Array[0])) // Obtención de longitud de array

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

char * itoa_mp(int i);

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif /* #ifndef _UTILS_H_ */
