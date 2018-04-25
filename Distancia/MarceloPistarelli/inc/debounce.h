
#ifndef _DEBOUNCE_FSM_H_
#define _DEBOUNCE_FSM_H_

/*==================[inclusiones]============================================*/

#include "sapi.h"

/*==================[c++]====================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/*==================[tipos de datos declarados por el usuario]===============*/

typedef enum {
	BUTTON_UP,
	BUTTON_UP_DEBOUNCED,
	BUTTON_FALLING,
	BUTTON_DOWN,
	BUTTON_DOWN_DEBOUNCED,
	BUTTON_RAISING
} fsm_debounce_State_t;

typedef struct {
	gpioMap_t 				IN;
	bool_t					ActiveHI;
	uint8_t 				period_MS;
	fsm_debounce_State_t 	state;
	delay_t 				debounceDelay;
	void 					(*button_down_callBack)(void);
	void 					(*button_up_callBack)(void);
} debounceIN_t;

/*==================[declaraciones de datos externos]========================*/

/*==================[declaraciones de funciones externas]====================*/

void actualizarDebounceMEF(debounceIN_t * debounce);
void inicializarDebounceMEF(debounceIN_t * debounce);

/*==================[c++]====================================================*/
#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif /* _DEBOUNCE_FSM_H_ */
