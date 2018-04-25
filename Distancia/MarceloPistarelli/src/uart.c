/*==================[inlcusiones]============================================*/

#include "uart.h"
#include "circular_buffer_mp.h"
#include "sapi_uart.h"

/*==================[macros and definitions]=================================*/

/*==================[definiciones de datos internos]=========================*/

static uartMap_t uart_selecionada;	// UART seleccionada para envío y recepción de datos

/*==================[definiciones de datos externos]=========================*/

circular_buffer_t uart_rx_buffer;	// buffer circular de recepción
circular_buffer_t uart_tx_buffer;	// buffer circular de transmisión

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[funcion principal]======================================*/

void UART_Init(uartMap_t uart)	// inicialización de UART seleccionada y buffers
{
	uart_selecionada = uart;
	circ_buffer_init(&uart_rx_buffer, RX_BUFFER_SIZE);
	circ_buffer_init(&uart_tx_buffer, TX_BUFFER_SIZE);
}

void UART_Update(void)
{

	uint8_t tx_rx_byte = 0;

	while (readByte_circ_buffer(&uart_tx_buffer, &tx_rx_byte) != 0) // si hay datos en buffer circular TX para transmitir los transmite
	{
		uartWriteByte(uart_selecionada, tx_rx_byte);
	}

	while (uartReadByte(uart_selecionada, &tx_rx_byte)) // si hay datos para leer de la UART los carga en el buffer circular RX
	{

		writeByte_circ_buffer(&uart_rx_buffer, tx_rx_byte);

	}

}

void UART_write_String_To_TX_Buffer(char* STR_PTR) // cargo datos en buffer circular TX para que luego sean transmitidos
{
	uint32_t i = 0;
	while (STR_PTR[i] != '\0')
	{
		writeByte_circ_buffer(&uart_tx_buffer, STR_PTR[i]);
		i++;
	}
}

uint32_t UART_read_String_From_RX_Buffer(char* STR_PTR) // Leo todos los datos presentes en el buffer circular RX, devuelve cantidad de bytes leidos
{

	return readAll_circ_buffer(&uart_rx_buffer, STR_PTR);
}

uint32_t UART_read_Char_From_RX_Buffer(char* STR_PTR) // Leo 1 byte del buffer circular RX, devuelve cantidad de bytes leidos
{
	return readByte_circ_buffer(&uart_rx_buffer, STR_PTR);

}

uint32_t UART_read_line_lowecase(char * charArray, uint32_t longitudMax) // Lectura y conversión a minúscula del buffer de entrada de UART
{

	uint32_t charCount = 0;
	uint8_t receivedChar;

	while (charCount < longitudMax && UART_read_Char_From_RX_Buffer(&receivedChar) != 0)
	{
		if (receivedChar != '\n' && receivedChar != '\r')
		{
			charArray[charCount] = tolower(receivedChar);
			charCount++;
		}
		else
		{
			break;
		}

	}
	return charCount;
}


