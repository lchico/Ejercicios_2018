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

/*==================[inlcusiones]============================================*/
// #include "Main.H"
// #include "Port.H"

#include "Intruso.h"

#include "Keypad.h"
// #include "PC_O.h"

/*==================[macros and definitions]=================================*/

// ------ Private data type declarations ---------------------------

// Posibles estados del sistema
typedef enum {DISARMED, ARMING, ARMED, DISARMING, INTRUSO} 
             eSystem_state;
#define MAX_STATE_CALL_COUNT 65534
			 
/*==================[declaraciones de funciones internas]====================*/

bool_t  INTRUSO_Get_Password_G(void);
bool_t  INTRUSO_Check_Window_Sensors(void);
bool_t  INTRUSO_Check_Door_Sensor(void);
void INTRUSO_Sound_Alarm(void);

/*==================[definiciones de datos internos]=========================*/

static eSystem_state System_state_G;

uint16_t State_call_count_G;

char Input_G[4] = {'X','X','X','X'};
char Password_G[4] = {'1','2','3','4'};

uint8_t Position_G;

bool_t New_state_G = 0;

bool_t Alarm_bit = 0;
bool_t Sounder_pin = 0;
const char msg_disarmed[] = "\n\rDesarmado            ";
const char msg_arming[]   = "\n\rArming..             ";
const char msg_disarming[]= "\n\rDisarming..          ";
const char msg_armed[]    = "\n\rArmado..             ";
const char msg_intruso[]  = "\n\rINTRUSO!!..          ";
const char msg_ventana1[] = "\n\rVentana 1 dañada!    ";
const char msg_ventana2[] = "\n\rVentana 2 dañada!    ";
const char msg_ventana3[] = "\n\rVentana 3 dañada!    ";
const char msg_puerta[]   = "\n\rPuerta abierta       ";


static char slocal[100];


/*==================[declaraciones de funciones externas]====================*/

/* Funcion publica de inicialización - Tarea INTRUSO */
void INTRUSO_Init(void)
   {
   System_state_G = DISARMED;

   State_call_count_G = 0;
   
   KEYPAD_Clear_Buffer();

   New_state_G = 1;

   //Window_sensor_pin = 1;
   Sounder_pin = 1;
   }


/* Funcion publica de Actualización - Tarea INTRUSO */

void INTRUSO_Update(void)
   {
   // Incremented every time
   if (State_call_count_G < MAX_STATE_CALL_COUNT)
      {
      State_call_count_G++;
      }

   // Call every 50 ms
   switch (System_state_G)
      {
      case DISARMED:
         {
         if (New_state_G)
            {
        	aLEDreset();
            //PrintOut(msg_disarmed);
            strncpy (slocal, msg_disarmed, strlen(msg_disarmed));
            SerieRxTx_Write_String_To_Buffer(slocal);
			vLEDSelect = LEDG;
			estadoLED = LED_FORZADOENCENDIDO;
			Sensor_Ventana1 = OFF;
			Sensor_Ventana2 = OFF;
			Sensor_Ventana3 = OFF;
			Puerta_Abierta  = OFF;
            New_state_G = 0;
            }

         // Make sure alarm is switched off
         Sounder_pin = 1;

         // Wait for correct password ...
         if (INTRUSO_Get_Password_G() == 1)
            {
            System_state_G = ARMING;
            New_state_G = 1;
            State_call_count_G = 0;
            break;
            }

         break;
         }

      case ARMING:
         {
         if (New_state_G)
            {
            //PrintOut("\nArming...");
        	strncpy (slocal, msg_arming, strlen(msg_arming));
        	SerieRxTx_Write_String_To_Buffer(slocal);
            New_state_G = 0;
            aLEDreset();
			vLEDSelect = LEDB;
			estadoLED = LED_NORMAL;
            }

         // Remain here for 60 seconds (50 ms tick assumed)
         if (++State_call_count_G > 1200)
            {
            System_state_G = ARMED;
            New_state_G = 1;
            State_call_count_G = 0;
            break;
            }

         break;
         }

      case ARMED: 
         {
         if (New_state_G)
            {
            //PrintOut("\nArmed");
         	strncpy (slocal, msg_armed, strlen(msg_armed));
         	SerieRxTx_Write_String_To_Buffer(slocal);
            New_state_G = 0;
            aLEDreset();
			vLEDSelect = LEDR;
			estadoLED = LED_FORZADOENCENDIDO;
            }

         // First, check the window sensors
         if (INTRUSO_Check_Window_Sensors() == 1)
            {
            // An INTRUSO detected
            System_state_G = INTRUSO;
            New_state_G = 1;
            State_call_count_G = 0;
            break;
            }

         // Next, check the door sensors
         if (INTRUSO_Check_Door_Sensor() == 1)
            {
            // May be authorised user - go to 'Disarming' state
            System_state_G = DISARMING;
            New_state_G = 1;
            State_call_count_G = 0;
            break;
            }

         // Finally, check for correct password
         if (INTRUSO_Get_Password_G() == 1)
            {
            System_state_G = DISARMED;
            New_state_G = 1;
            State_call_count_G = 0;
            break;
            }

         break;
         }

      case DISARMING:
         {
         if (New_state_G)
            {
            //PrintOut("\nDisarming...");
         	strncpy (slocal, msg_disarming, strlen(msg_disarming));
         	SerieRxTx_Write_String_To_Buffer(slocal);
            New_state_G = 0;
            aLEDreset();
			vLEDSelect = LEDB;
			estadoLED = LED_NORMAL; // blinking azul
            }

         // Remain here for 60 seconds (50 ms tick assumed)
         // to allow user to enter the password
         // - after time up, sound alarm
         if (++State_call_count_G > 1200)
            {
            System_state_G = INTRUSO;
            New_state_G = 1;
            State_call_count_G = 0;
            break;
            }

         // Still need to check the window sensors
         if (INTRUSO_Check_Window_Sensors() == 1)
            {
            // An INTRUSO detected
            System_state_G = INTRUSO;
            New_state_G = 1;
            State_call_count_G = 0;
            break;
            }

         // Finally, check for correct password
         if (INTRUSO_Get_Password_G() == 1)
            {
            System_state_G = DISARMED;
            New_state_G = 1;
            State_call_count_G = 0;
            break;
            }

         break;
         }

      case INTRUSO: 
         {
         if (New_state_G)
            {
            //PrintOut("\n** INTRUSO! **");
         	strncpy (slocal, msg_intruso, strlen(msg_intruso));
         	SerieRxTx_Write_String_To_Buffer(slocal);
            New_state_G = 0;
            aLEDreset();
			vLEDSelect = LEDR;
			estadoLED = LED_NORMAL;  //rojo blinking
            }

         // Sound the alarm!
         INTRUSO_Sound_Alarm();

         // Keep sounding alarm until we get correct password
         if (INTRUSO_Get_Password_G() == 1)
            {
            System_state_G = DISARMED;
            New_state_G = 1;
            State_call_count_G = 0;
            }

         break;
         }
      }
   }

