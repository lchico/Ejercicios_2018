/*
 * sensor.c
 *
 *  Created on: 16/4/2018
 *      Author: electro1
 */

#include "sensor.h"

sensor_status_t monitorSensores[] =
{
	{
		.pin = TEC1,
		.tipo = PUERTA,
		.release = NULL,
		.pressed = sensorOpen,
		.alarm = OFF
	},
	{
		.pin = TEC2,
		.tipo = VENTANA,
		.release = NULL,
		.pressed = sensorOpen,
		.alarm = OFF
	},
	{
		.pin = TEC3,
		.tipo = VENTANA,
		.release = NULL,
		.pressed = sensorOpen,
		.alarm = OFF
	},
	{
		.pin = TEC4,
		.tipo = VENTANA,
		.release = NULL,
		.pressed = sensorOpen,
		.alarm = OFF
	},
};

void fsmSensores(uint8_t index_sensor)//fsmPulsa_t *status, gpioMap_t pin, void(*pressed)(void), void(*release)(void))
{
	sensor_status_t *sensor;
	sensor = &monitorSensores[index_sensor];

	switch(sensor->statusSensor){
	case SENSOR_UP:
        if(!gpioRead(sensor->pin))
		{
        	sensor->statusSensor = SENSOR_FALLING;
		}
		break;

	case SENSOR_FALLING:
		if(!gpioRead(sensor->pin))
		{
			//EJECUTAMOS LO QUE SE QUIERA HACER!!!
			if(sensor->pressed!=NULL)
			{
				sensor->pressed(index_sensor);
			}
			sensor->statusSensor = SENSOR_DOWN;
		}
		else
		{
			sensor->statusSensor = SENSOR_UP;
		}
		break;

	case SENSOR_DOWN:

        if(gpioRead(sensor->pin))
		{
        	sensor->statusSensor = SENSOR_RAISING;
		}

	break;

    case SENSOR_RAISING:
		if(gpioRead(sensor->pin))
		{
			//EJECUTAMOS LO QUE SE QUIERA HACER!!!
			if(sensor->release!=NULL)
			{
				sensor->release(index_sensor);
			}
			sensor->statusSensor = SENSOR_UP;
		}
		else
		{
			sensor->statusSensor = SENSOR_DOWN;
		}
	break;
	default:
		sensor->statusSensor = SENSOR_UP;
		break;
	}
}

void sensorOpen(uint8_t index_sensor)
{
	monitorSensores[index_sensor].alarm = ON;//alarm.monitorSensor].alarm = ON;
}

void sensorClear()
{
	int i;
	for(i = 0; i < TOTAL_SENSORES; i++)
	{
		monitorSensores[i].alarm = OFF;
	}
}

bool check_sensores_ventanas()
{
	int i;
	bool actived = false;

	for(i = 0; i < TOTAL_SENSORES; i++)
	{
		if(monitorSensores[i].tipo == VENTANA)
		{
			if(monitorSensores[i].alarm)
			{
				actived = true;
			}
		}
	}
	return actived;
}

bool check_sensores_puertas()
{
	int i;
	bool actived = false;

	for(i = 0; i < TOTAL_SENSORES; i++)
	{
		if(monitorSensores[i].tipo == PUERTA)
		{
			if(monitorSensores[i].alarm)
			{
				actived = true;
			}
		}
	}
	return actived;
}

tipo_sensor_t getTipoSensor(uint8_t index_sensor)
{
	return monitorSensores[index_sensor].tipo;
}

bool getAlarmedSensor(uint8_t index_sensor)
{
	return monitorSensores[index_sensor].alarm;
}
