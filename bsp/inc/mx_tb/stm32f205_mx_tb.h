/*
 * stm32f205_mx_tb.h
 *
 *  Created on: 2018��8��21��
 *      Author: Administrator
 */

#ifndef __STM32F205_MX_TB_H__
#define __STM32F205_MX_TB_H__

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/** 
 * @name LEDs configuration.
 * @{
 */
#define LED0_GPIO       					    GPIOC
#define LED0_PIN            			        GPIO_PIN_2
#define LED0_GPIO_CLK_ENABLE()                  __HAL_RCC_GPIOC_CLK_ENABLE()
#define LED0_GPIO_CLK_DISABLE()                 __HAL_RCC_GPIOC_CLK_DISABLE()
#define LED1_GPIO       					    GPIOC
#define LED1_PIN            			        GPIO_PIN_1
#define LED1_GPIO_CLK_ENABLE()                  __HAL_RCC_GPIOC_CLK_ENABLE()
#define LED1_GPIO_CLK_DISABLE()                 __HAL_RCC_GPIOC_CLK_DISABLE()
#define LED2_GPIO       					    GPIOC
#define LED2_PIN            			        GPIO_PIN_0
#define LED2_GPIO_CLK_ENABLE()                  __HAL_RCC_GPIOC_CLK_ENABLE()
#define LED2_GPIO_CLK_DISABLE()                 __HAL_RCC_GPIOC_CLK_DISABLE()
#define LED_ON          					    GPIO_PIN_SET
#define LED_OFF         					    GPIO_PIN_RESET
/** @} */ // LEDs configuration.
 
/** 
 * @name Button configuration.
 * @{
 */                                       
#define BTN_GPIO        					    GPIOC
#define BTN_PIN         					    GPIO_PIN_3
#define BTN_IRQ         				        EXTI3_IRQn
#define BTN_IRQ_HANDLER                         EXTI3_IRQHandler
#define BTN_GPIO_CLK_ENABLE()                   __HAL_RCC_GPIOC_CLK_ENABLE()
#define BTN_GPIO_CLK_DISABLE()                  __HAL_RCC_GPIOC_CLK_DISABLE()
/** @} */ // Button configuration.

/**
 * @name Upper computer configuration.
 * @{
 */
#define UC_POWER_GPIO 					        GPIOC
#define UC_POWER_PIN						    GPIO_PIN_5
#define UC_POWER_GPIO_CLK_ENABLE()              __HAL_RCC_GPIOC_CLK_ENABLE()
#define UC_POWER_GPIO_CLK_DISABLE()             __HAL_RCC_GPIOC_CLK_DISABLE()
#define UC_WAKEUP_GPIO 					        GPIOB
#define UC_WAKEUP_PIN						    GPIO_PIN_1
#define UC_WAKEUP_GPIO_CLK_ENABLE()             __HAL_RCC_GPIOB_CLK_ENABLE()
#define UC_WAKEUP_GPIO_CLK_DISABLE()            __HAL_RCC_GPIOB_CLK_DISABLE()
#define UC_RESET_GPIO 					        GPIOB
#define UC_RESET_PIN						    GPIO_PIN_0
#define UC_RESET_GPIO_CLK_ENABLE()              __HAL_RCC_GPIOB_CLK_ENABLE()
#define UC_RESET_GPIO_CLK_DISABLE()             __HAL_RCC_GPIOB_CLK_DISABLE()
/** @} */ // Upper computer configuration.

/**
 * @name Ignition configuration.
 * @{
 */
#define IGN_GPIO        					    GPIOC
#define IGN_PIN         					    GPIO_PIN_4
#define IGN_IRQ   					            EXTI4_IRQn
#define IGN_IRQ_HANDLER                         EXTI4_IRQHandler
#define IGN_GPIO_CLK_ENABLE()                   __HAL_RCC_GPIOC_CLK_ENABLE()
#define IGN_GPIO_CLK_DISABLE()                  __HAL_RCC_GPIOC_CLK_DISABLE()
/** @} */ // Ignition configuration.

