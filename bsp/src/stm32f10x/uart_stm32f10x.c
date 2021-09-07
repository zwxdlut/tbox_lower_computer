/*
 * uart_stm32f10x.c
 *
 *  Created on: 2018��10��16��
 *      Author: Administrator
 */

#include "uart.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#if defined USING_OS_FREERTOS
SemaphoreHandle_t g_uart_tx_mutex[UART1_INDEX + 1] = {NULL, NULL}; /* the TX mutex */
#endif

uint8_t  g_uart_rx_queue[UART1_INDEX + 1][UART_RX_BUFFER_SIZE]; /* the RX queue */
uint16_t g_uart_rx_queue_head[UART1_INDEX + 1] = {0, 0}; /* the RX queue head */
uint16_t g_uart_rx_queue_tail[UART1_INDEX + 1] = {0, 0}; /* the RX queue tail */

typedef struct
{
	GPIO_TypeDef *gpio_;
	uint16_t     rx_pin_;
	uint16_t     tx_pin_;
	IRQn_Type    irqs_[1];
} comm_config_t;

static comm_config_t g_comm_config[UART1_INDEX + 1] =
{
	{
		.gpio_   = UART0_GPIO,
		.rx_pin_ = UART0_RX_PIN,
		.tx_pin_ = UART0_TX_PIN,
		.irqs_   = {UART0_IRQ}
	},
	{
		.gpio_   = UART1_GPIO,
		.rx_pin_ = UART1_RX_PIN,
		.tx_pin_ = UART1_TX_PIN,
		.irqs_   = {UART1_IRQ}
	}
};

static USART_TypeDef *g_handle[UART1_INDEX + 1] = {UART0_INST, UART1_INST};

/*******************************************************************************
 * Local function prototypes
 ******************************************************************************/
static void uart_irq_handler(const uint8_t _index);

/*******************************************************************************
 * Functions
 ******************************************************************************/
int32_t uart_init(const uint8_t _index, const uint32_t _baudrate, const uint32_t _data_bits, const uint32_t _stop_bits, const uint32_t _parity)
{
	assert(UART1_INDEX >= _index);
	
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;

	/* initialize the RX queue */
	g_uart_rx_queue_head[_index] = 0;
	g_uart_rx_queue_tail[_index] = 0;
	
#if defined USING_OS_FREERTOS
	g_uart_tx_mutex[_index] = xSemaphoreCreateRecursiveMutex();
#endif
	
	/* initialize the GPIOs */
	UART_GPIO_CLK_ENABLE(_index);
	GPIO_InitStructure.GPIO_Pin   = g_comm_config[_index].rx_pin_;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_Init(g_comm_config[_index].gpio_, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin   = g_comm_config[_index].tx_pin_;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(g_comm_config[_index].gpio_, &GPIO_InitStructure);
	
	/* initialize the UART */
	UART_CLK_ENABLE(_index);
	USART_InitStructure.USART_BaudRate            = _baudrate;
	USART_InitStructure.USART_WordLength          = _data_bits;
	USART_InitStructure.USART_StopBits            = _stop_bits;
	USART_InitStructure.USART_Parity              = _parity;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(g_handle[_index], &USART_InitStructure);
	USART_ITConfig(g_handle[_index], USART_IT_RXNE, ENABLE);
	USART_Cmd(g_handle[_index], ENABLE);
	USART_ClearFlag(g_handle[_index], USART_FLAG_TC);
	
	/* initialize the NVIC */
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	for (uint8_t i = 0; i < sizeof(g_comm_config[_index].irqs_); i++)
	{
		NVIC_InitStructure.NVIC_IRQChannel = g_comm_config[_index].irqs_[i];
		NVIC_Init(&NVIC_InitStructure);
	}
	
	return 0;
}

int32_t uart_deinit(const uint8_t _index)
{
	assert(UART1_INDEX >= _index);

	NVIC_InitTypeDef  NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd                = DISABLE;
	NVIC_Init(&NVIC_InitStructure);

	for (uint8_t i = 0; i < sizeof(g_comm_config[_index].irqs_); i++)
	{
		NVIC_InitStructure.NVIC_IRQChannel = g_comm_config[_index].irqs_[i];
		NVIC_Init(&NVIC_InitStructure);
	}

	USART_ClearITPendingBit(g_handle[_index], USART_IT_RXNE);
	USART_ClearFlag(g_handle[_index], USART_FLAG_TC);
	USART_Cmd(g_handle[_index], DISABLE);
	USART_ITConfig(g_handle[_index], USART_IT_RXNE, DISABLE);
	USART_DeInit(g_handle[_index]);
	UART_CLK_DISABLE(_index);
	UART_FORCE_RESET(_index);
	UART_RELEASE_RESET(_index);

#if defined USING_OS_FREERTOS
	vSemaphoreDelete(g_uart_tx_mutex[_index]);
#endif

	return 0;
}

uint16_t uart_send(const uint8_t _index, const uint8_t _buf[], const uint16_t _size)
{
	assert(UART1_INDEX >= _index && NULL != _buf);
	
#if defined USING_OS_FREERTOS
	xSemaphoreTakeRecursive( g_uart_tx_mutex[_index], portMAX_DELAY);
#endif

	while (USART_GetFlagStatus(g_handle[_index], USART_FLAG_TC) == RESET) {}

    for (uint16_t i = 0; i < _size; i++)        
    {           
		USART_SendData(g_handle[_index], _buf[i]); 
		while (USART_GetFlagStatus(g_handle[_index], USART_FLAG_TC) == RESET) {}
    }

#if defined USING_OS_FREERTOS
	xSemaphoreGiveRecursive( g_uart_tx_mutex[_index] );
#endif

	return _size;
}

/**
 * @name The IRQ handlers
 * @{
 */
/**
 * The UART0 IRQ handler.
 */
void UART0_IRQ_HANDLER(void)
{	
	uart_irq_handler(UART0_INDEX);
}

/**
 * The UART1 IRQ handler.
 */
void UART1_IRQ_HANDLER(void)
{
	uart_irq_handler(UART1_INDEX);
}
/** @} */ /* The IRQ handlers */

/*******************************************************************************
 * Local functions
 ******************************************************************************/
/**
 * The UART IRQ handler.
 *
 * @param [in] _index the UART channel index
 */
void uart_irq_handler(const uint8_t _index)
{
	/* RXNE */
	if (RESET != USART_GetITStatus(g_handle[_index], USART_IT_RXNE))
	{		
		USART_ClearITPendingBit(g_handle[_index], USART_IT_RXNE);

		/* check if the RX queue is not full */
		if (g_uart_rx_queue_head[_index] != (g_uart_rx_queue_tail[_index] + 1) % UART_RX_BUFFER_SIZE)
		{
			/* push the RX queue */
			g_uart_rx_queue[_index][g_uart_rx_queue_tail[_index]] = USART_ReceiveData(g_handle[_index]);
			g_uart_rx_queue_tail[_index] = (g_uart_rx_queue_tail[_index] + 1) % UART_RX_BUFFER_SIZE;
		}		
    }
}
