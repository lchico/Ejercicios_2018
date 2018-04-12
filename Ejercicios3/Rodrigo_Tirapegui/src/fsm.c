/*============================================================================
 * Autor:	Rodrigo Tirapegui
 * Licencia:
 * Fecha:	22/3/2018
 *===========================================================================*/

/*==================[inlcusiones]============================================*/

#include "fsm.h"

/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[definiciones de funciones internas]=====================*/

/*==================[definiciones de funciones externas]=====================*/

void fsm(fsmState_t** state, const uint8_t currentEvent, const uint32_t arg) {
   if(state) {
	   fsmState_t* tmpState = *state;

      while(tmpState->event && (currentEvent != tmpState->event))
    	  tmpState++;

      if(tmpState->event) {
    	  tmpState->actionCb(arg);
    	  *state = (fsmState_t*) tmpState->nextState;
      }
   }
}

/*==================[fin del archivo]========================================*/
