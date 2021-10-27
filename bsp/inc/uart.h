/*
 * uart.h
 *
 *  Created on: 2018��10��16��
 *      Author: Administrator
 */

#ifndef __UART_H__
#define __UART_H__

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/**
 * @name The UART channel indexes
 * @{
 */
#define UART0_INDEX       		                0
#define UART1_INDEX       		                1
/** @} */ /* The UART channel indexes */

#define UART_RX_BUFFER_SIZE                     1000 /**< the UART RX buffer size. */

/**
 * @name The UART data format definition
 * @{
 */
/*-----------------------------------------------------------------------------------
 * Data format
 * +--------------------+-----------------+-----------------+-----------------+-----+
 * | byte #0            | byte #1         | byte #2         | byte #3         | ... |
 * +--------------------+-----------------+-----------------+-----------------+-----+
 * | 0xAA               | 0x55            | data size low   | data size high  | ... |
 * +--------------------+-----------------+-----------------+-----------------+-----+
 *---------------------------------------------------------------------------------*/
#define UART_HEADER_FLAG      	                0xAA55
#define UART_HEADER_SIZE  	                    4
/** @} */ /* The UART data format definition */

/*******************************************************************************
 * Function prototypes
 ******************************************************************************/
/**
 * Initialize the UART.
 *
 * @param [in] _index the UART channel index
 * @param [in] _baudrate the baud rate
 * @param [in] _data_bits the data bits:
 * <ul>
 * <li>{@link UART_DATA_BITS_8} 8 bits data</li>
 * <li>{@link UART_DATA_BITS_9} 9 bits data</li>
 * <li>{@link UART_DATA_BITS_10} 10 bits data</li>
 * </ul>
 * @param [in] _stop_bits the stop bits:
 * <ul>
 * <li>{@link UART_STOP_BITS_0_5} 0.5 stop bit</li>
 * <li>{@link UART_STOP_BITS_1} 1 stop bit</li>
 * <li>{@link UART_STOP_BITS_1_5} 1.5 stop bit</li>
 * <li>{@link UART_STOP_BITS_2} 2 stop bit</li>
 * </ul>
 * @param [in] _parity the partity:
 * <ul>
 * <li>{@link UART_PARITY_MODE_NONE} no parity</li>
 * <li>{@link UART_PARITY_MODE_EVEN} even parity</li>
 * <li>{@link UART_PARITY_MODE_ODD} odd parity</li>
 * </ul>
 * @return 0(success) or other values(failure)
 */
int32_t uart_init(const uint8_t _index, const uint32_t _baudrate, const uint32_t _data_bits, const uint32_t _stop_bits, const uint32_t _parity);

/**
 * De-initialize the UART.
 *
 * @param [in] _index the UART channel index
 * @return 0(success) or other values(failure)
 */
int32_t uart_deinit(const uint8_t _index);

/**
 * Receive data.
 *
 * @param [in]  _index the UART channel index
 * @param [out] _buf the buffer to receive
 * @param [in]  _size the size to receive
 * @return the received size
 */
uint16_t uart_receive(const uint8_t _index, uint8_t _buf[], const uint16_t _size);

/**
 * Send data.
 *
 * @param [in] _index the UART channel index
 * @param [in] _buf the buffer to send
 * @param [in] _size the size to send
 * @return the sent size
 */
uint16_t uart_send(const uint8_t _index, const uint8_t _buf[], const uint16_t _size);

/**
 * Receive data with format in polling mode.
 *
 * @param [in]  _index the UART channel index
 * @param [out] _buf the buffer to receive
 * @param [in]  _size the size to receive
 * @return the received size without header
 */
uint16_t uart_receive_with_format_polling( const uint8_t _index, uint8_t _buf[], const uint16_t _size);

/**
 * Send data with format.
 *
 * @param [in] _index the UART channel index
 * @param [in] _buf the buffer to send
 * @param [in] _size the size to send
 * @return the sent size
 */
uint16_t uart_send_with_format(const uint8_t _index, const uint8_t _buf[], const uint16_t _size);

/**
 * Print the debug information.
 *
 * @param [in] _info the information format string
 */
void debug(const char* _info, ...);

/**
 * Print a buffer area.
 *
 * @param [in] _prefix the information prefix
 * @param [in] _id the information id
 * @param [in] _buf the buffer to be printed
 * @param [in] _size the size to print
 */
void print_buf(const char *_prefix, const uint32_t _id, const uint8_t _buf[], const uint16_t _size);

#ifdef __cplusplus
}
#endif

#endif /* __UART_H__ */
