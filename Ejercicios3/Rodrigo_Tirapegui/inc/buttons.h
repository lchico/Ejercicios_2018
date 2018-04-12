/*============================================================================
 * Autor:	Rodrigo Tirapegui
 * Licencia:
 * Fecha:	22/3/2018
 *===========================================================================*/

#ifndef _BUTTONS_H_
#define _BUTTONS_H_

/*==================[inclusiones]============================================*/

#include "sapi.h"
#include "conf_buttons.h"
#include "preprocessor.h"

/*==================[c++]====================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/* Macro que permite declarar la tarea de un determinado boton */
#define DECLARE_BUTTON_TASK(n, unused)   void debounceTec##n##Task(void);
#define DECLARE_BUTTON_TASK_VECTOR(n, unused) debounceTec##n##Task,

/*==================[tipos de datos declarados por el usuario]===============*/

/* Estados de un boton */
typedef enum {
	BUTTON_UP	=	1
,  BUTTON_DOWN
} buttonStatus_t;

/*==================[declaraciones de datos externos]========================*/

/*==================[declaraciones de funciones externas]====================*/

/**
 * \brief Inicializa el m�dulo de botones que se encargada de implementar 
 *        por software el antirebote al ser pulsado
 * 
 * \param No recibe par�metros
 * 
 * \return No devuelve nada
 */
void buttonsInit(void);

/**
 * \brief Permite obtener el estado de un bot�n (pulsado o liberado).
 *        El estado de un bot�n es determinado con seguridad al ser determinado
 *        su valor mediante una l�gica antirebote por software
 * 
 * \param buttonId Gpio del bot�n cuyo estado se quiere conocer
 * \param status Puntero a la variable donde se almacenar� el estado actual del 
 *               bot�n
 * 
 * \return bool_t TRUE si el gpio solicitado existe. FALSE en caso contrario
 */
bool_t buttonsGetStatus(const uint32_t buttonId, buttonStatus_t* status);

/* Intervalo de ejecucion de las tareas debounceTECnTask */
#define DEBOUNCE_TECn_TASK_TIME_PERIOD          20 // In ms

/**
 * \brief Tarea encargada de implementar el antirebote por software mediante el
 *        uso de una m�quina de estados finita.
 *
 * \param No recibe par�metros
 *
 * \return void No devuelve nada
 */
MREPEAT(BUTTONS_MAX, DECLARE_BUTTON_TASK, ~);

/*==================[c++]====================================================*/
#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif /* _BUTTONS_H_ */
