/*
 * board_stm32f2xx.c
 *
 *  Created on: 2018��8��21��
 *      Author: Administrator
 */

#include "board.h"

#if defined USING_OS_FREERTOS
extern void xPortSysTickHandler(void);
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
static WWDG_HandleTypeDef g_wdog_handle;

/*******************************************************************************
 * Local Function prototypes
 ******************************************************************************/
static int32_t sys_clk_config(void);

/*******************************************************************************
 * Functions
 ******************************************************************************/
int32_t sys_init(void)
{
	HAL_Init();
	sys_clk_config();

	return 0;
}

void gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* LEDs initialization */
	LED0_GPIO_CLK_ENABLE();
	GPIO_InitStructure.Pin   = LED0_PIN;
	GPIO_InitStructure.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH; 
	HAL_GPIO_Init(LED0_GPIO, &GPIO_InitStructure);
	HAL_GPIO_WritePin(LED0_GPIO, LED0_PIN, LED_OFF);
	LED1_GPIO_CLK_ENABLE();	
	GPIO_InitStructure.Pin = LED1_PIN;
	HAL_GPIO_Init(LED1_GPIO, &GPIO_InitStructure);
	HAL_GPIO_WritePin(LED1_GPIO, LED1_PIN, LED_OFF);
	LED2_GPIO_CLK_ENABLE();
	GPIO_InitStructure.Pin = LED2_PIN;  
	HAL_GPIO_Init(LED2_GPIO, &GPIO_InitStructure);
	HAL_GPIO_WritePin(LED2_GPIO, LED2_PIN, LED_OFF);
	
	/* Button initialization */
	BTN_GPIO_CLK_ENABLE();
	GPIO_InitStructure.Pin  = BTN_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(BTN_GPIO, &GPIO_InitStructure);
	HAL_NVIC_SetPriority(BTN_IRQ, 0, 0);
    HAL_NVIC_EnableIRQ(BTN_IRQ);
	
	/* Upper computer initialization */
	UC_POWER_GPIO_CLK_ENABLE();
	GPIO_InitStructure.Pin  = UC_POWER_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull = GPIO_NOPULL;	
	HAL_GPIO_Init(UC_POWER_GPIO, &GPIO_InitStructure);
	HAL_GPIO_WritePin(UC_POWER_GPIO, UC_POWER_PIN, GPIO_PIN_RESET);
	UC_WAKEUP_GPIO_CLK_ENABLE();
	GPIO_InitStructure.Pin = UC_WAKEUP_PIN; 
	HAL_GPIO_Init(UC_WAKEUP_GPIO, &GPIO_InitStructure);
	HAL_GPIO_WritePin(UC_WAKEUP_GPIO, UC_WAKEUP_PIN, GPIO_PIN_RESET);
	UC_RESET_GPIO_CLK_ENABLE();
	GPIO_InitStructure.Pin = UC_RESET_PIN;
	HAL_GPIO_Init(UC_RESET_GPIO, &GPIO_InitStructure);
	HAL_GPIO_WritePin(UC_RESET_GPIO, UC_RESET_PIN, GPIO_PIN_RESET);
	
    /* Ignition initialization */
	IGN_GPIO_CLK_ENABLE();
	GPIO_InitStructure.Pin  = IGN_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(IGN_GPIO, &GPIO_InitStructure);
	HAL_NVIC_SetPriority(IGN_IRQ, 0, 0);
    HAL_NVIC_EnableIRQ(IGN_IRQ);
}

void gpio_deinit(void)
{
	HAL_NVIC_DisableIRQ(IGN_IRQ);
	__HAL_GPIO_EXTI_CLEAR_IT(IGN_PIN);
	HAL_GPIO_DeInit(UC_POWER_GPIO, UC_POWER_PIN);
	HAL_GPIO_DeInit(UC_WAKEUP_GPIO, UC_WAKEUP_PIN);
	HAL_GPIO_DeInit(UC_RESET_GPIO, UC_RESET_PIN);
	HAL_GPIO_DeInit(IGN_GPIO, IGN_PIN);
	UC_POWER_GPIO_CLK_DISABLE();
	UC_WAKEUP_GPIO_CLK_DISABLE();
	UC_RESET_GPIO_CLK_DISABLE();
	IGN_GPIO_CLK_DISABLE();
	HAL_NVIC_DisableIRQ(BTN_IRQ);
	__HAL_GPIO_EXTI_CLEAR_IT(BTN_PIN);
	HAL_GPIO_DeInit(LED0_GPIO, LED0_PIN);
	HAL_GPIO_DeInit(LED1_GPIO, LED1_PIN);
	HAL_GPIO_DeInit(LED2_GPIO, LED2_PIN);
	HAL_GPIO_DeInit(BTN_GPIO, BTN_PIN);
	LED0_GPIO_CLK_DISABLE();
	LED1_GPIO_CLK_DISABLE();
	LED2_GPIO_CLK_DISABLE();
	BTN_GPIO_CLK_DISABLE();
}

