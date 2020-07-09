/*
 * spi_stm32f10x.c
 *
 *  Created on: 2018��10��16��
 *      Author: Administrator
 */

#include "spi.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*******************************************************************************
 * Local Function prototypes
 ******************************************************************************/
/*******************************************************************************
 * Functions
 ******************************************************************************/
int32_t spi_master_init(const uint8_t _index, const uint32_t _baudrate, const uint8_t _cpol, const uint8_t _cpha, const uint8_t _data_bits, const bool _lsb_first)
{
	assert(SPI0_INDEX >= _index);

	return 0;
}

int32_t spi_master_deinit(const uint8_t _index)
{
	assert(SPI0_INDEX >= _index);

	return 0;
}

int32_t spi_master_receive(const uint8_t _index, uint8_t *const _buf, const uint16_t _size)
{
	assert(SPI0_INDEX >= _index && NULL != _buf);
		
	return 0;
}

int32_t spi_master_transmit(const uint8_t _index, const uint8_t *const _buf, const uint16_t _size)
{
	assert(SPI0_INDEX >= _index && NULL != _buf);
		
	return 0;
}

int32_t spi_slave_init(const uint8_t _index, const uint8_t _cpol, const uint8_t _cpha, const uint8_t _data_bits, const bool _lsb_first)
{
	assert(SPI0_INDEX >= _index);

	return 0;
}

int32_t spi_slave_deinit(const uint8_t _index)
{
	assert(SPI0_INDEX >= _index);

	return 0;
}

int32_t spi_slave_receive(const uint8_t _index, uint8_t *const _buf, const uint16_t _size)
{
	assert(SPI0_INDEX >= _index);

	return 0;
}

int32_t spi_slave_transmit(const uint8_t _index, const uint8_t *const _buf, const uint16_t _size)
{
	assert(SPI0_INDEX >= _index);

	return 0;
}

/*******************************************************************************
 * Local Functions
 ******************************************************************************/
