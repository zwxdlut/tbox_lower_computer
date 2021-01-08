/*
 * timer.h
 *
 *  Created on: 2018��10��23��
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
 * @name Timer module index.
 * @{
 */
#define TIMER0_INDEX                            0
/** @} */ // Timer module index.
																	  
/******************************************************************************
 * Function prototypes
 ******************************************************************************/
/**
 * Initialize timer.
 *
 * @param [in] _index  Timer index
 * @param [in] _period Timer period in milliseconds
 * @return Success(0) or failure(other values).
 */

int32_t timer_init(const uint8_t _index, const uint32_t _period);

/**
 * Deinitialize timer.
 *
 * @param [in] _index Timer index
 * @return Success(0) or failure(other values).
 */
int32_t timer_deinit(const uint8_t _index);

/**
 * Start timer.
 *
 * @param [in] _index Timer index
 * @return Success(0) or failure(other values).
 */
int32_t timer_start(const uint8_t _index);

/**
 * Stop timer.
 *
 * @param [in] _index Timer index
 * @return Success(0) or failure(other values).
 */
int32_t timer_stop(const uint8_t _index);

/**
 * Timer IRQ callback.
 *
 * This weak function can be implemented by user.
 *
 * @param [in] _index Timer index
 */
void timer_irq_callback(const uint8_t _index);

#ifdef __cplusplus
}
#endif

#endif // __TIMER_H__
