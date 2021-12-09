#include "uart.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#if defined USING_OS_FREERTOS
SemaphoreHandle_t g_uart_tx_mutex[UART_CH2 + 1] = {NULL, NULL, NULL}; /* sending mutex */
#endif

uint8_t g_uart_rx_buf1[UART_CH2 + 1][UART_RX_BUFFER_SIZE]; /* receiving buffer1 */
uint8_t g_uart_rx_buf2[UART_CH2 + 1][UART_RX_BUFFER_SIZE]; /* receiving buffer2 */
uint16_t g_uart_rx_size1[UART_CH2 + 1] = {0, 0, 0}; /* receiving size1 */
uint16_t g_uart_rx_size2[UART_CH2 + 1] = {0, 0, 0}; /* receiving size2 */
uint16_t g_uart_rx_queue_head[UART_CH2 + 1] = {0, 0, 0}; /* receiving queue head */
uint16_t g_uart_rx_queue_tail[UART_CH2 + 1] = {0, 0, 0}; /* receiving queue tail */
uint8_t g_which_buf[UART_CH2 + 1] = {0, 0, 0}; /* Mark which buffer is used currently, 0: g_uart_rx_buf1；1: g_uart_rx_buf2. */
uint8_t g_uart_tx_buf[UART_CH2 + 1][UART_TX_BUFFER_SIZE];

typedef struct
{
	GPIO_TypeDef *gpio_;
	uint16_t rx_pin_;
	uint16_t tx_pin_;
	IRQn_Type irqs_[1];
	DMA_Channel_TypeDef* rx_dma_channel_;
	DMA_Channel_TypeDef* tx_dma_channel_;
	IRQn_Type rx_dma_irq_;
	IRQn_Type tx_dma_irq_;
	uint32_t dma_it_tc_;
	bool use_dma_rx_;
	bool use_dma_tx_;
} comm_config_t;

static comm_config_t g_comm_config[UART_CH2 + 1] =
{
	{
		.gpio_           = UART0_GPIO,
		.rx_pin_         = UART0_RX_PIN,
		.tx_pin_         = UART0_TX_PIN,
		.irqs_           = {UART0_IRQ},
		.rx_dma_channel_ = UART0_RX_DMA_CHANNEL,
		.tx_dma_channel_ = UART0_TX_DMA_CHANNEL,
		.rx_dma_irq_     = UART0_RX_DMA_IRQ,
		.tx_dma_irq_     = UART0_TX_DMA_IRQ,
		.dma_it_tc_      = UART0_DMA_IT_TC,
		.use_dma_rx_     = UART0_USE_DMA_RX,
		.use_dma_tx_     = UART0_USE_DMA_TX,
	},

	{
		.gpio_           = UART1_GPIO,
		.rx_pin_         = UART1_RX_PIN,
		.tx_pin_         = UART1_TX_PIN,
		.irqs_           = {UART1_IRQ},
		.rx_dma_channel_ = UART1_RX_DMA_CHANNEL,
		.tx_dma_channel_ = UART1_TX_DMA_CHANNEL,
		.rx_dma_irq_     = UART1_RX_DMA_IRQ,
		.tx_dma_irq_     = UART1_TX_DMA_IRQ,
		.dma_it_tc_      = UART1_DMA_IT_TC,
		.use_dma_rx_     = UART1_USE_DMA_RX,
		.use_dma_tx_     = UART1_USE_DMA_TX,
	},

	{
		.gpio_           = UART2_GPIO,
		.rx_pin_         = UART2_RX_PIN,
		.tx_pin_         = UART2_TX_PIN,
		.irqs_           = {UART2_IRQ},
		.rx_dma_channel_ = UART2_RX_DMA_CHANNEL,
		.tx_dma_channel_ = UART2_TX_DMA_CHANNEL,
		.rx_dma_irq_     = UART2_RX_DMA_IRQ,
		.tx_dma_irq_     = UART2_TX_DMA_IRQ,
		.dma_it_tc_      = UART2_DMA_IT_TC,
		.use_dma_rx_     = UART2_USE_DMA_RX,
		.use_dma_tx_     = UART2_USE_DMA_TX,
	}
};

static USART_TypeDef *g_handle[UART_CH2 + 1] = {UART0_INST, UART1_INST, UART2_INST};

/*******************************************************************************
 * Local function prototypes
 ******************************************************************************/