/**
 * @name UART configuration.
 * @{
 */
#define UART0_GPIO                              GPIOA
#define UART0_RX_PIN                            GPIO_PIN_3
#define UART0_TX_PIN                            GPIO_PIN_2
#define UART0_GPIO_AF                           GPIO_AF7_USART2
#define UART0_INST     		                    USART2
#define UART0_IRQ                               USART2_IRQn
#define UART0_IRQ_HANDLER                       USART2_IRQHandler
#define UART1_GPIO                              GPIOA
#define UART1_RX_PIN                            GPIO_PIN_10
#define UART1_TX_PIN                            GPIO_PIN_9
#define UART1_GPIO_AF                           GPIO_AF7_USART1
#define UART1_INST     		                    USART1
#define UART1_IRQ                               USART1_IRQn
#define UART1_IRQ_HANDLER                       USART1_IRQHandler
#define UART_GPIO_CLK_ENABLE(INDEX)             do { if (0 == (INDEX)) { __HAL_RCC_GPIOA_CLK_ENABLE(); } else\
                                                                       { __HAL_RCC_GPIOA_CLK_ENABLE(); }} while (0)
#define UART_GPIO_CLK_DISABLE(INDEX)            do { if (0 == (INDEX)) { __HAL_RCC_GPIOA_CLK_DISABLE(); } else\
                                                                       { __HAL_RCC_GPIOA_CLK_DISABLE(); }} while (0)
#define UART_CLK_ENABLE(INDEX)                  do { if (0 == (INDEX)) { __HAL_RCC_USART2_CLK_ENABLE(); } else\
                                                                       { __HAL_RCC_USART1_CLK_ENABLE(); }} while (0)
#define UART_CLK_DISABLE(INDEX)                 do { if (0 == (INDEX)) { __HAL_RCC_USART2_CLK_DISABLE(); } else\
                                                                       { __HAL_RCC_USART1_CLK_DISABLE(); }} while (0)
#define UART_FORCE_RESET(INDEX)                 do { if (0 == (INDEX)) { __HAL_RCC_USART2_FORCE_RESET(); } else\
                                                                       { __HAL_RCC_USART1_FORCE_RESET(); }} while (0)
#define UART_RELEASE_RESET(INDEX)               do { if (0 == (INDEX)) { __HAL_RCC_USART2_RELEASE_RESET(); } else\
                                                                       { __HAL_RCC_USART1_RELEASE_RESET(); }} while (0)
/** @} */ // UART configuration.

/** 
 * @name CAN configuration.
 * @{
 */
#define CAN0_GPIO                               GPIOB
#define CAN0_RX_PIN                             GPIO_PIN_8
#define CAN0_TX_PIN                             GPIO_PIN_9
#define CAN0_GPIO_AF                            GPIO_AF9_CAN1
#define CAN0_INST     		                    CAN1
#define CAN0_RX_IRQ                             CAN1_RX0_IRQn
#define CAN0_RX_IRQ_HANDLER                     CAN1_RX0_IRQHandler
#define CAN0_TRANS_STB_N_GPIO                   GPIOC
#define CAN0_TRANS_STB_N_PIN                    GPIO_PIN_7
#define CAN0_TRANS_EN_GPIO                      GPIOC
#define CAN0_TRANS_EN_PIN                       GPIO_PIN_9
#define CAN0_TRANS_INH_GPIO                     GPIOC
#define CAN0_TRANS_INH_PIN                      GPIO_PIN_8
#define CAN0_TRANS_INH_IRQ                      EXTI9_5_IRQn
#define CAN0_TRANS_INH_IRQ_HANDLER              EXTI9_5_IRQHandler
#define CAN1_GPIO                               GPIOB
#define CAN1_RX_PIN                             GPIO_PIN_12
#define CAN1_TX_PIN                             GPIO_PIN_13
#define CAN1_GPIO_AF                            GPIO_AF9_CAN2
#define CAN1_INST     		                    CAN2
#define CAN1_RX_IRQ                             CAN2_RX0_IRQn
#define CAN1_RX_IRQ_HANDLER                     CAN2_RX0_IRQHandler	
#define CAN1_TRANS_STB_N_GPIO                   GPIOB
#define CAN1_TRANS_STB_N_PIN                    GPIO_PIN_10
#define CAN1_TRANS_EN_GPIO                      GPIOB
#define CAN1_TRANS_EN_PIN                       GPIO_PIN_14
#define CAN1_TRANS_INH_GPIO                     GPIOB
#define CAN1_TRANS_INH_PIN                      GPIO_PIN_15
#define CAN1_TRANS_INH_IRQ                      EXTI15_10_IRQn
#define CAN1_TRANS_INH_IRQ_HANDLER              EXTI15_10_IRQHandler
#define CAN_GPIO_CLK_ENABLE(INDEX)              do { if (0 == (INDEX)) { __HAL_RCC_GPIOB_CLK_ENABLE(); } else\
                                                                       { __HAL_RCC_GPIOB_CLK_ENABLE(); }} while (0)
