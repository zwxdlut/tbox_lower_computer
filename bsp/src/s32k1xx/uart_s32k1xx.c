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
	PORT_Type *port_;
	uint8_t rx_pin_;
	uint8_t tx_pin_;
	port_mux_t gpio_af_;
	IRQn_Type irqs_[1];
} comm_config_t;

static comm_config_t g_comm_config[UART_CH2 + 1] =
{
	{
		.port_    = UART0_PORT,
		.rx_pin_  = UART0_RX_PIN,
		.tx_pin_  = UART0_TX_PIN,
		.gpio_af_ = UART0_GPIO_AF,
		.irqs_    = {UART0_IRQ},
	},

	{
		.port_    = UART1_PORT,
		.rx_pin_  = UART1_RX_PIN,
		.tx_pin_  = UART1_TX_PIN,
		.gpio_af_ = UART1_GPIO_AF,
		.irqs_    = {UART1_IRQ},
	}
};

static uint8_t g_handle[UART_CH2 + 1] =
{
#if defined INST_LPUART0
	INST_LPUART0,
#else
	0xFF,
#endif

#if defined INST_LPUART1
	INST_LPUART1,
#else
	0xFF,
#endif

#if defined INST_LPUART2
	INST_LPUART2
#else
	0xFF
#endif
};

static lpuart_user_config_t *g_config[UART_CH2 + 1] =
{
#if defined INST_LPUART0
	&lpuart0_InitConfig0,
#else
	NULL,
#endif

#if defined INST_LPUART1
	&lpuart1_InitConfig0,
#else
	NULL,
#endif

#if defined INST_LPUART2
	&lpuart2_InitConfig0
#else
	NULL
#endif
};

static lpuart_state_t *g_state[UART_CH2 + 1] =
{
#if defined INST_LPUART0
	&lpuart0_State,
#else
	NULL,
#endif

#if defined INST_LPUART1
	&lpuart1_State,
#else
	NULL,
#endif

#if defined INST_LPUART2
	&lpuart2_State
#else
	NULL
#endif
};

static uint8_t g_rx_byte[UART_CH2 + 1]; /* receiving byte */

/*******************************************************************************
 * Local function prototypes
 ******************************************************************************/
static void uart_irq_handler(void *_state, uart_event_t _event, void *_user_data);

/*******************************************************************************
 * Functions
 ******************************************************************************/
int32_t uart_init(const uint8_t _chl, const uint32_t _baudrate, const uint32_t _data_bits, const uint32_t _stop_bits, const uint32_t _parity)
{
	assert(UART_CH2 >= _chl);

	/* initialize the rx queue */
	g_uart_rx_queue_head[_chl] = 0;
	g_uart_rx_queue_tail[_chl] = 0;

#if defined USING_OS_FREERTOS
	g_uart_tx_mutex[_chl] = xSemaphoreCreateRecursiveMutex();
#endif

	/* initialize the GPIOs */
	PINS_DRV_SetMuxModeSel(g_comm_config[_chl].port_, g_comm_config[_chl].rx_pin_, g_comm_config[_chl].gpio_af_);
	PINS_DRV_SetMuxModeSel(g_comm_config[_chl].port_, g_comm_config[_chl].tx_pin_, g_comm_config[_chl].gpio_af_);
	PINS_DRV_SetPullSel(g_comm_config[_chl].port_, g_comm_config[_chl].rx_pin_, PORT_INTERNAL_PULL_UP_ENABLED);

	/* initialize the UART */
	g_config[_chl]->baudRate = _baudrate;
	g_config[_chl]->bitCountPerChar = (lpuart_bit_count_per_char_t)_data_bits;
	g_config[_chl]->stopBitCount = (lpuart_stop_bit_count_t)_stop_bits;
	g_config[_chl]->parityMode = (lpuart_parity_mode_t)_parity;
	LPUART_DRV_Init(g_handle[_chl], g_state[_chl], g_config[_chl]);
	LPUART_DRV_InstallRxCallback(g_handle[_chl], uart_irq_handler, (void *)((uint32_t)_chl));

#if defined USING_OS_FREERTOS
	/* The interrupt calls an interrupt safe API function - so its priority must
	   be equal to or lower than configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY */
	for (uint8_t i = 0; i < sizeof(g_comm_config[_chl].irqs_) && NotAvail_IRQn != g_comm_config[_chl].irqs_[i]; i++)
	{
		INT_SYS_SetPriority( g_comm_config[_chl].irqs_[i], configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
	}
#endif

    /* trigger receiving */
	LPUART_DRV_ReceiveData(g_handle[_chl], g_rx_byte + _chl, 1);

	return 0;
}

int32_t uart_deinit(const uint8_t _chl)
{
	assert(UART_CH2 >= _chl);

	LPUART_DRV_Deinit(g_handle[_chl]);
	PINS_DRV_SetMuxModeSel(g_comm_config[_chl].port_, g_comm_config[_chl].rx_pin_, PORT_PIN_DISABLED);
	PINS_DRV_SetMuxModeSel(g_comm_config[_chl].port_, g_comm_config[_chl].tx_pin_, PORT_PIN_DISABLED);

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

	if (STATUS_SUCCESS == LPUART_DRV_SendData(g_handle[_chl], _buf, _size))
	{
		status_t status = STATUS_SUCCESS;
		uint32_t bytes = 0;

		while (STATUS_BUSY == (status = LPUART_DRV_GetTransmitStatus(g_handle[_chl], &bytes))) {}

		if (STATUS_SUCCESS == status)
		{
			size = _size - bytes;
		}	
	}
	
#if defined USING_OS_FREERTOS
	xSemaphoreGiveRecursive(g_uart_tx_mutex[_chl]);
#endif

	return size;
}

/*******************************************************************************
 * Local functions
 ******************************************************************************/
/**
 * UART IRQ handler.
 *
 * @param [in] _state the driver state
 * @param [in] _event the UART event type
 * @param [in] _user_data the user data passed to this function
 */
static void uart_irq_handler(void *_state, uart_event_t _event, void *_user_data)
{
	(void)_state;

	uint8_t chl = (uint8_t)((uint32_t)_user_data);

    if (UART_EVENT_RX_FULL == _event)
    {
		/* check if the rx queue is full */
		if (g_uart_rx_queue_head[chl] == (g_uart_rx_queue_tail[chl] + 1) % UART_RX_BUFFER_SIZE)
		{
			/* dequeue */
			g_uart_rx_queue_head[chl] = (g_uart_rx_queue_head[chl] + 1) % UART_RX_BUFFER_SIZE;
		}

		/* enqueue */
		g_uart_rx_queue[chl][g_uart_rx_queue_tail[chl]] = g_rx_byte[chl];
		g_uart_rx_queue_tail[chl] = (g_uart_rx_queue_tail[chl] + 1) % UART_RX_BUFFER_SIZE;

		/* update the rx buffer and trigger the next receiving */
		LPUART_DRV_SetRxBuffer(g_handle[chl], g_rx_byte + chl, 1);
    }

    /* trigger receiving */
    LPUART_DRV_ReceiveData(g_handle[chl], g_rx_byte + chl, 1);
}
