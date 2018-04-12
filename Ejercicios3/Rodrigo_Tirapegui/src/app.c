/*============================================================================
 * Autor:	Rodrigo Tirapegui
 * Licencia:
 * Fecha:	22/3/2018
 *===========================================================================*/

/*==================[inlcusiones]============================================*/

#include "conf_app.h"
#include "conf_leds.h"
#include "app.h"
#include "buttons.h"
#include "leds.h"
#include "cooperativeOs_isr.h"
#include "cooperativeOs_scheduler.h"

/*==================[definiciones y macros]==================================*/

/* Informaci�n de control de la aplicaci�n */
typedef struct {
   uint8_t blinkLedIndex;
   uint8_t blinkPeriodIndex;
   buttonStatus_t statusTEC1;
   buttonStatus_t statusTEC2;
} appControl_t;

/*==================[definiciones de datos internos]=========================*/

/* Mapeo de los LEDs usados por el programa con los GPIOs correspondientes */
static const uint32_t gLedsGpioMap[] = LEDS_BOARD_GPIO_MAP;
#define LEDS_MAX   sizeof(gLedsGpioMap)/sizeof(gLedsGpioMap[0])

/* Listado de periodos de parpadeo entre los que permite iterar el programa */
static const uint32_t gBlinkPeriodsArray[] = APP_BLINK_PERIODS_MS;
#define PERIODS_MAX   sizeof(gBlinkPeriodsArray)/sizeof(gBlinkPeriodsArray[0])

/* Intervalo de ejecucion de la tarea control task */
#define CONTROL_TASK_TIME_PERIOD          40 // In ms

/* Variables de control de la aplicaci�n */
static appControl_t gAppControl;

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[definiciones de funciones internas]=====================*/
static void setBlinkPeriod() {

   uint8_t ledIndex;

   // Configurar el periodo de parpadeo para todos los leds usados
   // en la aplicaci�n
   for(ledIndex = 0;ledIndex < LEDS_MAX;ledIndex++)
      ledsSetBlinkPeriod(gLedsGpioMap[ledIndex],
                         gBlinkPeriodsArray[gAppControl.blinkPeriodIndex]);

   // Incrementar �ndice a periodo de parpadeo de los leds
   gAppControl.blinkPeriodIndex++;

   if(PERIODS_MAX <= gAppControl.blinkPeriodIndex)
      gAppControl.blinkPeriodIndex = 0;
}

static void setBlinkyLed() {

   // Incrementar �ndice a led de parpadeo de los leds
   gAppControl.blinkLedIndex++;

   if(LEDS_MAX <= gAppControl.blinkLedIndex)
      gAppControl.blinkLedIndex = 0;

   // Iniciar el parpadeo del nuevo led
   ledsStartBlinking(gLedsGpioMap[gAppControl.blinkLedIndex]);
}

/**
 * \brief Tarea que implementa la l�gica de control del programa:
 *        - Si se presiona TEC1, se cambia la frecuencia de parpadeo
 *          de los leds entre 50, 100 y 1000 milisegundos
 *        - Si se presiona TEC2, se detiene el parpadeo del led actual
 *          y se incia el parpadeo del siguiente led
 *
 * \param No recibe par�metros
 *
 * \return void No devuelve nada
 */
static void appTask(void) {
   buttonStatus_t currentStatusTEC1, currentStatusTEC2;

   // Obtener el estado de los botones TEC1 y TEC2
   buttonsGetStatus(TEC1, &currentStatusTEC1);
   buttonsGetStatus(TEC2, &currentStatusTEC2);

   // Operar en base al estado de TEC1
   if(gAppControl.statusTEC1 != currentStatusTEC1) {

      if (BUTTON_DOWN == currentStatusTEC1) {
         // Cambiar el periodo de parpadeo de los leds
         setBlinkPeriod();
      }

      // Almacenar el nuevo estado del boton TEC1
      gAppControl.statusTEC1 = currentStatusTEC1;
   }

   // Operar en base al estado de TEC2
   if(gAppControl.statusTEC2 != currentStatusTEC2) {

      if(BUTTON_DOWN == currentStatusTEC2) {
         // Cambiar el led que parpadea
         setBlinkyLed();
      }

      // Almacenar el nuevo estado del boton TEC2
      gAppControl.statusTEC2 = currentStatusTEC2;
   }
}

/*==================[definiciones de funciones externas]=====================*/
bool_t appInit() {

   // ---------- CONFIGURACIONES ------------------------------
   uint8_t i, taskInitialDelay = 0;

   // Inicializar y configurar la plataforma
   boardConfig();

   // Inicializar planificador de tareas
   schedulerInit();

   // Inicializar m�dulo de control de los botones
   buttonsInit();

   // Inicializar m�dulo de control de los leds
   ledsInit();

   // Determinar el estado inicial de los botones
   buttonsGetStatus(TEC1, &gAppControl.statusTEC1);
   buttonsGetStatus(TEC2, &gAppControl.statusTEC2);

   // Configurar el periodo inicial de parpadeo de los leds
   setBlinkPeriod();

   // Inicializar led RGB rojo parpadeando
   setBlinkyLed();

   // Cargar las tareas que componen el scheduler
   if(SCHEDULER_MAX_TASKS <= schedulerAddTask((sAPI_FuncPtr_t) blinkLedTask, taskInitialDelay++, BLINK_LED_TASK_TIME_PERIOD))
      return FALSE;

   const void (*buttonTasksVector[]) = {
      MREPEAT(BUTTONS_MAX, DECLARE_BUTTON_TASK_VECTOR, ~)
   };

   for(i = 0;i < sizeof(buttonTasksVector)/sizeof(buttonTasksVector[0]);i++) {
      if(SCHEDULER_MAX_TASKS <= schedulerAddTask((sAPI_FuncPtr_t) buttonTasksVector[i], taskInitialDelay++, DEBOUNCE_TECn_TASK_TIME_PERIOD))
         return FALSE;
   }

   if(SCHEDULER_MAX_TASKS <= schedulerAddTask((sAPI_FuncPtr_t) appTask, taskInitialDelay++, CONTROL_TASK_TIME_PERIOD))
      return FALSE;

   // Iniciar el planificador de tareas
   schedulerStart(SCHEDULER_TICK);

   return TRUE;
}

void appRun() {

   // ---------- REPETIR POR SIEMPRE --------------------------
   while( TRUE )
   {
      /* Ejecutar tareas que componen el programa */

      schedulerDispatchTasks();
   }
}

/*==================[fin del archivo]========================================*/
