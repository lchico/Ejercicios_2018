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

/*==================[inlcusiones]============================================*/

#include "main.h"
#include "utils.h"
#include "uart.h"
#include "teclado.h"
#include "debounce.h"
#include "alarma_MEF.h"
#include "cooperativeOs_isr.h"       // <= dispatcher and task management header
#include "cooperativeOs_scheduler.h" // <= scheduler and system initialization header
#include "sapi.h"
#include <string.h>
#include <ctype.h>

/*==================[macros and definitions]=================================*/
DEBUG_PRINT_ENABLE

/*==================[definiciones de datos internos]=========================*/

static uint8_t menu[] =
		"\n\r"
				"************************************************* MENU ******************************************************\n\r"
				"SP:[clave],[nueva clave] para setear una nueva clave alf-num de 4 caracteres\n\r"
				"[clave] arma o desarma el sistema\n\r"
				"*************************************************************************************************************\n\r";

static char DEFAULT_PASSWORD[] = "ab12"; //minúsculas y números

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

// tareas del sistema
static void UART_update_task(void);			// actualización de UART
static void debounce_teclas_Task(void);		// debounce
static void alarma_MEF_update_task(void);	// lógica de control de alarma
static void leds_blink_Task(void);			// encendido y blinkeo de leds
static void actualizarTeclado_task(void);	// lectura de teclado (lectura de buffer circular de uart)

// eventos generados por debounce asociados a los sensores, down->abierto, up->cerrado,
static void puerta_down(void);
static void puerta_up(void);
static void ventana_1_down(void);
static void ventana_1_up(void);
static void ventana_2_down(void);
static void ventana_2_up(void);
static void ventana_3_down(void);
static void ventana_3_up(void);

// eventos generados por alarma_MEF asociados a los estados de la alarma
static void desarmado_callBack(void);
static void armando_callBack(void);
static void armado_callBack(void);
static void desarmando_callBack(void);
static void intruso_callBack(gpioMap_t gpio);

// eventos generados por teclado a partir de la interpretación de los comandos recibidos por UART
static void cambio_password_callBack(char * old_pass, char * new_pass);
static void password_callBack(char * pass);
static void comando_invalido_callBack(void);

// declaro e inicializo estructura para debounce de TEC1 (sensor puerta)
static debounceIN_t puerta_antirebote =
{ .IN = PUERTA_GPIO, .period_MS = DEBOUNCE_PERIOD_MS, .button_down_callBack = &puerta_down, .button_up_callBack =
		&puerta_up };

// declaro e inicializo estructura para debounce de TEC2 (sensor ventana 1)
static debounceIN_t ventana_1_antirebote =
{ .IN = VENTANA_1_GPIO, .period_MS = DEBOUNCE_PERIOD_MS, .button_down_callBack = &ventana_1_down, .button_up_callBack =
		&ventana_1_up };

// declaro e inicializo estructura para debounce de TEC3 (sensor ventana 2)
static debounceIN_t ventana_2_antirebote =
{ .IN = VENTANA_2_GPIO, .period_MS = DEBOUNCE_PERIOD_MS, .button_down_callBack = &ventana_2_down, .button_up_callBack =
		&ventana_2_up };

// declaro e inicializo estructura para debounce de TEC4 (sensor ventana 3)
static debounceIN_t ventana_3_antirebote =
{ .IN = VENTANA_3_GPIO, .period_MS = DEBOUNCE_PERIOD_MS, .button_down_callBack = &ventana_3_down, .button_up_callBack =
		&ventana_3_up };

// declaración e inicialización de sensores
alarma_sensor_t sensor1 =
{ .gpio = PUERTA_GPIO, .disparado = false, .disparo_instantaneo = DISPARO_INSTANTANEO_PUERTA_GPIO };
alarma_sensor_t sensor2 =
{ .gpio = VENTANA_1_GPIO, .disparado = false, .disparo_instantaneo = DISPARO_INSTANTANEO_VENTANA_1_GPIO };
alarma_sensor_t sensor3 =
{ .gpio = VENTANA_2_GPIO, .disparado = false, .disparo_instantaneo = DISPARO_INSTANTANEO_VENTANA_2_GPIO };
alarma_sensor_t sensor4 =
{ .gpio = VENTANA_3_GPIO, .disparado = false, .disparo_instantaneo = DISPARO_INSTANTANEO_VENTANA_3_GPIO };

// array de sensores
alarma_sensor_t sensor_arr[N_SENSORES];