static void uart_irq_handler(const uint8_t _chl);
static void uart_tx_dma_irq_handler(const uint8_t _chl);

/*******************************************************************************
 * Functions
 ******************************************************************************/
int32_t uart_init(const uint8_t _chl, const uint32_t _baudrate, const uint32_t _data_bits, const uint32_t _stop_bits, const uint32_t _parity)
{
	assert(UART_CH2 >= _chl);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef DMA1_Init;

	/* initialize the rx queue */
	g_uart_rx_size1[_chl] = 0;
	g_uart_rx_size2[_chl] = 0;
	g_uart_rx_queue_head[_chl] = 0;
	g_uart_rx_queue_tail[_chl] = 0;
	g_which_buf[_chl] = 0;
	
#if defined USING_OS_FREERTOS
	g_uart_tx_mutex[_chl] = xSemaphoreCreateRecursiveMutex();
#endif
	
	/* initialize the GPIOs */
	UART_GPIO_CLK_ENABLE(_chl);
	GPIO_InitStructure.GPIO_Pin = g_comm_config[_chl].rx_pin_;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(g_comm_config[_chl].gpio_, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = g_comm_config[_chl].tx_pin_;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(g_comm_config[_chl].gpio_, &GPIO_InitStructure);
	
	/* initialize the UART */
	UART_CLK_ENABLE(_chl);
	USART_InitStructure.USART_BaudRate = _baudrate;
	USART_InitStructure.USART_WordLength = _data_bits;
	USART_InitStructure.USART_StopBits = _stop_bits;
	USART_InitStructure.USART_Parity = _parity;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(g_handle[_chl], &USART_InitStructure);
	USART_ITConfig(g_handle[_chl], g_comm_config[_chl].use_dma_rx_ ? USART_IT_IDLE : USART_IT_RXNE, ENABLE);
	USART_ClearFlag(g_handle[_chl], USART_FLAG_TC);
	USART_Cmd(g_handle[_chl], ENABLE);
	
	/* initialize the NVIC */
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	for (uint8_t i = 0; i < sizeof(g_comm_config[_chl].irqs_); i++)
	{
		NVIC_InitStructure.NVIC_IRQChannel = g_comm_config[_chl].irqs_[i];
		NVIC_Init(&NVIC_InitStructure);
	}

	/* initialize the DMA rx */
	if (g_comm_config[_chl].use_dma_rx_)
	{
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);						// 使能DMA1时钟
		
		/* UART -> RAM的数据传输 */
		DMA_DeInit(g_comm_config[_chl].rx_dma_channel_);  					    // 将DMA的通道寄存器重设为缺省值 
		DMA1_Init.DMA_PeripheralBaseAddr = (uint32_t)(&g_handle[_chl]->DR);	    // 启动传输前装入实际RAM地址
		DMA1_Init.DMA_MemoryBaseAddr = (uint32_t)(g_uart_rx_buf1[_chl]);        // 设置接收缓冲区首地址
		DMA1_Init.DMA_DIR = DMA_DIR_PeripheralSRC;								// 数据传输方向，从外设读取到内存
		DMA1_Init.DMA_BufferSize = UART_RX_BUFFER_SIZE; 						// DMA通道的DMA缓存的大小
		DMA1_Init.DMA_PeripheralInc = DMA_PeripheralInc_Disable;				// 外设地址寄存器不变
		DMA1_Init.DMA_MemoryInc = DMA_MemoryInc_Enable;							// 内存地址寄存器递增
		DMA1_Init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;			// 数据宽度为8位
		DMA1_Init.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;					// 数据宽度为8位
		DMA1_Init.DMA_Mode = DMA_Mode_Normal;									// 工作在正常模式
		DMA1_Init.DMA_Priority = DMA_Priority_VeryHigh; 						// DMA通道x拥有高优先级 
		DMA1_Init.DMA_M2M = DMA_M2M_Disable;									// DMA通道x没有设置为内存到内存传输
		DMA_Init(g_comm_config[_chl].rx_dma_channel_, &DMA1_Init); 			    // 对DMA通道进行初始化
		
		/* RX DMA1通道NVIC配置 */
		NVIC_InitStructure.NVIC_IRQChannel = g_comm_config[_chl].rx_dma_irq_;   // NVIC通道设置
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;				// 抢占优先级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;						// 子优先级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;							// IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);											// 根据指定的参数初始化NVIC寄存器
		
		DMA_ITConfig(g_comm_config[_chl].rx_dma_channel_, DMA_IT_TC, ENABLE);	// 开UART RX DMA中断
		DMA_Cmd(g_comm_config[_chl].rx_dma_channel_, ENABLE);                   // 使RX DMA通道开始工作
		USART_DMACmd(g_handle[_chl], USART_DMAReq_Rx, ENABLE);        		    // 开启串口DMA接收
	}
	
	/* initialize the DMA tx */
	if (g_comm_config[_chl].use_dma_tx_)
	{
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);						// 使能DMA1时钟
		
		/* RAM -> UART的数据传输 */
		DMA_DeInit(g_comm_config[_chl].tx_dma_channel_);						// 将DMA的通道寄存器重设为缺省值 
		DMA1_Init.DMA_PeripheralBaseAddr = (uint32_t)(&g_handle[_chl]->DR);	    // 启动传输前装入实际RAM地址
		DMA1_Init.DMA_MemoryBaseAddr = NULL;                                    // 设置发送缓冲区首地址
		DMA1_Init.DMA_DIR = DMA_DIR_PeripheralDST; 								// 数据传输方向，从内存发送到外设
		DMA1_Init.DMA_BufferSize = 0;							                // DMA通道的DMA缓存的大小
		DMA1_Init.DMA_PeripheralInc = DMA_PeripheralInc_Disable;				// 外设地址寄存器不变
		DMA1_Init.DMA_MemoryInc = DMA_MemoryInc_Enable;							// 内存地址寄存器递增
		DMA1_Init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;			// 数据宽度为8位
		DMA1_Init.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;					// 数据宽度为8位
		DMA1_Init.DMA_Mode = DMA_Mode_Normal;									// 工作在正常模式
		DMA1_Init.DMA_Priority = DMA_Priority_VeryHigh; 					    // DMA通道x拥有高优先级 
		DMA1_Init.DMA_M2M = DMA_M2M_Disable;									// DMA通道x没有设置为内存到内存传输
		DMA_Init(g_comm_config[_chl].tx_dma_channel_, &DMA1_Init); 		        // 对DMA通道进行初始化
	 
		/* TX DMA1通道NVIC配置 */
		NVIC_InitStructure.NVIC_IRQChannel = g_comm_config[_chl].tx_dma_irq_;	// NVIC通道设置
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;				// 抢占优先级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;						// 子优先级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;							// IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);											// 根据指定的参数初始化NVIC寄存器

		DMA_ITConfig(g_comm_config[_chl].tx_dma_channel_, DMA_IT_TC, ENABLE);   // 开UART TX DMA中断
		DMA_Cmd(g_comm_config[_chl].tx_dma_channel_, DISABLE);           		// 使TX DMA通道停止工作
		USART_DMACmd(g_handle[_chl], USART_DMAReq_Tx, ENABLE);        	        // 开启串口DMA发送
	}
	
	return 0;
}

