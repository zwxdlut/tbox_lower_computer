/*
 * i2c.c
 *
 *  Created on: 2019年1月9日
 *      Author: Administrator
 */

#include "i2c.h"

#if defined USING_OS_FREERTOS
extern SemaphoreHandle_t g_i2c_mutex[I2C0_INDEX + 1]; /* Receiving/Sending mutex */
#endif

/******************************************************************************
 * Definitions
 ******************************************************************************/
/******************************************************************************
 * Local function prototypes
 ******************************************************************************/
static int32_t i2c_master_read(const uint16_t _dev_addr, const uint8_t _mem__addr, uint8_t *const _buf, const uint16_t _size);

/******************************************************************************
 * Functions
 ******************************************************************************/
int32_t eeprom_read(const uint8_t _addr, uint8_t _buf[], const uint16_t _size)
{
	assert(NULL != _buf);

	int32_t ret = 0;

#if defined USING_OS_FREERTOS
	xSemaphoreTake( g_i2c_mutex[I2C0_INDEX], portMAX_DELAY);
#endif

	/* Sequential read */
	ret = i2c_master_read(EEPROM_ADDR, _addr, _buf, _size);

#if defined USING_OS_FREERTOS
	xSemaphoreGive( g_i2c_mutex[I2C0_INDEX] );
#endif

	return ret;
}

int32_t eeprom_write(const uint8_t _addr, const uint8_t _buf[], const uint16_t _size)
{
	assert(NULL != _buf);

	int32_t  ret = 0;
	uint8_t  addr = _addr;
	uint8_t  buf[EEPROM_PAGE_SIZE + 1];
	uint16_t size = _size;
	uint16_t n = 0;

#if defined USING_OS_FREERTOS
	xSemaphoreTake( g_i2c_mutex[I2C0_INDEX], portMAX_DELAY);
#endif

	/* Page write */
	while (0 == ret && 0 < size)
	{
		n = 8 - addr % 8;
		n = n >= size ? size : n;
		buf[0] = addr;
		memcpy(buf + 1, _buf + _size - size, n);
		ret = i2c_master_send(I2C0_INDEX, EEPROM_ADDR, buf, n + 1, true);
		addr += n;
		size -= n;
	}

#if defined USING_OS_FREERTOS
	xSemaphoreGive( g_i2c_mutex[I2C0_INDEX] );
#endif

	return ret;
}

int32_t accr_reset(void)
{
	int32_t ret = 0;
	uint8_t buf[2];

#if defined USING_OS_FREERTOS
	xSemaphoreTake( g_i2c_mutex[I2C0_INDEX], portMAX_DELAY);
#endif

	/* Write the ACTIVE bit to the desired system mode */
	buf[0] = ACCR_CTRL_REG2_REG;

	if (0 == (ret = i2c_master_read(ACCR_ADDR, buf[0], buf + 1, 1)))
	{
		buf[1] |= 0x40;
		ret = i2c_master_send(I2C0_INDEX, ACCR_ADDR, buf, 2, true);
	}

#if defined USING_OS_FREERTOS
	xSemaphoreGive( g_i2c_mutex[I2C0_INDEX] );
#endif

	return ret;
}

