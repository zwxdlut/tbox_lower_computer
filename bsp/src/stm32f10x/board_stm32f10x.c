/*
 * board_stm32f10x.c
 *
 *  Created on: 2018Äê8ÔÂ21ÈÕ
 *      Author: Administrator
 */

#include "board.h"

#if defined USING_OS_FREERTOS
extern void xPortSysTickHandler(void);
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
static uint32_t g_sys_tick_cnt = 0; // SysTick count

/*******************************************************************************
 * Local Function prototypes
 ******************************************************************************/
/*******************************************************************************
 * Functions
 ******************************************************************************/
int32_t sys_init(void)
{
	// Setup SysTick
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock/1000);
	return 0;
}

void gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	// LEDs initialization
	LED0_GPIO_CLK_ENABLE();
	GPIO_InitStructure.GPIO_Pin   = LED0_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(LED0_GPIO, &GPIO_InitStructure);
	GPIO_WriteBit(LED0_GPIO, LED0_PIN, LED_OFF);
	LED1_GPIO_CLK_ENABLE();	
	GPIO_InitStructure.GPIO_Pin = LED1_PIN; 
	GPIO_Init(LED1_GPIO, &GPIO_InitStructure);
	GPIO_WriteBit(LED1_GPIO, LED1_PIN, LED_OFF);
	LED2_GPIO_CLK_ENABLE();
	GPIO_InitStructure.GPIO_Pin = LED2_PIN;
	GPIO_Init(LED2_GPIO, &GPIO_InitStructure);
	GPIO_WriteBit(LED2_GPIO, LED2_PIN, LED_OFF);
	
	// Button initialization
	BTN_GPIO_CLK_ENABLE();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin  = BTN_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(BTN_GPIO, &GPIO_InitStructure);
	GPIO_EXTILineConfig(BTN_PORT_SRC, BTN_PIN_SRC);
  	EXTI_InitStructure.EXTI_Line    = BTN_EXTI_LINE;
  	EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);
   	NVIC_InitStructure.NVIC_IRQChannel                   = BTN_IRQ;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
}

void gpio_deinit(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel                   = BTN_IRQ;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd                = DISABLE;
  	NVIC_Init(&NVIC_InitStructure);
	EXTI_ClearITPendingBit(BTN_EXTI_LINE);
	EXTI_DeInit();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, DISABLE);
	GPIO_DeInit(LED0_GPIO);
	GPIO_DeInit(LED1_GPIO);
	GPIO_DeInit(LED2_GPIO);
	GPIO_DeInit(BTN_GPIO);
	LED0_GPIO_CLK_DISABLE();
	LED1_GPIO_CLK_DISABLE();
	LED2_GPIO_CLK_DISABLE();
	BTN_GPIO_CLK_DISABLE();
}

uint32_t sys_time(void)
{
	return g_sys_tick_cnt;
}

void delay(const uint32_t _ms)
{
	uint32_t start = g_sys_tick_cnt;
    uint32_t delta = 0;

    while (delta < _ms)
	{
		delta = g_sys_tick_cnt - start;
	}
}

void sys_reset(void)
{
	__set_FAULTMASK(1);
	NVIC_SystemReset();
}

void pwr_mode_trans(const uint8_t _mode)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	// Suspend SysTick
	SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk);

	switch(_mode)
	{
	case PWR_MODE_SLEEP:
		PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
		break;
	case PWR_MODE_DEEPSLEEP:
	    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
	default:
		break;
	}

	SystemInit();
	// Resume SysTick
	SysTick->CTRL |= (SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, DISABLE);
}

int32_t wdog_enable(void)
{
#if 0 // Individual watch dog
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDOG_PRV);
	IWDG_SetReload(IWDOG_RLV);
	IWDG_ReloadCounter();
	IWDG_Enable();
#endif

	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
	WWDG_ClearFlag();
	WWDG_SetPrescaler(WWDOG_PRV);
	WWDG_SetWindowValue(WWDOG_WV);    
	WWDG_Enable(WWDOG_RLV);
	WWDG_EnableIT();
	NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);

	return 0;
}

int32_t wdog_refresh(void)
{
#if 0 // Individual watch dog
	IWDG_ReloadCounter();	
#endif

	WWDG_SetCounter(WWDOG_RLV);

	return 0;
}

int32_t wdog_disable(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE; 
	NVIC_Init(&NVIC_InitStructure);
	WWDG_ClearFlag();
	WWDG_DeInit();
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_WWDG, DISABLE);

	return 0;
}

/**
 * @name IRQ handlers.
 * @{
 */
/**
 * System tick timer handler.
 */
void SysTick_Handler(void)
{
	g_sys_tick_cnt++;

#if defined USING_OS_FREERTOS
	xPortSysTickHandler();
#endif
}

/**
 * Button IRQ handler.
 */
void BTN_IRQ_HANDLER(void)
{
	if (RESET != EXTI_GetITStatus(BTN_EXTI_LINE))
		EXTI_ClearITPendingBit(BTN_EXTI_LINE);
}

/**
 * Window watch dog IRQ handler.
 */
void WWDG_IRQHandler(void)
{
	sys_reset();
}
/** @} */ // IRQ handlers.

/*******************************************************************************
 * Local Functions
 ******************************************************************************/
