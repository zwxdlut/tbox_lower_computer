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
	uint8_t gpio_af_;
	IRQn_Type irqs_[1];
	uint8_t start_filter_num_;
	GPIO_TypeDef *trans_stb_n_gpio_;
	uint16_t trans_stb_n_pin_;
	GPIO_TypeDef *trans_en_gpio_;
	uint16_t trans_en_pin_;
	GPIO_TypeDef *trans_inh_gpio_;
	uint16_t trans_inh_pin_;
	IRQn_Type trans_inh_irq_;
} comm_config_t;

static comm_config_t g_comm_config[CAN_CH1 + 1] =
{
	{
		.gpio_             = CAN0_GPIO,
		.rx_pin_           = CAN0_RX_PIN,
		.tx_pin_           = CAN0_TX_PIN,
		.gpio_af_          = CAN0_GPIO_AF,
		.irqs_             = {CAN0_RX_IRQ},
		.start_filter_num_ = 0,
		.trans_stb_n_gpio_ = CAN0_TRANS_STB_N_GPIO,
		.trans_stb_n_pin_  = CAN0_TRANS_STB_N_PIN,
		.trans_en_gpio_    = CAN0_TRANS_EN_GPIO,
		.trans_en_pin_     = CAN0_TRANS_EN_PIN,
		.trans_inh_gpio_   = CAN0_TRANS_INH_GPIO,
		.trans_inh_pin_    = CAN0_TRANS_INH_PIN,
		.trans_inh_irq_    = CAN0_TRANS_INH_IRQ,
	},

	{
		.gpio_             = CAN1_GPIO,
		.rx_pin_           = CAN1_RX_PIN,
		.tx_pin_           = CAN1_TX_PIN,
		.gpio_af_          = CAN1_GPIO_AF,
		.irqs_             = {CAN1_RX_IRQ},
		.start_filter_num_ = CAN_SLAVE_START_FILTER_BANK_NUM,
		.trans_stb_n_gpio_ = CAN1_TRANS_STB_N_GPIO,
		.trans_stb_n_pin_  = CAN1_TRANS_STB_N_PIN,
		.trans_en_gpio_    = CAN1_TRANS_EN_GPIO,
		.trans_en_pin_     = CAN1_TRANS_EN_PIN,
		.trans_inh_gpio_   = CAN1_TRANS_INH_GPIO,
		.trans_inh_pin_    = CAN1_TRANS_INH_PIN,
		.trans_inh_irq_    = CAN1_TRANS_INH_IRQ,
	}
};

/* baudrate = APB1 clock(MHz) / prescaler /(1 + BS1 + BS2) = 0.5(500kbps) */
static CanRxMsgTypeDef g_rx_msg[CAN_CH1 + 1];
static CanTxMsgTypeDef g_tx_msg[CAN_CH1 + 1];
static CAN_HandleTypeDef g_handle[CAN_CH1 + 1] = 
{
	{
		.Instance       = CAN0_INST,
		.Init.Prescaler = 4,
		.Init.Mode      = CAN_MODE_NORMAL,
		.Init.SJW       = CAN_SJW_1TQ,
		.Init.BS1       = CAN_BS1_6TQ,
		.Init.BS2       = CAN_BS2_8TQ,
		.Init.TTCM      = DISABLE,
		.Init.ABOM      = DISABLE,
		.Init.AWUM      = DISABLE,
		.Init.NART	    = DISABLE,
		.Init.RFLM      = DISABLE,
		.Init.TXFP      = DISABLE,
		.pTxMsg         = &g_tx_msg[CAN_CH0],
		.pRxMsg         = &g_rx_msg[CAN_CH0],
	},

	{
		.Instance       = CAN1_INST,
		.Init.Prescaler = 4,
		.Init.Mode      = CAN_MODE_NORMAL,
		.Init.SJW       = CAN_SJW_1TQ,
		.Init.BS1       = CAN_BS1_6TQ,
		.Init.BS2       = CAN_BS2_8TQ,
		.Init.TTCM      = DISABLE,
		.Init.ABOM      = DISABLE,
		.Init.AWUM      = DISABLE,
		.Init.NART	    = DISABLE,
		.Init.RFLM      = DISABLE,
		.Init.TXFP      = DISABLE,
		.pTxMsg         = &g_tx_msg[CAN_CH1],
		.pRxMsg         = &g_rx_msg[CAN_CH1],
	}
};

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
	CAN_FilterConfTypeDef CAN_FilterInitStructure;

	/* initialize the rx queue */
	g_can_rx_queue_head[_chl] = 0;
	g_can_rx_queue_tail[_chl] = 0;
	