int32_t accr_init(const uint8_t _int_src)
{
	int32_t ret = 0;
	uint8_t buf[2];

#if defined USING_OS_FREERTOS
	xSemaphoreTake( g_i2c_mutex[I2C0_INDEX], portMAX_DELAY);
#endif

	/* Configure XYZ data */
	buf[0] = ACCR_XYZ_DATA_CFG_REG;
	buf[1] = 0x01;
	ret = i2c_master_send(I2C0_INDEX, ACCR_ADDR, buf, 2, true);

	/* Configure data rate 100HZ */
	buf[0] = ACCR_CTRL_REG1_REG;

	if (0 == ret && 0 == (ret = i2c_master_read(ACCR_ADDR, buf[0], buf + 1, 1)))
	{
		buf[1] = (0xC7 & buf[1]) | 0x18;
		ret = i2c_master_send(I2C0_INDEX, ACCR_ADDR, buf, 2, true);		
	}

	if (0 == ret && (ACCR_INT_PULSE_MASK & _int_src))
	{
		/* Pulse detection. Configure register PULSE_CFG, PULSE_THSX,Y,Z, PULSE_TMLT, PULSE_LTCY, PULSE_WIND.
		   Configure configured for single tap and double tap with latch enabled */
		buf[0] = ACCR_PULSE_CFG_REG;
		buf[1] = 0x7F;
		ret = i2c_master_send(I2C0_INDEX, ACCR_ADDR, buf, 2, true);

		/* Set x threshold to 32 counts or 2g */
		buf[0] = ACCR_PULSE_THSX_REG;
		buf[1] = 0x20;
		if (0 == ret)
		{
			ret = i2c_master_send(I2C0_INDEX, ACCR_ADDR, buf, 2, true);
		}

		/* set y threshold to 32 counts or 2g */
		buf[0] = ACCR_PULSE_THSY_REG;
		buf[1] = 0x20;
		if (0 == ret)
		{
			ret = i2c_master_send(I2C0_INDEX, ACCR_ADDR, buf, 2, true);
		}

		/* Set z threshold to 48 counts or 3g */
		buf[0] = ACCR_PULSE_THSZ_REG;
		buf[1] = 0x30;
		if (0 == ret)
		{
			ret = i2c_master_send(I2C0_INDEX, ACCR_ADDR, buf, 2, true);
		}

		/* Set the pulse time limit to 30 ms at 100Hz ODR in normal mode without LPF: 30 ms/2.5 ms = 12 counts */
		buf[0] = ACCR_PULSE_TMLT_REG;
		buf[1] = 0x0C;
		if (0 == ret)
		{
			ret = i2c_master_send(I2C0_INDEX, ACCR_ADDR, buf, 2, true);
		}

		/* Set the pulse latency timer to 200ms, 100Hz ODR in normal mode without LPF: 200 ms/5.0 ms = 40 counts */
		buf[0] = ACCR_PULSE_LTCY_REG;
		buf[1] = 0x28;
		if (0 == ret)
		{
			ret = i2c_master_send(I2C0_INDEX, ACCR_ADDR, buf, 2, true);
		}

		/* Set the pulse window to 300 ms, 800Hz ODR in normal mode, without LPF: 300 ms/5 ms = 60 counts */
		buf[0] = ACCR_PULSE_WIND_REG;
		buf[1] = 0x3C;
		if (0 == ret)
		{
			ret = i2c_master_send(I2C0_INDEX, ACCR_ADDR, buf, 2, true);
		}
	}

	/* Enable the interrupt */
	buf[0] = ACCR_CTRL_REG4_REG;
	buf[1] = _int_src;
	if (0 == ret)
	{
		ret = i2c_master_send(I2C0_INDEX, ACCR_ADDR, buf, 2, true);
	}

#if defined USING_OS_FREERTOS
	xSemaphoreGive( g_i2c_mutex[I2C0_INDEX] );
#endif

	return ret;
}

int32_t accr_deinit(void)
{
	return 0;
}

int32_t accr_sys_mode_trans(const uint8_t _mode)
{
	int32_t ret = 0;
	uint8_t buf[2];

#if defined USING_OS_FREERTOS
	xSemaphoreTake( g_i2c_mutex[I2C0_INDEX], portMAX_DELAY);
#endif

	/* Write the ACTIVE bit to the desired system mode */
	buf[0] = ACCR_CTRL_REG1_REG;

	if (0 == (ret = i2c_master_read(ACCR_ADDR, buf[0], buf + 1, 1)))
	{
		switch(_mode)
		{
		case ACCR_SYSMOD_STANDBY:
			buf[1] &= 0xFE;
			break;

		case ACCR_SYSMOD_ACTIVE:
			buf[1] |= 0x01;
			break;
			
		default:
			break;
		}
	}

	if (0 == ret)
	{
		ret = i2c_master_send(I2C0_INDEX, ACCR_ADDR, buf, 2, true);
	}

#if defined USING_OS_FREERTOS
	xSemaphoreGive( g_i2c_mutex[I2C0_INDEX] );
#endif

	return ret;
}

uint8_t accr_get_int_src(void)
{
	uint8_t ret = 0;

#if defined USING_OS_FREERTOS
	xSemaphoreTake( g_i2c_mutex[I2C0_INDEX], portMAX_DELAY);
#endif

	/* Read the interrupt source register */
	i2c_master_read(ACCR_ADDR, ACCR_INT_SOURCE_REG, &ret, 1);

#if defined USING_OS_FREERTOS
	xSemaphoreGive( g_i2c_mutex[I2C0_INDEX] );
#endif

	return ret;
}

int32_t accr_get_xyz_sample(uint8_t _buf[], const uint8_t _size)
{
	assert(NULL != _buf);

	int32_t ret = 0;

#if defined USING_OS_FREERTOS
	xSemaphoreTake( g_i2c_mutex[I2C0_INDEX], portMAX_DELAY);
#endif

	/* Read the X, Y, Z-axis sample data */
	ret = i2c_master_read(ACCR_ADDR, ACCR_OUT_X_MSB_REG, _buf, _size);

#if defined USING_OS_FREERTOS
	xSemaphoreGive( g_i2c_mutex[I2C0_INDEX] );
#endif

	return ret;
}

/******************************************************************************
 * Local functions
 ******************************************************************************/
/**
 * Read data from the specified slave device memory.
 *
 * @param [in] _dev_addr The slave device address
 * @param [in] _mem_addr The slave device memory address
 * @param [in] _buf The buffer to read to
 * @param [in] _size The size to read
 * @return 0(success) or other values(failure).
 */
static int32_t i2c_master_read(const uint16_t _dev_addr, const uint8_t _mem_addr, uint8_t *const _buf, const uint16_t _size)
{
	assert(NULL != _buf);

	if (0 != i2c_master_send(I2C0_INDEX, _dev_addr, &_mem_addr, 1, false))
	{
		return -1;
	}

	return i2c_master_receive(I2C0_INDEX, _dev_addr, _buf, _size, true);
}
