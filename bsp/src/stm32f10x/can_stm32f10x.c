#include "can.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#if defined USING_OS_FREERTOS
SemaphoreHandle_t g_can_tx_mutex[CAN_CH1 + 1] = {NULL, NULL}; /* sending mutex */
#endif

can_msg_t g_can_rx_queue[CAN_CH1 + 1][CAN_MSG_RX_QUEUE_MAX_LENGTH]; /* receiving queue */
uint8_t g_can_rx_queue_head[CAN_CH1 + 1] = {0, 0}; /* receiving queue head */
uint8_t g_can_rx_queue_tail[CAN_CH1 + 1] = {0, 0}; /* receiving queue tail */

typedef struct
{
	GPIO_TypeDef *gpio_;
	uint16_t rx_pin_;
	uint16_t tx_pin_;
	uint32_t gpio_remap_;
	IRQn_Type irqs_[1];
	uint8_t start_filter_num_;
} comm_config_t;

static comm_config_t g_comm_config[CAN_CH1 + 1] =
{
	{
		.gpio_             = CAN0_GPIO,
		.rx_pin_           = CAN0_RX_PIN,
		.tx_pin_           = CAN0_TX_PIN,
		.gpio_remap_       = CAN0_GPIO_REMAP,
		.irqs_             = {CAN0_RX_IRQ},
		.start_filter_num_ = 0,
	},

	{
		.gpio_             = CAN1_GPIO,
		.rx_pin_           = CAN1_RX_PIN,
		.tx_pin_           = CAN1_TX_PIN,
		.gpio_remap_       = CAN1_GPIO_REMAP,
		.irqs_             = {CAN1_RX_IRQ},
		.start_filter_num_ = CAN_SLAVE_START_FILTER_BANK_NUM,
	}
};

/* baudrate = APB1 clock(MHz) / prescaler /(1 + BS1 + BS2) = 0.5(500kbps) */
static CAN_TypeDef *g_handle[CAN_CH1 + 1] = {CAN0_INST, CAN1_INST};

/*******************************************************************************
 * Local function prototypes
 ******************************************************************************/
static void can_irq_handler(const uint8_t _chl);

/*******************************************************************************
 * Functions
 ******************************************************************************/
int32_t can_init(const uint8_t _chl, const uint32_t *_filter_id_list, const uint8_t _filter_id_num)
{
	assert(CAN_CH1 >= _chl);
		
	GPIO_InitTypeDef GPIO_InitStructure;
	CAN_InitTypeDef CAN_InitStructure;
	CAN_FilterInitTypeDef CAN_FilterInitStructure;	
	NVIC_InitTypeDef NVIC_InitStructure;

	/* initialize the rx queue */
	g_can_rx_queue_head[_chl] = 0;
	g_can_rx_queue_tail[_chl] = 0;
	
#if defined USING_OS_FREERTOS
	g_can_tx_mutex[_chl] = xSemaphoreCreateMutex();
#endif
	
	/* initialize the GPIOs */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	CAN_GPIO_CLK_ENABLE(_chl);
	
	if (0 != g_comm_config[_chl].gpio_remap_)
	{
		GPIO_PinRemapConfig(g_comm_config[_chl].gpio_remap_, ENABLE);
	}

	GPIO_InitStructure.GPIO_Pin = g_comm_config[_chl].rx_pin_;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(g_comm_config[_chl].gpio_, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = g_comm_config[_chl].tx_pin_;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(g_comm_config[_chl].gpio_, &GPIO_InitStructure);
	
	/* initialize the CAN */
	CAN_CLK_ENABLE(_chl);
	CAN_DeInit(g_handle[_chl]);
	CAN_StructInit(&CAN_InitStructure);
	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = DISABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = DISABLE;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE;
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
	CAN_InitStructure.CAN_Prescaler = 12;
	CAN_Init(g_handle[_chl], &CAN_InitStructure);
    CAN_ITConfig(g_handle[_chl], CAN_IT_FMP0, ENABLE);

	/* initialize the CAN filter */
	CAN_SlaveStartBank(CAN_SLAVE_START_FILTER_BANK_NUM);
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
	CAN_FilterInitStructure.CAN_FilterNumber = g_comm_config[_chl].start_filter_num_;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;

	if (NULL == _filter_id_list || 0 == _filter_id_num)
	{
		CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
		CAN_FilterInit(&CAN_FilterInitStructure);		
	}
	else
	{
		uint8_t i = 0;
		uint8_t bank_num =  (0 == CAN_FilterInitStructure.CAN_FilterNumber ? CAN_SLAVE_START_FILTER_BANK_NUM : 28);
		
		CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdList;

		while (CAN_FilterInitStructure.CAN_FilterNumber < bank_num)
		{
			if (i < _filter_id_num)
			{
				CAN_FilterInitStructure.CAN_FilterIdHigh = (uint16_t)(_filter_id_list[i++] << 5);
			}

			if (i < _filter_id_num)
			{
				CAN_FilterInitStructure.CAN_FilterIdLow = (uint16_t)(_filter_id_list[i++] << 5);
			}

			if (i < _filter_id_num)
			{
				CAN_FilterInitStructure.CAN_FilterMaskIdHigh = (uint16_t)(_filter_id_list[i++]) << 5;
			}

			if (i < _filter_id_num)
			{
				CAN_FilterInitStructure.CAN_FilterMaskIdLow = (uint16_t)(_filter_id_list[i++] << 5);
			}

			CAN_FilterInit(&CAN_FilterInitStructure);

			if (i >= _filter_id_num)
			{
				break;
			}

			CAN_FilterInitStructure.CAN_FilterNumber++;
			CAN_FilterInitStructure.CAN_FilterIdHigh = 0;
			CAN_FilterInitStructure.CAN_FilterIdLow = 0;
			CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0;
			CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0;
		}
	}
	
	/* initialize the NVIC */
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	for (uint8_t i = 0; i < sizeof(g_comm_config[_chl].irqs_); i++)
	{
		NVIC_InitStructure.NVIC_IRQChannel = g_comm_config[_chl].irqs_[i];
		NVIC_Init(&NVIC_InitStructure);
	}

	return 0;
}

int32_t can_deinit(const uint8_t _chl)
{
	assert(CAN_CH1 >= _chl);
	
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&NVIC_InitStructure);

	for (uint8_t i = 0; i < sizeof(g_comm_config[_chl].irqs_); i++)
	{
		NVIC_InitStructure.NVIC_IRQChannel = g_comm_config[_chl].irqs_[i];
		NVIC_Init(&NVIC_InitStructure);
	}

	CAN_ITConfig(g_handle[_chl], CAN_IT_FMP0, DISABLE);
	CAN_DeInit(g_handle[_chl]);
	CAN_CLK_DISABLE(_chl);
	CAN_FORCE_RESET(_chl);
	CAN_RELEASE_RESET(_chl);

#if defined USING_OS_FREERTOS
	vSemaphoreDelete(g_can_tx_mutex[_chl]);
#endif

	return 0;
}

