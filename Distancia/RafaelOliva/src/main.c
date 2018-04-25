/*============================================================================
 * Autor:R. Oliva
 * Proyecto: Proy6 - TP5 CESE ProguP 18.4.2018
 * Licencia:
 * Fecha: 18.4.18 
 *===========================================================================*/

/* 
 * Practice only.. 
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/* v6_3 Cambios:
 * - LEDTask subimos de 500 a 100ms frecuencia, y agregamos un if() para ver si
 * el estado es LED_FORZADOENCENDIDO, con lo cual fuerza LED_State=ON.
 * - En los cambios de Estado de INTRUSO_Update() llamamos aLEDReset() - llevamos
 * RETINTRUSO A 100
 *  * v_6_3 Printout(argumento) Cambiamos tipo de argumento de char *s a const char *Msg
 * siguiendo tipo de strncpy()
 */

/*==================[inlcusiones]============================================*/

#include "main.h"

#include "cooperativeOs_isr.h"       // <= dispatcher and task management header
#include "cooperativeOs_scheduler.h" // <= scheduler and system initialization header
#include "sapi.h"
#include <string.h>
#include "SerieRxTx.h"
#include "Intruso.h"
#include "Keypad.h"

/*==================[macros and definitions]=================================*/
DEBUG_PRINT_ENABLE

//v 6_3 cambia de 500 a 100
#define TPARPADEO1 100
//#define TPARPADEO2 250
//#define TPARPADEO3 100
#define TTECLA1    20
#define TTECLA2    20
#define TTECLA3    20
#define TTECLA4    20
#define RETARTEC1  1
#define RETARTEC2  2
#define RETARTEC3  3
#define RETARTEC4  4
#define TSERIAL    5
#define RETSERIAL  5
#define TINTRUSO   50
#define RETINTRUSO 100



// 3 pasadas de 20 ms..TEC1_DMAX
#define TEC1_DMAX  3
// Se usa para las otras teclas este..
#define TEC2_DMAX  3

/*
typedef enum {LED_NORMAL, LED_FORZADOENCENDIDO} estadoBlinky_t;
/* Para las Teclas 27.3.2018 *
typedef enum{ NOPRESIONADO, DEBOUNCE, PRESIONADO} estadoMEF_t;
typedef enum{ PERIODO1,PERIODO2,PERIODO3} estadoCambPer_t;
typedef enum{ sLED1,sLED2,sLED3,sLEDR,sLEDB,sLEDG} estadoLED_t;
*/

/* Probamos con 70 ... */
#define DELAY_TEC 70

#define cPERIOD1  50
#define cPERIOD2  100
#define cPERIOD3  1000
#define cOFF      0
#define cON       1

//#define TCONSOLA   500

//#define RETCONSOLA 10


#define S_BUFFER_LEN 150


/*==================[definiciones de datos internos]=========================*/

static bool_t ledState = OFF;
bool_t ePeriodoActualizado = OFF;
estadoBlinky_t estadoLED = LED_NORMAL;
estadoCambPer_t estadoCambPer = PERIODO1;
estadoLED_t estadoLEDSel = sLEDR;

int TEC1DelayCtr = 0;
int TEC2DelayCtr = 0;
int TEC3DelayCtr = 0;
int TEC4DelayCtr = 0;

estadoMEF_t estadoTEC1_Puerta = NOPRESIONADO;
estadoMEF_t estadoTEC2_Ventana1 = NOPRESIONADO;
estadoMEF_t estadoTEC3_Ventana2 = NOPRESIONADO;
estadoMEF_t estadoTEC4_Ventana3 = NOPRESIONADO;

bool_t Puerta_Abierta  = OFF;
bool_t Sensor_Ventana1 = OFF;
bool_t Sensor_Ventana2 = OFF;
bool_t Sensor_Ventana3 = OFF;

tick_t vPeriodSelect = cPERIOD1;
gpioMap_t vLEDSelect = LEDR;


static uint8_t menu[] =
		"\n\r"
		"***************** MENU TP5 Alarma R.Oliva**********************\n\r"
		"\n\r"
		"TEC1: Abre Puerta.\n\r"
		"TEC2: Ventana 1.\n\r"
		"TEC3: Ventana 2.\n\r"
		"TEC4: Ventana 3.\n\r"
		"P: ingresar el Password desde el puerto serie.\n\r"
		;

char s[S_BUFFER_LEN];


/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