/*==================[definiciones de funciones internas]=====================*/

/* Ingreso de Password en  modo INTRUSO------------------------------------- */

bool_t INTRUSO_Get_Password_G(void)
   {
   signed char Key;
   uint8_t Password_G_count = 0;
   uint8_t i;

   // Update the keypad buffer
   KEYPAD_Update();
   
   // Are there any new data in the keypad buffer?
   if (KEYPAD_Get_Data_From_Buffer(&Key) == 0)
      {
      // No new data - password can't be correct
      return 0;
      }

   // If we are here, a key has been pressed

   // How long since last key was pressed?
   // Must be pressed within 50 seconds (assume 50 ms 'tick')
   if (State_call_count_G > 1000)
      {
      // More than 5 seconds since last key
      // - restart the input process
      State_call_count_G = 0;
      Position_G = 0;
      }

   if (Position_G == 0)
      {         
      SerieRxTx_Write_Char_To_Buffer('\n');
      }
       
   SerieRxTx_Write_Char_To_Buffer(Key);
   
   Input_G[Position_G] = Key;             

   // Have we got four numbers?
   if ((++Position_G) == 4)
      {
      Position_G = 0;
      Password_G_count = 0;

      // Check the password
      for (i = 0; i < 4; i++)
          {
          if (Input_G[i] == Password_G[i])
             {
             Password_G_count++;
             }
          }
      }
  
   if (Password_G_count == 4)
      {
      // Password correct
      return 1;
      }
   else
      {
      // Password NOT correct
      return 0;
      }
   }

/* --------------------------------------------------------------- */
/* Uses: 
  extern bool_t Sensor_Ventana1;
  extern bool_t Sensor_Ventana2;
  extern bool_t Sensor_Ventana3;
*/ 

bool_t INTRUSO_Check_Window_Sensors(void)
   {
   // Just a single window 'sensor' here
   // - easily extended
   if (Sensor_Ventana1 == TRUE)
      {
      // INTRUSO detected...
      //PrintOut("\n Ventana 1 dañada");
	  strncpy (slocal, msg_ventana1, strlen(msg_ventana1));
	  SerieRxTx_Write_String_To_Buffer(slocal);
      return TRUE;
      }
   if (Sensor_Ventana2 == TRUE)
      {
      // INTRUSO detected...
      //*PrintOut("\n Ventana 2 dañada");
	  strncpy (slocal, msg_ventana2, strlen(msg_ventana2));
	  SerieRxTx_Write_String_To_Buffer(slocal);
	  return TRUE;
      }
   if (Sensor_Ventana3 == TRUE)
      {
      // INTRUSO detected...
      // PrintOut("\n Ventana 3 dañada");
	  strncpy (slocal, msg_ventana3, strlen(msg_ventana3));
	  SerieRxTx_Write_String_To_Buffer(slocal);
      return TRUE;
      }
  
   // Default
   return FALSE;
   }

/* --------------------------------------------------------------- */
/* Uses: 
  extern bool_t Puerta_Abierta;
*/

bool_t INTRUSO_Check_Door_Sensor(void)
   {
   // Single door sensor (access route)
   if (Puerta_Abierta == TRUE)
      {
      // Someone has opened the door...
      // PrintOut("\nPuerta Abierta");
	  strncpy (slocal, msg_puerta, strlen(msg_puerta));
	  SerieRxTx_Write_String_To_Buffer(slocal);
      return TRUE;
      }

   // Default
   return FALSE;
   }

/* --------------------------------------------------------------- */

void INTRUSO_Sound_Alarm(void)
   {
   if (Alarm_bit)
      {
      // Alarm connected to this pin
      Sounder_pin = 0;
      Alarm_bit = 0;
      }
   else
      {
      Sounder_pin = 1;
      Alarm_bit = 1;
      }
   }

/*------------------------------------------------------------------*-
  ---- END OF FILE -------------------------------------------------
-*------------------------------------------------------------------*/