// esctructura de configuación e inicialización de alarma
static alarma_init_t alarma_config_parametros =
{ .update_period_MS = ALARMA_MEF_PERIOD_MS, .password = DEFAULT_PASSWORD, .desarmado_callBack = &desarmado_callBack,
		.armando_callBack = &armando_callBack, .armado_callBack = &armado_callBack, .desarmando_callBack =
				&desarmando_callBack, .intruso_callBack = &intruso_callBack };

// esctructura de configuación e inicialización de teclado
static teclado_init_t teclado_init_config =
{ .cambio_password_callBack = &cambio_password_callBack, .password_callBack = &password_callBack, .comando_invalido =
		&comando_invalido_callBack };

/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/

int main(void)
{
	//Inicializar el hardware, perifericos, puertos, clock, etc.
	boardConfig();
	// Inicializar UART_USB como salida de consola
	debugPrintConfigUart(UART_USB, 115200);
	//debugPrintlnString("UART_USB configurada.\n\r");

	// Inicialización de UART a utilizar
	UART_Init(UART_USB);

	// Inicializar MEF de cada tecla
	inicializarDebounceMEF(&puerta_antirebote);
	inicializarDebounceMEF(&ventana_1_antirebote);
	inicializarDebounceMEF(&ventana_2_antirebote);
	inicializarDebounceMEF(&ventana_3_antirebote);

	// Inicialización de Teclado
	teclado_init(teclado_init_config);

	// Inicialización/configuración de MEF alarma
	sensor_arr[0] = sensor1;
	sensor_arr[1] = sensor2;
	sensor_arr[2] = sensor3;
	sensor_arr[3] = sensor4;
	alarma_config_parametros.sensor_array = sensor_arr;
	inicializar_alarma_MEF(alarma_config_parametros);

	//FUNCION que inicializa el planificador de tareas
	schedulerInit();
	//Cargar las tareas del sistema operativo con sus periodicidades

	schedulerAddTask((sAPI_FuncPtr_t) UART_update_task, UART_DELAY_MS, UART_TIME_PERIOD_MS);
	schedulerAddTask((sAPI_FuncPtr_t) debounce_teclas_Task, DEBOUNCE_TEC_DELAY_MS, DEBOUNCE_TIME_PERIOD_MS);
	schedulerAddTask((sAPI_FuncPtr_t) alarma_MEF_update_task, ALARMA_MEF_DELAY_MS, ALARMA_MEF_PERIOD_MS);
	schedulerAddTask((sAPI_FuncPtr_t) leds_blink_Task, LEDS_BLINK_MEF_DELAY_MS, LEDS_BLINK_MEF_PERIOD_MS);
	schedulerAddTask((sAPI_FuncPtr_t) actualizarTeclado_task, TECLADO_DELAY_MS, TECLADO_PERIOD_MS);

	//Iniciar el planificador de tareas
	schedulerStart(1);

	UART_write_String_To_TX_Buffer(menu);	// presentación de menu por consola
	//Lazo infinito

	while (TRUE)
	{
		//Ejecutar tareas
		schedulerDispatchTasks();
	}
	return 0;
}

/*==================[definiciones de funciones internas]=====================*/

static void UART_update_task(void) // Tarea de acutalizaicón de recepción/envío de datos presentes en los buffers de la UART
{
	UART_Update();
}

static void actualizarTeclado_task(void) // Tarea de acutalizaicón de teclado/interpretador de datos recibidos por UART
{
	actualizarTeclado();
}

static void alarma_MEF_update_task(void) // Tarea de acutalizaicón de lógica de control de UART
{
	actualizar_alarma_MEF();
}

static void leds_blink_Task(void) // Tarea de acutalizaicón de encendido y blinkeo de leds
{
	update_LED();
}

static void debounce_teclas_Task(void)	// Tarea de acutalizaicón de MEF de debounce de pulsadores/sensores de entrada
{
	actualizarDebounceMEF(&puerta_antirebote);			// aplico debounce a tecla 1
	actualizarDebounceMEF(&ventana_1_antirebote);		// aplico debounce a tecla 2
	actualizarDebounceMEF(&ventana_2_antirebote);		// aplico debounce a tecla 3
	actualizarDebounceMEF(&ventana_3_antirebote);		// aplico debounce a tecla 4
}

static void puerta_down(void)		// callback tecla 1 down (sensor puerta)
{
	UART_write_String_To_TX_Buffer("Sensor puerta activado \n\r");
	set_sensor(true, PUERTA_GPIO);
}