static void taskBlinkLed(void);
static void debounceTEC1Task(void);
static void debounceTEC2Task(void);
static void debounceTEC3Task(void);
static void debounceTEC4Task(void);
static void taskMenuUpdate(void);

gpioMap_t actualizar_LEDactivo(void);
tick_t    actualizar_Periodo(void);

/* -- Funciones auxiliares --    */
void aLED(gpioMap_t vLEDSel, bool_t statusLED);
void aLEDreset(void);
void CambiarTareas(void);

// Auxiliar de impresión - eliminado
// void PrintOut(const char *Msg);



/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/

int main( void ){
	//Inicializar el hardware, perifericos, puertos, clock, etc.
		boardConfig();
		char test1[] ="UART_USB configurada.     \n\r";
		char test2[] ="Alarma: Ingrese Password. \n\r";

		KEYPAD_Init();
		INTRUSO_Init();
		// Inicializar UART_USB como salida de consola
		SerieRxTxBufInit();
		
		debugPrintConfigUart( UART_USB, 115200 );
		//debugPrintlnString(menu);
		//debugPrintlnString( "UART_USB configurada.\n\r" );

		// Desde aquí comenzamos el uso de SeriRxTxWrite..
		strncpy (s, test1, strlen(test1));
		// SerieRxTx_Write_Char_To_Buffer('2');
		SerieRxTx_Write_String_To_Buffer(s);





		//FUNCION que inicializa el planificador de tareas
		schedulerInit();
		//Cargar las tareas del sistema operativo con sus periodicidades
		schedulerAddTask( (sAPI_FuncPtr_t) taskBlinkLed,       0,         TPARPADEO1 );
		schedulerAddTask( (sAPI_FuncPtr_t) debounceTEC1Task,   RETARTEC1, TTECLA1);
		schedulerAddTask( (sAPI_FuncPtr_t) debounceTEC2Task,   RETARTEC2, TTECLA2);
		schedulerAddTask( (sAPI_FuncPtr_t) debounceTEC3Task,   RETARTEC3, TTECLA3);		
        schedulerAddTask( (sAPI_FuncPtr_t) debounceTEC4Task,   RETARTEC4, TTECLA4);		
		schedulerAddTask( (sAPI_FuncPtr_t) SerieRxTxBufUpdTask,RETSERIAL, TSERIAL);
		schedulerAddTask( (sAPI_FuncPtr_t) KEYPAD_Update,      RETARTEC2, TTECLA2);
		schedulerAddTask( (sAPI_FuncPtr_t) INTRUSO_Update,     RETINTRUSO,TINTRUSO );

		//Iniciar el planificador de tareas
		schedulerStart( 1 );

		// Menu_Display();
		strncpy (s, test2, strlen(test2));
		SerieRxTx_Write_String_To_Buffer(s);
		// Lazo infinito


		while(TRUE) {
			//Ejecutar tareas
			schedulerDispatchTasks();
			// if (ePeriodoActualizado) CambiarTareas();
		}
		return 0;
}

/*==================[definiciones de funciones internas]=====================*/
/*
 * v6_3 - Agrego consulta LED forzado, deja Encendido el led que corresponda..
 */
void taskBlinkLed(void){

  if(estadoLED == LED_NORMAL){   // Blink en LED normal, sino Fijo..
     if (ledState == ON){
      ledState = OFF; // Apago el pin
     }
     else{
      ledState = ON; // Prendo el pin
     }
   }
  else if (estadoLED == LED_FORZADOENCENDIDO){
	  ledState = ON; // Prendo el pin
  }
   // Use of digitalWrite
   aLED( vLEDSelect, ledState );
}

// Tasks que implementan Maquinas de Estado Finito TEC1 a TEC4
// estadoMEF_t estadoTEC1_Puerta = NOPRESIONADO;
// estadoMEF_t estadoTEC2_Ventana1 = NOPRESIONADO;
// estadoMEF_t estadoTEC3_Ventana2 = NOPRESIONADO;
// estadoMEF_t estadoTEC4_Ventana3 = NOPRESIONADO;

