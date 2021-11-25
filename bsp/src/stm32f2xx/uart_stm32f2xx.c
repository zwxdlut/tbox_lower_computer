/*
 * uart_stm32f2xx.c
 *
 *  Created on: 2018年10月16日
 *      Author: Administrator
 */

#include "uart.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#if defined USING_OS_FREERTOS
SemaphoreHandle_t g_uart_tx_mutex[UART1_INDEX + 1] = {NULL, NULL}; /* Sending mutex */
#endif

uint8_t g_uart_rx_queue[UART1_INDEX + 1][UART_RX_BUFFER_SIZE]; /* Receiving queue */
uint16_t g_uart_rx_queue_head[UART1_INDEX + 1] = {0, 0}; /* Receiving queue head */
uint16_t g_uart_rx_queue_tail[UART1_INDEX + 1] = {0, 0}; /* Receiving queue tail */

typedef struct
{
	GPIO_TypeDef *gpio_;
	uint16_t rx_pin_;
	uint16_t tx_pin_;
	uint8_t gpio_af_;
	IRQn_Type irqs_[1];
} comm_config_t;

static comm_config_t g_comm_config[UART1_INDEX + 1] =
{
	{
		.gpio_    = UART0_GPIO,
		.rx_pin_  = UART0_RX_PIN,
		.tx_pin_  = UART0_TX_PIN,
		.gpio_af_ = UART0_GPIO_AF,
		.irqs_    = {UART0_IRQ},
	},
	{
		.gpio_    = UART1_GPIO,
		.rx_pin_  = UART1_RX_PIN,
		.tx_pin_  = UART1_TX_PIN,
		.gpio_af_ = UART1_GPIO_AF,
		.irqs_    = {UART1_IRQ},
	}
};

static UART_HandleTypeDef g_handle[UART1_INDEX + 1] = 
{
	{
		.Instance          = UART0_INST,
		.Init.BaudRate     = 115200,
		.Init.WordLength   = UART_WORDLENGTH_8B,
		.Init.StopBits     = UART_STOPBITS_1,
		.Init.Parity       = UART_PARITY_NONE,
		.Init.HwFlowCtl    = UART_HWCONTROL_NONE,
		.Init.Mode         = UART_MODE_TX_RX,
		.Init.OverSampling = UART_OVERSAMPLING_16,
	},
	{
		.Instance          = UART1_INST,
		.Init.BaudRate     = 115200,
		.Init.WordLength   = UART_WORDLENGTH_8B,
		.Init.StopBits     = UART_STOPBITS_1,
		.Init.Parity       = UART_PARITY_NONE,
		.Init.HwFlowCtl    = UART_HWCONTROL_NONE,
		.Init.Mode         = UART_MODE_TX_RX,
		.Init.OverSampling = UART_OVERSAMPLING_16,
	}
};

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

	/* Initialize the rx queue */
	g_uart_rx_queue_head[_index] = 0;
	g_uart_rx_queue_tail[_index] = 0;
	
#if defined USING_OS_FREERTOS
	g_uart_tx_mutex[_index] = xSemaphoreCreateRecursiveMutex();
#endif
	
	/* Initialize the GPIOs */
	UART_GPIO_CLK_ENABLE(_index);
	GPIO_InitStructure.Pin       = g_comm_config[_index].rx_pin_;
	GPIO_InitStructure.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull      = GPIO_PULLUP;
	GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStructure.Alternate = g_comm_config[_index].gpio_af_;
	HAL_GPIO_Init(g_comm_config[_index].gpio_, &GPIO_InitStructure);
	GPIO_InitStructure.Pin       = g_comm_config[_index].tx_pin_;
	GPIO_InitStructure.Pull      = GPIO_NOPULL;
	HAL_GPIO_Init(g_comm_config[_index].gpio_, &GPIO_InitStructure);
	
	/* Initialize the UART */
	UART_CLK_ENABLE(_index);
	g_handle[_index].Init.BaudRate   = _baudrate;
	g_handle[_index].Init.WordLength = _data_bits;
	g_handle[_index].Init.StopBits   = _stop_bits;
	g_handle[_index].Init.Parity     = _parity;
	HAL_UART_Init(&g_handle[_index]);
	LL_USART_EnableIT_RXNE(g_handle[_index].Instance);
	LL_USART_EnableIT_ERROR(g_handle[_index].Instance);
	
	/* Initialize the NVIC */
	for (uint8_t i = 0; i < sizeof(g_comm_config[_index].irqs_); i++)
	{
		HAL_NVIC_SetPriority(g_comm_config[_index].irqs_[i], 0, 0);
		HAL_NVIC_EnableIRQ(g_comm_config[_index].irqs_[i]);
	}

	return 0;
}

