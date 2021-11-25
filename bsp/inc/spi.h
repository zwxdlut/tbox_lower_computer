/*
 * spi.h
 *
 *  Created on: 2018年10月16日
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
 * @name SPI channel indexes
 * @{
 */
#define SPI0_INDEX       		                0
/** @} */ /* SPI channel indexes */

/**
 * @name SPI clock polarity
 * @{
 */
#define SPI_CPOL_LOW      		                0
#define SPI_CPOL_HIGH      		                1
/** @} */ /* SPI clock polarity */

/**
 * @name SPI clock phase
 * @{
 */
#define SPI_CPHA_1EDGE      		            0
#define SPI_CPHA_2EDGE      		            1
/** @} */ /* SPI clock phase */

/**
 * @name SPI data bits
 * @{
 */
#define SPI_DATA_BITS_8      		            8
#define SPI_DATA_BITS_16      		            16
/** @} */ /* SPI data bits */

/*******************************************************************************
 * Function prototypes
 ******************************************************************************/
/**
 * Initialize the SPI master mode.
 *
 * @param [in] _index The SPI channel index
 * @param [in] _baudrate The baud rate of the SPI bus
 * @param [in] _cpol The clock polarity:
 * <ul>
 * <li>{@link SPI_CPOL_LOW} Clock is idle low.</li>
 * <li>{@link SPI_CPOL_HIGH} Clock is idle high.</li>
 * </ul>
 * @param [in] _cpha The clock phase:
 * <ul>
 * <li>{@link SPI_CPHA_1EDGE} Data is sampled on clock 1st edge, changed on 2nd.</li>
 * <li>{@link SPI_CPHA_2EDGE} Data is sampled on clock 2nd edge, changed on 1st.</li>
 * </ul>
 * @param [in] _data_bits The data bits:
 * <ul>
 * <li>{@link SPI_DATA_BITS_8} 8 bits data</li>
 * <li>{@link SPI_DATA_BITS_16} 16 bits data</li>
 * </ul>
 * @param [in] _lsb_first If LSB or MSB first
 * @return 0(success) or other values(failure).
 */
int32_t spi_master_init(const uint8_t _index, const uint32_t _baudrate, const uint8_t _cpol, const uint8_t _cpha, const uint8_t _data_bits, const bool _lsb_first);

/**
 * De-initialize the SPI master mode.
 *
 * @param [in] _index The SPI channel index
 * @return 0(success) or other values(failure).
 */
int32_t spi_master_deinit(const uint8_t _index);

/**
 * Receive data from the slave device.
 *
 * @param [in]  _index The SPI channel index
 * @param [out] _buf The buffer to receive
 * @param [in]  _size The size to receive
 * @return 0(success) or other values(failure).
 */
int32_t spi_master_receive(const uint8_t _index, uint8_t _buf[], const uint16_t _size);

/**
 * Send data to the slave device.
 *
 * @param [in] _index The SPI channel index
 * @param [in] _buf The buffer to send
 * @param [in] _size The size to send
 * @return 0(success) or other values(failure).
 */
int32_t spi_master_send(const uint8_t _index, const uint8_t _buf[], const uint16_t _size);

/**
 * Initialize the SPI slave mode.
 *
 * @param [in] _index The SPI channel index
 * @param [in] _cpol  The clock polarity:
 * <ul>
 * <li>{@link SPI_CPOL_LOW} clock is idle low.</li>
 * <li>{@link SPI_CPOL_HIGH} clock is idle high.</li>
 * </ul>
 * @param [in] _cpha The clock phase:
 * <ul>
 * <li>{@link SPI_CPHA_1EDGE} Data is sampled on clock 1st edge, changed on 2nd.</li>
 * <li>{@link SPI_CPHA_2EDGE} Data is sampled on clock 2nd edge, changed on 1st.</li>
 * </ul>
 * @param [in] _data_bits The data bits:
 * <ul>
 * <li>{@link SPI_DATA_BITS_8} 8 bits data</li>
 * <li>{@link SPI_DATA_BITS_16} 16 bits data</li>
 * </ul>
 * @param [in] _lsb_first If LSB or MSB first
 * @return 0(success) or other values(failure).
 */
int32_t spi_slave_init(const uint8_t _index, const uint8_t _cpol, const uint8_t _cpha, const uint8_t _data_bits, const bool _lsb_first);

/**
 * De-initialize the SPI slave mode.
 *
 * @param [in] _index The SPI channel index
 * @return 0(success) or other values(failure).
 */
int32_t spi_slave_deinit(const uint8_t _index);

/**
 * Receive data from the master device.
 *
 * @param [in] _index The SPI channel index
 * @param [out] _buf The buffer to receive
 * @param [in] _size The size to be recieved
 * @return 0(success) or other values(failure).
 */
int32_t spi_slave_receive(const uint8_t _index, uint8_t _buf[], const uint16_t _size);

/**
 * Send data to the master device.
 *
 * @param [in] _index The SPI channel index
 * @param [in] _buf The buffer to send
 * @param [in] _size The size to send
 * @return 0(success) or other values(failure).
 */
int32_t spi_slave_send(const uint8_t _index, const uint8_t _buf[], const uint16_t _size);

#ifdef __cplusplus
}
#endif

#endif /* __SPI_H__ */
