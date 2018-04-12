/*============================================================================
 * Autor: Ericson Joseph Estupiñan Pineda
 * Licencia:
 * Fecha: 2018-04-07
 *===========================================================================*/

/*==================[inlcusiones]============================================*/

#include "main.h"

#include "cooperativeOs_isr.h"       // <= dispatcher and task management header
#include "cooperativeOs_scheduler.h" // <= scheduler and system initialization header
#include "sapi.h"

/*==================[macros and definitions]=================================*/
DEBUG_PRINT_ENABLE

#define BUTTON_TICK TEC1
#define BUTTON_LED TEC2
#define BLINK_TIME_PERIOD 10
#define DEBOUNCE_TIME_PERIOD 20
#define MIN_COMMON_TIME_PERIOD 10
#define CONSOLE_TIME_PERIOD 10
#define SERIAL_DATA_BUFFER_SIZE 50

gpioMap_t seqLEDs[] = { LEDG, LEDR, LEDB, LED1, LED2, LED3 };
#define SEQ_LEN (sizeof(seqLEDs) / sizeof(gpioMap_t))

tick_t arrayTick[] = { 50, 500, 1000 };
#define LEN_TICKS sizeof(arrayTick) / sizeof(tick_t)

/*==================[definiciones de datos internos]=========================*/

gpioMap_t LED = LED3;
tick_t _tick_var = 50;
tick_t _tick_count = 0;

debonce_t dStateTEC1;
debonce_t dStateTEC2;

typedef struct {
	char end;
	tick_t tout;
	tick_t ctout;
	uint32_t bp;
	uint32_t size;
} sread_t;

static uint8_t menu[] = "\n\r"
		"********************* MENU *************************\n\r"
		"\n\r"
		"TEC1: cambia el tiempo de parpadeo del led activo.\n\r"
		"TEC2: cambia el led activo.\n\r"
		"TXXX: permite ingresar un tiempo desde el puerto serie ej: T100.\n\r"
		"LLEDX: permite ingresar un led desde el puerto serie. ej: LLED1.\n\r";

sread_t sread;
char buffer[SERIAL_DATA_BUFFER_SIZE];

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

static void taskBlinkLeds(void);
static void taskMenuUpdate(void);

gpioMap_t runSeq(gpioMap_t currentLed, gpioMap_t* seq, uint8_t seqLen);

tick_t runSeqTick(tick_t item, tick_t* array, uint8_t len);

uint32_t readData(sread_t* sread, char* buffer, uint32_t length);

void read(char* des, char* src, uint32_t size);

bool_t mstrcmp(char* a, char* b, uint32_t size);

uint32_t readCmd(char* prefix, uint32_t prefix_size, char* data,
		uint32_t data_size);

gpioMap_t readCmdLed(char* prefix, uint32_t prefix_size, char* data,
		uint32_t data_size);

/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/

int main(void) {

	boardConfig();

	sread.end = '\n';
	sread.tout = 500;
	sread.ctout = 0;
	sread.bp = 0;
	sread.size = 0;

	// Inicializar UART_USB como salida de consola
	debugPrintConfigUart(UART_USB, 115200);debugPrintlnString(
			"UART_USB configurada.\n\r");

	schedulerInit();
	schedulerAddTask((sAPI_FuncPtr_t) taskBlinkLeds, 0, BLINK_TIME_PERIOD);
	schedulerAddTask((sAPI_FuncPtr_t) taskMenuUpdate, 1, CONSOLE_TIME_PERIOD);

	schedulerStart(1);
	
	while (TRUE) {
		//Ejecutar tareas
		schedulerDispatchTasks();
	}
	
	return 0;

}

/*==================[definiciones de funciones internas]=====================*/

void taskBlinkLeds(void) {

	_tick_count = _tick_count + MIN_COMMON_TIME_PERIOD;
	if (_tick_count == _tick_var) {
		_tick_count = 0;
		gpioToggle(LED);
	}
}

gpioMap_t runSeq(gpioMap_t currentLed, gpioMap_t* seq, uint8_t seqLen) {
	uint8_t i;
	for (i = 0; i < seqLen; i++) {
		if (currentLed == seq[i] && i == (seqLen - 1)) {
			gpioWrite(currentLed, OFF);
			return seq[0];
		}
		if (currentLed == seq[i] && i < (seqLen - 1)) {
			gpioWrite(currentLed, OFF);
			return seq[i + 1];
		}
	}
	return seq[0];
}

