/*
 * alarma.h
 *
 *  Created on: 16/4/2018
 *      Author: electro1
 */

#ifndef EJER5_INC_ALARMA_H_
#define EJER5_INC_ALARMA_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "sapi.h"
#include <stdio.h>
#include "sensor.h"

#define MAX_FAIL_TRIES 3
typedef enum {DESARMADA, ARMANDO, ARMADA, DESARMANDO, INTRUSO} status_alarm_t;

/*!
 * Es la estructura para el control de la alarma
 */
typedef struct {
	status_alarm_t statusAlarm;
	bool statusSirena;
	bool changeStatus;
	uint32_t counterTimer;
	uint32_t monitorSensor;
	uint32_t password;
	bool accessed;
	uint32_t failtries;
} alarm_cotrol_t;


void fsmAlarmControl(void);					/**< Es la maquina de estados del control de alarma */
int8_t intrusoPutPassword();				/**< Informa si el usuario ingreso la clave correctamente, -1 si supero los intentos */

bool check_alarm_sensores_ventanas();		/**< Checkea si algun sensore de ventana se ha activado */
bool check_alarm_sensores_puertas();		/**< Checkea si algun sensore de puerta se ha activado */
uint8_t getMonitorSensorIndex();			/**< Devuelve el indece del sensor que esta monitorizando */
void nextMonitorSensor();					/**< Cambia el indice al proximo sensor */
uint8_t getAlarmPassword();					/**< Devuelve la password de la alarma */
void checkAlarmPassord(uint8_t pass);		/**< Checkea si pass es la password correcta y revisa si no la ingreso tantas veces */
bool alarmNewStatus();						/**< Devuelve si la alarma cambio de estado */

status_alarm_t getAlarmStatus();			/**< Devuelve el estado actual de la alarma */

void encenderSirena(bool status);			/**< Enciende o apaga la flag de sirena, que no se usa en nada */

#ifdef __cplusplus
}
#endif

#endif /* EJER5_INC_ALARMA_H_ */
