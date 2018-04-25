/*============================================================================
 * Autor: Marcelo Pistarelli
 * Licencia:
 * Fecha:
 *===========================================================================*/

/*==================[inclusiones]============================================*/

#include "debounce.h"
#include "sapi.h"        // <= Biblioteca sAPI

/*==================[definiciones y macros]==================================*/



/*==================[definiciones de datos internos]=========================*/

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

void actualizarDebounceMEF(debounceIN_t * debounce)
{

	bool_t tecValue = gpioRead(debounce->IN); /// Lectura del botón sobre el que se aplica el antirebote

	switch (debounce->state)
	{

	case BUTTON_UP:
		if (debounce->button_up_callBack != NULL)
		{	// llamo a función button_up_callBack si no es null
			debounce->button_up_callBack();
		}
		debounce->state = BUTTON_UP_DEBOUNCED;
		break;
	case BUTTON_UP_DEBOUNCED:
		if (tecValue == debounce->ActiveHI)
		{ // si se detecta el botón presionado se inicia el contador de ticks y setea estado en BUTTON_FALLING
			delayConfig(&debounce->debounceDelay, debounce->period_MS);
			debounce->state = BUTTON_FALLING;
		}
		break;
	case BUTTON_FALLING:
		if (delayRead(&debounce->debounceDelay))
		{ // si se cumplió el tiempo de contador de ticks leo botón y actualizao el estado
			if (tecValue == debounce->ActiveHI)
			{
				debounce->state = BUTTON_DOWN;
			}
			else
			{
				debounce->state = BUTTON_UP;
			}
		}
		break;
	case BUTTON_DOWN:
		if (debounce->button_down_callBack != NULL)
		{	// llamo a función button_down_callBack si no es null
			debounce->button_down_callBack();
		}
		debounce->state = BUTTON_DOWN_DEBOUNCED;

		break;
	case BUTTON_DOWN_DEBOUNCED:
		if (tecValue == !debounce->ActiveHI)
		{ // si se suelta el botón inicio el contador de ticks y seteo estado en BUTTON_RAISING
			delayConfig(&debounce->debounceDelay, debounce->period_MS);
			debounce->state = BUTTON_RAISING;
		}
		break;
	case BUTTON_RAISING:
		if (delayRead(&debounce->debounceDelay))
		{ // si se cumplió el tiempo de contador de ticks leo botón y actualizao el estado
			if (tecValue == debounce->ActiveHI)
			{
				debounce->state = BUTTON_DOWN;
			}
			else
			{
				debounce->state = BUTTON_UP;
			}
		}
		break;
	default:
		inicializarDebounceMEF(debounce);
		break;
	}
}

void inicializarDebounceMEF(debounceIN_t * debounce)
{
	debounce->state = BUTTON_UP;
}