int32_t uart_deinit(const uint8_t _index)
{
	assert(UART1_INDEX >= _index);

	for (uint8_t i = 0; i < sizeof(g_comm_config[_index].irqs_); i++)
	{
		HAL_NVIC_DisableIRQ(g_comm_config[_index].irqs_[i]);
	}

	LL_USART_ClearFlag_RXNE(g_handle[_index].Instance);
	LL_USART_DisableIT_RXNE(g_handle[_index].Instance);
	LL_USART_DisableIT_ERROR(g_handle[_index].Instance);
	HAL_UART_DeInit(&g_handle[_index]);

	UART_CLK_DISABLE(_index);
	UART_FORCE_RESET(_index);
	UART_RELEASE_RESET(_index);
	HAL_GPIO_DeInit(g_comm_config[_index].gpio_, g_comm_config[_index].rx_pin_ | g_comm_config[_index].tx_pin_);

#if defined USING_OS_FREERTOS
	vSemaphoreDelete(g_uart_tx_mutex[_index]);
#endif

	return 0;
}

uint16_t uart_send(const uint8_t _index, const uint8_t _buf[], const uint16_t _size)
{
	assert(UART1_INDEX >= _index && NULL != _buf);

	uint16_t size = 0;
	
#if defined USING_OS_FREERTOS
	xSemaphoreTakeRecursive( g_uart_tx_mutex[_index], portMAX_DELAY);
#endif

	if (HAL_OK == HAL_UART_Transmit(&g_handle[_index], (uint8_t*)_buf, _size, 1000))
	{
		size = _size;
	}

#if defined USING_OS_FREERTOS
	xSemaphoreGiveRecursive( g_uart_tx_mutex[_index] );
#endif

	return size;
}

/**
 * @name IRQ handlers
 * @{
 */

/**
 * UART0 IRQ handler.
 */
void UART0_IRQ_HANDLER(void)
{	
	uart_irq_handler(UART0_INDEX);
}

/**
 * UART1 IRQ handler.
 */
void UART1_IRQ_HANDLER(void)
{
	uart_irq_handler(UART1_INDEX);
}

/** @} */ /* IRQ handlers */

/*******************************************************************************
 * Local functions
 ******************************************************************************/
/**
 * UART IRQ handler.
 *
 * @param [in] _index UART channel index
 */
static void uart_irq_handler(const uint8_t _index)
{
	/* RXNE */
	if (0 != LL_USART_IsActiveFlag_RXNE(g_handle[_index].Instance) && 0 != LL_USART_IsEnabledIT_RXNE(g_handle[_index].Instance))
	{
		LL_USART_ClearFlag_RXNE(g_handle[_index].Instance);

		/* Check if the rx queue is full */
		if (g_uart_rx_queue_head[_index] == (g_uart_rx_queue_tail[_index] + 1) % UART_RX_BUFFER_SIZE)
		{
			/* Dequeue */
			g_uart_rx_queue_head[_index] = (g_uart_rx_queue_head[_index] + 1) % UART_RX_BUFFER_SIZE;
		}
		
		/* Enqueue */
		g_uart_rx_queue[_index][g_uart_rx_queue_tail[_index]] = LL_USART_ReceiveData8(g_handle[_index].Instance);
		g_uart_rx_queue_tail[_index] = (g_uart_rx_queue_tail[_index] + 1) % UART_RX_BUFFER_SIZE;		
    }
}
