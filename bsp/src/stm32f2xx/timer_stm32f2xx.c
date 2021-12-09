#include "timer.h"

/******************************************************************************
 * Definitions
 ******************************************************************************/
static timer_confg_t g_timer_config[TIMER0 + 1] =
{
	{
		.clk_ = TIMER0_CLK,
		.irq_ = TIMER0_IRQ,
	}
};

static TIM_HandleTypeDef g_handle[TIMER0 + 1] = 
{
	{
		.Instance               = TIMER0_INST,
		.Init.CounterMode       = TIM_COUNTERMODE_UP,
		.Init.ClockDivision     = 0,
		.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE,
	}
};

/******************************************************************************
 * Local function prototypes
 ******************************************************************************/
static void timer_irq_handler(const uint8_t _chl);

/******************************************************************************
 * Functions
 ******************************************************************************/
int32_t timer_init(const uint8_t _num, const uint32_t _period)
{
	assert(TIMER0 >= _num);

	/* initialize the timer */
	TIMER_CLK_ENABLE(_num);
	g_handle[_num].Init.Prescaler = 2 * HAL_RCC_GetPCLK1Freq() / g_timer_config[_num].clk_ - 1;
	g_handle[_num].Init.Period    = _period * g_timer_config[_num].clk_ / 1000 - 1;
	HAL_TIM_Base_Init(&g_handle[_num]);
	
	/* initialize the NVIC */
	HAL_NVIC_SetPriority(g_timer_config[_num].irq_, 0, 0);
	HAL_NVIC_EnableIRQ(g_timer_config[_num].irq_);

    return 0;
}

int32_t timer_deinit(const uint8_t _num)
{
	assert(TIMER0 >= _num);

	HAL_NVIC_DisableIRQ(g_timer_config[_num].irq_);
	HAL_TIM_Base_Stop_IT(&g_handle[_num]);
	__HAL_TIM_CLEAR_IT(&g_handle[TIMER0], TIM_IT_UPDATE);
	HAL_TIM_Base_DeInit(&g_handle[_num]);
	TIMER_CLK_DISABLE(_num);

	return 0;
}

int32_t timer_start(const uint8_t _num)
{
	assert(TIMER0 >= _num);

	HAL_TIM_Base_Start_IT(&g_handle[_num]);

    return 0;
}

int32_t timer_stop(const uint8_t _num)
{
	assert(TIMER0 >= _num);
	
	HAL_TIM_Base_Stop_IT(&g_handle[_num]);

	return 0;
}

__attribute__((weak)) void timer_irq_callback(const uint8_t _num)
{
	(void)_num;
}

/**
 * @name IRQ handlers
 * @{
 */

/**
 * Timer0 IRQ handler.
 */
void TIMER0_IRQ_HANDLER(void)   
{
	timer_irq_handler(TIMER0);
}

/** @} */ /* IRQ handlers */

/******************************************************************************
 * Local functions
 ******************************************************************************/
/**
 * Timer IRQ handler.
 *
 * @param [in] _num the timer number
 */
static void timer_irq_handler(const uint8_t _num)
{
	/* timer update event */
	if (RESET != __HAL_TIM_GET_FLAG(&g_handle[_num], TIM_FLAG_UPDATE) && RESET != __HAL_TIM_GET_IT_SOURCE(&g_handle[_num], TIM_IT_UPDATE))
	{
		__HAL_TIM_CLEAR_IT(&g_handle[_num], TIM_IT_UPDATE);
		timer_irq_callback(_num);	
	}
}