int32_t uart_deinit(const uint8_t _chl)
{
	assert(UART_CH2 >= _chl);

	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
	
	if (g_comm_config[_chl].use_dma_rx_)
	{
		USART_DMACmd(g_handle[_chl], USART_DMAReq_Rx, DISABLE);
		DMA_Cmd(g_comm_config[_chl].rx_dma_channel_, DISABLE);	
		DMA_ITConfig(g_comm_config[_chl].rx_dma_channel_, DMA_IT_TC, DISABLE);
		
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannel = g_comm_config[_chl].rx_dma_irq_;
		NVIC_Init(&NVIC_InitStructure);
	}
	
	if (g_comm_config[_chl].use_dma_tx_)
	{
		DMA_ClearITPendingBit(g_comm_config[_chl].dma_it_tc_);
		USART_DMACmd(g_handle[_chl], USART_DMAReq_Tx, DISABLE);
		DMA_Cmd(g_comm_config[_chl].tx_dma_channel_, DISABLE);
		DMA_ITConfig(g_comm_config[_chl].tx_dma_channel_, DMA_IT_TC, DISABLE);
		
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannel = g_comm_config[_chl].tx_dma_irq_;
		NVIC_Init(&NVIC_InitStructure);		
	}

	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	
	for (uint8_t i = 0; i < sizeof(g_comm_config[_chl].irqs_); i++)
	{
		NVIC_InitStructure.NVIC_IRQChannel = g_comm_config[_chl].irqs_[i];
		NVIC_Init(&NVIC_InitStructure);
	}

	USART_ClearITPendingBit(g_handle[_chl], USART_IT_RXNE);
	USART_ClearFlag(g_handle[_chl], USART_FLAG_TC);
	USART_Cmd(g_handle[_chl], DISABLE);
	USART_ITConfig(g_handle[_chl], g_comm_config[_chl].use_dma_rx_ ? USART_IT_IDLE : USART_IT_RXNE, DISABLE);
	USART_DeInit(g_handle[_chl]);

	UART_CLK_DISABLE(_chl);
	UART_FORCE_RESET(_chl);
	UART_RELEASE_RESET(_chl);

#if defined USING_OS_FREERTOS
	vSemaphoreDelete(g_uart_tx_mutex[_chl]);
#endif

	return 0;
}

