/*
 * can_stm32f2xx.c
 *
 *  Created on: 2018年8月21日
 *      Author: Administrator
 */
 
#include "can.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#if defined USING_OS_FREERTOS
SemaphoreHandle_t g_can_tx_mutex[CAN1_INDEX + 1] = {NULL, NULL}; /* Sending mutex */
#endif

can_msg_t g_can_rx_queue[CAN1_INDEX + 1][CAN_MSG_RX_QUEUE_MAX_LENGTH]; /* Receiving queue */
uint8_t g_can_rx_queue_head[CAN1_INDEX + 1] = {0, 0}; /* Receiving queue head */
uint8_t g_can_rx_queue_tail[CAN1_INDEX + 1] = {0, 0}; /* Receiving queue tail */

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

static comm_config_t g_comm_config[CAN1_INDEX + 1] =
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
static CanRxMsgTypeDef g_rx_msg[CAN1_INDEX + 1];
static CanTxMsgTypeDef g_tx_msg[CAN1_INDEX + 1];
static CAN_HandleTypeDef g_handle[CAN1_INDEX + 1] = 
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
		.pTxMsg         = &g_tx_msg[CAN0_INDEX],
		.pRxMsg         = &g_rx_msg[CAN0_INDEX],
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
		.pTxMsg         = &g_tx_msg[CAN1_INDEX],
		.pRxMsg         = &g_rx_msg[CAN1_INDEX],
	}
};

/*******************************************************************************
 * Local function prototypes
 ******************************************************************************/
static void can_irq_handler(const uint8_t _index);

/*******************************************************************************
 * Functions
 ******************************************************************************/