// Task Tecla 1 Puerta 18.4.2018
void debounceTEC1Task(void){
	bool_t eTEC1_Presionado = !gpioRead(TEC1);

    switch (estadoTEC1_Puerta) {
          case NOPRESIONADO:
            if(eTEC1_Presionado){
            	estadoTEC1_Puerta = DEBOUNCE;
            }
            break;
          case DEBOUNCE:
      		/* Si el delay está corriendo, lo dejo */
      		if (TEC1DelayCtr > 0 ){
      			; // Nichts
      		}
      			else { /*  setearlo al maximo */
      			TEC1DelayCtr = TEC1_DMAX;
      		}
      		/* Si el tiempo del delay expiró, cambio el estado de la FSM*/
      		if ( --TEC1DelayCtr == 0){
                if(eTEC1_Presionado){
                	estadoTEC1_Puerta = PRESIONADO;
                } else {
                	estadoTEC1_Puerta = NOPRESIONADO;
                }
      		}
      		break;
          case PRESIONADO:
		    Puerta_Abierta = ON;
			// gpioWrite( LED1,1);  // Signal with led1..
        	// aLEDreset();
        	// vPeriodSelect = actualizar_Periodo();
        	// ePeriodoActualizado = ON;
			
            if(!eTEC1_Presionado){
            	estadoTEC1_Puerta = NOPRESIONADO;
               }
            break;
          default:
        	 estadoTEC1_Puerta = NOPRESIONADO;
            break;
          } // end switch
}


// estadoMEF_t estadoTEC2_Ventana1 = NOPRESIONADO;
// Task Tecla 2 - Ventana1
void debounceTEC2Task(void){
	bool_t eTEC2_Presionado = !gpioRead(TEC2);

    switch (estadoTEC2_Ventana1) {
          case NOPRESIONADO:
		    gpioWrite( LED1,0);  // Turn off Signal with led1..
            if(eTEC2_Presionado){
            	estadoTEC2_Ventana1 = DEBOUNCE;
            }
            break;
          case DEBOUNCE:
      		/* Si el delay está corriendo, lo dejo */
      		if (TEC2DelayCtr > 0 ){
      			; // Nichts
      		}
      			else { /*  setearlo al maximo */
      			TEC2DelayCtr = TEC2_DMAX;
      		}
      		/* Si el tiempo del delay expiró, cambio el estado de la FSM*/
      		if ( --TEC2DelayCtr == 0){
                if(eTEC2_Presionado){
                	estadoTEC2_Ventana1 = PRESIONADO;
                } else {
                	estadoTEC2_Ventana1 = NOPRESIONADO;
                }
      		}
      		break;
          case PRESIONADO:
  		    Sensor_Ventana1 = ON;
			gpioWrite( LED1,1);  // Signal with led1..
         	// aLEDreset();
        	// vLEDSelect = actualizar_LEDactivo();
            if(!eTEC2_Presionado){
            	estadoTEC2_Ventana1 = NOPRESIONADO;
               }
            break;
          default:
        	 estadoTEC2_Ventana1 = NOPRESIONADO;
            break;
          } // end switch
}

// estadoMEF_t estadoTEC3_Ventana2 = NOPRESIONADO;
// Task Tecla 3 - Ventana2
void debounceTEC3Task(void){
	bool_t eTEC3_Presionado = !gpioRead(TEC3);

    switch (estadoTEC3_Ventana2) {
          case NOPRESIONADO:
		    gpioWrite( LED2,0);  // Turn off Signal with led1..
            if(eTEC3_Presionado){
            	estadoTEC3_Ventana2 = DEBOUNCE;
            }
            break;
          case DEBOUNCE:
      		/* Si el delay está corriendo, lo dejo */
      		if (TEC3DelayCtr > 0 ){
      			; // Nichts
      		}
      			else { /*  setearlo al maximo */
      			TEC3DelayCtr = TEC2_DMAX;
      		}
      		/* Si el tiempo del delay expiró, cambio el estado de la FSM*/
      		if ( --TEC3DelayCtr == 0){
                if(eTEC3_Presionado){
                	estadoTEC3_Ventana2 = PRESIONADO;
                } else {
                	estadoTEC3_Ventana2 = NOPRESIONADO;
                }
      		}
      		break;
          case PRESIONADO:
  		    Sensor_Ventana2 = ON;
			gpioWrite( LED2,1);  // Signal with led1..
         	// aLEDreset();
        	// vLEDSelect = actualizar_LEDactivo();
            if(!eTEC3_Presionado){
            	estadoTEC3_Ventana2 = NOPRESIONADO;
               }
            break;
          default:
        	 estadoTEC3_Ventana2 = NOPRESIONADO;
            break;
          } // end switch
}

