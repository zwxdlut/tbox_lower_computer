#pragma once

#ifndef __SERIAL_PORT_H__
#define __SERIAL_PORT_H__

#include <windows.h>
#include <string>
#include <mutex>

/**
 * @name Serial port data bits
 * @{
 */
#define SP_DATA_BITS_4                            4
#define SP_DATA_BITS_5                            5
#define SP_DATA_BITS_6                            6
#define SP_DATA_BITS_7                            7
#define SP_DATA_BITS_8                            8
 /** @} */ // Serial port data bits

/**
 * @name Serial port stop bits
 * @{
 */
#define SP_STOP_BITS_1                            0
#define SP_STOP_BITS_1_5                          1
#define SP_STOP_BITS_2                            2
 /** @} */ // Serial port stop bits

/**
 * @name Serial port parity
 * @{
 */
#define SP_PARITY_MODE_NONE                       0
#define SP_PARITY_MODE_ODD                        1
#define SP_PARITY_MODE_EVEN                       2
#define SP_PARITY_MODE_MARK                       3
#define SP_PARITY_MODE_SPACE                      4
 /** @} */ // Serial port parity

#define SERIAL_PORT_BUFFER_SIZE                   1024 /**< serial port buffer size */

/*-----------------------------------------------------------------------------------
 * Header
 * +--------------------+-----------------+-----------------+-----------------+-----+
 * | byte #0            | byte #1         | byte #2         | byte #3         | ... |
 * +--------------------+-----------------+-----------------+-----------------+-----+
 * | 0xAA               | 0x55            | data size low   | data size high  | ... |
 * +--------------------+-----------------+-----------------+-----------------+-----+
 *---------------------------------------------------------------------------------*/
#define HEADER_FLAG      	                      0xAA55
#define HEADER_SIZE  	                          4

/** 
 * Serial port communication class.
 * TODO: serial port channels management
 */
class serial_port
{
public:
	/** 
	 * Constructor.
	 */
	serial_port(void);

	/** 
	 * Destructor.
	 */
	~serial_port(void);

	/**
	 * Open the serial port.
	 *
	 * @param [in] _chl the serial port channel number
	 * @param [in] _baudrate the baud rate of the serial port
	 * @param [in] _data_bits the data bits:
	 * <ul>
	 * <li>{@link SP_DATA_BITS_4} 4 bits data</li>
	 * <li>{@link SP_DATA_BITS_5} 5 bits data</li>
	 * <li>{@link SP_DATA_BITS_6} 6 bits data</li>
     * <li>{@link SP_DATA_BITS_7} 7 bits data</li>
	 * <li>{@link SP_DATA_BITS_8} 8 bits data</li>
	 * </ul>
	 * @param [in] _stop_bits the stop bits:
	 * <ul>
	 * <li>{@link SP_STOP_BITS_1} 1 stop bit</li>
	 * <li>{@link SP_STOP_BITS_1_5} 1.5 stop bit</li>
	 * <li>{@link SP_STOP_BITS_2} 2 stop bit</li>
	 * </ul>
	 * @param [in] _parity the partity:
	 * <ul>
	 * <li>{@link SP_PARITY_MODE_NONE} no parity</li>
	 * <li>{@link SP_PARITY_MODE_ODD} even parity</li>
	 * <li>{@link SP_PARITY_MODE_EVEN} odd parity</li>
	 * <li>{@link SP_PARITY_MODE_MARK} mark parity</li>
	 * <li>{@link SP_PARITY_MODE_SPACE} space parity</li>
	 * </ul>
	 * @return 0(success) or other values(failure)
	 */
	int32_t open(const std::string &_chl, const uint32_t _baud_rate, const uint32_t _data_bits, const uint32_t _stop_bits, const uint32_t _parity);

	/**
	 * Close the serial port.
	 *
	 * @return 0(success) or other values(failure)
	 */
	int32_t close(void);

	/**
	* Receive data from the serial port.
	*
	* @param [out] _buf the buffer to receive
	* @param [in]  _size the size to receive
	* @return the received data size
	*/
	uint16_t receive(uint8_t _buf[], const uint16_t _size);

	/**
	* Send data to the serial port.
	*
	* @param [in] _buf the buffer to send
	* @param [in] _size the size to send
	* @return the sent data size
	*/
	uint16_t send(const uint8_t _buf[], const uint16_t _size);

	/**
	* Receive data with header(0xAA55) form the serial port in poll mode.
	*
	* @param [out] _buf the buffer to receive
	* @param [in]  _size the size to receive
	* @return the received data size without header
	*/
	uint16_t receive_with_header_poll(uint8_t _buf[], const uint16_t _size);

	/**
	 * Send data with header to the serial port.
	 *
	 * @param [in] _buf the buffer to send
	 * @param [in] _size the size to send
	 * @return the sent data size
	 */
	uint16_t send_with_header(const uint8_t _buf[], const uint16_t _size);

private:
	HANDLE handle_;
	std::recursive_mutex tx_mutex_;
};

#endif // __SERIAL_PORT_H__