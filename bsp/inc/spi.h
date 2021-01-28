/*
 * spi.h
 *
 *  Created on: 2018��10��16��
 *      Author: Administrator
 */

#ifndef __SPI_H__
#define __SPI_H__

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/**
 * @name The SPI channel indexes
 * @{
 */
#define SPI0_INDEX       		                0
/** @} */ /* The SPI channel indexes */

/**
 * @name The SPI clock polarity
 * @{
 */
#define SPI_CPOL_LOW      		                0
#define SPI_CPOL_HIGH      		                1
/** @} */ /* The SPI clock polarity */

/**
 * @name The SPI clock phase
 * @{
 */
#define SPI_CPHA_1EDGE      		            0
#define SPI_CPHA_2EDGE      		            1
/** @} */ /* The SPI clock phase */

/**
 * @name The SPI data bits
 * @{
 */
#define SPI_DATA_BITS_8      		            8
#define SPI_DATA_BITS_16      		            16
/** @} */ /* The SPI data bits */

/*******************************************************************************
 * Function prototypes
 ******************************************************************************/
/**
 * Initialize the master SPI.
 *
 * @param [in] _index the SPI channel index
 * @param [in] _baudrate the baud rate
 * @param [in] _cpol the clock polarity:
 * <ul>
 * <li>{@link SPI_CPOL_LOW} Clock is idle low.</li>
 * <li>{@link SPI_CPOL_HIGH} Clock is idle high.</li>
 * </ul>
 * @param [in] _cpha the clock phase:
 * <ul>
 * <li>{@link SPI_CPHA_1EDGE} Data is sampled on clock 1st edge, changed on 2nd.</li>
 * <li>{@link SPI_CPHA_2EDGE} Data is sampled on clock 2nd edge, changed on 1st.</li>
 * </ul>
 * @param [in] _data_bits the data bits:
 * <ul>
 * <li>{@link SPI_DATA_BITS_8} 8 bits data</li>
 * <li>{@link SPI_DATA_BITS_16} 16 bits data</li>
 * </ul>
 * @param [in] _lsb_first if LSB or MSB first
 * @return 0(success) or other values(failure)
 */
int32_t spi_master_init(const uint8_t _index, const uint32_t _baudrate, const uint8_t _cpol, const uint8_t _cpha, const uint8_t _data_bits, const bool _lsb_first);

/**
 * De-initialize the master SPI.
 *
 * @param [in] _index the SPI channel index
 * @return 0(success) or other values(failure)
 */
int32_t spi_master_deinit(const uint8_t _index);

/**
 * Receive data.
 *
 * @param [in]  _index the SPI channel index
 * @param [out] _buf the buffer to receive to
 * @param [in]  _size the size to receive
 * @return 0(success) or other values(failure)
 */
int32_t spi_master_receive(const uint8_t _index, uint8_t _buf[], const uint16_t _size);

/**
 * Send data.
 *
 * @param [in] _index the SPI channel index
 * @param [in] _buf the buffer to send from
 * @param [in] _size the size to send
 * @return 0(success) or other values(failure)
 */
int32_t spi_master_send(const uint8_t _index, const uint8_t _buf[], const uint16_t _size);

/**
 * Initialize the slave SPI.
 *
 * @param [in] _index the SPI channel index
 * @param [in] _cpol  the clock polarity:
 * <ul>
 * <li>{@link SPI_CPOL_LOW} clock is idle low.</li>
 * <li>{@link SPI_CPOL_HIGH} clock is idle high.</li>
 * </ul>
 * @param [in] _cpha the clock phase:
 * <ul>
 * <li>{@link SPI_CPHA_1EDGE} Data is sampled on clock 1st edge, changed on 2nd.</li>
 * <li>{@link SPI_CPHA_2EDGE} Data is sampled on clock 2nd edge, changed on 1st.</li>
 * </ul>
 * @param [in] _data_bits the data bits:
 * <ul>
 * <li>{@link SPI_DATA_BITS_8} 8 bits data</li>
 * <li>{@link SPI_DATA_BITS_16} 16 bits data</li>
 * </ul>
 * @param [in] _lsb_first if LSB or MSB first
 * @return 0(success) or other values(failure)
 */
int32_t spi_slave_init(const uint8_t _index, const uint8_t _cpol, const uint8_t _cpha, const uint8_t _data_bits, const bool _lsb_first);

/**
 * De-initialize the slave SPI.
 *
 * @param [in] _index the SPI channel index
 * @return 0(success) or other values(failure)
 */
int32_t spi_slave_deinit(const uint8_t _index);

/**
 * Receive data.
 *
 * @param [in] _index the SPI channel index
 * @param [out] _buf the buffer to receive to
 * @param [in] _size the size to be recieved
 * @return 0(success) or other values(failure)
 */
int32_t spi_slave_receive(const uint8_t _index, uint8_t _buf[], const uint16_t _size);

/**
 * Send data.
 *
 * @param [in] _index the SPI channel index
 * @param [in] _buf the buffer to send from
 * @param [in] _size the size to send
 * @return 0(success) or other values(failure)
 */
int32_t spi_slave_send(const uint8_t _index, const uint8_t _buf[], const uint16_t _size);

#ifdef __cplusplus
}
#endif

#endif /* __SPI_H__ */