#define CAN_GPIO_CLK_DISABLE(INDEX)             do { if (0 == (INDEX)) { __HAL_RCC_GPIOB_CLK_DISABLE(); } else\
                                                                       { __HAL_RCC_GPIOB_CLK_DISABLE(); }} while (0)
#define CAN_CLK_ENABLE(INDEX)                   do { if (0 == (INDEX)) { __HAL_RCC_CAN1_CLK_ENABLE(); } else\
                                                                       { __HAL_RCC_CAN2_CLK_ENABLE(); }} while (0)
#define CAN_CLK_DISABLE(INDEX)                  do { if (0 == (INDEX)) { __HAL_RCC_CAN1_CLK_DISABLE(); } else\
                                                                       { __HAL_RCC_CAN2_CLK_DISABLE(); }} while (0)
#define CAN_FORCE_RESET(INDEX)                  do { if (0 == (INDEX)) { __HAL_RCC_CAN1_FORCE_RESET(); } else\
                                                                       { __HAL_RCC_CAN2_FORCE_RESET(); }} while (0)
#define CAN_RELEASE_RESET(INDEX)                do { if (0 == (INDEX)) { __HAL_RCC_CAN1_RELEASE_RESET(); } else\
                                                                       { __HAL_RCC_CAN2_RELEASE_RESET(); }} while (0)
#define CAN_TRANS_STB_N_GPIO_CLK_ENABLE(INDEX)  do { if (0 == (INDEX)) { __HAL_RCC_GPIOC_CLK_ENABLE(); } else\
                                                                       { __HAL_RCC_GPIOB_CLK_ENABLE(); }} while (0)
#define CAN_TRANS_STB_N_GPIO_CLK_DISABLE(INDEX) do { if (0 == (INDEX)) { __HAL_RCC_GPIOC_CLK_DISABLE(); } else\
                                                                       { __HAL_RCC_GPIOB_CLK_DISABLE(); }} while (0)
#define CAN_TRANS_EN_GPIO_CLK_ENABLE(INDEX)     do { if (0 == (INDEX)) { __HAL_RCC_GPIOC_CLK_ENABLE(); } else\
                                                                       { __HAL_RCC_GPIOB_CLK_ENABLE(); }} while (0)
#define CAN_TRANS_EN_GPIO_CLK_DISABLE(INDEX)    do { if (0 == (INDEX)) { __HAL_RCC_GPIOC_CLK_DISABLE(); } else\
                                                                       { __HAL_RCC_GPIOB_CLK_DISABLE(); }} while (0)
#define CAN_TRANS_INH_GPIO_CLK_ENABLE(INDEX)    do { if (0 == (INDEX)) { __HAL_RCC_GPIOC_CLK_ENABLE(); } else\
                                                                       { __HAL_RCC_GPIOB_CLK_ENABLE(); }} while (0)
#define CAN_TRANS_INH_GPIO_CLK_DISABLE(INDEX)   do { if (0 == (INDEX)) { __HAL_RCC_GPIOC_CLK_DISABLE(); } else\
                                                                       { __HAL_RCC_GPIOB_CLK_DISABLE(); }} while (0)