#if defined USING_OS_FREERTOS
	g_can_tx_mutex[_chl] = xSemaphoreCreateMutex();
#endif

	/* initialize the CAN transceiver */
	CAN_TRANS_STB_N_GPIO_CLK_ENABLE(_chl);
	GPIO_InitStructure.Pin = g_comm_config[_chl].trans_stb_n_pin_;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(g_comm_config[_chl].trans_stb_n_gpio_, &GPIO_InitStructure);
	HAL_GPIO_WritePin(g_comm_config[_chl].trans_stb_n_gpio_, g_comm_config[_chl].trans_stb_n_pin_, GPIO_PIN_SET);

	CAN_TRANS_EN_GPIO_CLK_ENABLE(_chl);
	GPIO_InitStructure.Pin = g_comm_config[_chl].trans_en_pin_;
	HAL_GPIO_Init(g_comm_config[_chl].trans_en_gpio_, &GPIO_InitStructure);
	HAL_GPIO_WritePin(g_comm_config[_chl].trans_en_gpio_, g_comm_config[_chl].trans_en_pin_, GPIO_PIN_SET);

	CAN_TRANS_INH_GPIO_CLK_ENABLE(_chl);
	GPIO_InitStructure.Pin = g_comm_config[_chl].trans_inh_pin_;
	GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(g_comm_config[_chl].trans_inh_gpio_, &GPIO_InitStructure);
	HAL_NVIC_SetPriority(g_comm_config[_chl].trans_inh_irq_, 0, 0);
    HAL_NVIC_EnableIRQ(g_comm_config[_chl].trans_inh_irq_);

	/* initialize the GPIOs */
	CAN_GPIO_CLK_ENABLE(_chl);
	GPIO_InitStructure.Pin = g_comm_config[_chl].rx_pin_ | g_comm_config[_chl].tx_pin_;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Alternate = g_comm_config[_chl].gpio_af_;
	HAL_GPIO_Init(g_comm_config[_chl].gpio_, &GPIO_InitStructure);
	
	/* initialize the CAN */
	CAN_CLK_ENABLE(_chl);
	HAL_CAN_Init(&g_handle[_chl]);
	__HAL_CAN_ENABLE_IT(&g_handle[_chl], CAN_IT_FMP0 | CAN_IT_FF0 | CAN_IT_FOV0 | CAN_IT_EWG | CAN_IT_EPV | CAN_IT_BOF | CAN_IT_LEC | CAN_IT_ERR);

	/* initialize the CAN filter */
	CAN_FilterInitStructure.FilterIdHigh = 0;
	CAN_FilterInitStructure.FilterIdLow = 0;
	CAN_FilterInitStructure.FilterMaskIdHigh = 0;
	CAN_FilterInitStructure.FilterMaskIdLow = 0;
	CAN_FilterInitStructure.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	CAN_FilterInitStructure.FilterNumber = g_comm_config[_chl].start_filter_num_;
	CAN_FilterInitStructure.FilterScale  = CAN_FILTERSCALE_16BIT;
	CAN_FilterInitStructure.FilterActivation = ENABLE;
	CAN_FilterInitStructure.BankNumber = CAN_SLAVE_START_FILTER_BANK_NUM;

	if (NULL == _filter_id_list || 0 == _filter_id_num)
	{
		CAN_FilterInitStructure.FilterMode = CAN_FILTERMODE_IDMASK;
		HAL_CAN_ConfigFilter(&g_handle[_chl], &CAN_FilterInitStructure);		
	}
	else
	{
		uint8_t i = 0;
		uint8_t bank_num =  (0 == CAN_FilterInitStructure.FilterNumber ? CAN_SLAVE_START_FILTER_BANK_NUM : 28);
		
		CAN_FilterInitStructure.FilterMode = CAN_FILTERMODE_IDLIST;

		while (CAN_FilterInitStructure.FilterNumber < bank_num)
		{
			if (i < _filter_id_num)
			{
				CAN_FilterInitStructure.FilterIdHigh = _filter_id_list[i++] << 5;
			}

			if (i < _filter_id_num)
			{
				CAN_FilterInitStructure.FilterIdLow = _filter_id_list[i++] << 5;
			}

			if (i < _filter_id_num)
			{
				CAN_FilterInitStructure.FilterMaskIdHigh = _filter_id_list[i++] << 5;
			}

			if (i < _filter_id_num)
			{
				CAN_FilterInitStructure.FilterMaskIdLow = _filter_id_list[i++] << 5;
			}

			HAL_CAN_ConfigFilter(&g_handle[_chl], &CAN_FilterInitStructure);

			if (i >= _filter_id_num)
			{
				break;
			}

			CAN_FilterInitStructure.FilterNumber++;
			CAN_FilterInitStructure.FilterIdHigh = 0;
			CAN_FilterInitStructure.FilterIdLow = 0;
			CAN_FilterInitStructure.FilterMaskIdHigh = 0;
			CAN_FilterInitStructure.FilterMaskIdLow = 0;
		}
	}

	/* initialize the NVIC */
	for (uint8_t i = 0; i < sizeof(g_comm_config[_chl].irqs_); i++)
	{
		HAL_NVIC_SetPriority(g_comm_config[_chl].irqs_[i], 0, 0);
		HAL_NVIC_EnableIRQ(g_comm_config[_chl].irqs_[i]);
	}

	return 0;
}

