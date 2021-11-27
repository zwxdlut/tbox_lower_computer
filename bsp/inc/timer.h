/*
 * timer.h
 *
 *  Created on: 2018年10月23日
 *      Author: Administrator
 */

#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdint.h>
#include <assert.h>

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * Definitions
 ******************************************************************************/
/**
 * @name Timer indexes
 * @{
 */
#define TIMER0_INDEX                            0
/** @} */ /* Timer indexes */
																	  
/******************************************************************************
 * Function prototypes
 ******************************************************************************/
/**
 * Initialize the timer.
 *
 * @param [in] _index the timer index
 * @param [in] _period the timer period in milliseconds
 * @return 0(success) or other values(failure)
 */

int32_t timer_init(const uint8_t _index, const uint32_t _period);

/**
 * Deinitialize the timer.
 *
 * @param [in] _index the timer index
 * @return 0(success) or other values(failure)
 */
int32_t timer_deinit(const uint8_t _index);

/**
 * Start the timer.
 *
 * @param [in] _index the timer index
 * @return 0(success) or other values(failure)
 */
int32_t timer_start(const uint8_t _index);

/**
 * Stop the timer.
 *
 * @param [in] _index the timer index
 * @return 0(success) or other values(failure)
 */
int32_t timer_stop(const uint8_t _index);

/**
 * Timer IRQ callback.
 *
 * This function can be implemented by user, default implemention is "weak".
 *
 * @param [in] _index the timer index
 */
void timer_irq_callback(const uint8_t _index);

#ifdef __cplusplus
}
#endif

#endif /* __TIMER_H__ */
