/*============================================================================
 * Autor:	Rodrigo Tirapegui
 * Licencia:
 * Fecha:	22/3/2018
 *===========================================================================*/

#ifndef _FSM_H_
#define _FSM_H_

/*==================[inclusiones]============================================*/

#include "sapi.h"

/*==================[c++]====================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[tipos de datos declarados por el usuario]===============*/

/* Formato de un estado de la m�quina de estados finita */
typedef struct fsmState fsmState_t;
typedef void (*fsmActionCb_t) (const uint32_t arg);

struct fsmState {
   const uint8_t event;
   const fsmState_t* const nextState;
   const fsmActionCb_t actionCb;
};

/*==================[macros]=================================================*/

/* Usar la siguiente macro para definir la tabla de estados finita
 *
 * Ejemplo:
 *
 * #define DEFINE_FSM_STATE(Estado0,
 *                          {EVENTO_1, Estado1, accionTipo1}
 *                          ,{EVENTO_2, Estado2, accionTipo2}
 *                          ,{EVENTO_3, Estado3, accionTipo3}
 *                         )
 *
 * NOTA: Los campos EVENTO_n no pueden valer 0 (vease fsm.h)
 * */
#define DEFINE_FSM_STATE(name,...)               \
                                                 \
static const fsmState_t name[] = {               \
                                    __VA_ARGS__  \
                                    ,  {0,0,0}   \
                                 };

/*==================[declaraciones de datos externos]========================*/

/*==================[declaraciones de funciones externas]====================*/

/**
 * \brief Funci�n gen�rica que permite avanzar la m�quina de estados finita
 *        implementada por alg�n m�dulo que conforma el programa.
 *        La misma debe ser invocada desde el superloop principal del programa
 * 
 * \param state Direcci�n del puntero al estado actual de la m�quina de estados 
 *              finita
 * \param currentEvent Evento actual del m�dulo invocante.
 *                     Su valor debe ser distinto de 0.
 * \param arg Argumento que ser� pasado por par�metro al callback de acci�n 
 *            asociado con el estado actual de la m�quina de estados finita
 * 
 * \return void No devuelve nada
 */
void fsm(fsmState_t** state, const uint8_t currentEvent, const uint32_t arg);

/*==================[c++]====================================================*/
#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif /* _FSM_H_ */
