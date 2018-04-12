/*============================================================================
 * Autor:
 * Licencia:
 * Fecha:
 *===========================================================================*/

/*==================[inclusiones]============================================*/

#include "sapi.h"        // <= Biblioteca sAPI

/*==================[definiciones y macros]==================================*/

typedef enum {LED_ON, LED_OFF} fsmBlinky_t;

/*==================[definiciones de datos internos]=========================*/

#define DELAY_DURATION 1000

fsmBlinky_t state = LED_ON;

delay_t blinkyDelay;

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/
void fsmBlinky(void);

/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main( void ){

	// ---------- CONFIGURACIONES ------------------------------

	// Inicializar y configurar la plataforma
	boardConfig();

	// Crear varias variables del tipo booleano
	bool_t tec1Value = OFF;
	bool_t tec2Value = OFF;
	bool_t tec3Value = OFF;
	bool_t tec4Value = OFF;

	// ---------- REPETIR POR SIEMPRE --------------------------
	while( TRUE )
	{
		fsmBlinky();

		/* Si se presiona TEC1, enciende el LEDR */

		// Leer pin conectado a la tecla.
		tec1Value = gpioRead( TEC1 );
		// Invertir el valor leido, pues lee un 0 (OFF) con tecla
		// presionada y 1 (ON) al liberarla.
		tec1Value = !tec1Value;
		// Escribir el valor leido en el LED correspondiente.
		gpioWrite( LEDR, tec1Value );


		/* Si se presiona TEC2, enciende el LED1 */

		// Leer pin conectado a la tecla.
		tec2Value = gpioRead( TEC2 );
		// Invertir el valor leido, pues lee un 0 (OFF) con tecla
		// presionada y 1 (ON) al liberarla.
		tec2Value = !tec2Value;
		// Escribir el valor leido en el LED correspondiente.
		gpioWrite( LED1, tec2Value );


		/* Si se presiona TEC3, enciende el LED2 */

		// Leer pin conectado a la tecla.
		tec3Value = gpioRead( TEC3 );
		// Invertir el valor leido, pues lee un 0 (OFF) con tecla
		// presionada y 1 (ON) al liberarla.
		tec3Value = !tec3Value;
		// Escribir el valor leido en el LED correspondiente.
		gpioWrite( LED2, tec3Value );


		/* Si se presiona TEC4, enciende el LED3 */

		// Leer pin conectado a la tecla.
		tec4Value = gpioRead( TEC4 );
		// Invertir el valor leido, pues lee un 0 (OFF) con tecla
		// presionada y 1 (ON) al liberarla.
		tec4Value = !tec4Value;
		// Escribir el valor leido en el LED correspondiente.
		gpioWrite( LED3, tec4Value );

		//delay(100);


	}

	// NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa se ejecuta
	// directamenteno sobre un microcontroladore y no es llamado por ningun
	// Sistema Operativo, como en el caso de un programa para PC.
	return 0;
}



/*==================[definiciones de funciones internas]=====================*/

void fsmBlinky(void){

	switch(state){

	case LED_ON:
		/* Si el delay no está corriendo, lo configuro y cambio el estado del led*/
		if (!blinkyDelay.running){
			delayConfig(&blinkyDelay, DELAY_DURATION);
			/* para que el delay arranque hay que leerlo*/
			delayRead(&blinkyDelay);
			gpioToggle( LEDB );
		}
		/* Si el tiempo del delay expiró, cambio el estado de la FSM*/
		if (delayRead(&blinkyDelay))
			state = LED_OFF;
		break;

	case LED_OFF:
		/* Si el delay no está corriendo, lo configuro y cambio el estado del led*/
		if (!blinkyDelay.running){
			delayConfig(&blinkyDelay, DELAY_DURATION);
			/* para que el delay arranque hay que leerlo*/
			delayRead(&blinkyDelay);
			gpioToggle( LEDB );
		}
		/* Si el tiempo del delay expiró, cambio el estado de la FSM*/
		if (delayRead(&blinkyDelay))
			state = LED_ON;
		break;

	/* Condición de error, reinicio la máquina de estados*/
	default:
		state = LED_ON;
		break;


	}

	return;
}
/*==================[definiciones de funciones externas]=====================*/

/*==================[fin del archivo]========================================*/