static void puerta_up(void)		// callback tecla 1 up (sensor puerta)
{
	UART_write_String_To_TX_Buffer("Sensor puerta desactivado \n\r");
	set_sensor(false, PUERTA_GPIO);
}

static void ventana_1_down(void)		// callback tecla 2 down (sensor ventana 1)
{
	UART_write_String_To_TX_Buffer("Sensor ventana 1 activado \n\r");
	set_sensor(true, VENTANA_1_GPIO);
}

static void ventana_1_up(void)		// callback tecla 2 up (sensor ventana 1)
{
	UART_write_String_To_TX_Buffer("Sensor ventana 1 desactivado \n\r");
	set_sensor(false, VENTANA_1_GPIO);
}

static void ventana_2_down(void)		// callback tecla 3 down (sensor ventana 2)
{
	UART_write_String_To_TX_Buffer("Sensor ventana 2 activado \n\r");
	set_sensor(true, VENTANA_2_GPIO);
}

static void ventana_2_up(void)		// callback tecla 3 up (sensor ventana 2)
{
	UART_write_String_To_TX_Buffer("Sensor ventana 2 desactivado \n\r");
	set_sensor(false, VENTANA_2_GPIO);
}

static void ventana_3_down(void)		// callback tecla 4 down (sensor ventana 3)
{
	UART_write_String_To_TX_Buffer("Sensor ventana 3 activado \n\r");
	set_sensor(true, VENTANA_3_GPIO);
}

static void ventana_3_up(void)		// callback tecla 4 up (sensor ventana 3)
{
	UART_write_String_To_TX_Buffer("Sensor ventana 3 desactivado \n\r");
	set_sensor(false, VENTANA_3_GPIO);
}

static void desarmado_callBack(void)	// callback generada por alarma_MEF indicando cambio de estado
{
	UART_write_String_To_TX_Buffer("DESARMADO \n\r");
	set_single_led(LED_DESARMADO);
}
static void armando_callBack(void)	// callback generada por alarma_MEF indicando cambio de estado
{
	UART_write_String_To_TX_Buffer("ARMANDO... \n\r");
	set_blink_LED(LED_ARMANDO, LED_ARMANDO_BLINK_PERIOD_MS);
}
static void armado_callBack(void)	// callback generada por alarma_MEF indicando cambio de estado
{
	UART_write_String_To_TX_Buffer("ARMADO \n\r");
	set_single_led(LED_ARMADO);
}
static void desarmando_callBack(void)	// callback generada por alarma_MEF indicando cambio de estado
{
	UART_write_String_To_TX_Buffer("DESARMANDO... Introducir clave \n\r");
	set_single_led(LED_DESARMANDO);
}
static void intruso_callBack(gpioMap_t gpio)	// callback generada por alarma_MEF indicando cambio de estado
{
	UART_write_String_To_TX_Buffer("ALARMA!! INTRUSO \n\r");
	set_blink_LED(LED_INTRUSO, LED_INTRUSO_BLINK_PERIOD_MS);

	switch (gpio)
	// set de led asociado a la primer entrada que se disparó durante el estado armado
	{
	case VENTANA_1_GPIO:
		set_led(LED_VENTANA_1);
		break;
	case VENTANA_2_GPIO:
		set_led(LED_VENTANA_2);
		break;
	case VENTANA_3_GPIO:
		set_led(LED_VENTANA_3);
		break;

	}
}

void cambio_password_callBack(char * old_pass, char * new_pass) // callback del teclado/interprete de uart para cambio de clave
{
	if (cambiar_password(old_pass, new_pass))
	{
		UART_write_String_To_TX_Buffer("Password modificada correctamente \n\r");
	}
	else
	{
		UART_write_String_To_TX_Buffer("Error al modificar password \n\r");
	}
}

void password_callBack(char * pass) // callback del teclado/interprete de uart con clave introducida
{
	if (password_entry(pass))
	{
		UART_write_String_To_TX_Buffer("Password correcta ingresada \n\r");
	}
	else
	{
		UART_write_String_To_TX_Buffer("Password incorrecta ingresada \n\r");
	}
}

void comando_invalido_callBack(void)
{
	UART_write_String_To_TX_Buffer("Comando no permitido \n\r");
	UART_write_String_To_TX_Buffer(menu);
}

/*==================[definiciones de funciones externas]=====================*/

/*==================[fin del archivo]========================================*/

