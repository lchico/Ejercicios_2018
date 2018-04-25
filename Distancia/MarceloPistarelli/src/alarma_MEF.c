/*==================[inlcusiones]============================================*/

#include "alarma_MEF.h"
#include "main.h"
#include <string.h>

/*==================[macros and definitions]=================================*/

/*==================[definiciones de datos internos]=========================*/

static alarma_init_t alarma_parametros;
static alarma_mef_state_t alarma_state = DESARMADO;	// estado de alarma, inicialización en desarmado
static bool_t new_state = true;	// indica cambio de estado
static uint32_t state_call_count = 0; // cuenta de llamadas a actualizar_alarma_MEF, usado para temporizar
static uint8_t n_ingresos_pass = 0; // cuenta de ingresos de contraseña
static gpioMap_t gpio_disparo_alarma = 0; // gpio que produjo el diaparo de la alarma
static char password_ingresada[LONGITUD_PASSWORD]; // contiene la contraseña ingresada por teclado

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

static uint8_t get_sensor_id(gpioMap_t gpio);
static bool_t check_instant_sensors();	// chequeo de estado de sensores instantaneos (las ventanas), true=sensor activado
static bool_t check_delayed_sensors();// chequeo de estado de sensores instantaneos (la puerta), true=sensor activado
static bool_t check_pass(void);	// verificación de contraseña, ture=contraseña correcta

/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/

void inicializar_alarma_MEF(alarma_init_t init_params)
{
	alarma_parametros = init_params;
	state_call_count = 0;
}

void actualizar_alarma_MEF(void)
{
	state_call_count++;

	switch (alarma_state)
	{

	case DESARMADO:

		if (new_state)
		{
			if (alarma_parametros.desarmado_callBack != NULL) // callback estado DESARMADO
			{	// llamo a función si no es null
				alarma_parametros.desarmado_callBack();
			}
			new_state = false;
		}

		// verifica contraseña ingresada
		if (check_pass())
		{
			alarma_state = ARMANDO;
			new_state = true;
			state_call_count = 0;
		}

		// estando DESARMADO si se ingresa mal la contraseña > N_MAX_INGRESOS_PASS pasa a INTRUSO
		if (n_ingresos_pass > N_MAX_INGRESOS_PASS)
		{
			alarma_state = INTRUSO;
			new_state = true;
			state_call_count = 0;
		}

		break;

	case ARMANDO:
		if (new_state)
		{
			if (alarma_parametros.armando_callBack != NULL) // callback estado ARMANDO
			{	// llamo a función si no es null
				alarma_parametros.armando_callBack();
			}
			new_state = false;
		}

		// pasa a ARMADO si supera ARMANDO_DELAY_MS en estado ARMANDO
		if (state_call_count > (ARMANDO_DELAY_MS / alarma_parametros.update_period_MS))	// retardo independiente de update period task
		{
			alarma_state = ARMADO;
			new_state = true;
			state_call_count = 0;
		}

		// verifica contraseña ingresada
		if (check_pass())
		{
			alarma_state = DESARMADO;
			new_state = true;
			state_call_count = 0;
		}

		break;

	case ARMADO:
		if (new_state)
		{
			if (alarma_parametros.armado_callBack != NULL) // callback estado ARMADO
			{	// llamo a función si no es null
				alarma_parametros.armado_callBack();
			}
			new_state = false;
		}

		// verificación de sensores instantáneos (las ventanas)
		if (check_instant_sensors())
		{
			alarma_state = INTRUSO;
			new_state = true;
			state_call_count = 0;
		}

		// verificación de sensores retardados (la puerta)
		else if (check_delayed_sensors())
		{
			alarma_state = DESARMANDO;
			new_state = true;
			state_call_count = 0;
		}

		// verifica contraseña ingresada
		if (check_pass())
		{
			alarma_state = DESARMADO;
			new_state = true;
			state_call_count = 0;
		}

		// estando ARMADO si se ingresa mal la contraseña > N_MAX_INGRESOS_PASS pasa a INTRUSO
		if (n_ingresos_pass > N_MAX_INGRESOS_PASS)
		{
			alarma_state = INTRUSO;
			new_state = true;
			state_call_count = 0;
		}

		break;

	case DESARMANDO:
		if (new_state)
		{
			if (alarma_parametros.desarmando_callBack != NULL) // callback estado DESARMANDO
			{	// llamo a función si no es null
				alarma_parametros.desarmando_callBack();
			}
			new_state = false;
		}

		// verificación de sensores instantáneos (las ventanas)
		if (check_instant_sensors())
		{
			alarma_state = INTRUSO;
			new_state = true;
			state_call_count = 0;
		}

		// pasa a INTRUSO si supera SENSOR_DISPARO_RETARDADO_DELAY_MS en estado DESARMANDO
		if (state_call_count > (SENSOR_DISPARO_RETARDADO_DELAY_MS / alarma_parametros.update_period_MS))// retardo independiente de update period task
		{
			alarma_state = INTRUSO;
			new_state = true;
			state_call_count = 0;
			break;
		}

		// verifica contraseña ingresada
		if (check_pass())
		{
			alarma_state = DESARMADO;
			new_state = true;
			state_call_count = 0;
		}

		// estando DESARMANDO si se ingresa mal la contraseña > N_MAX_INGRESOS_PASS pasa a INTRUSO
		if (n_ingresos_pass > N_MAX_INGRESOS_PASS)
		{
			alarma_state = INTRUSO;
			new_state = true;
			state_call_count = 0;
		}

		break;

	case INTRUSO:
		if (new_state)
		{
			if (alarma_parametros.intruso_callBack != NULL) // callback estado INTRUSO
			{	// llamo a función si no es null
				alarma_parametros.intruso_callBack(gpio_disparo_alarma);
			}
			new_state = false;
		}

		// verifica contraseña ingresada
		if (check_pass())
		{
			alarma_state = DESARMADO;
			new_state = true;
			state_call_count = 0;
		}
		break;

	default:
		break;

	}

}

