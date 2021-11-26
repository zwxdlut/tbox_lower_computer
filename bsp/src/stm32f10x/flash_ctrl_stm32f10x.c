/*
 * flash_ctrl_stm32f10x.c
 *
 *  Created on: 2018年10月17日
 *      Author: Administrator
 */

#include "flash_ctrl.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Local function prototypes
 ******************************************************************************/
static uint32_t get_sector(uint32_t _addr);
static uint32_t get_sector_addr(const uint32_t _sector);
static uint32_t get_sector_size(const uint32_t _sector);

/*******************************************************************************
 * Functions
 ******************************************************************************/
int32_t flash_ctrl_init(void)
{
	return 0;
}

int32_t flash_ctrl_deinit(void)
{
	return 0;
}

int32_t flash_ctrl_erase_sector(const uint32_t _addr, const uint32_t _size)
{
	assert(_addr >= FLASH_BASE_ADDR && (_addr + _size <= FLASH_BASE_ADDR + FLASH_TOTAL_SIZE));
	
	int32_t  ret = 0;
	uint32_t start_sector_addr = get_sector_addr(get_sector(_addr));
	uint32_t end_sector_addr = get_sector_addr(get_sector(_addr + _size - 1));
	
	ret = FLASH_COMPLETE;

	FLASH_Unlock();

	while (FLASH_COMPLETE == ret && start_sector_addr <= end_sector_addr)
	{
		ret = FLASH_ErasePage(start_sector_addr);
		start_sector_addr += get_sector_size(get_sector(start_sector_addr));
	}

	FLASH_Lock();

	if (FLASH_COMPLETE == ret)
	{
		ret = 0;
	}

    return ret;
}

int32_t flash_ctrl_verify_sector(const uint32_t _addr, const uint32_t _size)
{
	assert(_addr >= FLASH_BASE_ADDR && (_addr + _size <= FLASH_BASE_ADDR + FLASH_TOTAL_SIZE));

	int32_t  ret = 0;
	uint32_t i = 0;
	
	while (i < _size)
	{
		if (_size - i >= 4)
		{
			if (0xFFFFFFFF != *(__IO uint32_t *)(_addr + i))
			{
				ret++;
			}

			i += 4;
		}
		else if (_size - i >= 2)
		{
			if (0xFFFF != *(__IO uint16_t *)(_addr + i))
			{
				ret++;
			}

			i += 2;
		}
		else
		{
			if (0xFF != *(__IO uint8_t *)(_addr + i))
			{
				ret++;
			}

			i++;
		}
	}

    return ret;
}

bool flash_ctrl_is_sector_aligned(const uint32_t _addr)
{
	assert(_addr >= FLASH_BASE_ADDR && _addr <= FLASH_BASE_ADDR + FLASH_TOTAL_SIZE);

	return (0 == (_addr - FLASH_BASE_ADDR) % FLASH_SECTOR_SIZE );	
}

int32_t flash_ctrl_program(const uint32_t _addr, const uint32_t _size, const uint8_t _buf[])
{
	assert(_addr >= FLASH_BASE_ADDR && (_addr + _size <= FLASH_BASE_ADDR + FLASH_TOTAL_SIZE) && NULL != _buf);
	assert(0 == _size % 2);
	
	int32_t  ret = 0;
	uint32_t i = 0;
	
	ret = FLASH_COMPLETE;

	FLASH_Unlock();

	while (FLASH_COMPLETE == ret && i < _size)
	{
		if (_size - i >= 4)
		{			
			ret = FLASH_ProgramWord( _addr + i, *((uint32_t*)(_buf + i)));
			i += 4;
		}
		else
		{
			ret = FLASH_ProgramHalfWord(_addr + i, *((uint16_t*)(_buf + i)));
			i += 2;
		}
	}

	FLASH_Lock();

	if (FLASH_COMPLETE == ret)
	{
		ret = 0;
	}

    return ret;
}

int32_t flash_ctrl_program_verify(const uint32_t _addr, const uint32_t _size, const uint8_t _buf[])
{
	assert(_addr >= FLASH_BASE_ADDR && (_addr + _size <= FLASH_BASE_ADDR + FLASH_TOTAL_SIZE) && NULL != _buf);

	int32_t  ret = 0;
	uint32_t i = 0;

	while (i < _size)
	{
		if (_size - i >= 4)
		{
			if (*((uint32_t *)(_buf + i)) != *(__IO uint32_t *)(_addr + i))
			{
				ret++;
			}

			i += 4;
		}
		else if (_size - i >= 2)
		{
			if (*((uint16_t *)(_buf + i)) != *(__IO uint16_t *)(_addr + i))
			{
				ret++;
			}

			i += 2;
		}
		else
		{
			if (*((uint8_t *)(_buf + i)) != *(__IO uint8_t *)(_addr + i))
			{
				ret++;
			}

			i++;
		}
    }

    return ret;
}

int32_t flash_ctrl_write_e2(const uint32_t _addr, const uint32_t _size, const uint8_t _buf[])
{
	return 0;
}

/*******************************************************************************
 * Local function prototypes
 ******************************************************************************/
/**
 * Get the sector of a given address.
 *
 * @param [in] _addr A sector start address
 * @return The sector of the given address.
 */
static uint32_t get_sector(const uint32_t _addr)
{
	assert(_addr >= FLASH_BASE_ADDR && _addr <= FLASH_BASE_ADDR + FLASH_TOTAL_SIZE);

	return ((_addr - FLASH_BASE_ADDR) / FLASH_SECTOR_SIZE);
}

/**
 * Get the address of a given sector.
 *
 * @param [in] _sector A sector
 * @return The address of the given sector.
 */
static uint32_t get_sector_addr(const uint32_t _sector)
{
	assert(FLASH_TOTAL_SIZE / FLASH_SECTOR_SIZE -1 >= _sector);

	return (FLASH_BASE_ADDR + _sector * FLASH_SECTOR_SIZE);
}

/**
 * Get a sector size.
 *
 * @param [in] _sector A sector
 * @return The size of the given sector.
 */
static uint32_t get_sector_size(const uint32_t _sector)
{
	assert((FLASH_TOTAL_SIZE / FLASH_SECTOR_SIZE -1) >= _sector);
	
	return FLASH_SECTOR_SIZE;
}
