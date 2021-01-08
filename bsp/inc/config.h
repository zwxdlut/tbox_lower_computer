/*
 * config.h
 *
 *  Created on: 2018��8��21��
 *      Author: Administrator
 */

#ifndef __CONFIG__H__
#define __CONFIG__H__

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#if defined USING_OS_FREERTOS
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#endif

#if defined S32K144
#include "Cpu.h"
/**
 * @name Pin operation macros.
 * @{
 */
#define GPIO_WRITE_PIN(GPIO, PIN, LEVEL)        PINS_DRV_WritePin(GPIO, PIN, LEVEL)
#define GPIO_TOGGLE_PIN(GPIO, PIN)              PINS_DRV_TogglePins(GPIO, (1 << PIN))
#define GPIO_READ_PIN(GPIO, PIN)                ((1 << PIN) & PINS_DRV_ReadPins(GPIO))
/** @} */ // Pin operation macros.

/**
 * @name UART data bits.
 * @{
 */
#define UART_DATA_BITS_8                        (uint32_t)LPUART_8_BITS_PER_CHAR
#define UART_DATA_BITS_9                        (uint32_t)LPUART_9_BITS_PER_CHAR
#define UART_DATA_BITS_10                       (uint32_t)LPUART_10_BITS_PER_CHAR
/** @} */ // UART data bits.

/**
 * @name UART stop bits.
 * @{
 */
#define UART_STOP_BITS_1                        (uint32_t)LPUART_ONE_STOP_BIT
#define UART_STOP_BITS_2                        (uint32_t)LPUART_TWO_STOP_BIT
/** @} */ // UART stop bits.

/**
 * @name UART parity.
 * @{
 */
#define UART_PARITY_MODE_NONE                   (uint32_t)LPUART_PARITY_DISABLED
#define UART_PARITY_MODE_EVEN                   (uint32_t)LPUART_PARITY_EVEN
#define UART_PARITY_MODE_ODD                    (uint32_t)LPUART_PARITY_ODD
/** @} */ // UART parity.

/**
 * @name Timer configuration.
 * @{
 */
#define TIMER0_IRQ                              LPTMR0_IRQn
/** @} */ // Timer configuration.

/**
 * @name Flash configuration.
 * @{
 */
#define FLASH_BASE_ADDR 			            0x00000000
#define FLASH_SECTOR_SIZE                       4096 ///< 4k
#define FLASH_TOTAL_SIZE                        0x80000 ///< 512k
/** @} */ // Flash configuration.

#if defined MX_TB
#include "mx_tb/s32k144_mx_tb.h"
#elif defined S32K144_EVB
#include "devb/s32k144_evb.h"
#elif defined S32K144_FRDM
#include "devb/s32k144_frdm.h"
#else
#error Board type not defined!!!
#endif
#elif defined STM32F10X_CL
#include "stm32f10x.h"

/**
 * @name Pin operation macros.
 * @{
 */
#define GPIO_INIT(GPIO, INIT)                   GPIO_Init(GPIO, INIT)
#define GPIO_DEINIT(GPIO, PIN)                  GPIO_DeInit(GPIO)
#define GPIO_WRITE_PIN(GPIO, PIN, LEVEL)        GPIO_WriteBit(GPIO, PIN, (BitAction)LEVEL)
#define GPIO_TOGGLE_PIN(GPIO, PIN)              GPIO_WriteBit(GPIO, PIN, (Bit_RESET == GPIO_ReadOutputDataBit(GPIO, PIN) ? Bit_SET : Bit_RESET))
#define GPIO_READ_PIN(GPIO, PIN)                GPIO_ReadInputDataBit(GPIO, PIN)
/** @} */ // Pin operation macros.

/**
 * @name Independent watch dog.
 * @details Tout = (4*2^prv) / LSI * rlv (ms).
 *          LSI: LSI clock in kHz.
 *          prv: Prescaler(IWDOG_PRV).
 *          rlv: Reload value(IWDOG_RLV).
 * @{
 */
#define IWDOG_PRV                               IWDG_Prescaler_8
#define IWDOG_RLV                               0xFFF
/** @} */ // Independent watch dog.

/**
 * @name Window watch dog.
 * @details Tout = T(PCLK1) * 4096 * 2^WDGTB * (T[5:0]+1) (ms).
 *          T(PCLK1): APB1 clock period in milliseconds.
 *          WDGTB: Prescaler(WWDOG_PRV).
 *          T[5:0]: Download counter lower 6 bits(T[5:0] + 1 = WWDOG_RLV).
 * @{
 */