// función llamada externamente para setear valores de activación/desactivación de sensores en estructura de parámetros de la alarma
void set_sensor(bool_t sensor_state, gpioMap_t sensor)
{
	int8_t sensor_id = get_sensor_id(sensor);
	if (sensor_id >= 0)
	{
		alarma_parametros.sensor_array[sensor_id].disparado = sensor_state;

	}

}

// función llamada externamente para setear contraseña ingresada
bool_t password_entry(char * password)
{
	bool_t passCorrecta = false;

	strncpy(password_ingresada, password, LONGITUD_PASSWORD);

	n_ingresos_pass++;

	return strncmp(password_ingresada, alarma_parametros.password, LONGITUD_PASSWORD) == 0;
}

// función llamada externamente cambiar la password almacenada en la estructura de parámetros de la alarma
bool_t cambiar_password(char * old_password, char * new_password)
{
	bool_t changed = false;
	if (strncmp(alarma_parametros.password, old_password, LONGITUD_PASSWORD) == 0)
	{
		alarma_parametros.password = new_password;
		changed = true;
	}
	return changed;
}


// verifica contraseña ingresada
static bool_t check_pass(void)
{
	bool_t pass_ok = false;

	if (strncmp(password_ingresada, alarma_parametros.password, LONGITUD_PASSWORD) == 0)
	{
		password_ingresada[0] = '\0';
		n_ingresos_pass = 0;	// al ingresar pass correcta se resetea el contador de pass incorrectas
		pass_ok = true;
	}

	return pass_ok;
}


// verificación de sensores instantáneos (las ventanas)
static bool_t check_instant_sensors()
{
	bool_t disparado = false;
	uint8_t i;
	for (i = 0; i < N_SENSORES; i++)
	{
		if (alarma_parametros.sensor_array[i].disparo_instantaneo == true
				&& alarma_parametros.sensor_array[i].disparado == true)
		{
			gpio_disparo_alarma = alarma_parametros.sensor_array[i].gpio;
			disparado = true;
		}
	}
	return disparado;
}

// verificación de sensores retardados (la puerta)
static bool_t check_delayed_sensors()
{
	bool_t disparado = false;
	uint8_t i;
	for (i = 0; i < N_SENSORES; i++)
	{
		if (alarma_parametros.sensor_array[i].disparo_instantaneo == false
				&& alarma_parametros.sensor_array[i].disparado == true)
		{
			gpio_disparo_alarma = 0;
			disparado = true;

		}

	}

	return disparado;
}

// obtiene el index en el array de sensores en estructura de parámetros de la alarma asociado al sensor indicado por una gpio
static uint8_t get_sensor_id(gpioMap_t gpio)
{

	uint8_t i;
	int8_t id = -1;

	for (i = 0; i < N_SENSORES; i++)
	{
		if (alarma_parametros.sensor_array[i].gpio == gpio)
		{
			id = i;
			break;
		}
	}
	return id;

}

