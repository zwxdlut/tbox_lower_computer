/*
 * uart_s32k1xx.c
 *
 *  Created on: 2018��10��16��
 *      Author: Administrator
 */

#include "uart.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#if defined USING_OS_FREERTOS
SemaphoreHandle_t g_uart_tx_mutex[UART1_INDEX + 1] = {NULL, NULL}; /* Tx mutex*/
#endif

uint8_t  g_uart_rx_queue[UART1_INDEX + 1][UART_BUFFER_SIZE];       /**< Ring queue */
uint16_t g_uart_rx_queue_head[UART1_INDEX + 1] = {0, 0};           /**< Ring queue head */
uint16_t g_uart_rx_queue_tail[UART1_INDEX + 1] = {0, 0};           /**< Ring queue tail */

typedef struct
{
	PORT_Type    *port_;
	uint8_t      rx_pin_;
	uint8_t      tx_pin_;
	port_mux_t   gpio_af_;
	IRQn_Type    irqs_[1];
}comm_config_t;

static comm_config_t g_comm_config[UART1_INDEX + 1] =
{
	{
		.port_    = UART0_PORT,
		.rx_pin_  = UART0_RX_PIN,
		.tx_pin_  = UART0_TX_PIN,
		.gpio_af_ = UART0_GPIO_AF,
		.irqs_    = {UART0_IRQ}
	},
	{
		.port_    = UART1_PORT,
		.rx_pin_  = UART1_RX_PIN,
		.tx_pin_  = UART1_TX_PIN,
		.gpio_af_ = UART1_GPIO_AF,
		.irqs_    = {UART1_IRQ}
	}
};

static uint8_t g_handle[UART1_INDEX + 1] =
{
#if defined INST_LPUART0
	INST_LPUART0,
#else
	0xFF,
#endif
#if defined INST_LPUART1
	INST_LPUART1
#else
	0xFF
#endif
};

static lpuart_user_config_t *g_config[UART1_INDEX + 1] =
{
#if defined INST_LPUART0
	&lpuart0_InitConfig0,
#else
	NULL,
#endif
#if defined INST_LPUART1
	&lpuart1_InitConfig0
#else
	NULL
#endif
};

static lpuart_state_t *g_state[UART1_INDEX + 1] =
{
#if defined INST_LPUART0
	&lpuart0_State,
#else
	NULL,
#endif
#if defined INST_LPUART1
	&lpuart1_State
#else
	NULL
#endif
};

static uint8_t  g_rx_byte[UART1_INDEX + 1];  /**< Rx byte */

/*******************************************************************************
 * Local Function prototypes
 ******************************************************************************/
static void uart_irq_handler(void *_state, uart_event_t _event, void *_user_data);

/*******************************************************************************
 * Functions
 ******************************************************************************/
