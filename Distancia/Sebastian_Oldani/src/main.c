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

/*==================[inclusiones]============================================*/

#include "main.h"

#include "cooperativeOs_isr.h"       // <= dispatcher and task management header
#include "cooperativeOs_scheduler.h" // <= scheduler and system initialization header
#include "sapi.h"
#include "sensor.h"
#include "alarma.h"
#include <stdio.h>
#include <string.h>

/*==================[macros and definitions]=================================*/
DEBUG_PRINT_ENABLE

#define CONSOLE_TIME_PERIOD 100
#define ALARM_TIME_CTRL 50
#define DEBOUNCE_SENSORS 10
#define MAX_RX_BUFFER 128

#define MSG_ALARMA_DESARMADA "ALARMA DESARMADA\r\n"
#define MSG_ALARMA_ARMANDO "ALARMA ARMANDO\r\n"
#define MSG_ALARMA_ARMADA "ALARMA ARMADA\r\n"
#define MSG_ALARMA_DESARMANDO "ALARMA DESARMANDO\r\n"
#define MSG_ALARMA_INTRUSO "ALARMA INTRUSO\r\n"

const gpioMap_t ledEnable[] = {LED1,LED2,LED3};



/*==================[definiciones de datos internos]=========================*/


typedef enum {FULL,BLINKING} tipo_display_t;

/*!
 * Es la estructura para mostrar los leds
 */
typedef struct {
	gpioMap_t pin;
	tipo_display_t tipo;
	uint8_t counter;
}display_t;

display_t display_led;

static uint8_t rxBuffer[MAX_RX_BUFFER];
static uint32_t indexRxBuffer;

static uint8_t menu[] =
		"\n\r"
		"********************* MENU *************************\n\r"
		"\n\r"
		"Ingrese la password, para activar o desactivar la alarma"
		;


/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/

int main( void ){
	//Inicializar el hardware, perifericos, puertos, clock, etc.
	boardConfig();
	// Inicializar UART_USB como salida de consola
	debugPrintConfigUart( UART_USB, 115200 );
	debugPrintlnString( "UART_USB configurada.\n\r" );

	//FUNCION que inicializa el planificador de tareas
	schedulerInit();
	//Cargar las tareas del sistema operativo con sus periodicidades

	indexRxBuffer = 0;

	//Configuro las tareas para control de alarma, antirebote, y consola
	schedulerAddTask( (sAPI_FuncPtr_t) taskAlarmControl, 0, ALARM_TIME_CTRL );
	schedulerAddTask( (sAPI_FuncPtr_t) taskSensores, 1, DEBOUNCE_SENSORS );
	schedulerAddTask( (sAPI_FuncPtr_t) taskMenuUpdate, 2, CONSOLE_TIME_PERIOD );

	//Iniciar el planificador de tareas
	schedulerStart( 1 );

	debugPrintlnString(menu);
	//Lazo infinito

	while(TRUE) {
		//Ejecutar tareas
		schedulerDispatchTasks();
	}
	return 0;
}

/*==================[definiciones de funciones internas]=====================*/

static void taskAlarmControl(void)
{
	//Se acciona cada 50ms => 1200 = 1s
	//Muestra el estado de los leds segun el ultimo estado
	displayRespuesta();
	fsmAlarmControl();
}

static void taskSensores()
{
	fsmSensores(getMonitorSensorIndex());
	nextMonitorSensor();
}

static void taskMenuUpdate(void){

	uint8_t receiveByte;
	uint32_t aux;

	if (uartReadByte( UART_USB, &receiveByte ) != FALSE) {
		if(indexRxBuffer > MAX_RX_BUFFER)
		{
			indexRxBuffer = 0;
		}

		rxBuffer[indexRxBuffer] = receiveByte;
		indexRxBuffer++;
		if(receiveByte == '\n')
		{
			rxBuffer[indexRxBuffer-1] = 0;
			aux = atoi(rxBuffer);
			checkAlarmPassord(aux);
			indexRxBuffer = 0;
		}
	}
}

static void displayRespuesta()
{
	int i, indexVentana = 0;
	if(alarmNewStatus())
	{
		gpioWrite( display_led.pin, OFF );

		switch(getAlarmStatus())
		{
			case DESARMADA:
				debugPrintlnString(MSG_ALARMA_DESARMADA);
				encenderSirena(OFF);
				display_led.pin = LEDG;
				display_led.tipo = FULL;

				for(i = 0; i < TOTAL_SENSORES; i++)
				{
					if(getTipoSensor(i) == VENTANA)
					{
						gpioWrite(ledEnable[indexVentana], OFF);
						indexVentana++;
					}
				}
				break;
			case ARMANDO:
				debugPrintlnString(MSG_ALARMA_ARMANDO);
				break;
			case ARMADA:
				debugPrintlnString(MSG_ALARMA_ARMADA);
				display_led.pin = LEDR;
				display_led.tipo = FULL;
				break;
			case DESARMANDO:
				debugPrintlnString(MSG_ALARMA_DESARMANDO);
				display_led.pin = LEDB;
				display_led.tipo = FULL;
				break;
			case INTRUSO:
				debugPrintlnString(MSG_ALARMA_INTRUSO);
				encenderSirena(ON);
				display_led.pin = LEDR;
				display_led.tipo = BLINKING;
				display_led.counter = 0;
				break;
		}
		gpioWrite( display_led.pin, ON );
	}

	if(display_led.tipo == BLINKING)
	{
		if(++display_led.counter % 2)
		{
			gpioWrite( display_led.pin, ON );
		}
		else
		{
			gpioWrite( display_led.pin, OFF );
		}
	}

	if(getAlarmStatus() == INTRUSO)
	{
		for(i = 0; i < TOTAL_SENSORES; i++)
		{
			if(getTipoSensor(i) == VENTANA)
			{
				if(getAlarmedSensor(i))
				{
					gpioWrite(ledEnable[indexVentana], ON);
				}
				indexVentana++;
			}
		}
	}


}

/*==================[definiciones de funciones externas]=====================*/

/*==================[fin del archivo]========================================*/

