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
 * @name SPI module index.
 * @{
 */
#define SPI0_INDEX       		                0
/** @} */ // SPI module index.

/**
 * @name SPI clock polarity.
 * @{
 */
#define SPI_CPOL_LOW      		                0
#define SPI_CPOL_HIGH      		                1
/** @} */ // SPI clock polarity.

/**
 * @name SPI clock phase.
 * @{
 */
#define SPI_CPHA_1EDGE      		            0
#define SPI_CPHA_2EDGE      		            1
/** @} */ // SPI clock phase.

/**
 * @name SPI data bits.
 * @{
 */
#define SPI_DATA_BITS_8      		            8
#define SPI_DATA_BITS_16      		            16
/** @} */ // SPI data bits.

/*******************************************************************************
 * Function prototypes
 ******************************************************************************/
/**
 * Initialize master SPI.
 *
 * @param [in] _index SPI index
 * @param [in] _baudrate Baud rate
 * @param [in] _cpol Clock polarity:
 *  - SPI_CPOL_LOW: Clock is idle low
 *  - SPI_CPOL_HIGH: Clock is idle high
 * @param [in] _cpha Clock phase:
 *  - SPI_CPHA_1EDGE: Data is sampled on clock 1st edge, changed on 2nd
 *  - SPI_CPHA_2EDGE: Data is sampled on clock 2nd edge, changed on 1st
 * @param [in] _data_bits Data bits:
 *  - SPI_DATA_BITS_8: 8 bits data
 *  - SPI_DATA_BITS_16: 16 bits data
 * @param [in] _lsb_first Select LSB or MSB first
 * @return Success(0) or failure(other values).
 */
int32_t spi_master_init(const uint8_t _index, const uint32_t _baudrate, const uint8_t _cpol, const uint8_t _cpha, const uint8_t _data_bits, const bool _lsb_first);

/**
 * De-initialize master SPI.
 *
 * @param [in] _index SPI index
 * @return Success(0) or failure(other values).
 */
int32_t spi_master_deinit(const uint8_t _index);

/**
 * Receive master SPI data.
 *
 * @param [in]  _index SPI index
 * @param [out] _buf Receive buffer
 * @param [in]  _size Receive size
 * @return Success(0) or failure(other values).
 */
int32_t spi_master_receive(const uint8_t _index, uint8_t *const _buf, const uint16_t _size);

/**
 * Transmit master SPI data.
 *
 * @param [in] _index SPI index
 * @param [in] _buf Transmit buffer
 * @param [in] _size Transmit size
 * @return Success(0) or failure(other values).
 */
int32_t spi_master_transmit(const uint8_t _index, const uint8_t *const _buf, const uint16_t _size);

/**
 * Initialize slave SPI.
 *
 * @param [in] _index SPI index
 * @param [in] _cpol  Clock polarity:
 *  - SPI_CPOL_LOW: Signal is idle low
 *  - SPI_CPOL_HIGH: Signal is idle high
 * @param [in] _cpha Clock phase:
 *  - SPI_CPOL_1EDGE: Data is sampled on SCK 1st edge, changed on 2nd
 *  - SPI_CPOL_2EDGE: Data is sampled on SCK 2nd edge, changed on 1st
 * @param [in] _data_bits Data bits:
 *  - SPI_DATA_BITS_8: 8 bits data
 *  - SPI_DATA_BITS_16: 16 bits data
 * @param [in] _lsb_first Select LSB or MSB first
 * @return Success(0) or failure(other values).
 */
int32_t spi_slave_init(const uint8_t _index, const uint8_t _cpol, const uint8_t _cpha, const uint8_t _data_bits, const bool _lsb_first);

/**
 * De-initialize slave SPI.
 *
 * @param [in] _index SPI index
 * @return Success(0) or failure(other values).
 */
int32_t spi_slave_deinit(const uint8_t _index);

/**
 * Receive slave SPI data.
 *
 * @param [in] _index SPI index
 * @param [out] _buf Receive buffer
 * @param [in] _size Receive size
 * @return Success(0) or failure(other values).
 */
int32_t spi_slave_receive(const uint8_t _index, uint8_t *const _buf, const uint16_t _size);

/**
 * Transmit slave SPI data.
 *
 * @param [in] _index SPI index
 * @param [in] _buf Transmit buffer
 * @param [in] _size Transmit size
 * @return Success(0) or failure(other values).
 */
int32_t spi_slave_transmit(const uint8_t _index, const uint8_t *const _buf, const uint16_t _size);

#ifdef __cplusplus
}
#endif

#endif // __SPI_H__