#define CAN_SLAVE_START_FILTER_BANK_NUM         21 ///< Slave CAN start filter bank number
/** @} */ // CAN configuration.

/** 
 * @name I2C configuration.
 * @{
 */
#define I2C0_GPIO                               GPIOB
#define I2C0_SCL_PIN                            GPIO_PIN_6
#define I2C0_SDA_PIN                            GPIO_PIN_7
#define I2C0_GPIO_AF                            GPIO_AF4_I2C1
#define I2C0_INST     		                    I2C1
#define I2C0_EV_IRQ                             I2C1_EV_IRQn
#define I2C0_EV_IRQ_HANDLER                     I2C1_EV_IRQHandler
#define I2C0_ER_IRQ                             I2C1_ER_IRQn
#define I2C0_ER_IRQ_HANDLER                     I2C1_ER_IRQHandler
#define I2C_GPIO_CLK_ENABLE(INDEX)              do { if (0 == (INDEX)) { __HAL_RCC_GPIOB_CLK_ENABLE(); } else\
                                                                       { __HAL_RCC_GPIOB_CLK_ENABLE(); }} while (0)
#define I2C_GPIO_CLK_DISABLE(INDEX)             do { if (0 == (INDEX)) { __HAL_RCC_GPIOB_CLK_DISABLE(); } else\
                                                                       { __HAL_RCC_GPIOB_CLK_DISABLE(); }} while (0)
#define I2C_CLK_ENABLE(INDEX)                   do { if (0 == (INDEX)) { __HAL_RCC_I2C1_CLK_ENABLE(); } else\
                                                                       { __HAL_RCC_I2C1_CLK_ENABLE(); }} while (0)
#define I2C_CLK_DISABLE(INDEX)                  do { if (0 == (INDEX)) { __HAL_RCC_I2C1_CLK_DISABLE(); } else\
                                                                       { __HAL_RCC_I2C1_CLK_DISABLE(); }} while (0)
#define I2C_FORCE_RESET(INDEX)                  do { if (0 == (INDEX)) { __HAL_RCC_I2C1_FORCE_RESET(); } else\
                                                                       { __HAL_RCC_I2C1_FORCE_RESET(); }} while (0)
#define I2C_RELEASE_RESET(INDEX)                do { if (0 == (INDEX)) { __HAL_RCC_I2C1_RELEASE_RESET(); } else\
                                                                       { __HAL_RCC_I2C1_RELEASE_RESET(); }} while (0)
/** @} */ // of I2C configuration.

/**
 * @name SPI configuration.
 * @{
 */
#define SPI0_SCK_GPIO                           GPIOC
#define SPI0_SCK_PIN                            GPIO_PIN_10
#define SPI0_MISO_GPIO                          GPIOC
#define SPI0_MISO_PIN                           GPIO_PIN_11
#define SPI0_MOSI_GPIO                          GPIOC
#define SPI0_MOSI_PIN                           GPIO_PIN_12
#define SPI0_CS_GPIO                            GPIOA
#define SPI0_CS_PIN                             GPIO_PIN_4
#define SPI0_GPIO_AF                            GPIO_AF6_SPI3
#define SPI0_INST     		                    SPI3
#define SPI0_IRQ                                SPI3_IRQn
#define SPI0_IRQ_HANDLER                        SPI3_IRQHandler
#define SPI0_RX_DMA_STREAM                      DMA1_Stream0
#define SPI0_RX_DMA_CHANNEL                     DMA_CHANNEL_0
#define SPI0_TX_DMA_STREAM                      DMA1_Stream5
#define SPI0_TX_DMA_CHANNEL                     DMA_CHANNEL_0
#define SPI0_RX_DMA_IRQ                         DMA1_Stream0_IRQn
#define SPI0_RX_DMA_IRQ_HANDLER                 DMA1_Stream0_IRQHandler
#define SPI0_TX_DMA_IRQ                         DMA1_Stream5_IRQn
#define SPI0_TX_DMA_IRQ_HANDLER                 DMA1_Stream5_IRQHandler
#define SPI_SCK_GPIO_CLK_ENABLE(INDEX)          do { if (0 == (INDEX)) { __HAL_RCC_GPIOC_CLK_ENABLE(); } else\
                                                                       { __HAL_RCC_GPIOC_CLK_ENABLE(); }} while (0)
