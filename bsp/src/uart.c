#include <stdlib.h>

#include "uart.h"

#if defined USING_OS_FREERTOS
extern SemaphoreHandle_t g_uart_tx_mutex[UART_CH2 + 1]; /* sending mutex */
#endif

#ifndef UART_DMA_IMPL
extern uint8_t g_uart_rx_queue[UART_CH2 + 1][UART_RX_BUFFER_SIZE]; /* receiving queue */
extern uint16_t g_uart_rx_queue_head[UART_CH2 + 1]; /* receiving queue head */
extern uint16_t g_uart_rx_queue_tail[UART_CH2 + 1]; /* receiving queue tail */
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Local function prototypes
 ******************************************************************************/

/*******************************************************************************
 * Functions
 ******************************************************************************/
#ifndef UART_DMA_IMPL
uint16_t uart_receive(const uint8_t _chl, uint8_t _buf[], const uint16_t _size)
{
	assert(UART_CH2 >= _chl && NULL != _buf);

	uint16_t i = 0;

	/* check if the rx queue is empty */
	while (g_uart_rx_queue_head[_chl] != g_uart_rx_queue_tail[_chl] && i < _size)
	{
		/* dequeue */
		_buf[i++] = g_uart_rx_queue[_chl][g_uart_rx_queue_head[_chl]];
		g_uart_rx_queue_head[_chl] = (g_uart_rx_queue_head[_chl] + 1) % UART_RX_BUFFER_SIZE;
	}

	return i;
}
#endif

uint16_t uart_receive_with_format_polling(const uint8_t _chl, uint8_t _buf[], const uint16_t _size)
{
	uint16_t size = 0;
	uint16_t out_size = 0;

	/* receive 0xAA */
	size = 1;

	if (size != uart_receive(_chl, _buf, size) ||  (0xFF & (UART_HEADER_FLAG >> 8)) != _buf[0])
	{
		return 0;
	}

	/* receive 0x55 */
	size = 1;
	out_size = 0;

	while (out_size < size)
	{
#if defined USING_OS_FREERTOS
		vTaskDelay(pdMS_TO_TICKS(1));
#endif
		out_size += uart_receive(_chl, _buf + out_size, size - out_size);
	}

	if ((UART_HEADER_FLAG & 0xFF) != _buf[0])
	{
		return 0;
	}

	/* receive data length */
	size = UART_HEADER_SIZE - 2;
	out_size = 0;

	while (out_size < size)
	{
#if defined USING_OS_FREERTOS
		vTaskDelay(pdMS_TO_TICKS(1));
#endif
		out_size += uart_receive(_chl, _buf + out_size, size - out_size);
	}

	/* receive data */
	memcpy(&size, _buf, UART_HEADER_SIZE - 2);
	size = _size > size ? size : _size;
	out_size = 0;

	while (out_size < size)
	{
#if defined USING_OS_FREERTOS
		vTaskDelay(pdMS_TO_TICKS(1));
#endif
		out_size += uart_receive(_chl, _buf + out_size, size - out_size);
	}

	return size;
}

uint16_t uart_send_with_format(const uint8_t _chl, const uint8_t _buf[], const uint16_t _size)
{
	uint16_t size = 0;

#if defined USING_OS_FREERTOS
	xSemaphoreTakeRecursive( g_uart_tx_mutex[_chl], portMAX_DELAY);
#endif

#if defined (UART_HEADER_FLAG) && defined (UART_HEADER_SIZE)
	uint8_t	header[UART_HEADER_SIZE];
	header[0] = UART_HEADER_FLAG >> 8;
	header[1] = UART_HEADER_FLAG & 0xFF;
	header[2] = _size;
	header[3] = _size >> 8;
	size = uart_send(_chl, header, UART_HEADER_SIZE);
#endif

	size += uart_send(_chl, _buf, _size);

#if defined USING_OS_FREERTOS
	xSemaphoreGiveRecursive( g_uart_tx_mutex[_chl] );
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
	char *info = malloc(100);
	char s[10] = "";

	memset(info, 0, 100);
	sprintf(info, "%s(0x%X,%d): ", _prefix, (unsigned int)_id, _size);

	for (uint16_t i = 0; i < _size; i++)
	{
		sprintf(s, "%02X ", _buf[i]);
		strcat(info, s);
	}

	strcat(info, "\r\n");
	printf("%s", info);
	free(info);
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
	uart_init(UART_CH0, kBaud115200, UART_DATA_BITS_8, UART_STOP_BITS_1, UART_PARITY_MODE_NONE);
#endif

	return kUARTNoError;
}

UARTError ReadUARTN(void* bytes, unsigned long limit)
{
#if defined _UDEBUG
	uint16_t  size = 0;

	while (size < limit)
	{
		size += uart_receive(UART_CH0, bytes + size, limit - size);
	}
#endif

	return kUARTNoError;
}

UARTError WriteUARTN(const void * bytes, unsigned long length)
{
#if defined _UDEBUG
	uart_send(UART_CH0, bytes, length);
#endif

	return kUARTNoError;
}

#elif defined __NEWLIB__
int _write(int iFileHandle, char *pcBuffer, int iLength)
{
#if defined _UDEBUG
	return uart_send(UART_CH0, (uint8_t*)pcBuffer, iLength);
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
	uart_send(UART_CH0, (uint8_t*)(&ch), 1);
#endif

	return (ch);
}
#endif
/** @} */ /* Retarget printf */

/*******************************************************************************
 * Local functions
 ******************************************************************************/
