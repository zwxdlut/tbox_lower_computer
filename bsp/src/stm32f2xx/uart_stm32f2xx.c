#include "uart.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#if defined USING_OS_FREERTOS
SemaphoreHandle_t g_uart_tx_mutex[UART_CH2 + 1] = {NULL, NULL, NULL}; /* sending mutex */
#endif

uint8_t g_uart_rx_queue[UART_CH2 + 1][UART_RX_BUFFER_SIZE]; /* receiving queue */
uint16_t g_uart_rx_queue_head[UART_CH2 + 1] = {0, 0, 0}; /* receiving queue head */
uint16_t g_uart_rx_queue_tail[UART_CH2 + 1] = {0, 0, 0}; /* receiving queue tail */

typedef struct
{
	GPIO_TypeDef *gpio_;
	uint16_t rx_pin_;
	uint16_t tx_pin_;
	uint8_t gpio_af_;
	IRQn_Type irqs_[1];
} comm_config_t;

static comm_config_t g_comm_config[UART_CH2 + 1] =
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

static UART_HandleTypeDef g_handle[UART_CH2 + 1] = 
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
static void uart_irq_handler(const uint8_t _chl);

/*******************************************************************************
 * Functions
 ******************************************************************************/
int32_t uart_init(const uint8_t _chl, const uint32_t _baudrate, const uint32_t _data_bits, const uint32_t _stop_bits, const uint32_t _parity)
{
	assert(UART_CH2 >= _chl);
	
	GPIO_InitTypeDef GPIO_InitStructure;

	/* initialize the rx queue */
	g_uart_rx_queue_head[_chl] = 0;
	g_uart_rx_queue_tail[_chl] = 0;
	
#if defined USING_OS_FREERTOS
	g_uart_tx_mutex[_chl] = xSemaphoreCreateRecursiveMutex();
#endif
	
	/* initialize the GPIOs */
	UART_GPIO_CLK_ENABLE(_chl);
	GPIO_InitStructure.Pin = g_comm_config[_chl].rx_pin_;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStructure.Alternate = g_comm_config[_chl].gpio_af_;
	HAL_GPIO_Init(g_comm_config[_chl].gpio_, &GPIO_InitStructure);
	GPIO_InitStructure.Pin = g_comm_config[_chl].tx_pin_;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(g_comm_config[_chl].gpio_, &GPIO_InitStructure);
	
	/* initialize the UART */
	UART_CLK_ENABLE(_chl);
	g_handle[_chl].Init.BaudRate = _baudrate;
	g_handle[_chl].Init.WordLength = _data_bits;
	g_handle[_chl].Init.StopBits = _stop_bits;
	g_handle[_chl].Init.Parity = _parity;
	HAL_UART_Init(&g_handle[_chl]);
	LL_USART_EnableIT_RXNE(g_handle[_chl].Instance);
	LL_USART_EnableIT_ERROR(g_handle[_chl].Instance);
	
	/* initialize the NVIC */
	for (uint8_t i = 0; i < sizeof(g_comm_config[_chl].irqs_); i++)
	{
		HAL_NVIC_SetPriority(g_comm_config[_chl].irqs_[i], 0, 0);
		HAL_NVIC_EnableIRQ(g_comm_config[_chl].irqs_[i]);
	}

	return 0;
}

int32_t uart_deinit(const uint8_t _chl)
{
	assert(UART_CH2 >= _chl);

	for (uint8_t i = 0; i < sizeof(g_comm_config[_chl].irqs_); i++)
	{
		HAL_NVIC_DisableIRQ(g_comm_config[_chl].irqs_[i]);
	}

	LL_USART_ClearFlag_RXNE(g_handle[_chl].Instance);
	LL_USART_DisableIT_RXNE(g_handle[_chl].Instance);
	LL_USART_DisableIT_ERROR(g_handle[_chl].Instance);
	HAL_UART_DeInit(&g_handle[_chl]);

	UART_CLK_DISABLE(_chl);
	UART_FORCE_RESET(_chl);
	UART_RELEASE_RESET(_chl);
	HAL_GPIO_DeInit(g_comm_config[_chl].gpio_, g_comm_config[_chl].rx_pin_ | g_comm_config[_chl].tx_pin_);

#if defined USING_OS_FREERTOS
	vSemaphoreDelete(g_uart_tx_mutex[_chl]);
#endif

	return 0;
}

uint16_t uart_send(const uint8_t _chl, const uint8_t _buf[], const uint16_t _size)
{
	assert(UART_CH2 >= _chl && NULL != _buf);

	uint16_t size = 0;
	
#if defined USING_OS_FREERTOS
	xSemaphoreTakeRecursive( g_uart_tx_mutex[_chl], portMAX_DELAY);
#endif

	if (HAL_OK == HAL_UART_Transmit(&g_handle[_chl], (uint8_t*)_buf, _size, 1000))
	{
		size = _size;
	}

#if defined USING_OS_FREERTOS
	xSemaphoreGiveRecursive( g_uart_tx_mutex[_chl] );
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
	uart_irq_handler(UART_CH0);
}

/**
 * UART1 IRQ handler.
 */
void UART1_IRQ_HANDLER(void)
{
	uart_irq_handler(UART_CH1);
}

/** @} */ /* IRQ handlers */

/*******************************************************************************
 * Local functions
 ******************************************************************************/
/**
 * UART IRQ handler.
 *
 * @param [in] _chl the UART channel number
 */
static void uart_irq_handler(const uint8_t _chl)
{
	/* RXNE */
	if (0 != LL_USART_IsActiveFlag_RXNE(g_handle[_chl].Instance) && 0 != LL_USART_IsEnabledIT_RXNE(g_handle[_chl].Instance))
	{
		LL_USART_ClearFlag_RXNE(g_handle[_chl].Instance);

		/* check if the rx queue is full */
		if (g_uart_rx_queue_head[_chl] == (g_uart_rx_queue_tail[_chl] + 1) % UART_RX_BUFFER_SIZE)
		{
			/* dequeue */
			g_uart_rx_queue_head[_chl] = (g_uart_rx_queue_head[_chl] + 1) % UART_RX_BUFFER_SIZE;
		}
		
		/* enqueue */
		g_uart_rx_queue[_chl][g_uart_rx_queue_tail[_chl]] = LL_USART_ReceiveData8(g_handle[_chl].Instance);
		g_uart_rx_queue_tail[_chl] = (g_uart_rx_queue_tail[_chl] + 1) % UART_RX_BUFFER_SIZE;		
    }
}
