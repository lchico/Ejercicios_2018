/* Copyright 2017, Marcelo Pistarelli.

 */


#ifndef _LEDS_BLINK_H_
#define _LEDS_BLINK_H_

/*==================[inclusions]=============================================*/
#include "sapi.h"

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

void init_LEDS(uint32_t update_period); // inicialización de LEDS, se recibe como parámetro el período de llamado
void set_blink_LED(gpioMap_t LED, uint32_t blink_delay);		// set de LED a blinkear
void update_LED(void);
void set_single_led(gpioMap_t LED);
void set_led(gpioMap_t LED);
void reset_led(gpioMap_t LED);


/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif /* #ifndef _LEDS_BLINK_H_ */
