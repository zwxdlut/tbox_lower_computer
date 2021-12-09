#include "spi.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Local function prototypes
 ******************************************************************************/

/*******************************************************************************
 * Functions
 ******************************************************************************/
int32_t spi_master_init(const uint8_t _chl, const uint32_t _baudrate, const uint8_t _cpol, const uint8_t _cpha, const uint8_t _data_bits, const bool _lsb_first)
{
	assert(SPI_CH0 >= _chl);

	return 0;
}

int32_t spi_master_deinit(const uint8_t _chl)
{
	assert(SPI_CH0 >= _chl);

	return 0;
}

int32_t spi_master_receive(const uint8_t _chl, uint8_t _buf[], const uint16_t _size)
{
	assert(SPI_CH0 >= _chl && NULL != _buf);

	return 0;
}

int32_t spi_master_send(const uint8_t _chl, const uint8_t _buf[], const uint16_t _size)
{
	assert(SPI_CH0 >= _chl && NULL != _buf);

	return 0;
}

int32_t spi_slave_init(const uint8_t _chl, const uint8_t _cpol, const uint8_t _cpha, const uint8_t _data_bits, const bool _lsb_first)
{
	assert(SPI_CH0 >= _chl);

	return 0;
}

int32_t spi_slave_deinit(const uint8_t _chl)
{
	assert(SPI_CH0 >= _chl);

	return 0;
}

int32_t spi_slave_receive(const uint8_t _chl, uint8_t _buf[], const uint16_t _size)
{
	assert(SPI_CH0 >= _chl);

	return 0;
}

int32_t spi_slave_send(const uint8_t _chl, const uint8_t _buf[], const uint16_t _size)
{
	assert(SPI_CH0 >= _chl);
	
	return 0;
}

/*******************************************************************************
 * Local functions
 ******************************************************************************/
