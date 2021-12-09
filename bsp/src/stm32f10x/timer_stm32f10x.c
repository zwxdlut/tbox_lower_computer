#include "timer.h"

/******************************************************************************
 * Definitions
 ******************************************************************************/
static timer_confg_t g_timer_config[TIMER0 + 1] =
{
	{
		.clk_  = TIMER0_CLK,
		.irq_  = TIMER0_IRQ,
	}
};
 
static TIM_TypeDef *g_handle[TIMER0 + 1] = {TIMER0_INST};

/******************************************************************************
 * Local function prototypes
 ******************************************************************************/
static void timer_irq_handler(const uint8_t _num);

/******************************************************************************
 * Functions
 ******************************************************************************/
int32_t timer_init(const uint8_t _num, const uint32_t _period)
{
	assert(TIMER0 >= _num);

	RCC_ClocksTypeDef clks;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

 	/* initialize the timer */
	TIMER_CLK_ENABLE(_num);
	RCC_GetClocksFreq(&clks);
	TIM_TimeBaseStructure.TIM_Prescaler = 2 * clks.PCLK1_Frequency / g_timer_config[_num].clk_ - 1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseStructure.TIM_Period = _period * g_timer_config[_num].clk_ / 1000 - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
	TIM_TimeBaseInit(g_handle[_num], &TIM_TimeBaseStructure);
	TIM_ITConfig(g_handle[_num], TIM_IT_Update, ENABLE);
	
	/* initialize the NVIC */
	NVIC_InitStructure.NVIC_IRQChannel = g_timer_config[_num].irq_;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;         
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure); 

    return 0;
}

int32_t timer_deinit(const uint8_t _num)
{
	assert(TIMER0 >= _num);

	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel = g_timer_config[_num].irq_;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_Cmd(g_handle[_num], DISABLE); 
	TIM_ClearITPendingBit(g_handle[TIMER0], TIM_IT_Update);
	TIM_ITConfig(g_handle[_num], TIM_IT_Update, DISABLE);
	TIM_DeInit(g_handle[_num]);
	TIMER_CLK_DISABLE(_num);

	return 0;
}

int32_t timer_start(const uint8_t _num)
{
	assert(TIMER0 >= _num);

	TIM_Cmd(g_handle[_num], ENABLE);

    return 0;
}

int32_t timer_stop(const uint8_t _num)
{
	assert(TIMER0 >= _num);
	
	TIM_Cmd(g_handle[_num], DISABLE);
	
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
	if (TIM_GetITStatus(g_handle[_num], TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(g_handle[_num], TIM_IT_Update);
		timer_irq_callback(_num);	
	}	
}