uint16_t uart_receive(const uint8_t _chl, uint8_t _buf[], const uint16_t _size)
{
	assert(UART_CH2 >= _chl && NULL != _buf);

	uint16_t i = 0;

	if (g_comm_config[_chl].use_dma_rx_)
	{
		if (g_which_buf[_chl])
		{
			if (0 < g_uart_rx_size1[_chl] && UART_RX_BUFFER_SIZE >= g_uart_rx_size1[_chl])
			{
				i = (_size >= g_uart_rx_size1[_chl] ? g_uart_rx_size1[_chl] : _size);
				memcpy(_buf, g_uart_rx_buf1[_chl], i);
				g_uart_rx_size1[_chl] = 0;
			}
		}
		else
		{
			if (0 < g_uart_rx_size2[_chl] && UART_RX_BUFFER_SIZE >= g_uart_rx_size2[_chl])
			{
				i = (_size >= g_uart_rx_size2[_chl] ? g_uart_rx_size2[_chl] : _size);
				memcpy(_buf, g_uart_rx_buf2[_chl], i);
				g_uart_rx_size2[_chl] = 0;
			}
		}
	}
	else
	{
		/* check if the rx queue is empty */
		while (g_uart_rx_queue_head[_chl] != g_uart_rx_queue_tail[_chl] && i < _size)
		{
			/* dequeue */
			_buf[i++] = g_uart_rx_buf1[_chl][g_uart_rx_queue_head[_chl]];
			g_uart_rx_queue_head[_chl] = (g_uart_rx_queue_head[_chl] + 1) % UART_RX_BUFFER_SIZE;
		}
	}
	
	return i;
}