int32_t can_deinit(const uint8_t _chl)
{
	assert(CAN_CH1 >= _chl);
	
	for (uint8_t i = 0; i < sizeof(g_comm_config[_chl].irqs_); i++)
	{
		HAL_NVIC_DisableIRQ(g_comm_config[_chl].irqs_[i]);
	}

	__HAL_CAN_DISABLE_IT(&g_handle[_chl],CAN_IT_FMP0 | CAN_IT_FF0 | CAN_IT_FOV0 | CAN_IT_EWG | CAN_IT_EPV | CAN_IT_BOF | CAN_IT_LEC | CAN_IT_ERR);
	HAL_CAN_DeInit(&g_handle[_chl]);
	CAN_CLK_DISABLE(_chl);
	CAN_FORCE_RESET(_chl);
	CAN_RELEASE_RESET(_chl);

	HAL_GPIO_DeInit(g_comm_config[_chl].gpio_, g_comm_config[_chl].rx_pin_ | g_comm_config[_chl].tx_pin_);
	__HAL_GPIO_EXTI_CLEAR_IT(g_comm_config[_chl].trans_inh_pin_);
	HAL_GPIO_DeInit(g_comm_config[_chl].trans_stb_n_gpio_, g_comm_config[_chl].trans_stb_n_pin_);
	HAL_GPIO_DeInit(g_comm_config[_chl].trans_en_gpio_, g_comm_config[_chl].trans_en_pin_);
	HAL_GPIO_DeInit(g_comm_config[_chl].trans_inh_gpio_, g_comm_config[_chl].trans_inh_pin_);

#if defined USING_OS_FREERTOS
	vSemaphoreDelete(g_can_tx_mutex[_chl]);
#endif

	return 0;
}

