/*
 * flash_ctrl.h
 *
 *  Created on: 2018��10��17��
 *      Author: Administrator
 */

#ifndef __FLASH_CTRL_H__
#define __FLASH_CTRL_H__

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*******************************************************************************
 * Function prototypes
 ******************************************************************************/
/**
 * @brief  Initialize flash control module.
 *
 * @return Success(0) or failure(other values).
 */
int32_t flash_ctrl_init(void);

/**
 * @brief  De-initialize flash control module.
 *
 * @return Success(0) or failure(other values).
 */
int32_t flash_ctrl_deinit(void);

/** 
 * @brief  Erase continuous sectors.
 *
 * @param  [in] _addr Sector start address.
 * @param  [in] _size Erase size aligned to sector.
 * @return Success(0) or failure(other values).
 */
int32_t flash_ctrl_erase_sector(const uint32_t _addr, const uint32_t _size);

/**
 * @brief  Verify continuous sectors.
 *
 * @param  [in] _addr Sector start address.
 * @param  [in] _size Erase size aligned to sector.
 * @return Success(0) or failure(other values).
 */
int32_t flash_ctrl_verify_sector(const uint32_t _addr, const uint32_t _size);

/**
 * @brief  Check if the the address is sector aligned.
 *
 * @param  [in] _addr Address.
 * @return Aligned(true) or not(false).
 */
bool flash_ctrl_is_sector_aligned(const uint32_t _addr);

/**
 * @brief  Program flash memory.
 *
 * @param  [in] _addr Program start address.
 * @param  [in] _size Program size.
 * @param  [in] _buf  Program buffer.
 * @return Success(0) or failure(other values).
 */
int32_t flash_ctrl_program(const uint32_t _addr, const uint32_t _size, const uint8_t *const _buf);

/**
 * @brief  Verify programed flash memory.
 *
 * @param  [in] _addr Verify start address.
 * @param  [in] _size Verify size.
 * @param  [in] _buf  Verify buffer.
 * @return Success(0) or failure(other values).
 */
int32_t flash_ctrl_program_verify(const uint32_t _addr, const uint32_t _size, const uint8_t *const _buf);

/**
 * @brief  Write data to a flash area which is partitioned as EEPROM.
 *
 * @param  [in] _addr Write start address.
 * @param  [in] _size Write size.
 * @param  [in] _buf  Write buffer.
 * @return Success(0) or failure(other values).
 */
int32_t flash_ctrl_write_e2(const uint32_t _addr, const uint32_t _size, const uint8_t *const _buf);

#ifdef __cplusplus
}
#endif

#endif /* __FLASH_CTRL_H__ */
