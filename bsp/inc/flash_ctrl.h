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
 * Initialize the flash controller.
 *
 * @return 0(success) or other values(failure)
 */
int32_t flash_ctrl_init(void);

/**
 * De-initialize the flash controller.
 *
 * @return 0(success) or other values(failure)
 */
int32_t flash_ctrl_deinit(void);

/** 
 * Erase continuous sectors.
 *
 * @param [in] _addr the sector start address to erase
 * @param [in] _size the size to erase which must be aligned to sector
 * @return 0(success) or other values(failure)
 */
int32_t flash_ctrl_erase_sector(const uint32_t _addr, const uint32_t _size);

/**
 * Verify continuous sectors.
 *
 * @param [in] _addr the sector start address to verify
 * @param [in] _size the size to verify which must be aligned to sector
 * @return 0(success) or other values(failure)
 */
int32_t flash_ctrl_verify_sector(const uint32_t _addr, const uint32_t _size);

/**
 * Check if an address is sector aligned.
 *
 * @param [in] _addr the address to be checked
 * @return true(aligned) or false(not alined)
 */
bool flash_ctrl_is_sector_aligned(const uint32_t _addr);

/**
 * Program a flash memory area.
 *
 * @param [in] _addr the start address of the flash memory area to program
 * @param [in] _size the size of the flash memory area to program
 * @param [in] _buf the buffer to program from
 * @return 0(success) or other values(failure)
 */
int32_t flash_ctrl_program(const uint32_t _addr, const uint32_t _size, const uint8_t _buf[]);

/**
 * Verify a programed flash memory area.
 *
 * @param [in] _addr the start address of the programed flash memory area to be verified
 * @param [in] _size the size of the programed flash memory area to verify
 * @param [in] _buf the buffer to verify from
 * @return 0(success) or other values(failure)
 */
int32_t flash_ctrl_program_verify(const uint32_t _addr, const uint32_t _size, const uint8_t _buf[]);

/**
 * Write data to a flash memory area which is partitioned as EEPROM.
 *
 * @param [in] _addr the start address of the flash memory area to write
 * @param [in] _size the size of the flash memory area write
 * @param [in] _buf the data buffer to write from
 * @return 0(success) or other values(failure)
 */
int32_t flash_ctrl_write_e2(const uint32_t _addr, const uint32_t _size, const uint8_t _buf[]);

#ifdef __cplusplus
}
#endif

#endif /* __FLASH_CTRL_H__ */