int32_t can_init(const uint8_t _index, const uint32_t *_filter_id_list, const uint8_t _filter_id_num)
{
	assert(CAN1_INDEX >= _index);
		
	GPIO_InitTypeDef GPIO_InitStructure;
	CAN_FilterConfTypeDef CAN_FilterInitStructure;

	/* Initialize the rx queue */
	g_can_rx_queue_head[_index] = 0;
	g_can_rx_queue_tail[_index] = 0;
	
#if defined USING_OS_FREERTOS
	g_can_tx_mutex[_index] = xSemaphoreCreateMutex();
#endif

	/* Initialize the CAN Transceiver */
	CAN_TRANS_STB_N_GPIO_CLK_ENABLE(_index);
	GPIO_InitStructure.Pin = g_comm_config[_index].trans_stb_n_pin_;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(g_comm_config[_index].trans_stb_n_gpio_, &GPIO_InitStructure);
	HAL_GPIO_WritePin(g_comm_config[_index].trans_stb_n_gpio_, g_comm_config[_index].trans_stb_n_pin_, GPIO_PIN_SET);

	CAN_TRANS_EN_GPIO_CLK_ENABLE(_index);
	GPIO_InitStructure.Pin = g_comm_config[_index].trans_en_pin_;
	HAL_GPIO_Init(g_comm_config[_index].trans_en_gpio_, &GPIO_InitStructure);
	HAL_GPIO_WritePin(g_comm_config[_index].trans_en_gpio_, g_comm_config[_index].trans_en_pin_, GPIO_PIN_SET);

	CAN_TRANS_INH_GPIO_CLK_ENABLE(_index);
	GPIO_InitStructure.Pin = g_comm_config[_index].trans_inh_pin_;
	GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStructure.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(g_comm_config[_index].trans_inh_gpio_, &GPIO_InitStructure);
	HAL_NVIC_SetPriority(g_comm_config[_index].trans_inh_irq_, 0, 0);
    HAL_NVIC_EnableIRQ(g_comm_config[_index].trans_inh_irq_);

	/* Initialize the GPIOs */
	CAN_GPIO_CLK_ENABLE(_index);
	GPIO_InitStructure.Pin = g_comm_config[_index].rx_pin_ | g_comm_config[_index].tx_pin_;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Alternate = g_comm_config[_index].gpio_af_;
	HAL_GPIO_Init(g_comm_config[_index].gpio_, &GPIO_InitStructure);
	
	/* Initialize the CAN */
	CAN_CLK_ENABLE(_index);
	HAL_CAN_Init(&g_handle[_index]);
	__HAL_CAN_ENABLE_IT(&g_handle[_index], CAN_IT_FMP0 | CAN_IT_FF0 | CAN_IT_FOV0 | CAN_IT_EWG | CAN_IT_EPV | CAN_IT_BOF | CAN_IT_LEC | CAN_IT_ERR);

	/* Initialize the CAN filter */
	CAN_FilterInitStructure.FilterIdHigh         = 0;
	CAN_FilterInitStructure.FilterIdLow          = 0;
	CAN_FilterInitStructure.FilterMaskIdHigh     = 0;
	CAN_FilterInitStructure.FilterMaskIdLow      = 0;
	CAN_FilterInitStructure.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	CAN_FilterInitStructure.FilterNumber         = g_comm_config[_index].start_filter_num_;
	CAN_FilterInitStructure.FilterScale          = CAN_FILTERSCALE_16BIT;
	CAN_FilterInitStructure.FilterActivation     = ENABLE;
	CAN_FilterInitStructure.BankNumber           = CAN_SLAVE_START_FILTER_BANK_NUM;

	if (NULL == _filter_id_list || 0 == _filter_id_num)
	{
		CAN_FilterInitStructure.FilterMode = CAN_FILTERMODE_IDMASK;
		HAL_CAN_ConfigFilter(&g_handle[_index], &CAN_FilterInitStructure);		
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
				CAN_FilterInitStructure.FilterIdHigh     = _filter_id_list[i++] << 5;
			}

			if (i < _filter_id_num)
			{
				CAN_FilterInitStructure.FilterIdLow      = _filter_id_list[i++] << 5;
			}

			if (i < _filter_id_num)
			{
				CAN_FilterInitStructure.FilterMaskIdHigh = _filter_id_list[i++] << 5;
			}

			if (i < _filter_id_num)
			{
				CAN_FilterInitStructure.FilterMaskIdLow  = _filter_id_list[i++] << 5;
			}

			HAL_CAN_ConfigFilter(&g_handle[_index], &CAN_FilterInitStructure);

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

	/* Initialize the NVIC */
	for (uint8_t i = 0; i < sizeof(g_comm_config[_index].irqs_); i++)
	{
		HAL_NVIC_SetPriority(g_comm_config[_index].irqs_[i], 0, 0);
		HAL_NVIC_EnableIRQ(g_comm_config[_index].irqs_[i]);
	}

	return 0;
}

int32_t can_deinit(const uint8_t _index)
{
	assert(CAN1_INDEX >= _index);
	
	for (uint8_t i = 0; i < sizeof(g_comm_config[_index].irqs_); i++)
	{
		HAL_NVIC_DisableIRQ(g_comm_config[_index].irqs_[i]);
	}

	__HAL_CAN_DISABLE_IT(&g_handle[_index],CAN_IT_FMP0 | CAN_IT_FF0 | CAN_IT_FOV0 | CAN_IT_EWG | CAN_IT_EPV | CAN_IT_BOF | CAN_IT_LEC | CAN_IT_ERR);
	HAL_CAN_DeInit(&g_handle[_index]);
	CAN_CLK_DISABLE(_index);
	CAN_FORCE_RESET(_index);
	CAN_RELEASE_RESET(_index);

	HAL_GPIO_DeInit(g_comm_config[_index].gpio_, g_comm_config[_index].rx_pin_ | g_comm_config[_index].tx_pin_);
	__HAL_GPIO_EXTI_CLEAR_IT(g_comm_config[_index].trans_inh_pin_);
	HAL_GPIO_DeInit(g_comm_config[_index].trans_stb_n_gpio_, g_comm_config[_index].trans_stb_n_pin_);
	HAL_GPIO_DeInit(g_comm_config[_index].trans_en_gpio_, g_comm_config[_index].trans_en_pin_);
	HAL_GPIO_DeInit(g_comm_config[_index].trans_inh_gpio_, g_comm_config[_index].trans_inh_pin_);

#if defined USING_OS_FREERTOS
	vSemaphoreDelete(g_can_tx_mutex[_index]);
#endif

	return 0;
}