// estadoMEF_t estadoTEC4_Ventana3 = NOPRESIONADO;
// Task Tecla 4 - Ventana3
void debounceTEC4Task(void){
	bool_t eTEC4_Presionado = !gpioRead(TEC4);

    switch (estadoTEC4_Ventana3) {
          case NOPRESIONADO:
		    gpioWrite( LED3,0);  // Turn off Signal with led1..
            if(eTEC4_Presionado){
            	estadoTEC4_Ventana3 = DEBOUNCE;
            }
            break;
          case DEBOUNCE:
      		/* Si el delay está corriendo, lo dejo */
      		if (TEC4DelayCtr > 0 ){
      			; // Nichts
      		}
      			else { /*  setearlo al maximo */
      			TEC4DelayCtr = TEC2_DMAX;
      		}
      		/* Si el tiempo del delay expiró, cambio el estado de la FSM*/
      		if ( --TEC4DelayCtr == 0){
                if(eTEC4_Presionado){
                	estadoTEC4_Ventana3 = PRESIONADO;
                } else {
                	estadoTEC4_Ventana3 = NOPRESIONADO;
                }
      		}
      		break;
          case PRESIONADO:
  		    Sensor_Ventana3 = ON;
			gpioWrite( LED3,1);  // Signal with led1..
         	// aLEDreset();
        	// vLEDSelect = actualizar_LEDactivo();
            if(!eTEC4_Presionado){
            	estadoTEC4_Ventana3 = NOPRESIONADO;
               }
            break;
          default:
        	 estadoTEC4_Ventana3 = NOPRESIONADO;
            break;
          } // end switch
}


/* Serie update movido a modulo externo */ 
/*
 * v6 esta rutina no va mas..
 */

static void taskMenuUpdate(void){

	uint8_t receiveByte;
	char StrMsg1[] = "T recibido \n\r";
	char StrMsg2[] = "L recibido \n\r";
	char StrMsg3[] = "Noconocido \n\r";
    char s1[50];

    strncpy (s1, StrMsg1, strlen(StrMsg1));

	//if (uartReadByte( UART_USB, &receiveByte ) != FALSE) {
    if(SerieRxTx_Get_Char_From_Buffer(&receiveByte) != 0){
		if(receiveByte != 0){
			switch (receiveByte) {
			case 't':
			case 'T':
				/*//uartWriteString(UART_USB, menu);*/
				/*//debugPrintlnString( "Caracter recibido: T\n\r");*/
				SerieRxTx_Write_String_To_Buffer(s1);
	        	aLEDreset();
	        	vPeriodSelect = actualizar_Periodo();
	        	ePeriodoActualizado = ON;
				break;

			case 'L':
				/*//uartWriteString(UART_USB, menu);
				//uartWriteString(UART_USB, "Caracter recibido: L\n\r");*/
				strncpy (s1, StrMsg2, strlen(StrMsg2));
				SerieRxTx_Write_String_To_Buffer(s1);
				aLEDreset();
				vLEDSelect = actualizar_LEDactivo();
				break;

			case '\n':
			case '\r':
				break;

			default:
				strncpy (s1, StrMsg3, strlen(StrMsg3));  // Comando no reconocido..
				SerieRxTx_Write_String_To_Buffer(s1);
				// uartWriteString(UART_USB, menu);
				// uartWriteString(UART_USB, "Caracter recibido fuera de menu\n\r");
				SerieRxTxBufInit();
				break;
			}
	}
  }
}

/* Funciones serie movidas a modulo externo 17.4.18 */


/* Funciones de acción */
void aLED(gpioMap_t vLEDSel, bool_t statusLED)
{
       gpioWrite(vLEDSel, statusLED);
}

tick_t actualizar_Periodo(void)
{
	tick_t vPer_retorno;
	// typedef enum{ PERIODO1,PERIODO2,PERIODO3} estadoCambPer;


	switch(estadoCambPer){
	case PERIODO1:
		vPer_retorno = cPERIOD1;
		break;
	case PERIODO2:
		vPer_retorno = cPERIOD2;
		break;
	case PERIODO3:
		vPer_retorno = cPERIOD3;
		break;
	default:
		estadoCambPer = PERIODO1;
		vPer_retorno = cPERIOD1;
	} // fin del switch..
	if(++estadoCambPer > PERIODO3) estadoCambPer = PERIODO1;
	return(vPer_retorno);
}

