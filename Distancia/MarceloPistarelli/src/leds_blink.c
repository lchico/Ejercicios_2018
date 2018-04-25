/*============================================================================
 * Autor: Marcelo Pistarelli
 *===========================================================================*/

// se recortó/simplificó el blink hecho con máquinas de estado de ejercicios anteriores, no se usa MEF
/*==================[inclusiones]============================================*/

#include "debounce.h"
#include "main.h"
#include "utils.h"
#include "sapi.h"        // <= Biblioteca sAPI

/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/

static const uint8_t leds_disponibles[] =
{ LEDR, LEDG, LEDB, LED1, LED2, LED3 };	// leds presentes en la palca

static uint32_t call_count = 0;	// cuenta de llamadas a la funcion de actualizacion, usado para temporizacion no bloqueante
static uint32_t led_blink_delay_MS = 0;	// tiempo de blink para blinkeo d eled
static bool_t blink = false;			// habilitación de blinkeo
static uint8_t indexLedActual = 0;		// index de led a blinkear perteneciente a leds_disponibles
static uint32_t update_period_MS = 0;	// tiempo de actualización de tarea, usado para calcular temporizacion

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

static void clean_LEDS(void);	// función para forzar el apagado de todos los leds

/*==================[declaraciones de funciones externas]====================*/

void init_LEDS(uint32_t update_period)
{
	update_period_MS = update_period;
}

void set_blink_LED(gpioMap_t LED, uint32_t blink_delay)		// set de LED a blinkear
{

	uint8_t i;

	clean_LEDS();

	for (i = 0; i < _length(leds_disponibles); i++)
	{
		if (leds_disponibles[i] == LED)
		{
			indexLedActual = i;	// index de led a blinkear perteneciente a leds_disponibles
			led_blink_delay_MS = blink_delay; // establezco tiempo de blinkeo
			blink = true;	// habilito blinkeo
			break;
		}
	}

}

void set_single_led(gpioMap_t LED) // apaga todos los leds, prende 1 solo led
{
	clean_LEDS();
	gpioWrite(LED, true);
}

void set_led(gpioMap_t LED) // prende 1 solo led
{
	gpioWrite(LED, true);
}

void reset_led(gpioMap_t LED) // apaga 1 solo led
{
	gpioWrite(LED, false);
}

static void clean_LEDS(void)		// apaga todos los leds, detiene blink
{

	uint8_t i;

	blink = false;
	for (i = 0; i < _length(leds_disponibles); i++)
	{
		gpioWrite(leds_disponibles[i], false);
	}

}

void update_LED(void)	// actualización de función de blinkeo
{
	call_count++;

	if (blink && (call_count > (led_blink_delay_MS / update_period_MS)))// retardo independiente de update period de la tarea
	{
		gpioToggle(leds_disponibles[indexLedActual]);
		call_count = 0;
	}

}

