/*============================================================================
 * Autor:	Rodrigo Tirapegui
 * Licencia:
 * Fecha:	22/3/2018
 *===========================================================================*/

/*==================[inlcusiones]============================================*/

#include "buttons.h"
#include "fsm.h"

/*==================[definiciones y macros]==================================*/

/* Informaci�n de control de un bot�n */
typedef struct {
   uint32_t id;
   buttonStatus_t status;
   int32_t delay;
   fsmState_t* state;
} button_t;

/* Mapeo de los GPIO's de los botones que contiene el BOARD usado */
static const uint32_t buttonsGpioBoardMap[] = BUTTONS_BOARD_GPIO_MAP;

/* Macro que permite definir la tarea que avanza la maquina de estados
 * de un determinado boton */
#define DEFINE_BUTTON_TASK(n, unused)                                 \
                                                                      \
void debounceTec##n##Task(void) {                                     \
                                                                      \
   if(gInitFlag) {                                                    \
      uint8_t event;                                                  \
                                                                      \
      if(eventGenerator(&gButtons[n], &event))                        \
         fsm(&gButtons[n].state, event, gButtons[n].id);              \
   }                                                                  \
}

/* Sensibilidad antirebote de boton */
#define TRANSITION_WAIT_INTERVAL_MS             40 // In ms. Debe ser mayor o igual a DEBOUNCE_TECn_TASK_TIME_PERIOD

/*==================[declaracion de datos internos]=========================*/
static const fsmState_t buttonRelased[];
static const fsmState_t buttonTransitionPressing[];
static const fsmState_t buttonPressed[];
static const fsmState_t buttonTransitionReleasing[];

/*==================[declaraciones de funciones internas]====================*/
static void doNothing(const uint32_t buttonId);
static void setButtonDelay(const uint32_t buttonId);
static void setButtonStatusAsPressed(const uint32_t buttonId);
static void setButtonStatusAsReleased(const uint32_t buttonId);

/*==================[declaraciones de funciones externas]====================*/

/*==================[definiciones de datos internos]=========================*/

/* Tabla de la m�quina de estados finita */

DEFINE_FSM_STATE(buttonRelased,
                 {BUTTON_DOWN, buttonTransitionPressing, setButtonDelay})

DEFINE_FSM_STATE(buttonTransitionPressing,
                 {BUTTON_UP, buttonRelased, doNothing}
                 ,{BUTTON_DOWN, buttonPressed, setButtonStatusAsPressed})

DEFINE_FSM_STATE(buttonPressed,
                 {BUTTON_UP, buttonTransitionReleasing, setButtonDelay})

DEFINE_FSM_STATE(buttonTransitionReleasing,
                 {BUTTON_UP, buttonRelased, setButtonStatusAsReleased}
                 ,{BUTTON_DOWN, buttonPressed, doNothing})

/* Variables de control */
static bool_t gInitFlag;
static button_t gButtons[BUTTONS_MAX];

/*==================[definiciones de datos externos]=========================*/

/*==================[definiciones de funciones internas]=====================*/

static button_t* getButton(const uint32_t buttonId) {

	uint8_t i;

	for(i = 0;i < BUTTONS_MAX;i++) {
	   if(buttonId == gButtons[i].id)
		   return &gButtons[i];
	}

	return NULL;
}

static bool_t eventGenerator(button_t* button, uint8_t* event) {

   if(button && event) {
		// Leer el nivel del bot�n y generar el evento correspondiente
		if(!button->delay || (button->delay && (0 == --button->delay))) {
		   *event = (0 == gpioRead(button->id) ? BUTTON_DOWN : BUTTON_UP);

		   return TRUE;
		}
	}

	return FALSE;
}

/* Funciones de acci�n de la m�quina de estados finita */

static void doNothing(const uint32_t buttonId) {
	(void) buttonId;
}

static void setButtonDelay(const uint32_t buttonId) {

   button_t* button = getButton(buttonId);

   if(button) {

      // Configurar delay
      button->delay = TRANSITION_WAIT_INTERVAL_MS/DEBOUNCE_TECn_TASK_TIME_PERIOD;
   }
}

static void setButtonStatusAsPressed(const uint32_t buttonId) {
   button_t* button = getButton(buttonId);

   if(button) {

      // Configurar el bot�n como presionado
	  button->status = BUTTON_DOWN;
   }
}
static void setButtonStatusAsReleased(const uint32_t buttonId) {
   button_t* button = getButton(buttonId);

   if(button) {

     // Configurar el bot�n como liberado
     button->status = BUTTON_UP;
   }
}

/*==================[definiciones de funciones externas]=====================*/

void buttonsInit(void) {
   uint8_t i;

   // Inicializar y configurar el arreglo de control de los botones
   for(i = 0;i < BUTTONS_MAX;i++) {

	   // Asignar como id del bot�n el gpio correpondiente
	   gButtons[i].id = buttonsGpioBoardMap[i];

	   // Determinar el estado inicial del bot�n.
	   // Es necesario invertir el valor leido, pues un 0 (OFF) significa
	   // tecla presionada y un 1 (ON) significa tecla liberada.
      gButtons[i].status = (0 == gpioRead(gButtons[i].id) ? BUTTON_DOWN : BUTTON_UP);

      // Determinar el estado inicial de la maquina de estados en funcion
      // del estado del bot�n (presionado o liberado)
      gButtons[i].state = (fsmState_t* ) (BUTTON_DOWN == gButtons[i].status ? buttonPressed : buttonRelased);

      // Deshabilitar el delay asociado al bot�n
      gButtons[i].delay = 0;
   }

   gInitFlag = TRUE;
}

bool_t buttonsGetStatus(const uint32_t buttonId, buttonStatus_t* status) {
   if(gInitFlag && status) {
      button_t* button = getButton(buttonId);

      if(button) {

         // Devolver el estado actual del bot�n (presionado o liberado)
    	   *status = button->status;

         return TRUE;
      }
   }

   return FALSE;
}

/* Tareas autogeneradas en terminos de la cantidad de botones de la BOARD */
MREPEAT(BUTTONS_MAX, DEFINE_BUTTON_TASK, ~);

/*==================[fin del archivo]========================================*/

