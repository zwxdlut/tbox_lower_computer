/*
 * timer_s32k1xx.c
 *
 *  Created on: 2018年10月23日
 *      Author: Administrator
 */

#include "timer.h"

/******************************************************************************
 * Definitions
 ******************************************************************************/
static IRQn_Type g_timer_irq[TIMER0_INDEX + 1] = {TIMER0_IRQ};

static uint8_t g_handle[TIMER0_INDEX + 1] =
{
#if defined INST_LPTMR0
	INST_LPTMR0,
#else
	0xFF
#endif
};

static lptmr_config_t *g_config[TIMER0_INDEX + 1] =
{
#if defined INST_LPTMR0
	&lpTmr0_config0,
#else
	NULL
#endif
};

/******************************************************************************
 * Local function prototypes
 ******************************************************************************/
static void timer_irq_handler(void);

/******************************************************************************
 * Functions
 ******************************************************************************/
int32_t timer_init(const uint8_t _index, const uint32_t _period)
{
	assert(TIMER0_INDEX >= _index);

	/* Initialize LPTMR as timer:
	   - interrupt after _period milliseconds
	   - SIRC as clock source
	   - counter disabled */
	LPTMR_DRV_Init(g_handle[_index], g_config[_index], false);

	/* install the IRQ handler for LPTMR interrupt */
	INT_SYS_InstallHandler(g_timer_irq[_index], timer_irq_handler, (isr_t *)0);

	/* enable the IRQ for LPTMR */
	INT_SYS_EnableIRQ(g_timer_irq[_index]);

    return 0;
}

int32_t timer_deinit(const uint8_t _index)
{
	assert(TIMER0_INDEX >= _index);

	INT_SYS_DisableIRQ(g_timer_irq[_index]);
    LPTMR_DRV_ClearCompareFlag(g_handle[_index]);
	LPTMR_DRV_Deinit(g_handle[_index]);

	return 0;
}

int32_t timer_start(const uint8_t _index)
{
	assert(TIMER0_INDEX >= _index);

    /* start the LPTMR counter */
    LPTMR_DRV_StartCounter(g_handle[_index]);

    return 0;
}

int32_t timer_stop(const uint8_t _index)
{
	assert(TIMER0_INDEX >= _index);

    /* stop the LPTMR counter */
	LPTMR_DRV_StopCounter(g_handle[_index]);

	return 0;
}

__attribute__((weak)) void timer_irq_callback(const uint8_t _index)
{
	(void)_index;
}

/******************************************************************************
 * Local functions
 ******************************************************************************/
/**
 * Timer IRQ handler.
 */
static void timer_irq_handler(void)
{
	for (uint8_t i = TIMER0_INDEX; i < TIMER0_INDEX + 1; i++)
	{
		/* check the compare flag */
		while (LPTMR_DRV_GetCompareFlag(g_handle[i]))
		{
		    /* clear the compare flag */
			LPTMR_DRV_ClearCompareFlag(g_handle[i]);
			
			timer_irq_callback(i);
		}
	}
}
