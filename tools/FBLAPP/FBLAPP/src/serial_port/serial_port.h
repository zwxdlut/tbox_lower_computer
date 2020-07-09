#pragma once

#ifndef __SERIAL_PORT_H__
#define __SERIAL_PORT_H__

#include <windows.h>
#include <string>
#include <mutex>

/**
 * @defgroup Serial port data bits.
 * @{
 */
#define SP_DATA_BITS_4                            4
#define SP_DATA_BITS_5                            5
#define SP_DATA_BITS_6                            6
#define SP_DATA_BITS_7                            7
#define SP_DATA_BITS_8                            8
 /** @} */ /* End of group Serial port data bits. */

/**
 * @defgroup Serial port stop bits.
 * @{
 */
#define SP_STOP_BITS_1                            0
#define SP_STOP_BITS_1_5                          1
#define SP_STOP_BITS_2                            2
 /** @} */ /* End of group Serial port stop bits. */

/**
 * @defgroup Serial port parity.
 * @{
 */
#define SP_PARITY_MODE_NONE                       0
#define SP_PARITY_MODE_ODD                        1
#define SP_PARITY_MODE_EVEN                       2
#define SP_PARITY_MODE_MARK                       3
#define SP_PARITY_MODE_SPACE                      4
 /** @} */ /* End of group Serial port parity. */

#define SERIAL_PORT_BUFFER_SIZE                   1024 /**< Serial port buffer size. */

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
 * @brief Serial port communication class. 
 */
class serial_port
{
public:
	/** 
	 * @brief Constructor. 
	 */
	serial_port(void);

	/** 
	 * @brief Destructor. 
	 */
	~serial_port(void);

	/**
	 * @brief  Open serial port.
	 *
	 * @param  [in] _com       UART number.
	 * @param  [in] _baud_rate Baud rate.
	 * @param  [in] _data_bits Data bits:
	 *              @arg SP_DATA_BITS_4 4 bits data.
	 *              @arg SP_DATA_BITS_5 5 bits data.
	 *              @arg SP_DATA_BITS_6 6 bits data.
     *              @arg SP_DATA_BITS_7 7 bits data.
	 *              @arg SP_DATA_BITS_8 8 bits data.
	 * @param  [in] _stop_bits Stop bits:
	 *              @arg SP_STOP_BITS_1   1 stop bit.
	 *              @arg SP_STOP_BITS_1_5 1.5 stop bits.
	 *              @arg SP_STOP_BITS_2   2 stop bits.
	 * @param  [in] _parity    Partiy:
	 *              @arg SP_PARITY_MODE_NONE  No parity.
	 *              @arg SP_PARITY_MODE_ODD   Odd parity.
	 *              @arg SP_PARITY_MODE_EVEN  Even parity.
	 *              @arg SP_PARITY_MODE_MARK  Mark parity.
	 *              @arg SP_PARITY_MODE_SPACE Space parity.
	 * @return Success(0) or failure(other values).
	 */
	int32_t open(const std::string &_com,
				 const uint32_t _baud_rate,
				 const uint32_t _data_bits,
				 const uint32_t _stop_bits,
				 const uint32_t _parity);

	/**
	 * @brief  Close serial port.
	 *
	 * @return Success(0) or failure(other values).
	 */
	int32_t close(void);

	/**
	 * @brief  Receive data.
	 *
	 * @param  [out] _buf   Receive buffer.
	 * @param  [in]  _size  Receive size.
	 * @return Received size.
	 */
	uint16_t receive(uint8_t *const _buf, const uint16_t _size);

	/**
	 * @brief  Receive data with header(0xAA55) in poll mode.
	 *
	 * @param  [out] _buf   Receive buffer.
	 * @param  [in]  _size  Receive size.
	 * @return Received size without header.
	 */
	uint16_t receive_with_header_poll(uint8_t* const _buf, const uint16_t _size);

	/**
	 * @brief  Transmit data.
	 *
	 * @param  [in] _buf   Transmit buffer.
	 * @param  [in] _size  Transmit size.
	 * @return Transmitted size.
	 */
	uint16_t transmit(const uint8_t *const _buf, const uint16_t _size);

	/**
	 * @brief  Send with header.
	 *
	 * @param  [in] _buf   Send buffer.
	 * @param  [in] _size  Send size.
	 * @return Sent size.
	 */
	uint16_t transmit_with_header(const uint8_t *const _buf, const uint16_t _size);

private:
	HANDLE               handle_;
	std::recursive_mutex tx_mutex_;
};

#endif // __SERIAL_PORT_H__