tick_t runSeqTick(tick_t item, tick_t* array, uint8_t len) {
	uint8_t i;
	for (i = 0; i < len; i++) {
		if (item == array[i] && i == (len - 1)) {
			gpioWrite(item, OFF);
			return array[0];
		}
		if (item == array[i] && i < (len - 1)) {
			gpioWrite(item, OFF);
			return array[i + 1];
		}
	}
	return array[0];
}

static void taskMenuUpdate(void) {
	uint32_t bytes = readData(&sread, buffer, SERIAL_DATA_BUFFER_SIZE);
	if (bytes > 0) {
		char data[bytes];
		read(data, buffer, bytes);
		if (mstrcmp(data, "TIC1", 4)) {
			_tick_var = runSeqTick(_tick_var, arrayTick, LEN_TICKS);
			_tick_count = 0;
			uartWriteString(UART_USB, "OK");
		} else if (mstrcmp(data, "TIC2", 4)) {
			LED = runSeq(LED, seqLEDs, SEQ_LEN);
			uartWriteString(UART_USB, "OK");
		} else if (mstrcmp(data, "T", 1)) {
			uint32_t ticks = readCmd("T", 1, data, bytes);
			_tick_var = ticks;
			_tick_count = 0;
		} else if (mstrcmp(data, "LLED", 4)) {
			gpioWrite(LED, OFF);
			LED = readCmdLed("LLED", 4, data, bytes);
		} else {
			uartWriteString(UART_USB, menu);
		}
	}
}

uint32_t readData(sread_t* sread, char* buffer, uint32_t length) {

	uint8_t receiveByte;
	if (uartReadByte(UART_USB, &receiveByte) != FALSE) {
		if (receiveByte != 0) {
			if (receiveByte == sread->end) {
				sread->ctout = 0;
				sread->bp = 0;
				uint32_t a = sread->size;
				sread->size = 0;
				return a;
			} else {
				buffer[sread->bp] = receiveByte;
				sread->size++;
				sread->bp++;
				sread->ctout = 0;
			}
		}
	}

	sread->ctout += MIN_COMMON_TIME_PERIOD;
	if (sread->ctout == sread->tout) {
		sread->ctout = 0;
		sread->bp = 0;
		sread->size = 0;
		return 0;
	}

	return 0;
}

void read(char* des, char* src, uint32_t size) {
	uint32_t i = 0;
	for (; i < size; ++i) {
		des[i] = src[i];
	}
}

gpioMap_t readCmdLed(char* prefix, uint32_t prefix_size, char* data,
		uint32_t data_size) {
	uint32_t num = data_size - prefix_size;
	char ledcmd[num];
	uint32_t i = 0;
	for (; i < data_size; ++i) {
		ledcmd[i] = data[i + prefix_size];
	}
	// LEDG, LEDR, LEDB, LED1, LED2, LED3
	gpioMap_t led = LEDR;
	if (mstrcmp(ledcmd, "R", 1)) {
		led = LEDR;
	} else if (mstrcmp(ledcmd, "G", 1)) {
		led = LEDG;
	} else if (mstrcmp(ledcmd, "B", 1)) {
		led = LEDB;
	} else if (mstrcmp(ledcmd, "1", 1)) {
		led = LED1;
	} else if (mstrcmp(ledcmd, "2", 1)) {
		led = LED2;
	} else if (mstrcmp(ledcmd, "3", 1)) {
		led = LED3;
	} else {
		led = LEDR;
	}
	return led;
}

uint32_t readCmd(char* prefix, uint32_t prefix_size, char* data,
		uint32_t data_size) {
	uint32_t num = data_size - prefix_size;
	char number[num];
	uint32_t i = 0;
	for (; i < data_size; ++i) {
		number[i] = data[i + prefix_size];
	}
	return atoi(number);
}

bool_t mstrcmp(char* a, char* b, uint32_t size) {
	uint32_t i = 0;
	for (; i < size; ++i) {
		if (a[i] != b[i]) {
			return FALSE;
		}
	}
	return TRUE;
}

/*==================[definiciones de funciones externas]=====================*/

/*==================[fin del archivo]========================================*/

