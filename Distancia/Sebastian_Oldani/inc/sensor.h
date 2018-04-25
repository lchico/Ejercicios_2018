/*
 * sensor.h
 *
 *  Created on: 16/4/2018
 *      Author: electro1
 */

#ifndef EJER5_INC_SENSOR_H_
#define EJER5_INC_SENSOR_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "sapi.h"
#include <stdio.h>
#include <string.h>

//#define TOTAL_SENSORES sizeof(monitorSensores)/sizeof(sensor_status_t)
#define TOTAL_SENSORES 4						/**< Cantidad de sensores totales */

typedef enum {PUERTA, VENTANA} tipo_sensor_t;
typedef enum {SENSOR_UP,SENSOR_FALLING,SENSOR_DOWN,SENSOR_RAISING} fsmPulsa_t ;

/*!
 * Es la estructura para el control de los sensores
 */
typedef struct {
	fsmPulsa_t statusSensor;
	gpioMap_t pin;
	tipo_sensor_t tipo;
	void(*pressed)(uint8_t);
	void(*release)(uint8_t);
	bool alarm;
} sensor_status_t;

void fsmSensores(uint8_t index_sensor);				/**< Es la maquina de estados finitos de los sensores */
void sensorClear();									/**< Borra los estados de alarma de todos los sensores */
void sensorOpen(uint8_t index_sensor);				/**< Si la alarma esta actica guarda el estado del sensor si se abrio */
bool check_sensores_ventanas();						/**< Checkea si algun sensor de ventana se disparo */
bool check_sensores_puertas();						/**< Checkea si algun sensor de puerta se disparo */

tipo_sensor_t getTipoSensor(uint8_t index_sensor);	/**< Devuelve el tipo del sensor que se uso */
bool getAlarmedSensor(uint8_t index_sensor);		/**< Devuelve si el sensor del index se disparo */


#ifdef __cplusplus
}
#endif

#endif /* EJER5_INC_SENSOR_H_ */
