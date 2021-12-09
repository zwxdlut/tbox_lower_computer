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
 * @name Timer numbers
 * @{
 */
#define TIMER0                                  0
/** @} */ /* Timer numbers */
																	  
/******************************************************************************
 * Function prototypes
 ******************************************************************************/
/**
 * Initialize a timer.
 *
 * @param [in] _num the timer number
 * @param [in] _period the timer period in milliseconds
 * @return 0(success) or other values(failure)
 */

int32_t timer_init(const uint8_t _num, const uint32_t _period);

/**
 * De-initialize a timer.
 *
 * @param [in] _num the timer number
 * @return 0(success) or other values(failure)
 */
int32_t timer_deinit(const uint8_t _num);

/**
 * Start a timer.
 *
 * @param [in] _num the timer number
 * @return 0(success) or other values(failure)
 */
int32_t timer_start(const uint8_t _num);

/**
 * Stop a timer.
 *
 * @param [in] _num the timer number
 * @return 0(success) or other values(failure)
 */
int32_t timer_stop(const uint8_t _num);

/**
 * Timer IRQ callback.
 *
 * This function can be implemented by user, default implemention is "weak".
 *
 * @param [in] _num the timer number
 */
void timer_irq_callback(const uint8_t _num);

#ifdef __cplusplus
}
#endif

#endif /* __TIMER_H__ */
