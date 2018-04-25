#ifndef _ALARMA_MEF_H_
#define _ALARMA_MEF_H_

/*==================[inclusiones]============================================*/

#include "sapi.h"

/*==================[c++]====================================================*/

#ifdef __cplusplus
extern "C"
{
#endif

/*==================[macros]=================================================*/

#define ARMANDO_DELAY_MS					30000	// tiempo de espera para armado luego de introducción de clave de armado
#define SENSOR_DISPARO_RETARDADO_DELAY_MS	20000	// tiempo de espera de disparo por activación de sensor retardado

#define N_MAX_INGRESOS_PASS					3		// máximos ingresos incorrectos de pass, a la 4 incorrecta suela la alarma

/*==================[tipos de datos declarados por el usuario]===============*/

typedef enum
{
	DESARMADO, ARMANDO, ARMADO, DESARMANDO, INTRUSO
} alarma_mef_state_t;

typedef struct
{
	gpioMap_t gpio;	//gpio asociada al sensor
	bool_t disparado; // estado del sensor
	bool_t disparo_instantaneo; //tipo de sensor (de disparo instantaneo->ventanas, o disparo retardado->puertas)
} alarma_sensor_t;

typedef struct
{

	uint32_t update_period_MS;	// periodo de actualización de la tarea
	alarma_sensor_t * sensor_array; // array de sensores de la alarma
	char * password;	// contraseña de la alarma
	void (*desarmado_callBack)(void); //callBack de estado desarmado
	void (*armando_callBack)(void); //callBack de estado armando
	void (*armado_callBack)(void); //callBack de estado armado
	void (*desarmando_callBack)(void); //callBack de estado desarmando
	void (*intruso_callBack)(gpioMap_t gpio); //callBack de estado intruso
} alarma_init_t;

/*==================[declaraciones de datos externos]========================*/

/*==================[declaraciones de funciones externas]====================*/

void actualizar_alarma_MEF(void);

void inicializar_alarma_MEF(alarma_init_t init_params);
bool_t password_entry(char * password);

void set_sensor(bool_t sensor_state, uint8_t sensor_id);
bool_t cambiar_password(char * old_password, char * new_password);

/*==================[c++]====================================================*/
#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif /* _ALARMA_MEF_H_ */
