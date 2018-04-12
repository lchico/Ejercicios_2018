/*============================================================================
 * Autor:   Rodrigo Tirapegui
 * Licencia:
 * Fecha:   22/3/2018
 *===========================================================================*/

/*==================[inlcusiones]============================================*/

#include "conf_leds.h"
#include "leds.h"
#include "fsm.h"

/*==================[definiciones y macros]==================================*/

/* Eventos de la maquina de estados finita del led que parpadea */
typedef enum {
   LED_BLINK_PERIOD_TIMEOUT = 1
} ledBlinkEvents_t;

/* Informaci�n de control del led que parpadea */
typedef struct {
   uint32_t id;
   uint32_t periodMs;
   bool_t isBlinking;
   uint32_t delay;
   fsmState_t* state;
} ledBlinkControl_t;

/* Mapeo de los GPIO's de los leds que contiene el BOARD usado */
static const uint32_t ledsGpioBoardMap[] = LEDS_BOARD_GPIO_MAP;

/* Cantidad de leds que contiene el BOARD usado */
#define LEDS_MAX  sizeof(ledsGpioBoardMap)/sizeof(ledsGpioBoardMap[0])

/* Etiquetas para encender/apagar un led */
#define LED_TURN_OFF   0
#define LED_TURN_ON    !LED_TURN_OFF

/*==================[declaracion de datos internos]=========================*/
static const fsmState_t ledBlinkOnWaitPeriodTimeout[];
static const fsmState_t ledBlinkOffWaitPeriodTimeout[];

/*==================[declaraciones de funciones internas]====================*/
static void setLedOnAndBlinkPeriodDelay(const uint32_t ledId);
static void setLedOffAndBlinkPeriodDelay(const uint32_t ledId);

/*==================[declaraciones de funciones externas]====================*/

/*==================[definiciones de datos internos]=========================*/

/* Tabla de la m�quina de estados finita */

DEFINE_FSM_STATE(ledBlinkOnWaitPeriodTimeout,
                 {LED_BLINK_PERIOD_TIMEOUT, ledBlinkOffWaitPeriodTimeout, setLedOffAndBlinkPeriodDelay})

DEFINE_FSM_STATE(ledBlinkOffWaitPeriodTimeout,
                 {LED_BLINK_PERIOD_TIMEOUT, ledBlinkOnWaitPeriodTimeout, setLedOnAndBlinkPeriodDelay})

/* Variables de control */
static bool_t gInitFlag;
static ledBlinkControl_t gLeds[LEDS_MAX];

/*==================[definiciones de datos externos]=========================*/

/*==================[definiciones de funciones internas]=====================*/

static ledBlinkControl_t* getBlinkyLed(const uint32_t ledId) {
   uint8_t i;

   for(i = 0;i < LEDS_MAX;i++) {
      if(ledId == gLeds[i].id)
         return &gLeds[i];
   }

   return NULL;
}

static bool_t eventGenerator(ledBlinkControl_t* ledBlinky, uint8_t* event) {
   if(ledBlinky && event) {

      // Evaluar si transcurri� el periodo del led que parpadea
      if(ledBlinky->delay && (0 == --ledBlinky->delay)) {
         *event = LED_BLINK_PERIOD_TIMEOUT;

         return TRUE;
      }
   }

   return FALSE;
}

static void setBlinkPeriodDelay(ledBlinkControl_t* ledBlinky) {

   if(ledBlinky) {

      // Configurar delay
      ledBlinky->delay = ledBlinky->periodMs/BLINK_LED_TASK_TIME_PERIOD;
   }
}

/* Funciones de acci�n de la m�quina de estados finita */

static void setLedOnAndBlinkPeriodDelay(const uint32_t ledId) {

   ledBlinkControl_t* ledBlinky = getBlinkyLed(ledId);

   if(ledBlinky) {

      // Encender led
      gpioWrite(ledBlinky->id, LED_TURN_ON);

      setBlinkPeriodDelay(ledBlinky);
   }
}

static void setLedOffAndBlinkPeriodDelay(const uint32_t ledId) {

   ledBlinkControl_t* ledBlinky = getBlinkyLed(ledId);

   if(ledBlinky) {

      // Apagar led
      gpioWrite(ledBlinky->id, LED_TURN_OFF);

      setBlinkPeriodDelay(ledBlinky);
   }
}

/*==================[definiciones de funciones externas]=====================*/

void ledsInit(void) {
   uint8_t i;

   // Inicializar y configurar el arreglo de control de los leds
   for(i = 0;i < LEDS_MAX;i++) {

     // Asignar como id del led el gpio correpondiente
     gLeds[i].id = ledsGpioBoardMap[i];

     // Deshabilitar el parpadeo del led
     gLeds[i].isBlinking = FALSE;
   }

   gInitFlag = TRUE;
}

void ledsStartBlinking(const uint32_t ledId) {
   if(gInitFlag) {
      ledBlinkControl_t* ledBlinky = getBlinkyLed(ledId);

      if(ledBlinky) {
         uint8_t i;

         // Detener el led que esta parpadeando actualmente
         for(i = 0;i < LEDS_MAX;i++) {
            if(gLeds[i].isBlinking) {
               // Apagar led
               gpioWrite(gLeds[i].id, LED_TURN_OFF);

               // Deshabilitar el parpadeo del led
               gLeds[i].isBlinking = FALSE;

               break;
            }
         }

         // Asignar un estado a la m�quina de estados finita
         ledBlinky->state = (fsmState_t* ) ledBlinkOnWaitPeriodTimeout;

         // Encender led
         gpioWrite(ledBlinky->id, LED_TURN_ON);

         // Habilitar el parpadeo del led
         ledBlinky->isBlinking = TRUE;

         // Configurar el periodo de parpadeo del led
         setBlinkPeriodDelay(ledBlinky);
      }
   }
}

void ledsSetBlinkPeriod(const uint32_t ledId, const uint32_t periodMs) {
   if(gInitFlag) {
      ledBlinkControl_t* ledBlinky = getBlinkyLed(ledId);

      if(ledBlinky) {

         // Asignar el periodo de parpadeo del led
         ledBlinky->periodMs = periodMs;
      }
   }
}

void ledsTurnOn(const uint32_t ledId) {
   if(gInitFlag) {
      ledBlinkControl_t* ledBlinky = getBlinkyLed(ledId);

      if(ledBlinky) {

         // Encender el led
         gpioWrite(ledBlinky->id, LED_TURN_ON);
      }
   }
}

void ledsTurnOff(const uint32_t ledId) {
   if(gInitFlag) {
      ledBlinkControl_t* ledBlinky = getBlinkyLed(ledId);

      if(ledBlinky) {

         // Apagar el led
         gpioWrite(ledBlinky->id, LED_TURN_OFF);
      }
   }
}

void blinkLedTask(void) {
   if(gInitFlag) {
      uint8_t i, event;

      // Actualizar el estado de la m�quina de estados finita de cada led
      for(i = 0;i < LEDS_MAX;i++) {
         // Evaluar si el led actual est� parpadeando
         if(gLeds[i].isBlinking) {
            // Obtener el evento actual
            if(eventGenerator(&gLeds[i], &event))
               fsm(&gLeds[i].state, event, gLeds[i].id);

            // Solamente un led parpadea por vez
            break;
         }
      }
   }
}

/*==================[fin del archivo]========================================*/

