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
 * @name Power modes.
 * @{
 */
#define PWR_MODE_SLEEP                          0
#define PWR_MODE_DEEPSLEEP                      1
/** @} */ /* Power modes. */

/*******************************************************************************
 * Function prototypes
 ******************************************************************************/
/**
 * @brief  Initialize clocks, etc...
 *
 * @return Success(0) or failure(other values).
 */
int32_t sys_init(void);

/**
 * @brief Initialize GPIO.
 */
void gpio_init(void);

/**
 * @brief Deinitialize GPIO.
 */
void gpio_deinit(void);

/**
 * @brief  Get current system time since startup.
 *
 * @return Time in milliseconds.
 */
uint32_t sys_time(void);

/**
 * @brief Delay milliseconds function.
 *
 * @param [in] _ms Time in milliseconds.
 */
void delay(const uint32_t _ms);

/**
 * @brief Reset system.
 */
void sys_reset(void);

/**
 * @brief Transfer power mode.
 *
 * @param [in] _mode Power mode:
 *  - PWR_MODE_SLEEP:     Sleep.
 *  - PWR_MODE_DEEPSLEEP: Deep sleep.
 */
void pwr_mode_trans(const uint8_t _mode);

/**
 * @brief Enable watch dog.
 *
 * @return Success(0) or failure(other values).
 */
int32_t wdog_enable(void);

/**
 * @brief Refresh watch dog.
 *
 * @return Success(0) or failure(other values).
 */
int32_t wdog_refresh(void);

/**
 * @brief Disable watch dog.
 *
 * @return Success(0) or failure(other values).
 */
int32_t wdog_disable(void);

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H__ */
