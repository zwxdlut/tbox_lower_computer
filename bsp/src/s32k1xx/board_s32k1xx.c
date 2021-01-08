/*
 * board_s32k1xx.c
 *
 *  Created on: 2018��8��21��
 *      Author: Administrator
 */

#include "board.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*******************************************************************************
 * Local Function prototypes
 ******************************************************************************/
static void pin_irq_handler(void);
static void wdog_irq_handler(void);

/*******************************************************************************
 * Functions
 ******************************************************************************/
int32_t sys_init(void)
{
    /* Initialize and configure clocks:
	   - Setup system clocks, dividers.
       - see clock manager component for more details */
    CLOCK_SYS_Init(g_clockManConfigsArr, CLOCK_MANAGER_CONFIG_CNT,
            g_clockManCallbacksArr, CLOCK_MANAGER_CALLBACK_CNT);
    CLOCK_SYS_UpdateConfiguration(0U, CLOCK_MANAGER_POLICY_AGREEMENT);

	return 0;
}

void gpio_init(void)
{
	// LEDs initialization
	PINS_DRV_SetMuxModeSel(LED0_PORT, LED0_PIN, PORT_MUX_AS_GPIO);
	PINS_DRV_SetPinDirection(LED0_GPIO, LED0_PIN, GPIO_OUTPUT_DIRECTION);
	PINS_DRV_WritePin(LED0_GPIO, LED0_PIN, LED_OFF);
	PINS_DRV_SetMuxModeSel(LED1_PORT, LED1_PIN, PORT_MUX_AS_GPIO);
	PINS_DRV_SetPinDirection(LED1_GPIO, LED1_PIN, GPIO_OUTPUT_DIRECTION);
	PINS_DRV_WritePin(LED1_GPIO, LED1_PIN, LED_OFF);
	PINS_DRV_SetMuxModeSel(LED2_PORT, LED2_PIN, PORT_MUX_AS_GPIO);
	PINS_DRV_SetPinDirection(LED2_GPIO, LED2_PIN, GPIO_OUTPUT_DIRECTION);
	PINS_DRV_WritePin(LED2_GPIO, LED2_PIN, LED_OFF);

	// Button initialization
	PINS_DRV_SetMuxModeSel(BTN_PORT, BTN_PIN, PORT_MUX_AS_GPIO);
	PINS_DRV_SetPinDirection(BTN_GPIO, BTN_PIN, GPIO_INPUT_DIRECTION);
    PINS_DRV_SetPinIntSel(BTN_PORT, BTN_PIN, PORT_INT_RISING_EDGE);
    INT_SYS_InstallHandler(BTN_IRQ, &pin_irq_handler, NULL);
    INT_SYS_EnableIRQ(BTN_IRQ);

#if defined USING_OS_FREERTOS
    /* The interrupt calls an interrupt safe API function - so its priority must
       be equal to or lower than configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY */
    INT_SYS_SetPriority( BTN_IRQ, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY );
#endif

#if defined MX_TB
	// Upper computer initialization
	PINS_DRV_SetMuxModeSel(UC_POWER_PORT, UC_POWER_PIN, PORT_MUX_AS_GPIO);
	PINS_DRV_SetPinDirection(UC_POWER_GPIO, UC_POWER_PIN, GPIO_OUTPUT_DIRECTION);
	PINS_DRV_WritePin(UC_POWER_GPIO, UC_POWER_PIN, 0);
	PINS_DRV_SetMuxModeSel(UC_RESET_PORT, UC_RESET_PIN, PORT_MUX_AS_GPIO);
	PINS_DRV_SetPinDirection(UC_RESET_GPIO, UC_RESET_PIN, GPIO_OUTPUT_DIRECTION);
	PINS_DRV_WritePin(UC_RESET_GPIO, UC_RESET_PIN, 0);
	PINS_DRV_SetMuxModeSel(UC_WAKEUP_PORT, UC_WAKEUP_PIN, PORT_MUX_AS_GPIO);
	PINS_DRV_SetPinDirection(UC_WAKEUP_GPIO, UC_WAKEUP_PIN, GPIO_OUTPUT_DIRECTION);
	PINS_DRV_WritePin(UC_WAKEUP_GPIO, UC_WAKEUP_PIN, 0);

    // Ignition initialization
	PINS_DRV_SetMuxModeSel(IGN_PORT, IGN_PIN, PORT_MUX_AS_GPIO);
	PINS_DRV_SetPinDirection(IGN_GPIO, IGN_PIN, GPIO_INPUT_DIRECTION);
    PINS_DRV_SetPinIntSel(IGN_PORT, IGN_PIN, PORT_INT_FALLING_EDGE);
    INT_SYS_InstallHandler(IGN_IRQ, &pin_irq_handler, NULL);
    INT_SYS_EnableIRQ(IGN_IRQ);

#if defined USING_OS_FREERTOS
    /* The interrupt calls an interrupt safe API function - so its priority must
       be equal to or lower than configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY */
    INT_SYS_SetPriority( IGN_IRQ, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY );
#endif
#endif
}

