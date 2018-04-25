/* Copyright 2017, Agustin Bassi.
 * All rights reserved.
 *
 * This file is part sAPI library for microcontrollers.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Date: 2017-11-24
 */

#ifndef _MAIN_H_
#define _MAIN_H_

/*==================[inclusions]=============================================*/

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C"
{
#endif

/*==================[macros]=================================================*/

// parámetros de tareas
#define UART_TIME_PERIOD_MS 				200		// periodo de ejecución de tarea UART_Update_Task
#define DEBOUNCE_TIME_PERIOD_MS 			20		// periodo de ejecución de tarea debounce_Teclas_Task
#define ALARMA_MEF_PERIOD_MS    			50		// periodo de ejecución de tarea alarma_MEF_update_task
#define LEDS_BLINK_MEF_PERIOD_MS			100		// retardo de arranque de tarea leds_blink_Task
#define TECLADO_PERIOD_MS	        		300		// retardo de arranque de tarea actualizarTeclado_task

#define UART_DELAY_MS						3		// retardo de arranque de tarea UART_Update_Task
#define DEBOUNCE_TEC_DELAY_MS 				7		// retardo de arranque de tarea debounce_Teclas_Task
#define ALARMA_MEF_DELAY_MS 				11		// retardo de arranque de tarea alarma_MEF_update_task
#define LEDS_BLINK_MEF_DELAY_MS 			13		// retardo de arranque de tarea leds_blink_Task
#define TECLADO_DELAY_MS 			        17		// retardo de arranque de tarea actualizarTeclado_task

#define DEBOUNCE_PERIOD_MS					20		// valor en mS de período de antirrebote de pulsadores

#define PUERTA_GPIO							TEC1	// gpio asociada
#define DISPARO_INSTANTANEO_PUERTA_GPIO		false	// tipo de disparo, instantaneo o retardado

#define VENTANA_1_GPIO						TEC2	// gpio asociada
#define DISPARO_INSTANTANEO_VENTANA_1_GPIO	true	// tipo de disparo, instantaneo o retardado

#define VENTANA_2_GPIO						TEC3	// gpio asociada
#define DISPARO_INSTANTANEO_VENTANA_2_GPIO	true	// tipo de disparo, instantaneo o retardado

#define VENTANA_3_GPIO						TEC4	// gpio asociada
#define DISPARO_INSTANTANEO_VENTANA_3_GPIO	true	// tipo de disparo, instantaneo o retardado

#define N_SENSORES							4		// número de sensores del sistema

#define LED_ARMADO 							LEDR	// LED/gpio asociada a estado ARMADO
#define LED_INTRUSO							LEDR	// LED/gpio asociada a estado INTRUSO
#define LED_DESARMADO						LEDG	// LED/gpio asociada a estado DESARMANDO
#define LED_ARMANDO							LEDB	// LED/gpio asociada a estado ARMANDO
#define LED_DESARMANDO						LEDB	// LED/gpio asociada a estado DESARMADO
#define LED_VENTANA_1						LED1	// LED/gpio asociado a VENTANA 1
#define LED_VENTANA_2						LED2	// LED/gpio asociado a VENTANA 2
#define LED_VENTANA_3						LED3	// LED/gpio asociado a VENTANA 3

#define LED_INTRUSO_BLINK_PERIOD_MS 		500		// periodo blink led estado INTRUSO
#define LED_ARMANDO_BLINK_PERIOD_MS 		1000	// periodo blink led estado ARMANDO

#define LONGITUD_PASSWORD					4		// longitud de password permitida

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

int main(void);

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif /* #ifndef _MAIN_H_ */