uint8_t can_send(const uint8_t _chl, const uint32_t _id, const uint8_t _buf[], const uint8_t _size)
{
	assert(CAN_CH1 >= _chl && NULL != _buf);

	uint16_t size = 0;

#if defined USING_OS_FREERTOS
	xSemaphoreTake( g_can_tx_mutex[_chl], portMAX_DELAY);
#endif

	g_handle[_chl].pTxMsg->StdId = _id;
	g_handle[_chl].pTxMsg->ExtId = 0;
	g_handle[_chl].pTxMsg->IDE = CAN_ID_STD;
	g_handle[_chl].pTxMsg->RTR = CAN_RTR_DATA;
	g_handle[_chl].pTxMsg->DLC = _size >= 8 ? 8 : _size;
	memcpy(g_handle[_chl].pTxMsg->Data, _buf, g_handle[_chl].pTxMsg->DLC);

	if (HAL_OK == HAL_CAN_Transmit(&g_handle[_chl], 10))
	{
		size = _size; 
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
		HAL_CAN_Sleep(&g_handle[_chl]);
		HAL_GPIO_WritePin(g_comm_config[_chl].trans_stb_n_gpio_, g_comm_config[_chl].trans_stb_n_pin_, GPIO_PIN_RESET);
		break;

	case CAN_PWR_MODE_RUN:	
		HAL_GPIO_WritePin(g_comm_config[_chl].trans_stb_n_gpio_, g_comm_config[_chl].trans_stb_n_pin_, GPIO_PIN_SET);
		HAL_CAN_WakeUp(&g_handle[_chl]);
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

/**
 * CAN0 transcevier INH IRQ handler.
 */
void CAN0_TRANS_INH_IRQ_HANDLER(void)
{
	HAL_GPIO_EXTI_IRQHandler(CAN0_TRANS_INH_PIN);
}

/**
 * CAN1 transcevier INH IRQ handler.
 */
void CAN1_TRANS_INH_IRQ_HANDLER(void)
{
	HAL_GPIO_EXTI_IRQHandler(CAN1_TRANS_INH_PIN);
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
	if (0 != __HAL_CAN_MSG_PENDING(&g_handle[_chl], CAN_FIFO0) && RESET != __HAL_CAN_GET_IT_SOURCE(&g_handle[_chl], CAN_IT_FMP0))
	{
		/* get the ID */
		g_handle[_chl].pRxMsg->IDE = (uint8_t)0x04 & g_handle[_chl].Instance->sFIFOMailBox[CAN_FIFO0].RIR;

		if (g_handle[_chl].pRxMsg->IDE == CAN_ID_STD)
		{
			g_handle[_chl].pRxMsg->StdId = 0x000007FFU & (g_handle[_chl].Instance->sFIFOMailBox[CAN_FIFO0].RIR >> 21U);
		}
		else
		{
			g_handle[_chl].pRxMsg->ExtId = 0x1FFFFFFFU & (g_handle[_chl].Instance->sFIFOMailBox[CAN_FIFO0].RIR >> 3U);
		}

		/* get the RTR */
		g_handle[_chl].pRxMsg->RTR = (uint8_t)0x02 & g_handle[_chl].Instance->sFIFOMailBox[CAN_FIFO0].RIR;

		/* get the DLC */
		g_handle[_chl].pRxMsg->DLC = (uint8_t)0x0F & g_handle[_chl].Instance->sFIFOMailBox[CAN_FIFO0].RDTR;

		/* get the FIFONumber */
		g_handle[_chl].pRxMsg->FIFONumber = CAN_FIFO0;

		/* get the FMI */
		g_handle[_chl].pRxMsg->FMI = (uint8_t)0xFF & (g_handle[_chl].Instance->sFIFOMailBox[CAN_FIFO0].RDTR >> 8U);

		/* get the data field */
		g_handle[_chl].pRxMsg->Data[0] = (uint8_t)0xFF &  g_handle[_chl].Instance->sFIFOMailBox[CAN_FIFO0].RDLR;
		g_handle[_chl].pRxMsg->Data[1] = (uint8_t)0xFF & (g_handle[_chl].Instance->sFIFOMailBox[CAN_FIFO0].RDLR >> 8U);
		g_handle[_chl].pRxMsg->Data[2] = (uint8_t)0xFF & (g_handle[_chl].Instance->sFIFOMailBox[CAN_FIFO0].RDLR >> 16U);
		g_handle[_chl].pRxMsg->Data[3] = (uint8_t)0xFF & (g_handle[_chl].Instance->sFIFOMailBox[CAN_FIFO0].RDLR >> 24U);
		g_handle[_chl].pRxMsg->Data[4] = (uint8_t)0xFF &  g_handle[_chl].Instance->sFIFOMailBox[CAN_FIFO0].RDHR;
		g_handle[_chl].pRxMsg->Data[5] = (uint8_t)0xFF & (g_handle[_chl].Instance->sFIFOMailBox[CAN_FIFO0].RDHR >> 8U);
		g_handle[_chl].pRxMsg->Data[6] = (uint8_t)0xFF & (g_handle[_chl].Instance->sFIFOMailBox[CAN_FIFO0].RDHR >> 16U);
		g_handle[_chl].pRxMsg->Data[7] = (uint8_t)0xFF & (g_handle[_chl].Instance->sFIFOMailBox[CAN_FIFO0].RDHR >> 24U);
		
		/* release the FIFO0 */
		__HAL_CAN_FIFO_RELEASE(&g_handle[_chl], CAN_FIFO0);

		/* check if the rx queue is full */
		if (g_can_rx_queue_head[_chl] == (g_can_rx_queue_tail[_chl] + 1) % CAN_MSG_RX_QUEUE_MAX_LENGTH)
		{
			/* dequeue */
			g_can_rx_queue_head[_chl] = (g_can_rx_queue_head[_chl] + 1) % CAN_MSG_RX_QUEUE_MAX_LENGTH;
		}

		/* enqueue */
		g_can_rx_queue[_chl][g_can_rx_queue_tail[_chl]].id_ = (CAN_ID_STD ==  g_handle[_chl].pRxMsg->IDE) ?  g_handle[_chl].pRxMsg->StdId :g_handle[_chl].pRxMsg->ExtId;
		g_can_rx_queue[_chl][g_can_rx_queue_tail[_chl]].dlc_ = g_handle[_chl].pRxMsg->DLC > 8u ? 8u : g_handle[_chl].pRxMsg->DLC;
		memcpy(g_can_rx_queue[_chl][g_can_rx_queue_tail[_chl]].data_, g_handle[_chl].pRxMsg->Data, g_can_rx_queue[_chl][g_can_rx_queue_tail[_chl]].dlc_);
		g_can_rx_queue_tail[_chl] = (g_can_rx_queue_tail[_chl] + 1u) % CAN_MSG_RX_QUEUE_MAX_LENGTH;
	}

	/* error warning */
	if (0 != __HAL_CAN_GET_FLAG(&g_handle[_chl], CAN_FLAG_EWG) && RESET != __HAL_CAN_GET_IT_SOURCE(&g_handle[_chl], CAN_IT_EWG)) {}

	/* error passive */
	if (0 != __HAL_CAN_GET_FLAG(&g_handle[_chl], CAN_FLAG_EPV) && RESET != __HAL_CAN_GET_IT_SOURCE(&g_handle[_chl], CAN_IT_EPV)) {}
		
	/* bus-off */
	if (0 != __HAL_CAN_GET_FLAG(&g_handle[_chl], CAN_FLAG_BOF) && RESET !=  __HAL_CAN_GET_IT_SOURCE(&g_handle[_chl], CAN_IT_BOF)) {}
}
