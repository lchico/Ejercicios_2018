/*============================================================================
 * Autor:	Rodrigo Tirapegui
 * Licencia:
 * Fecha:	22/3/2018
 *===========================================================================*/

#ifndef _LEDS_H_
#define _LEDS_H_

/*==================[inclusiones]============================================*/

#include "sapi.h"

/*==================[c++]====================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/*==================[tipos de datos declarados por el usuario]===============*/

/*==================[declaraciones de datos externos]========================*/

/*==================[declaraciones de funciones externas]====================*/

/**
 * \brief Inicializa el m�dulo de leds, que permite operar sobre los leds de
 *        la BOARD utilizada
 * 
 * \param No recibe par�metros
 * 
 * \return bool_t No devuelve nada
 */
void ledsInit(void);


/**
 * \brief Permite iniciar el parpadeo de un determinado led.
 *        El driver permite el parpadeo de 1 solo led por vez,
 *        por lo tanto, al iniciarse, el parpadeo del led previo
 *        es detenido
 * 
 * \param ledId Gpio del led que se busca comience a parpadear
 * 
 * \return void No devuelve nada
 */
void ledsStartBlinking(const uint32_t ledId);

/**
 * \brief Permite establecer el periodo de parpadeo de un led
 *
 * \param ledId Gpio del led cuyo periodo de parpadeo se busca establecer
 * \param period Intervalo de tiempo (en milisegundos) de parpadeo
 *
 * \return void No devuelve nada
 */
void ledsSetBlinkPeriod(const uint32_t ledId, const uint32_t periodMs);

/**
 * \brief Permite encender un led
 * 
 * \param ledId Gpio del led que se busca encender
 * 
 * \return void No devuelve nada
 */
void ledsTurnOn(const uint32_t ledId);

/**
 * \brief Permite apagar un led
 * 
 * \param ledId Gpio del led que se busca apagar
 * 
 * \return void No devuelve nada
 */
void ledsTurnOff(const uint32_t ledId);

/* Intervalo de ejecucion de la tarea blinkLedTask */
#define BLINK_LED_TASK_TIME_PERIOD   10 // In ms

/**
 * \brief Tarea encargada de implementar el parpadeo de un led mediante
 *        una m�quina de estado finita.
 *
 * \param No recibe par�metros
 *
 * \return void No devuelve nada
 */
void blinkLedTask(void);

/*==================[c++]====================================================*/
#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif /* _LEDS_H_ */
