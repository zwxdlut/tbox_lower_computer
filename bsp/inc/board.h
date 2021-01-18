/*
 * board.h
 *
 *  Created on: 2018��8��21��
 *      Author: Administrator
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/**
 * @name The power modes
 * 
 * @{
 */
#define PWR_MODE_SLEEP                          0
#define PWR_MODE_DEEPSLEEP                      1
/** @} */ // The power modes

/*******************************************************************************
 * Function prototypes
 ******************************************************************************/
/**
 * Initialize the clocks, etc...
 *
 * @return 0(success) or other values(failure)
 */
int32_t sys_init(void);

/**
 * Initialize the GPIOs.
 */
void gpio_init(void);

/**
 * De-initialize the GPIOs.
 */
void gpio_deinit(void);

/**
 * Get current system time since startup.
 *
 * @return current time in milliseconds
 */
uint32_t sys_time(void);

/**
 * Delay specified time in milliseconds.
 *
 * @param [in] _ms the time in milliseconds to delay 
 */
void delay(const uint32_t _ms);

/**
 * Reset the system.
 */
void sys_reset(void);

/**
 * Transfer the power mode.
 *
 * @param [in] _mode the power mode to transfer to:
 * <ul>
 * <li>{@link PWR_MODE_SLEEP} sleep</li>
 * <li>{@link PWR_MODE_DEEPSLEEP} deep sleep</li>
 * </ul>
 */
void pwr_mode_trans(const uint8_t _mode);

/**
 * Enable the watch dog.
 *
 * @return 0(success) or other values(failure)
 */
int32_t wdog_enable(void);

/**
 * Refresh the watch dog.
 *
 * @return 0(success) or other values(failure)
 */
int32_t wdog_refresh(void);

/**
 * Disable the watch dog.
 *
 * @return 0(success) or other values(failure)
 */
int32_t wdog_disable(void);

#ifdef __cplusplus
}
#endif

#endif // __BOARD_H__