int32_t uart_init(const uint8_t _index, const uint32_t _baudrate, const uint32_t _data_bits, const uint32_t _stop_bits, const uint32_t _parity)
{
	assert(UART1_INDEX >= _index);

	/* Rx ring queue initialization */
	g_uart_rx_queue_head[_index] = 0;
	g_uart_rx_queue_tail[_index] = 0;

#if defined USING_OS_FREERTOS
	g_uart_tx_mutex[_index] = xSemaphoreCreateRecursiveMutex();
#endif

	/* GPIO initialization */
	PINS_DRV_SetMuxModeSel(g_comm_config[_index].port_, g_comm_config[_index].rx_pin_, g_comm_config[_index].gpio_af_);
	PINS_DRV_SetMuxModeSel(g_comm_config[_index].port_, g_comm_config[_index].tx_pin_, g_comm_config[_index].gpio_af_);
	PINS_DRV_SetPullSel(g_comm_config[_index].port_, g_comm_config[_index].rx_pin_, PORT_INTERNAL_PULL_UP_ENABLED);

	/* UART initialization */
	g_config[_index]->baudRate        = _baudrate;
	g_config[_index]->bitCountPerChar = (lpuart_bit_count_per_char_t)_data_bits;
	g_config[_index]->stopBitCount    = (lpuart_stop_bit_count_t)_stop_bits;
	g_config[_index]->parityMode      = (lpuart_parity_mode_t)_parity;
	LPUART_DRV_Init(g_handle[_index], g_state[_index], g_config[_index]);
	LPUART_DRV_InstallRxCallback(g_handle[_index], uart_irq_handler, (void *)((uint32_t)_index));
#if defined USING_OS_FREERTOS
	/* The interrupt calls an interrupt safe API function - so its priority must
	   be equal to or lower than configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY */
	for(uint8_t i = 0; i < sizeof(g_comm_config[_index].irqs_) && NotAvail_IRQn != g_comm_config[_index].irqs_[i]; i++)
		INT_SYS_SetPriority( g_comm_config[_index].irqs_[i], configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
#endif

    /* Trigger receiving */
	LPUART_DRV_ReceiveData( g_handle[_index], g_rx_byte + _index, 1);

	return 0;
}

int32_t uart_deinit(const uint8_t _index)
{
	assert(UART1_INDEX >= _index);

	LPUART_DRV_Deinit(g_handle[_index]);
	PINS_DRV_SetMuxModeSel(g_comm_config[_index].port_, g_comm_config[_index].rx_pin_, PORT_PIN_DISABLED);
	PINS_DRV_SetMuxModeSel(g_comm_config[_index].port_, g_comm_config[_index].tx_pin_, PORT_PIN_DISABLED);
#if defined USING_OS_FREERTOS
	vSemaphoreDelete(g_uart_tx_mutex[_index]);
#endif

	return 0;
}

uint16_t uart_transmit(const uint8_t _index, const uint8_t *const _buf, const uint16_t _size)
{
	assert(UART1_INDEX >= _index && NULL != _buf);

	uint16_t size = 0;

#if defined USING_OS_FREERTOS
	xSemaphoreTakeRecursive( g_uart_tx_mutex[_index], portMAX_DELAY);
#endif
	if(STATUS_SUCCESS == LPUART_DRV_SendData(g_handle[_index], _buf, _size))
	{
		status_t status = STATUS_SUCCESS;
		uint32_t bytes = 0;

		while(STATUS_BUSY == (status = LPUART_DRV_GetTransmitStatus(g_handle[_index], &bytes))){}
		if(STATUS_SUCCESS == status)
			size = _size - bytes;
	}
#if defined USING_OS_FREERTOS
	xSemaphoreGiveRecursive( g_uart_tx_mutex[_index] );
#endif

	return size;
}

/*******************************************************************************
 * Local Functions
 ******************************************************************************/
/**
 * @brief UART IRQ handler.
 *
 * @param [in] _state     Driver state.
 * @param [in] _event     Event type.
 * @param [in] _user_data Callback parameter.
 */
static void uart_irq_handler(void *_state, uart_event_t _event, void *_user_data)
{
	(void)_state;

	uint8_t index = (uint8_t)((uint32_t)_user_data);

    if (UART_EVENT_RX_FULL == _event)
    {
		/* Rx queue is not full */
		if(g_uart_rx_queue_head[index] != (g_uart_rx_queue_tail[index] + 1) % UART_BUFFER_SIZE)
		{
			/* Push rx queue */
			g_uart_rx_queue[index][g_uart_rx_queue_tail[index]] = g_rx_byte[index];
			g_uart_rx_queue_tail[index] = (g_uart_rx_queue_tail[index] + 1) % UART_BUFFER_SIZE;
		}

		/* Update rx buffer and trigger next receive */
		LPUART_DRV_SetRxBuffer(g_handle[index], g_rx_byte + index, 1);
    }

    /* Trigger receiving*/
    LPUART_DRV_ReceiveData( g_handle[index], g_rx_byte + index, 1);
}
