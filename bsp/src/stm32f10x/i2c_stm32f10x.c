/*
 * i2c_stm32f10x.c
 *
 *  Created on: 2019��1��9��
 *      Author: Administrator
 */

#include "i2c.h"

/******************************************************************************
 * Definitions
 ******************************************************************************/
#if defined USING_OS_FREERTOS
SemaphoreHandle_t g_i2c_mutex[I2C0_INDEX + 1] = {NULL}; // Rx/Tx Mutex
#endif

/******************************************************************************
 * Local Function prototypes
 ******************************************************************************/
/******************************************************************************
 * Functions
 ******************************************************************************/
int32_t i2c_master_init(const uint8_t _index, const uint32_t _baudrate, const bool _is_10bit_addr)
{
	assert(I2C0_INDEX >= _index);
	return 0;
}

int32_t i2c_master_deinit(const uint8_t _index)
{
	assert(I2C0_INDEX >= _index);
	return 0;
}

int32_t i2c_master_receive(const uint8_t _index, const uint16_t _addr, uint8_t *const _buf, const uint16_t _size, const bool _stop)
{
	assert(I2C0_INDEX >= _index && NULL != _buf);
	return 0;
}

int32_t i2c_master_transmit(const uint8_t _index, const uint16_t _addr, const uint8_t *const _buf, const uint16_t _size, const bool _stop)
{
	assert(I2C0_INDEX >= _index && NULL != _buf);
	return 0;
}

/**
 * @name IRQ handlers.
 * @{
 */
/**
 * I2C0 event IRQ handler.
 */
void I2C0_EV_IRQ_HANDLER(void)
{
}

/**
 * I2C0 error IRQ handler.
 */
void I2C0_ER_IRQ_HANDLER(void)
{
}
/** @} */ // IRQ handlers.

/******************************************************************************
 * Local Functions
 ******************************************************************************/