uint32_t sys_time(void)
{
	return HAL_GetTick();
}

void delay(const uint32_t _ms)
{
	HAL_Delay(_ms);
}

void sys_reset(void)
{
	__set_FAULTMASK(1);
	NVIC_SystemReset();
}

void pwr_mode_trans(const uint8_t _mode)
{
	__HAL_RCC_PWR_CLK_ENABLE();
	/* Suspend Tick increment to prevent wakeup by Systick interrupt
	   Otherwise the Systick interrupt will wake up the device within 1ms (HAL time base) */
	HAL_SuspendTick();
	switch(_mode)
	{
	case PWR_MODE_SLEEP:
		HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
		break;
	case PWR_MODE_DEEPSLEEP:
		HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
	default:
		break;
	}
	sys_clk_config();
	/* Resume Tick interrupt if disabled prior to sleep mode entry */
	HAL_ResumeTick();
	__HAL_RCC_PWR_CLK_DISABLE();
}

int32_t wdog_enable(void)
{
#if 0 /* Individual watch dog */
	g_wdog_handle.Instance       = IWDG;
	g_wdog_handle.Init.Prescaler = IWDOG_PRV;
	g_wdog_handle.Init.Reload    = IWDOG_RLV;
	HAL_IWDG_Init(&g_wdog_handle);
#endif
	__HAL_RCC_WWDG_CLK_ENABLE();
	g_wdog_handle.Instance       = WWDG;
	g_wdog_handle.Init.Prescaler = WWDOG_PRV;
	g_wdog_handle.Init.Window    = WWDOG_WV;
	g_wdog_handle.Init.Counter   = WWDOG_RLV;
	g_wdog_handle.Init.EWIMode   = WWDG_EWI_ENABLE;
	HAL_WWDG_Init(&g_wdog_handle);
	HAL_NVIC_SetPriority(WWDG_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(WWDG_IRQn);

	return 0;
}

int32_t wdog_refresh(void)
{
#if 0 /* Individual watch dog */
	HAL_IWDG_Refresh(&g_wdog_handle);
#endif
	HAL_WWDG_Refresh(&g_wdog_handle);
	
	return 0;
}

int32_t wdog_disable(void)
{
	HAL_NVIC_DisableIRQ(WWDG_IRQn);
	__HAL_WWDG_CLEAR_FLAG(&g_wdog_handle, WWDG_FLAG_EWIF);
	__HAL_RCC_WWDG_CLK_DISABLE();

	return 0;
}

/**
 * @name IRQ handlers.
 * @{
 */
/**
 * @brief System tick timer handler.
 */
void SysTick_Handler(void)
{
	HAL_IncTick();
#if defined USING_OS_FREERTOS
	xPortSysTickHandler();
#endif
}

/**
 * @brief Button IRQ handler.
 */
void BTN_IRQ_HANDLER(void)
{
	HAL_GPIO_EXTI_IRQHandler(BTN_PIN);
}

/**
 * @brief Ignition IRQ handler.
 */
void IGN_IRQ_HANDLER(void)
{
	HAL_GPIO_EXTI_IRQHandler(IGN_PIN);
}

/**
 * @brief Window watch dog IRQ handler.
 */
void WWDG_IRQHandler(void)
{
	/* Early wakeup */
	if(RESET != __HAL_WWDG_GET_FLAG(&g_wdog_handle, WWDG_FLAG_EWIF) && RESET != __HAL_WWDG_GET_IT_SOURCE(&g_wdog_handle, WWDG_IT_EWI))
	{
		/* Clear early wakeup flag */
		__HAL_WWDG_CLEAR_FLAG(&g_wdog_handle, WWDG_FLAG_EWIF);
		sys_reset();
	}
}
/** @} */ /* IRQ handlers. */

/*******************************************************************************
 * Local Functions
 ******************************************************************************/
/**
 * @brief   System Clock Configuration.
 *
 * @details The system Clock is configured as follow: 
 *          - System Clock source            = PLL (HSE)
 *          - SYSCLK(Hz)                     = 120000000
 *          - HCLK(Hz)                       = 120000000
 *          - AHB Prescaler                  = 1
 *          - APB1 Prescaler                 = 4
 *          - APB2 Prescaler                 = 2
 *          - HSE Frequency(Hz)              = 8000000
 *          - PLL_M                          = 8
 *          - PLL_N                          = 240
 *          - PLL_P                          = 2
 *          - PLL_Q                          = 5
 *          - VDD(V)                         = 3.3
 *          - Flash Latency(WS)              = 3
 *
 * @return  Success(0) or failure(other values).
 */
static int32_t sys_clk_config(void)
{
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;
	
	/* Enable HSE Oscillator and activate PLL with HSE as source */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 240;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 5;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);
	
	/* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
	   clocks dividers */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3);
	
	return 0;
}