#define WWDOG_PRV                               WWDG_Prescaler_8
#define WWDOG_WV                                0x7F
#define WWDOG_RLV                               0x7F
/** @} */ // Window watch dog.

/**
 * @name UART data bits.
 * @{
 */
#define UART_DATA_BITS_8                        (uint32_t)USART_WordLength_8b
#define UART_DATA_BITS_9                        (uint32_t)USART_WordLength_9b
/** @} */ // UART data bits.

/**
 * @name UART stop bits.
 * @{
 */
#define UART_STOP_BITS_0_5                      (uint32_t)USART_StopBits_0_5
#define UART_STOP_BITS_1                        (uint32_t)USART_StopBits_1
#define UART_STOP_BITS_1_5                      (uint32_t)USART_StopBits_1_5
#define UART_STOP_BITS_2                        (uint32_t)USART_StopBits_2
/** @} */ // UART stop bits.

/**
 * @name UART parity.
 * @{
 */
#define UART_PARITY_MODE_NONE                   (uint32_t)USART_Parity_No
#define UART_PARITY_MODE_EVEN                   (uint32_t)USART_Parity_Even
#define UART_PARITY_MODE_ODD                    (uint32_t)USART_Parity_Odd
/** @} */ // UART parity.

/**
 * @name Timer configuration.
 * @{
 */
#define TIMER_CLK_ENABLE(INDEX)                 do { if (0 == (INDEX)) { RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); } else\
                                                                       { RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); }} while (0)
#define TIMER_CLK_DISABLE(INDEX)                do { if (0 == (INDEX)) { RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, DISABLE); } else\
                                                                       { RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, DISABLE); }} while (0)
#define TIMER0_INST                             TIM3
#define TIMER0_CLK                              10000
#define TIMER0_IRQ                              TIM3_IRQn
#define TIMER0_IRQ_HANDLER                      TIM3_IRQHandler

/**
 * @brief Timer config.
 * @{
 */
typedef struct
{ 
	uint16_t  clk_;
	IRQn_Type irq_;
}timer_confg_t;
/** @} */ // Timer configuration.

/**
 * @name Flash configuration.
 * @{
 */
#define FLASH_BASE_ADDR 			            0x08000000
#define FLASH_SECTOR_SIZE                       2048 ///< 2k
#define FLASH_TOTAL_SIZE                        0x40000 ///< 256k
/** @} */ // Flash configuration.
#if defined CPATAIN_DESIGN
#include "devb/stm32f10x_captain_design.h"
#else
#error Board type not defined!!!
#endif
#elif defined STM32F205xx
#include "stm32f2xx.h"
#include "stm32f2xx_ll_usart.h"
/**
 * @name Pin operation macros.
 * @{
 */
#define GPIO_INIT(GPIO, INIT)                   HAL_GPIO_Init(GPIO, INIT)
#define GPIO_DEINIT(GPIO, PIN)                  HAL_GPIO_DeInit(GPIO, PIN);
#define GPIO_WRITE_PIN(GPIO, PIN, LEVEL)        HAL_GPIO_WritePin(GPIO, PIN, (GPIO_PinState)LEVEL)
#define GPIO_TOGGLE_PIN(GPIO, PIN)              HAL_GPIO_TogglePin(GPIO, PIN)
#define GPIO_READ_PIN(GPIO, PIN)                HAL_GPIO_ReadPin(GPIO, PIN)
/** @} */ // Pin operation macros. 

/**
 * @name Independent watch dog.
 *
 * @details  Tout = (4*2^prv) / LSI * rlv (ms).
 * 			 LSI: LSI clock in kHz.
 *           prv: Prescaler(IWDOG_PRV).
 *           rlv: Reload value(IWDOG_RLV).
 */
#define IWDOG_PRV                               IWDG_PRESCALER_8
#define IWDOG_RLV                               0xFFF
/** @} */ // Independent watch dog.

/**
 * @name Window watch dog.
 *
 * @details  Tout = 4096 * 2^WDGTB / PCLK1 * (T[5:0]+1) (ms).
 *           PCLK1:  APB1 clock.
 *           WDGTB:  Watch dog timer base(WWDOG_PRV >> WWDG_CFR_WDGTB_Pos).
 *           T[5:0]: Download counter lower 6 bits(T[6:0] = WWDOG_RLV).
 */
