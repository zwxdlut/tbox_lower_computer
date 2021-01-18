/*
 * s32k144_mx_tb.h
 *
 *  Created on: 2018��8��21��
 *      Author: Administrator
 */

#ifndef __S32K144_MX_TB_H__
#define __S32K144_MX_TB_H__

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/** 
 * @name The LEDs configuration
 * @{
 */
#define LED0_PORT       					    PORTE
#define LED0_GPIO       					    PTE
#define LED0_PIN           					    8
#define LED1_PORT       					    PORTB
#define LED1_GPIO       					    PTB
#define LED1_PIN           					    5
#define LED2_PORT       					    PORTB
#define LED2_GPIO       					    PTB
#define LED2_PIN           					    4
#define LED_ON          					    1
#define LED_OFF         					    0
/** @} */ // The LEDs configuration
 
/** 
 * @name The buttons configuration
 * @{
 */                                       
#define BTN_PORT        					    PORTA
#define BTN_GPIO        					    PTA
#define BTN_PIN         					    0
#define BTN_IRQ                                 PORTA_IRQn
/** @} */ // The buttons configuration

/**
 * @name The upper computer configuration
 * @{
 */
#define UC_POWER_PORT						    PORTE
#define UC_POWER_GPIO 					        PTE
#define UC_POWER_PIN						    2
#define UC_WAKEUP_PORT						    PORTA
#define UC_WAKEUP_GPIO 					        PTA
#define UC_WAKEUP_PIN						    12
#define UC_RESET_PORT						    PORTA
#define UC_RESET_GPIO 					        PTA
#define UC_RESET_PIN						    13
/** @} */ // The upper computer configuration

/**
 * @name The ignition configuration
 * @{
 */
#define IGN_PORT        					    PORTD
#define IGN_GPIO        					    PTD
#define IGN_PIN         					    16
#define IGN_IRQ                                 PORTD_IRQn
/** @} */ // The ignition configuration

/**
 * @name The UART configuration
 * @{
 */
#define UART0_PORT                              PORTB
#define UART0_RX_PIN                            0
#define UART0_TX_PIN                            1
#define UART0_GPIO_AF                           PORT_MUX_ALT2
#define UART0_IRQ                               LPUART0_RxTx_IRQn
#define UART1_PORT                              PORTD
#define UART1_RX_PIN                            6
#define UART1_TX_PIN                            7
#define UART1_GPIO_AF                           PORT_MUX_ALT2
#define UART1_IRQ                               LPUART2_RxTx_IRQn
/** @} */ // The UART configuration

/** 
 * @name The CAN configuration
 * @{
 */
#define CAN0_PORT                               PORTE
#define CAN0_RX_PIN                             4
#define CAN0_TX_PIN                             5
#define CAN0_GPIO_AF                            PORT_MUX_ALT5
#define CAN0_ORED_IRQ                           CAN0_ORed_IRQn
#define CAN0_ERROR_IRQ                          CAN0_Error_IRQn
#define CAN0_WAKEUP_IRQ                         CAN0_Wake_Up_IRQn
#define CAN0_ORED_0_15_IRQ                      CAN0_ORed_0_15_MB_IRQn
#define CAN0_ORED_16_31_IRQ                     CAN0_ORed_16_31_MB_IRQn
#define CAN0_TRANS_STB_N_PORT				    PORTC
#define CAN0_TRANS_STB_N_GPIO    			    PTC
#define CAN0_TRANS_STB_N_PIN				    8
#define CAN0_TRANS_EN_PORT				        PORTC
#define CAN0_TRANS_EN_GPIO    			        PTC
#define CAN0_TRANS_EN_PIN 					    9
#define CAN0_TRANS_INH_PORT				        PORTC
#define CAN0_TRANS_INH_GPIO    			        PTC
#define CAN0_TRANS_INH_PIN					    17
#define CAN0_TRANS_INH_IRQ                      PORTC_IRQn
#define CAN1_PORT                               PORTC
#define CAN1_RX_PIN                             6
#define CAN1_TX_PIN                             7
#define CAN1_GPIO_AF                            PORT_MUX_ALT3
#define CAN1_ORED_IRQ                           CAN1_ORed_IRQn
#define CAN1_ERROR_IRQ                          CAN1_Error_IRQn
#define CAN1_ORED_0_15_IRQ                      CAN1_ORed_0_15_MB_IRQn
#define CAN1_TRANS_STB_N_PORT				    PORTC
#define CAN1_TRANS_STB_N_GPIO				    PTC
#define CAN1_TRANS_STB_N_PIN				    14
#define CAN1_TRANS_EN_PORT    				    PORTC
#define CAN1_TRANS_EN_GPIO    				    PTC
#define CAN1_TRANS_EN_PIN 					    15
#define CAN1_TRANS_INH_PORT				        PORTB
#define CAN1_TRANS_INH_GPIO    			        PTB
#define CAN1_TRANS_INH_PIN					    2
#define CAN1_TRANS_INH_IRQ  		            PORTB_IRQn
/** @} */ // The CAN configuration

/** 
 * @name The I2C configuration
 * @{
 */
#define I2C0_PORT                               PORTA
#define I2C0_SCL_PIN                            3
#define I2C0_SDA_PIN                            2
#define I2C0_GPIO_AF                            PORT_MUX_ALT3
#define I2C0_IRQ                                LPI2C0_Master_IRQn
/** @} */ // The I2C configuration

/**
 * @name The SPI configuration
 * @{
 */
#define SPI0_SCK_PORT                           PORTB
#define SPI0_SCK_PIN                            14
#define SPI0_SDI_PORT                           PORTD
#define SPI0_SDI_PIN                            1
#define SPI0_SDO_PORT                           PORTD
#define SPI0_SDO_PIN                            2
#define SPI0_CS_PORT                            PORTD
#define SPI0_CS_PIN                             3
#define SPI0_GPIO_AF                            PORT_MUX_ALT3
#define SPI0_IRQ                                LPSPI1_IRQn
/** @} */ // The SPI configuration

#ifdef __cplusplus
}
#endif

#endif /* __S32K144_MX_TB_H__ */