#define SPI_SCK_GPIO_CLK_DISABLE(INDEX)         do { if (0 == (INDEX)) { __HAL_RCC_GPIOC_CLK_DISABLE(); } else\
                                                                       { __HAL_RCC_GPIOC_CLK_DISABLE(); }} while (0)
#define SPI_MISO_GPIO_CLK_ENABLE(INDEX)         do { if (0 == (INDEX)) { __HAL_RCC_GPIOC_CLK_ENABLE(); } else\
                                                                       { __HAL_RCC_GPIOC_CLK_ENABLE(); }} while (0)
#define SPI_MISO_GPIO_CLK_DISABLE(INDEX)        do { if (0 == (INDEX)) { __HAL_RCC_GPIOC_CLK_DISABLE(); } else\
                                                                       { __HAL_RCC_GPIOC_CLK_DISABLE(); }} while (0)
#define SPI_MOSI_GPIO_CLK_ENABLE(INDEX)         do { if (0 == (INDEX)) { __HAL_RCC_GPIOC_CLK_ENABLE(); } else\
                                                                       { __HAL_RCC_GPIOC_CLK_ENABLE(); }} while (0)
#define SPI_MOSI_GPIO_CLK_DISABLE(INDEX)        do { if (0 == (INDEX)) { __HAL_RCC_GPIOC_CLK_DISABLE(); } else\
                                                                       { __HAL_RCC_GPIOC_CLK_DISABLE(); }} while (0)
#define SPI_CS_GPIO_CLK_ENABLE(INDEX)           do { if (0 == (INDEX)) { __HAL_RCC_GPIOC_CLK_ENABLE(); } else\
                                                                       { __HAL_RCC_GPIOC_CLK_ENABLE(); }} while (0)
#define SPI_CS_GPIO_CLK_DISABLE(INDEX)          do { if (0 == (INDEX)) { __HAL_RCC_GPIOC_CLK_DISABLE(); } else\
                                                                       { __HAL_RCC_GPIOC_CLK_DISABLE(); }} while (0)
#define SPI_CLK_ENABLE(INDEX)                   do { if (0 == (INDEX)) { __HAL_RCC_SPI3_CLK_ENABLE(); } else\
                                                                       { __HAL_RCC_SPI3_CLK_ENABLE(); }} while (0)
#define SPI_CLK_DISABLE(INDEX)                  do { if (0 == (INDEX)) { __HAL_RCC_SPI3_CLK_DISABLE(); } else\
                                                                       { __HAL_RCC_SPI3_CLK_DISABLE(); }} while (0)
#define SPI_FORCE_RESET(INDEX)                  do { if (0 == (INDEX)) { __HAL_RCC_SPI3_FORCE_RESET(); } else\
                                                                       { __HAL_RCC_SPI3_FORCE_RESET(); }} while (0)
#define SPI_RELEASE_RESET(INDEX)                do { if (0 == (INDEX)) { __HAL_RCC_SPI3_RELEASE_RESET(); } else\
                                                                       { __HAL_RCC_SPI3_RELEASE_RESET(); }} while (0)
#define SPI_DMA_CLK_ENABLE(INDEX)               do { if (0 == (INDEX)) { __HAL_RCC_DMA1_CLK_ENABLE(); } else\
                                                                       { __HAL_RCC_DMA1_CLK_ENABLE(); }} while (0)
#define SPI_DMA_CLK_DISABLE(INDEX)              do { if (0 == (INDEX)) { __HAL_RCC_DMA1_CLK_DISABLE(); } else\
                                                                       { __HAL_RCC_DMA1_CLK_DISABLE(); }} while (0)
/** @} */ // SPI configuration.

#ifdef __cplusplus
}
#endif

#endif // __STM32F205_MX_TB_H__
