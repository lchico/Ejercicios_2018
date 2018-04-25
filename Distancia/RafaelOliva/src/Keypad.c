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

#include "main.h"
// #include "Port.h"

#include "Keypad.h"

// ------ Private function prototypes ------------------------------

//bool_t KEYPAD_Scan(char* const);
bool_t KEYPAD_Scan(uint8_t *receiveByte);

// ------ Private constants ----------------------------------------

#define KEYPAD_RECV_BUFFER_LENGTH 6

// Any valid character will do - must not match anything on keypad
#define KEYPAD_NO_NEW_DATA (char) '-' 

// ------ Private variables ----------------------------------------

static char KEYPAD_recv_buffer[KEYPAD_RECV_BUFFER_LENGTH+1];

static uint8_t KEYPAD_in_read_index;     // Data in buffer that has been read 
static uint8_t KEYPAD_in_waiting_index;  // Data in buffer not yet read

static char Last_valid_key_G = KEYPAD_NO_NEW_DATA;

static char Old_key_G;

/*------------------------------------------------------------------*-

  KEYPAD_Init()

  Init the keypad.

-*------------------------------------------------------------------*/
void KEYPAD_Init(void)
   {
   KEYPAD_in_read_index = 0;
   KEYPAD_in_waiting_index = 0;
   }

/*------------------------------------------------------------------*-

  KEYPAD_Update()

  The main 'update' function for the keypad library.

  Must schedule this function (approx every 50 - 200 ms).

-*------------------------------------------------------------------*/
void KEYPAD_Update(void)
   {
   uint8_t Key;

   // Scan keypad here...
   if (KEYPAD_Scan(&Key) == 0)
      {
      // No new key data - just return
      return;
      }

   // Want to read into index 0, if old data has been read
   // (simple ~circular buffer)
   if (KEYPAD_in_waiting_index == KEYPAD_in_read_index)
      { 
      KEYPAD_in_waiting_index = 0;
      KEYPAD_in_read_index = 0;
      } 
   
   // Load keypad data into buffer
   KEYPAD_recv_buffer[KEYPAD_in_waiting_index] = Key;

   if (KEYPAD_in_waiting_index < KEYPAD_RECV_BUFFER_LENGTH)
      {
      // Increment without overflowing buffer
      KEYPAD_in_waiting_index++;
      }
   }


/*------------------------------------------------------------------*-

  KEYPAD_Get_Char_From_Buffer()

  The Update function copies data into the keypad buffer.  
  This extracts data from the buffer.
  
-*------------------------------------------------------------------*/

bool_t KEYPAD_Get_Data_From_Buffer(char* const pKey)
   {
   // If there is new data in the buffer
   if (KEYPAD_in_read_index < KEYPAD_in_waiting_index)
      {
      *pKey = KEYPAD_recv_buffer[KEYPAD_in_read_index];

      KEYPAD_in_read_index++;

      return 1;
      }

   return 0;
   }


/*------------------------------------------------------------------*-

  KEYPAD_Clear_Buffer()

-*------------------------------------------------------------------*/
void KEYPAD_Clear_Buffer(void)
   {
   KEYPAD_in_waiting_index = 0;
   KEYPAD_in_read_index = 0;
   }

/*------------------------------------------------------------------*-

  KEYPAD_Scan()

  This function is called from scheduled keypad function.

  Must be edited as required to match your key labels.

  Includes two 'function keys' which may be used simultaneously
  with keys from any other column.  

  New implementation - use serial input.. 4.2018
 
-*------------------------------------------------------------------*/
bool_t KEYPAD_Scan(uint8_t *receiveByte){

  uint8_t Key; 

  if(SerieRxTx_Get_Char_From_Buffer(&Key) != 0){
		if(Key != 0){
			*receiveByte = Key;
			return TRUE;
		}
  }
  return FALSE;
  
  /* char Key = KEYPAD_NO_NEW_DATA;*/
   /*if (K0 == 0) { Key = '0'; }
   if (K1 == 0) { Key = '1'; }
   if (K2 == 0) { Key = '2'; }
   if (K3 == 0) { Key = '3'; }
   if (K4 == 0) { Key = '4'; }
   if (K5 == 0) { Key = '5'; }
   if (K6 == 0) { Key = '6'; }
   if (K7 == 0) { Key = '7'; } 
    
   if (Key == KEYPAD_NO_NEW_DATA)
      {
      // No key pressed 
      Old_key_G = KEYPAD_NO_NEW_DATA;
      Last_valid_key_G = KEYPAD_NO_NEW_DATA;

      return 0;  // No new data
      }
   
   // A key has been pressed: debounce by checking twice
   if (Key == Old_key_G)
      {
      // A valid (debounced) key press has been detected
      
      // Must be a new key to be valid - no 'auto repeat'
      if (Key != Last_valid_key_G)
         { 
         // New key!
         *pKey = Key;
         Last_valid_key_G = Key;

         return 1;
         }
      }

   // No new data
   Old_key_G = Key;
   */
 
}

/*------------------------------------------------------------------*-
  ---- END OF FILE -------------------------------------------------
-*------------------------------------------------------------------*/
