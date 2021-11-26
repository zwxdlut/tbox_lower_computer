/*
 * uart.c
 *
 *  Created on: 2018年10月16日
 *      Author: Administrator
 */

#include "uart.h"

#if defined USING_OS_FREERTOS
extern SemaphoreHandle_t g_uart_tx_mutex[UART1_INDEX + 1]; /* Sending mutex */
#endif

extern uint8_t g_uart_rx_queue[UART1_INDEX + 1][UART_RX_BUFFER_SIZE]; /* Receiving queue */
extern uint16_t g_uart_rx_queue_head[UART1_INDEX + 1]; /* Receiving queue head */
extern uint16_t g_uart_rx_queue_tail[UART1_INDEX + 1]; /* Receiving queue tail */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Local function prototypes
 ******************************************************************************/

/*******************************************************************************
 * Functions
 ******************************************************************************/
uint16_t uart_receive(const uint8_t _index, uint8_t _buf[], const uint16_t _size)
{
	assert(UART1_INDEX >= _index && NULL != _buf);

	uint16_t i = 0;

	/* Check if the RX queue is empty */
	while (g_uart_rx_queue_head[_index] != g_uart_rx_queue_tail[_index] && i < _size)
	{
		/* Dequeue */
		_buf[i++] = g_uart_rx_queue[_index][g_uart_rx_queue_head[_index]];
		g_uart_rx_queue_head[_index] = (g_uart_rx_queue_head[_index] + 1) % UART_RX_BUFFER_SIZE;
	}

	return i;
}

uint16_t uart_receive_with_format_polling( const uint8_t _index, uint8_t _buf[], const uint16_t _size)
{
	assert(NULL != _buf);

	uint16_t size = 0;
	uint16_t out_size = 0;

	/* Receive 0xAA */
	size = 1;

	if (size != uart_receive(_index, _buf, size) ||  (0xFF & (UART_HEADER_FLAG >> 8)) != _buf[0])
	{
		return 0;
	}

	/* Receive 0x55 */
	size = 1;
	out_size = 0;

	while (out_size < size)
	{
#if defined USING_OS_FREERTOS
		vTaskDelay(pdMS_TO_TICKS(1));
#endif
		out_size += uart_receive(_index, _buf + out_size, size - out_size);
	}

	if ((UART_HEADER_FLAG & 0xFF) != _buf[0])
	{
		return 0;
	}

	/* Receive data length */
	size = UART_HEADER_SIZE - 2;
	out_size = 0;

	while (out_size < size)
	{
#if defined USING_OS_FREERTOS
		vTaskDelay(pdMS_TO_TICKS(1));
#endif
		out_size += uart_receive(_index, _buf + out_size, size - out_size);
	}

	/* Receive data */
	memcpy(&size, _buf, UART_HEADER_SIZE - 2);
	size = _size > size ? size : _size;
	out_size = 0;

	while (out_size < size)
	{
#if defined USING_OS_FREERTOS
		vTaskDelay(pdMS_TO_TICKS(1));
#endif
		out_size += uart_receive(_index, _buf + out_size, size - out_size);
	}

	return size;
}

uint16_t uart_send_with_format(const uint8_t _index, const uint8_t _buf[], const uint16_t _size)
{
	assert(UART1_INDEX >= _index && NULL != _buf);

	uint16_t size = 0;

#if defined USING_OS_FREERTOS
	xSemaphoreTakeRecursive( g_uart_tx_mutex[_index], portMAX_DELAY);
#endif

#if defined (UART_HEADER_FLAG) && defined (UART_HEADER_SIZE)
	uint8_t	header[UART_HEADER_SIZE];
	header[0] = UART_HEADER_FLAG >> 8;
	header[1] = UART_HEADER_FLAG & 0xFF;
	header[2] = _size;
	header[3] = _size >> 8;
	size = uart_send(_index, header, UART_HEADER_SIZE);
#endif

	size += uart_send(_index, _buf, _size);

#if defined USING_OS_FREERTOS
	xSemaphoreGiveRecursive( g_uart_tx_mutex[_index] );
#endif

	return size;
}

void debug(const char* _info, ...)
{
#ifdef _UDEBUG
	va_list args;
	va_start(args, _info);
	vprintf(_info, args);
	va_end(args);
#endif
}

void print_buf(const char *_prefix, const uint32_t _id, const uint8_t _buf[], const uint16_t _size)
{
#if defined _UDEBUG
#if defined USING_OS_FREERTOS
	static SemaphoreHandle_t g_debug_mutex;
	static bool init = false;

	if (!init)
	{
		g_debug_mutex = xSemaphoreCreateMutex();
		init = true;
	}

	xSemaphoreTake( g_debug_mutex, portMAX_DELAY);
#endif

	printf("%s(0x%X,%d): ", _prefix, (unsigned int)_id, _size);

	for (uint16_t i = 0; i < _size; i++)
	{
		printf("%02X ", _buf[i]);
	}

	printf("\r\n");

#if defined USING_OS_FREERTOS
	xSemaphoreGive( g_debug_mutex );
#endif
#endif
}

/**
 * @name Retarget printf
 * @{
 */
#if defined __EWL__
#include <sys/UART.h>

UARTError InitializeUART(UARTBaudRate baudRate)
{
#if defined _UDEBUG
	uart_init(UART0_INDEX, kBaud115200, UART_DATA_BITS_8, UART_STOP_BITS_1, UART_PARITY_MODE_NONE);
#endif

	return kUARTNoError;
}

UARTError ReadUARTN(void* bytes, unsigned long limit)
{
#if defined _UDEBUG
	uint16_t  size = 0;

	while (size < limit)
	{
		size += uart_receive(UART0_INDEX, bytes + size, limit - size);
	}
#endif

	return kUARTNoError;
}

UARTError WriteUARTN(const void * bytes, unsigned long length)
{
#if defined _UDEBUG
	uart_send(UART0_INDEX, bytes, length);
#endif

	return kUARTNoError;
}

#elif defined __NEWLIB__
int _write(int iFileHandle, char *pcBuffer, int iLength)
{
#if defined _UDEBUG
	return uart_send(UART0_INDEX, (uint8_t*)pcBuffer, iLength);
#else
	return 0;
#endif
}

#elif defined __ARMCLIB_VERSION
#pragma import(__use_no_semihosting)

void _sys_exit(int x)
{
    x = x;
}

void _ttywrch(int x)
{
    x = x;
}

struct __FILE
{
    int handle;
};

FILE __stdout;

int fputc(int ch, FILE *f)
{
#if defined _UDEBUG
	uart_send(UART0_INDEX, (uint8_t*)(&ch), 1);
#endif

	return (ch);
}
#endif
/** @} */ /* Retarget printf */

/*******************************************************************************
 * Local functions
 ******************************************************************************/