gpioMap_t  actualizar_LEDactivo(void)
{
	gpioMap_t  vPin_activo;
	// typedef enum{ sLED1,sLED2,sLED3,sLEDR,sLEDB,sLEDG} estadoLED_t;


	switch(estadoLEDSel){
	case sLED1:
		vPin_activo = LED1;
		break;
	case sLED2:
		vPin_activo = LED2;
		break;
	case sLED3:
		vPin_activo = LED3;
		break;
	case sLEDR:
		vPin_activo = LEDR;
		break;
	case sLEDB:
		vPin_activo = LEDB;
		break;
	case sLEDG:
		vPin_activo = LEDG;
		break;
	default:
		estadoLEDSel = sLED1;
		vPin_activo = LED1;
	} // fin del switch..
	if(++estadoLEDSel>sLEDG)estadoLEDSel=sLED1;
	return(vPin_activo);
}

/* Limpiar todos los LEDs ante un cambio en cualquier parametro */

void aLEDreset(void){
    gpioWrite( LEDB,0);
    gpioWrite( LEDR,0);
    gpioWrite( LEDG,0);
    gpioWrite( LED1,0);
    gpioWrite( LED2,0);
    gpioWrite( LED3,0);
}

/* Funcion reseteadora - 16.04.2018 v5C aquí faltaba llamar a las tareas
 *
 * nuevas del Proy 5B - Menu (taskMenuUpdate) y Serial(SerieRxTxBufUpdTask) */
#ifdef CAMBIAR_TAREAS
void CambiarTareas(void) {

	switch(vPeriodSelect)
	{
	case cPERIOD1:
		schedulerInit();
		//Cargar tareas - LED con TPERIODO1
		schedulerAddTask( (sAPI_FuncPtr_t) taskBlinkLed,     0,         TPARPADEO1 );
		schedulerAddTask( (sAPI_FuncPtr_t) debounceTEC1Task, RETARTEC1, TTECLA1);
		schedulerAddTask( (sAPI_FuncPtr_t) debounceTEC2Task, RETARTEC2, TTECLA2);
		schedulerAddTask( (sAPI_FuncPtr_t) SerieRxTxBufUpdTask,RETSERIAL, TSERIAL);
		schedulerAddTask( (sAPI_FuncPtr_t) taskMenuUpdate,     RETCONSOLA,TCONSOLA );
		//reIniciar el planificador de tareas
		schedulerStart( 1 );
		break;
	case cPERIOD2:
		schedulerInit();
		//Cargar tareas - LED con TPERIODO2
		schedulerAddTask( (sAPI_FuncPtr_t) taskBlinkLed,     0,         TPARPADEO2 );
		schedulerAddTask( (sAPI_FuncPtr_t) debounceTEC1Task, RETARTEC1, TTECLA1);
		schedulerAddTask( (sAPI_FuncPtr_t) debounceTEC2Task, RETARTEC2, TTECLA2);
		schedulerAddTask( (sAPI_FuncPtr_t) SerieRxTxBufUpdTask,RETSERIAL, TSERIAL);
		schedulerAddTask( (sAPI_FuncPtr_t) taskMenuUpdate,     RETCONSOLA,TCONSOLA );
		//reIniciar el planificador de tareas
		schedulerStart( 1 );
		break;
	case cPERIOD3:
		schedulerInit();
		//Cargar tareas - LED con TPERIODO2
		schedulerAddTask( (sAPI_FuncPtr_t) taskBlinkLed,     0,         TPARPADEO3 );
		schedulerAddTask( (sAPI_FuncPtr_t) debounceTEC1Task, RETARTEC1, TTECLA1);
		schedulerAddTask( (sAPI_FuncPtr_t) debounceTEC2Task, RETARTEC2, TTECLA2);
		schedulerAddTask( (sAPI_FuncPtr_t) SerieRxTxBufUpdTask,RETSERIAL, TSERIAL);
		schedulerAddTask( (sAPI_FuncPtr_t) taskMenuUpdate,     RETCONSOLA,TCONSOLA );
		//reIniciar el planificador de tareas
		schedulerStart( 1 );
		break;
	default:
		break; // nada bueno..
	}
	// Resetear el Flag
	ePeriodoActualizado = OFF;
}
#endif

/*
 * v_6_3
 * Eliminado
 *  *
 */
/*
void PrintOut(const char *Msg){
	 char s1[200];
	 //SerieRxTxBufInit();
     strncpy (s1, Msg, strlen(Msg));
     SerieRxTx_Write_String_To_Buffer(s1);
}
*/

/*==================[definiciones de funciones externas]=====================*/

/*==================[fin del archivo]========================================*/