#define WWDOG_PRV                               WWDG_PRESCALER_8
#define WWDOG_WV                                0x7F
#define WWDOG_RLV                               0x7F
/** @} */ // Window watch dog.

/**
 * @name UART data bits.
 * @{
 */
#define UART_DATA_BITS_8                       (uint32_t)UART_WORDLENGTH_8B
#define UART_DATA_BITS_9                       (uint32_t)UART_WORDLENGTH_9B
/** @} */ // UART data bits.

/**
 * @name UART stop bits.
 * @{
 */
#define UART_STOP_BITS_1                        (uint32_t)UART_STOPBITS_1
#define UART_STOP_BITS_2                        (uint32_t)UART_STOPBITS_2
/** @} */ // UART stop bits.

/**
 * @name UART parity.
 * @{
 */
#define UART_PARITY_MODE_NONE                   (uint32_t)UART_PARITY_NONE
#define UART_PARITY_MODE_EVEN                   (uint32_t)UART_PARITY_EVEN
#define UART_PARITY_MODE_ODD                    (uint32_t)UART_PARITY_ODD
/** @} */ // UART parity.

/**
 * @name Timer configuration.
 * @{
 */
#define TIMER_CLK_ENABLE(INDEX)                 do { if (0 == (INDEX)) { __HAL_RCC_TIM3_CLK_ENABLE(); } else\
                                                                       { __HAL_RCC_TIM3_CLK_ENABLE(); }} while (0)
#define TIMER_CLK_DISABLE(INDEX)                do { if (0 == (INDEX)) { __HAL_RCC_TIM3_CLK_DISABLE(); } else\
                                                                       { __HAL_RCC_TIM3_CLK_DISABLE(); }} while (0)

#define TIMER0_INST                             TIM3
#define TIMER0_CLK                              10000
#define TIMER0_IRQ                              TIM3_IRQn
#define TIMER0_IRQ_HANDLER                      TIM3_IRQHandler

/**
 * @brief Timer config.
 * @{
 */
typedef struct
{ 
	uint16_t  clk_;
	IRQn_Type irq_;
}timer_confg_t;
/** @} */ // Timer configuration.

/**
 * @name Flash configuration.
 * @{
 */
#define FLASH_BASE_ADDR 			            0x08000000
#define ADDR_FLASH_SECTOR_0                     FLASH_BASE_ADDR ///< Base @ of Sector 0, 16 Kbytes
#define ADDR_FLASH_SECTOR_1                     ((uint32_t)0x08004000) ///< Base @ of Sector 1, 16 Kbytes
#define ADDR_FLASH_SECTOR_2                     ((uint32_t)0x08008000) ///< Base @ of Sector 2, 16 Kbytes
#define ADDR_FLASH_SECTOR_3                     ((uint32_t)0x0800C000) ///< Base @ of Sector 3, 16 Kbytes
#define ADDR_FLASH_SECTOR_4                     ((uint32_t)0x08010000) ///< Base @ of Sector 4, 64 Kbytes
#define ADDR_FLASH_SECTOR_5                     ((uint32_t)0x08020000) ///< Base @ of Sector 5, 128 Kbytes
#define ADDR_FLASH_SECTOR_6                     ((uint32_t)0x08040000) ///< Base @ of Sector 6, 128 Kbytes
#define ADDR_FLASH_SECTOR_7                     ((uint32_t)0x08060000) ///< Base @ of Sector 7, 128 Kbytes
#define ADDR_FLASH_SECTOR_8                     ((uint32_t)0x08080000) ///< Base @ of Sector 8, 128 Kbytes
#define ADDR_FLASH_SECTOR_9                     ((uint32_t)0x080A0000) ///< Base @ of Sector 9, 128 Kbytes
#define ADDR_FLASH_SECTOR_10                    ((uint32_t)0x080C0000) ///< Base @ of Sector 10, 128 Kbytes
#define ADDR_FLASH_SECTOR_11                    ((uint32_t)0x080E0000) ///< Base @ of Sector 11, 128 Kbytes
#define FLASH_TOTAL_SIZE                        0x80000 ///< 512k
/** @} */ // Flash configuration.
#if defined MX_TB
#include "mx_tb/stm32f205_mx_tb.h"
#else
#error Board type not defined!!!
#endif
#else
#error Mcu type not defined!!!
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*******************************************************************************
 * Function prototypes
 ******************************************************************************/
#ifdef __cplusplus
}
#endif

#endif // __CONFIG__H__