uint8_t can_send(const uint8_t _index, const uint32_t _id, const uint8_t _buf[], const uint8_t _size)
{
	assert(CAN1_INDEX >= _index && NULL != _buf);

	uint16_t size = 0;

#if defined USING_OS_FREERTOS
	xSemaphoreTake( g_can_tx_mutex[_index], portMAX_DELAY);
#endif

	g_handle[_index].pTxMsg->StdId = _id;
	g_handle[_index].pTxMsg->ExtId = 0;
	g_handle[_index].pTxMsg->IDE   = CAN_ID_STD;
	g_handle[_index].pTxMsg->RTR   = CAN_RTR_DATA;
	g_handle[_index].pTxMsg->DLC   = _size >= 8 ? 8 : _size;
	memcpy(g_handle[_index].pTxMsg->Data, _buf, g_handle[_index].pTxMsg->DLC);

	if (HAL_OK == HAL_CAN_Transmit(&g_handle[_index], 10))
	{
		size = _size; 
	}

#if defined USING_OS_FREERTOS
	xSemaphoreGive( g_can_tx_mutex[_index] );
#endif

	return size;
}

int32_t can_pwr_mode_trans(const uint8_t _index, const uint8_t _mode)
{
	assert(CAN1_INDEX >= _index);

	switch(_mode)
	{
	case CAN_PWR_MODE_SLEEP:
		HAL_CAN_Sleep(&g_handle[_index]);
		HAL_GPIO_WritePin(g_comm_config[_index].trans_stb_n_gpio_, g_comm_config[_index].trans_stb_n_pin_, GPIO_PIN_RESET);
		break;

	case CAN_PWR_MODE_RUN:	
		HAL_GPIO_WritePin(g_comm_config[_index].trans_stb_n_gpio_, g_comm_config[_index].trans_stb_n_pin_, GPIO_PIN_SET);
		HAL_CAN_WakeUp(&g_handle[_index]);
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
	can_irq_handler(CAN0_INDEX);
}

/**
 * CAN1 RX IRQ handler.
 */
void CAN1_RX_IRQ_HANDLER(void)
{	
	can_irq_handler(CAN1_INDEX);
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
 * @param [in] _index CAN channel index
 */
static void can_irq_handler(const uint8_t _index)
{
	assert(CAN1_INDEX >=  _index);

	/* FIFO 0 message pending */
	if (0 != __HAL_CAN_MSG_PENDING(&g_handle[_index], CAN_FIFO0) && RESET != __HAL_CAN_GET_IT_SOURCE(&g_handle[_index], CAN_IT_FMP0))
	{
		/* Get the ID */
		g_handle[_index].pRxMsg->IDE = (uint8_t)0x04 & g_handle[_index].Instance->sFIFOMailBox[CAN_FIFO0].RIR;

		if (g_handle[_index].pRxMsg->IDE == CAN_ID_STD)
		{
			g_handle[_index].pRxMsg->StdId = 0x000007FFU & (g_handle[_index].Instance->sFIFOMailBox[CAN_FIFO0].RIR >> 21U);
		}
		else
		{
			g_handle[_index].pRxMsg->ExtId = 0x1FFFFFFFU & (g_handle[_index].Instance->sFIFOMailBox[CAN_FIFO0].RIR >> 3U);
		}

		/* Get the RTR */
		g_handle[_index].pRxMsg->RTR = (uint8_t)0x02 & g_handle[_index].Instance->sFIFOMailBox[CAN_FIFO0].RIR;

		/* Get the DLC */
		g_handle[_index].pRxMsg->DLC = (uint8_t)0x0F & g_handle[_index].Instance->sFIFOMailBox[CAN_FIFO0].RDTR;

		/* Get the FIFONumber */
		g_handle[_index].pRxMsg->FIFONumber = CAN_FIFO0;

		/* Get the FMI */
		g_handle[_index].pRxMsg->FMI = (uint8_t)0xFF & (g_handle[_index].Instance->sFIFOMailBox[CAN_FIFO0].RDTR >> 8U);

		/* Get the data field */
		g_handle[_index].pRxMsg->Data[0] = (uint8_t)0xFF &  g_handle[_index].Instance->sFIFOMailBox[CAN_FIFO0].RDLR;
		g_handle[_index].pRxMsg->Data[1] = (uint8_t)0xFF & (g_handle[_index].Instance->sFIFOMailBox[CAN_FIFO0].RDLR >> 8U);
		g_handle[_index].pRxMsg->Data[2] = (uint8_t)0xFF & (g_handle[_index].Instance->sFIFOMailBox[CAN_FIFO0].RDLR >> 16U);
		g_handle[_index].pRxMsg->Data[3] = (uint8_t)0xFF & (g_handle[_index].Instance->sFIFOMailBox[CAN_FIFO0].RDLR >> 24U);
		g_handle[_index].pRxMsg->Data[4] = (uint8_t)0xFF &  g_handle[_index].Instance->sFIFOMailBox[CAN_FIFO0].RDHR;
		g_handle[_index].pRxMsg->Data[5] = (uint8_t)0xFF & (g_handle[_index].Instance->sFIFOMailBox[CAN_FIFO0].RDHR >> 8U);
		g_handle[_index].pRxMsg->Data[6] = (uint8_t)0xFF & (g_handle[_index].Instance->sFIFOMailBox[CAN_FIFO0].RDHR >> 16U);
		g_handle[_index].pRxMsg->Data[7] = (uint8_t)0xFF & (g_handle[_index].Instance->sFIFOMailBox[CAN_FIFO0].RDHR >> 24U);
		
		/* Release the FIFO0 */
		__HAL_CAN_FIFO_RELEASE(&g_handle[_index], CAN_FIFO0);

		/* Check if the rx queue is full */
		if (g_can_rx_queue_head[_index] == (g_can_rx_queue_tail[_index] + 1) % CAN_MSG_RX_QUEUE_MAX_LENGTH)
		{
			/* Dequeue */
			g_can_rx_queue_head[_index] = (g_can_rx_queue_head[_index] + 1) % CAN_MSG_RX_QUEUE_MAX_LENGTH;
		}

		/* Enqueue */
		g_can_rx_queue[_index][g_can_rx_queue_tail[_index]].id_ = (CAN_ID_STD ==  g_handle[_index].pRxMsg->IDE) ?  g_handle[_index].pRxMsg->StdId :g_handle[_index].pRxMsg->ExtId;
		g_can_rx_queue[_index][g_can_rx_queue_tail[_index]].dlc_ = g_handle[_index].pRxMsg->DLC > 8u ? 8u : g_handle[_index].pRxMsg->DLC;
		memcpy(g_can_rx_queue[_index][g_can_rx_queue_tail[_index]].data_, g_handle[_index].pRxMsg->Data, g_can_rx_queue[_index][g_can_rx_queue_tail[_index]].dlc_);
		g_can_rx_queue_tail[_index] = (g_can_rx_queue_tail[_index] + 1u) % CAN_MSG_RX_QUEUE_MAX_LENGTH;
	}

	/* Error warning */
	if (0 != __HAL_CAN_GET_FLAG(&g_handle[_index], CAN_FLAG_EWG) && RESET != __HAL_CAN_GET_IT_SOURCE(&g_handle[_index], CAN_IT_EWG)) {}

	/* Error passive */
	if (0 != __HAL_CAN_GET_FLAG(&g_handle[_index], CAN_FLAG_EPV) && RESET != __HAL_CAN_GET_IT_SOURCE(&g_handle[_index], CAN_IT_EPV)) {}
		
	/* bus-off */
	if (0 != __HAL_CAN_GET_FLAG(&g_handle[_index], CAN_FLAG_BOF) && RESET !=  __HAL_CAN_GET_IT_SOURCE(&g_handle[_index], CAN_IT_BOF)) {}
}
