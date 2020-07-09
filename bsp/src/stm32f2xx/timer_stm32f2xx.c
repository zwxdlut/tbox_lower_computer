/*
 * timer_stm32f2xx.c
 *
 *  Created on: 2018��10��23��
 *      Author: Administrator
 */

#include "timer.h"

/******************************************************************************
 * Definitions
 ******************************************************************************/
static timer_confg_t g_timer_config[TIMER0_INDEX + 1] =
{
	{
		.clk_  = TIMER0_CLK,
		.irq_  = TIMER0_IRQ
	}
};

static TIM_HandleTypeDef g_handle[TIMER0_INDEX + 1] = 
{
	{
		.Instance               = TIMER0_INST,
		.Init.CounterMode       = TIM_COUNTERMODE_UP,
		.Init.ClockDivision     = 0,
		.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE
	}
};

/******************************************************************************
 * Local Function prototypes
 ******************************************************************************/
static void timer_irq_handler(const uint8_t _index);

/******************************************************************************
 * Functions
 ******************************************************************************/
int32_t timer_init(const uint8_t _index, const uint32_t _period)
{
	assert(TIMER0_INDEX >= _index);

	/* Timer initialization */
	TIMER_CLK_ENABLE(_index);
	g_handle[_index].Init.Prescaler = 2 * HAL_RCC_GetPCLK1Freq() / g_timer_config[_index].clk_ - 1;
	g_handle[_index].Init.Period    = _period * g_timer_config[_index].clk_ / 1000 - 1;
	HAL_TIM_Base_Init(&g_handle[_index]);
	
	/* NVIC initialization */
	HAL_NVIC_SetPriority(g_timer_config[_index].irq_, 0, 0);
	HAL_NVIC_EnableIRQ(g_timer_config[_index].irq_);

    return 0;
}

int32_t timer_deinit(const uint8_t _index)
{
	assert(TIMER0_INDEX >= _index);

	HAL_NVIC_DisableIRQ(g_timer_config[_index].irq_);
	HAL_TIM_Base_Stop_IT(&g_handle[_index]);
	__HAL_TIM_CLEAR_IT(&g_handle[TIMER0_INDEX], TIM_IT_UPDATE);
	HAL_TIM_Base_DeInit(&g_handle[_index]);
	TIMER_CLK_DISABLE(_index);

	return 0;
}

int32_t timer_start(const uint8_t _index)
{
	assert(TIMER0_INDEX >= _index);

	HAL_TIM_Base_Start_IT(&g_handle[_index]);

    return 0;
}

int32_t timer_stop(const uint8_t _index)
{
	assert(TIMER0_INDEX >= _index);

	HAL_TIM_Base_Stop_IT(&g_handle[_index]);

	return 0;
}

__attribute__((weak)) void timer_irq_callback(const uint8_t _index)
{
	(void)_index;
}

/**
 * @name IRQ handlers.
 * @{
 */
/**
 * @brief Timer0 IRQ handler.
 */
void TIMER0_IRQ_HANDLER(void)   
{
	timer_irq_handler(TIMER0_INDEX);
}
/** @} */ /* IRQ handlers. */

/******************************************************************************
 * Local Functions
 ******************************************************************************/
/**
 * @brief Timer IRQ handler.
 *
 * @param [in] _index Timer index.
 */
static void timer_irq_handler(const uint8_t _index)
{
	/* Timer Update event */
	if (RESET != __HAL_TIM_GET_FLAG(&g_handle[_index], TIM_FLAG_UPDATE) && RESET != __HAL_TIM_GET_IT_SOURCE(&g_handle[_index], TIM_IT_UPDATE))
	{
		__HAL_TIM_CLEAR_IT(&g_handle[_index], TIM_IT_UPDATE);
		timer_irq_callback(_index);	
	}
}