uint16_t uart_send(const uint8_t _chl, const uint8_t _buf[], const uint16_t _size)
{
	assert(UART_CH2 >= _chl && NULL != _buf);

	uint16_t size = _size;
	
#if defined USING_OS_FREERTOS
	xSemaphoreTakeRecursive( g_uart_tx_mutex[_chl], portMAX_DELAY);
#endif

	if (g_comm_config[_chl].use_dma_tx_)
	{
		while(0 != DMA_GetCurrDataCounter(g_comm_config[_chl].tx_dma_channel_)) {}; // 等待上一次发送完成
		size = _size >= UART_TX_BUFFER_SIZE ? UART_TX_BUFFER_SIZE : _size;
		memcpy(g_uart_tx_buf, _buf, size);
		g_comm_config[_chl].tx_dma_channel_->CMAR = (uint32_t)g_uart_tx_buf;        // 设置要发送的数据地址
		g_comm_config[_chl].tx_dma_channel_->CNDTR = size;                          // 设置要发送的字节数目
		DMA_Cmd(g_comm_config[_chl].tx_dma_channel_, ENABLE);                       // 开始DMA发送
	}
	else
	{
		while (USART_GetFlagStatus(g_handle[_chl], USART_FLAG_TC) == RESET) {}
		
		for (uint16_t i = 0; i < _size; i++)        
		{           
			USART_SendData(g_handle[_chl], _buf[i]); 
			while (USART_GetFlagStatus(g_handle[_chl], USART_FLAG_TC) == RESET) {}
		}		
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

/**
 * UART2 IRQ handler.
 */
void UART2_IRQ_HANDLER(void)
{
	uart_irq_handler(UART_CH2);
}

/**
 * The UART0 TX DMA IRQ handler.
 */
void UART0_TX_DMA_IRQ_HANDLER(void)
{	
	uart_tx_dma_irq_handler(UART_CH0);
}

/**
 * UART1 TX DMA IRQ handler.
 */
void UART1_TX_DMA_IRQ_HANDLER(void)
{
	uart_tx_dma_irq_handler(UART_CH1);
}

/**
 * UART2 TX DMA IRQ handler.
 */
void UART2_TX_DMA_IRQ_HANDLER(void)
{
	uart_tx_dma_irq_handler(UART_CH2);
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
void uart_irq_handler(const uint8_t _chl)
{
	if (g_comm_config[_chl].use_dma_rx_)
	{
		if (RESET != USART_GetITStatus(g_handle[_chl], USART_IT_IDLE)) // 串口空闲中断
		{
			USART_ReceiveData(g_handle[_chl]); 						   // 清除串口空闲中断IDLE标志位
			//USART_ClearFlag(g_handle[_chl], USART_FLAG_TC);		   // 清除UART标志位
			DMA_Cmd(g_comm_config[_chl].rx_dma_channel_, DISABLE);     // 关闭UART TX DMA1所指示的通道
			
			if (g_which_buf[_chl])                        			   // 之前用的g_uart_rx_buf2，切换为g_uart_rx_buf1
			{
				g_uart_rx_size2[_chl] = UART_RX_BUFFER_SIZE 
					- g_comm_config[_chl].rx_dma_channel_->CNDTR;      // 获得接收到的字节数			
				g_comm_config[_chl].rx_dma_channel_->CMAR 
					= (uint32_t)g_uart_rx_buf1[_chl];	               // 切换为g_uart_rx_buf1缓冲区地址
				g_which_buf[_chl] = 0;                     			   // 下一次切换为g_uart_rx_buf2
			}                                                              
			else                               						   // 之前用的g_uart_rx_buf1，切换为g_uart_rx_buf2
			{
				g_uart_rx_size1[_chl] = UART_RX_BUFFER_SIZE 
					- g_comm_config[_chl].rx_dma_channel_->CNDTR;      // 获得接收到的字节数			
				g_comm_config[_chl].rx_dma_channel_->CMAR 
					= (uint32_t)g_uart_rx_buf2[_chl];	               // 切换为g_uart_rx_buf2缓冲区地址
				g_which_buf[_chl] = 1;                     			   // 下一次切换为g_uart_rx_buf1
			}
			
			g_comm_config[_chl].rx_dma_channel_->CNDTR 
				= UART_RX_BUFFER_SIZE;                                 // DMA通道的DMA缓存的大小
			DMA_Cmd(g_comm_config[_chl].rx_dma_channel_, ENABLE);      // 使能UART TX DMA1所指示的通道
		}
	}
	else
	{
		/* RXNE */
		if (RESET != USART_GetITStatus(g_handle[_chl], USART_IT_RXNE))
		{		
			USART_ClearITPendingBit(g_handle[_chl], USART_IT_RXNE);
			
			/* check if the rx queue is full */
			if (g_uart_rx_queue_head[_chl] == (g_uart_rx_queue_tail[_chl] + 1) % UART_RX_BUFFER_SIZE)
			{
				/* Dequeue */
				g_uart_rx_queue_head[_chl] = (g_uart_rx_queue_head[_chl] + 1) % UART_RX_BUFFER_SIZE;
			}
			
			/* enqueue */
			g_uart_rx_buf1[_chl][g_uart_rx_queue_tail[_chl]] = USART_ReceiveData(g_handle[_chl]);
			g_uart_rx_queue_tail[_chl] = (g_uart_rx_queue_tail[_chl] + 1) % UART_RX_BUFFER_SIZE;
		}
	}
}

/**
 * UART TX DMA IRQ handler.
 *
 * @param [in] _chl the UART channel number
 */
void uart_tx_dma_irq_handler(const uint8_t _chl)
{
	if (RESET != DMA_GetITStatus(g_comm_config[_chl].dma_it_tc_)) // DMA接收完成标志
	{
		DMA_ClearITPendingBit(g_comm_config[_chl].dma_it_tc_);    // 清除中断标志 
		USART_ClearFlag(g_handle[_chl], USART_FLAG_TC);	          // 清除串口的标志位
		DMA_Cmd(g_comm_config[_chl].tx_dma_channel_, DISABLE);    // 关闭UART TX DMA1 所指示的通道
	}
}
