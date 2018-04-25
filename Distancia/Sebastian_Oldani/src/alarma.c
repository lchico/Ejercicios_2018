/*
 * alarma.c
 *
 *  Created on: 16/4/2018
 *      Author: electro1
 */

#include "alarma.h"

alarm_cotrol_t alarm = {
	.statusAlarm = DESARMADA,
	.statusSirena = OFF,
	.changeStatus = true,
	.counterTimer = 0,
	.monitorSensor = 0,
	.password = 1234,
	.accessed = false,
	.failtries = 0
};

void fsmAlarmControl()
{
	//Se acciona cada 50ms => 1200 = 1s

	//Muestra el estado de los leds segun el ultimo estado
	//displayRespuesta();

	switch(alarm.statusAlarm)
	{
	case DESARMADA:
		alarm.changeStatus = false;
		if(intrusoPutPassword() == 1)
		{
			alarm.statusAlarm = ARMANDO;
			alarm.changeStatus = true;
			alarm.counterTimer = 0;
		}
		break;
	case ARMANDO:
		alarm.changeStatus = false;
		if (++alarm.counterTimer > 120)
		{
			sensorClear();
			alarm.statusAlarm = ARMADA;
			alarm.changeStatus = true;
			alarm.counterTimer = 0;
			break;
		}
		break;
	case ARMADA:
		alarm.changeStatus = false;
		if(check_alarm_sensores_ventanas())
		{
			alarm.changeStatus = true;
			alarm.statusAlarm = INTRUSO;
			alarm.counterTimer = 0;
		}
		if(check_alarm_sensores_puertas())
		{
			alarm.changeStatus = true;
			alarm.statusAlarm = DESARMANDO;
			alarm.counterTimer = 0;
		}
		if(intrusoPutPassword() == 1)
		{
			alarm.statusAlarm = DESARMADA;
			alarm.changeStatus = true;
			alarm.counterTimer = 0;
		}else if(intrusoPutPassword() == -1)
		{
			alarm.statusAlarm = INTRUSO;
			alarm.changeStatus = true;
			alarm.counterTimer = 0;
		}
		break;
	case DESARMANDO:
		alarm.changeStatus = false;
		if (++alarm.counterTimer > 120)
		{
			alarm.statusAlarm = INTRUSO;
			alarm.changeStatus = true;
			alarm.counterTimer = 0;
			break;
		}
		if(check_alarm_sensores_ventanas())
		{
			alarm.changeStatus = true;
			alarm.statusAlarm = INTRUSO;
			alarm.counterTimer = 0;
		}
		if(intrusoPutPassword() == 1)
		{
			alarm.statusAlarm = DESARMADA;
			alarm.changeStatus = true;
			alarm.counterTimer = 0;
		}else if(intrusoPutPassword() == -1)
		{
			alarm.statusAlarm = INTRUSO;
			alarm.changeStatus = true;
			alarm.counterTimer = 0;
		}
		break;
	case INTRUSO:
		alarm.changeStatus = false;
		if(intrusoPutPassword() == 1)
		{
			alarm.statusAlarm = DESARMADA;
			alarm.changeStatus = true;
			alarm.counterTimer = 0;
		}
		break;
	}

}

int8_t intrusoPutPassword()
{
	if(alarm.accessed)
	{
		alarm.accessed = false;
		return 1;
	}
	else if(alarm.failtries >= MAX_FAIL_TRIES)
	{
		return -1;
	}
	return 0;
}

bool check_alarm_sensores_ventanas()
{
	return check_sensores_ventanas();
}

bool check_alarm_sensores_puertas()
{
	return check_sensores_puertas();
}

uint8_t getMonitorSensorIndex()
{
	return alarm.monitorSensor;
}

void nextMonitorSensor()
{
	alarm.monitorSensor++;
	if(alarm.monitorSensor >= TOTAL_SENSORES)
	{
		alarm.monitorSensor = 0;
	}
}

uint8_t getAlarmPassword()
{
	return alarm.password;
}

void checkAlarmPassord(uint8_t pass)
{
	if(pass == getAlarmPassword())
	{
		alarm.accessed = true;
		alarm.failtries = 0;
	}
	else
	{
		alarm.failtries++;
	}
}

bool alarmNewStatus()
{
	return alarm.changeStatus;
}

status_alarm_t getAlarmStatus()
{
	return alarm.statusAlarm;
}

void encenderSirena(bool status)
{
	alarm.statusSirena = status;
}