uint8_t can_send(const uint8_t _chl, const uint32_t _id, const uint8_t _buf[], const uint8_t _size)
{
	assert(CAN_CH1 >= _chl && NULL != _buf);

	uint16_t size = 0;
	CanTxMsg msg;
	uint8_t  mailbox = 0;

	msg.StdId = _id;
	msg.ExtId = 0;
	msg.IDE = CAN_ID_STD;
	msg.RTR = CAN_RTR_DATA;
	msg.DLC = _size >= 8 ? 8 : _size;
	memcpy(msg.Data, _buf, msg.DLC);

#if defined USING_OS_FREERTOS
	xSemaphoreTake( g_can_tx_mutex[_chl], portMAX_DELAY);
#endif

	if (CAN_TxStatus_NoMailBox != (mailbox = CAN_Transmit(g_handle[_chl], &msg)))
	{
		if (CAN_TxStatus_Ok == CAN_TransmitStatus(g_handle[_chl], mailbox))
		{
			size = _size;
		}
		else
		{
			CAN_CancelTransmit(g_handle[_chl], mailbox);
		}
	}

#if defined USING_OS_FREERTOS
	xSemaphoreGive( g_can_tx_mutex[_chl] );
#endif

	return size;
}

int32_t can_pwr_mode_trans(const uint8_t _chl, const uint8_t _mode)
{
	assert(CAN_CH1 >= _chl);

	switch(_mode)
	{
	case CAN_PWR_MODE_SLEEP:
		CAN_Sleep(g_handle[_chl]);
		break;

	case CAN_PWR_MODE_RUN:	
		CAN_WakeUp(g_handle[_chl]);
		break;

	default:
		break;
	}

    return 0;
}

/**
 * @name IRQ handlers
 * @{
 */

/*
 * CAN0 RX IRQ handler.
 */
void CAN0_RX_IRQ_HANDLER(void)
{	
	can_irq_handler(CAN_CH0);
}

/**
 * CAN1 RX IRQ handler.
 */
void CAN1_RX_IRQ_HANDLER(void)
{	
	can_irq_handler(CAN_CH1);
}

/** @} */ /* IRQ handlers */

/*******************************************************************************
 * Local functions
 ******************************************************************************/
/**
 * CAN IRQ handler.
 *
 * @param [in] _chl the CAN channel number
 */
static void can_irq_handler(const uint8_t _chl)
{
	assert(CAN_CH1 >=  _chl);

	/* FIFO 0 message pending */
	if (0 != CAN_MessagePending(g_handle[_chl], CAN_FIFO0))
	{		
		CanRxMsg msg;
	
		/* receive the message */
		CAN_Receive(g_handle[_chl], CAN_FIFO0, &msg);
	
		/* check if the rx queue is full */
		if (g_can_rx_queue_head[_chl] == (g_can_rx_queue_tail[_chl] + 1) % CAN_MSG_RX_QUEUE_MAX_LENGTH)
		{
			/* dequeue */
			g_can_rx_queue_head[_chl] = (g_can_rx_queue_head[_chl] + 1) % CAN_MSG_RX_QUEUE_MAX_LENGTH;
		}
		
		/* enqueue */
		g_can_rx_queue[_chl][g_can_rx_queue_tail[_chl]].id_ = (CAN_ID_STD ==  msg.IDE) ? msg.StdId : msg.ExtId;
		g_can_rx_queue[_chl][g_can_rx_queue_tail[_chl]].dlc_ = msg.DLC > 8u ? 8u : msg.DLC;
		memcpy(g_can_rx_queue[_chl][g_can_rx_queue_tail[_chl]].data_, msg.Data, g_can_rx_queue[_chl][g_can_rx_queue_tail[_chl]].dlc_);
		g_can_rx_queue_tail[_chl] = (g_can_rx_queue_tail[_chl] + 1u) % CAN_MSG_RX_QUEUE_MAX_LENGTH;
	}
}