void gpio_deinit(void)
{
#if defined MX_TB
	INT_SYS_DisableIRQ(IGN_IRQ);
	PINS_DRV_ClearPortIntFlagCmd(IGN_PORT);
	PINS_DRV_ClearPinIntFlagCmd(IGN_PORT, IGN_PIN);
	PINS_DRV_SetMuxModeSel(UC_POWER_PORT, UC_POWER_PIN, PORT_PIN_DISABLED);
	PINS_DRV_SetMuxModeSel(UC_RESET_PORT, UC_RESET_PIN, PORT_PIN_DISABLED);
	PINS_DRV_SetMuxModeSel(UC_WAKEUP_PORT, UC_WAKEUP_PIN, PORT_PIN_DISABLED);
	PINS_DRV_SetMuxModeSel(IGN_PORT, IGN_PIN, PORT_PIN_DISABLED);
#endif

	INT_SYS_DisableIRQ(BTN_IRQ);
	PINS_DRV_ClearPortIntFlagCmd(BTN_PORT);
	PINS_DRV_ClearPinIntFlagCmd(BTN_PORT, BTN_PIN);
	PINS_DRV_SetMuxModeSel(LED0_PORT, LED0_PIN, PORT_PIN_DISABLED);
	PINS_DRV_SetMuxModeSel(LED1_PORT, LED1_PIN, PORT_PIN_DISABLED);
	PINS_DRV_SetMuxModeSel(LED2_PORT, LED2_PIN, PORT_PIN_DISABLED);
	PINS_DRV_SetMuxModeSel(BTN_PORT, BTN_PIN, PORT_PIN_DISABLED);
}

uint32_t sys_time(void)
{
	return OSIF_GetMilliseconds();
}

void delay(const uint32_t _ms)
{
	OSIF_TimeDelay(_ms);
}

void sys_reset(void)
{
	SystemSoftwareReset();
}

void pwr_mode_trans(const uint8_t _mode)
{
	switch(_mode)
	{
	case PWR_MODE_SLEEP:
	case PWR_MODE_DEEPSLEEP:
	    /* Allow very low power modes*/
	    SMC->PMPROT |= SMC_PMPROT_AVLP_MASK;

	    /* CLKBIASDIS=1: In VLPS mode, the bias currents and reference voltages
	       for the following clock modules are disabled: SIRC, FIRC, PLL */
	    PMC->REGSC |= PMC_REGSC_BIASEN_MASK | PMC_REGSC_CLKBIASDIS_MASK;

	    /* Enable Stop Modes in the Core */
	    S32_SCB->SCR |= S32_SCB_SCR_SLEEPDEEP_MASK;

	    /*  Select VLPS Mode */
	    SMC->PMCTRL = SMC_PMCTRL_STOPM(0b10);
	    /*
	     *
	     *  Transition from RUN to VLPR
	     *
	     *                              */
	    if (0b01 == SMC->PMSTAT)
	    {
	        __asm("DSB");
	        __asm("ISB");
	        /* Call WFI to enter DeepSleep mode */
	        __asm("WFI");
	    }
	    else
	    {
	    	//error
	    }

	    /* Verify VLPSA bit is not set */
	    if (0 != (SMC->PMCTRL & SMC_PMCTRL_VLPSA_MASK))
	    {
	        //error
	    }
		break;
	default:
		break;
	}
}

int32_t wdog_enable(void)
{
#if defined EXTWDOG
	// Initialize EWM
	EWM_DRV_Init(EXTWDOG, &extWdog_Config0);
#endif

#if defined INST_WATCHDOG
	// Initialize WDOG
    WDOG_DRV_Init(INST_WATCHDOG, &watchdog_Config0);
#endif

	// Install IRQ handler
	INT_SYS_InstallHandler(WDOG_EWM_IRQn, wdog_irq_handler, (isr_t *)0);
    // Enable IRQ
    INT_SYS_EnableIRQ(WDOG_EWM_IRQn);

    return 0;
}

int32_t wdog_refresh(void)
{
#if defined EXTWDOG
	EWM_DRV_Refresh(EXTWDOG);
#endif

#if defined INST_WATCHDOG
	WDOG_DRV_Trigger(INST_WATCHDOG);
#endif

	return 0;
}

int32_t wwdog_disable(void)
{
	// Disable IRQ
	INT_SYS_DisableIRQ(WDOG_EWM_IRQn);
	
#if defined INST_WATCHDOG
	WDOG_DRV_Deinit(INST_WATCHDOG);
#endif

	return 0;
}

/*******************************************************************************
 * Local Functions
 ******************************************************************************/
/**
 * Pin IRQ handler.
 */
static void pin_irq_handler(void)
{
    uint32_t pins = PINS_DRV_GetPortIntFlag(BTN_PORT) & ((1 << BTN_PIN));

    if (pins != 0)
    {
        switch (pins)
        {
            case (1 << BTN_PIN):
                PINS_DRV_ClearPinIntFlagCmd(BTN_PORT, BTN_PIN);
                break;
            default:
                PINS_DRV_ClearPortIntFlagCmd(BTN_PORT);
                break;
        }
    }

#if defined MX_TB
    pins = PINS_DRV_GetPortIntFlag(IGN_PORT) & ((1 << IGN_PIN));

    if (pins != 0)
    {
        switch (pins)
        {
            case (1 << IGN_PIN):
                PINS_DRV_ClearPinIntFlagCmd(IGN_PORT, IGN_PIN);
                break;
            default:
                PINS_DRV_ClearPortIntFlagCmd(IGN_PORT);
                break;
        }
    }
#endif
}

/**
 * Watch dog IRQ handler.
 */
static void wdog_irq_handler(void)
{
	sys_reset();
}
