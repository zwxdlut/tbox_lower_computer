#include "spi.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
typedef struct
{
	GPIO_TypeDef *sck_gpio_;
	uint16_t sck_pin_;
    GPIO_TypeDef *miso_gpio_;
	uint16_t miso_pin_;
	GPIO_TypeDef *mosi_gpio_;
	uint16_t mosi_pin_;
    GPIO_TypeDef *cs_gpio_;
	uint16_t cs_pin_;
	uint8_t gpio_af_;
	IRQn_Type irqs_[3];
} comm_config_t;

static comm_config_t g_comm_config[SPI_CH0 + 1] =
{
	{
		.sck_gpio_  = SPI0_SCK_GPIO,
		.sck_pin_   = SPI0_SCK_PIN,
		.miso_gpio_ = SPI0_MISO_GPIO,
		.miso_pin_  = SPI0_MISO_PIN,
		.mosi_gpio_ = SPI0_MOSI_GPIO,
		.mosi_pin_  = SPI0_MOSI_PIN,
		.cs_gpio_   = SPI0_CS_GPIO,
		.cs_pin_    = SPI0_CS_PIN,
		.gpio_af_   = SPI0_GPIO_AF,
		.irqs_      = {SPI0_IRQ, SPI0_RX_DMA_IRQ, SPI0_TX_DMA_IRQ},
	}
};

static SPI_HandleTypeDef g_handle[SPI_CH0 + 1] = 
{
	{
		.Instance               = SPI0_INST,
		.Init.Mode              = SPI_MODE_MASTER,
		.Init.Direction         = SPI_DIRECTION_2LINES,
		.Init.DataSize          = SPI_DATASIZE_8BIT,
		.Init.CLKPolarity       = SPI_POLARITY_HIGH,
		.Init.CLKPhase          = SPI_PHASE_1EDGE,
		.Init.NSS               = SPI_NSS_SOFT,
		.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256,
		.Init.FirstBit          = SPI_FIRSTBIT_MSB,
		.Init.TIMode            = SPI_TIMODE_DISABLE,
		.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE,
		.Init.CRCPolynomial     = 7,
	}
};

static DMA_HandleTypeDef g_rx_dma[SPI_CH0 + 1] =
{
	{
		.Instance                 = SPI0_RX_DMA_STREAM,
		.Init.Channel             = SPI0_RX_DMA_CHANNEL,
		.Init.FIFOMode            = DMA_FIFOMODE_DISABLE,
		.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL,
		.Init.MemBurst            = DMA_MBURST_INC4,
		.Init.PeriphBurst         = DMA_PBURST_INC4,
		.Init.Direction           = DMA_PERIPH_TO_MEMORY,
		.Init.PeriphInc           = DMA_PINC_DISABLE,
		.Init.MemInc              = DMA_MINC_ENABLE,
		.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE,
		.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE,
		.Init.Mode                = DMA_NORMAL,
		.Init.Priority            = DMA_PRIORITY_HIGH,
	}
};

static DMA_HandleTypeDef g_tx_dma[SPI_CH0 + 1] =
{
	{
		.Instance                 = SPI0_TX_DMA_STREAM,
		.Init.Channel             = SPI0_TX_DMA_CHANNEL,
		.Init.FIFOMode            = DMA_FIFOMODE_DISABLE,
		.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL,
		.Init.MemBurst            = DMA_MBURST_INC4,
		.Init.PeriphBurst         = DMA_PBURST_INC4,
		.Init.Direction           = DMA_MEMORY_TO_PERIPH,
		.Init.PeriphInc           = DMA_PINC_DISABLE,
		.Init.MemInc              = DMA_MINC_ENABLE,
		.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE,
		.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE,
		.Init.Mode                = DMA_NORMAL,
		.Init.Priority            = DMA_PRIORITY_LOW,
	}
};

/*******************************************************************************
 * Local function prototypes
 ******************************************************************************/
static int32_t spi_init(const uint8_t _chl, const uint8_t _cpol, const uint8_t _cpha, const uint8_t _data_bits, const bool _lsb_first);

/*******************************************************************************
 * Functions
 ******************************************************************************/
int32_t spi_master_init(const uint8_t _chl, const uint32_t _baudrate, const uint8_t _cpol, const uint8_t _cpha, const uint8_t _data_bits, const bool _lsb_first)
{
	uint32_t psr = 0;
	
	/* calculate the prescaler */
	if (SPI1 == g_handle[_chl].Instance)
	{
		psr = HAL_RCC_GetPCLK2Freq() / _baudrate;
	}
	else
	{
		psr = HAL_RCC_GetPCLK1Freq() / _baudrate;
	}

	if (3 >= psr)
	{
		g_handle[_chl].Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	}
	else if (6 >= psr && 3 < psr)
	{
		g_handle[_chl].Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
	}
	else if (12 >= psr && 6 < psr)
	{
		g_handle[_chl].Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
	}
	else if (24 >= psr && 12 < psr)
	{
		g_handle[_chl].Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
	}
	else if (48 >= psr && 24 < psr)
	{
		g_handle[_chl].Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
	}
	else if (96 >= psr && 48 < psr)
	{
		g_handle[_chl].Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
	}
	else if (192 >= psr && 96 < psr)
	{
		g_handle[_chl].Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
	}
	else
	{
		g_handle[_chl].Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	}
	
	g_handle[_chl].Init.Mode = SPI_MODE_MASTER;
	
	return spi_init(_chl, _cpol, _cpha, _data_bits, _lsb_first);
}

int32_t spi_master_deinit(const uint8_t _chl)
{
	assert(SPI_CH0 >= _chl);

	for (uint8_t i = 0; i < sizeof(g_comm_config[_chl].irqs_); i++)
	{
		HAL_NVIC_DisableIRQ(g_comm_config[_chl].irqs_[i]);
	}

	HAL_DMA_DeInit(&g_rx_dma[_chl]);
    HAL_DMA_DeInit(&g_tx_dma[_chl]);
	SPI_DMA_CLK_DISABLE(_chl);

	HAL_SPI_DeInit(&g_handle[_chl]);
	SPI_CLK_DISABLE(_chl);
	SPI_FORCE_RESET(_chl);
	SPI_RELEASE_RESET(_chl);

	HAL_GPIO_DeInit(g_comm_config[_chl].sck_gpio_, g_comm_config[_chl].sck_pin_);
	HAL_GPIO_DeInit(g_comm_config[_chl].miso_gpio_, g_comm_config[_chl].miso_pin_);
	HAL_GPIO_DeInit(g_comm_config[_chl].mosi_gpio_, g_comm_config[_chl].mosi_pin_);
	HAL_GPIO_DeInit(g_comm_config[_chl].cs_gpio_, g_comm_config[_chl].cs_pin_);

	return 0;
}

int32_t spi_master_receive(const uint8_t _chl, uint8_t _buf[], const uint16_t _size)
{
	assert(SPI_CH0 >= _chl && NULL != _buf);
	
	if (HAL_OK != HAL_SPI_Receive_DMA(&g_handle[_chl], _buf, _size))
	{
		return -1;
	}

	while (HAL_SPI_STATE_READY != HAL_SPI_GetState(&g_handle[_chl])) {}
		
	return 0;
}

int32_t spi_master_send(const uint8_t _chl, const uint8_t _buf[], const uint16_t _size)
{
	assert(SPI_CH0 >= _chl && NULL != _buf);
	
	if (HAL_OK != HAL_SPI_Transmit_DMA(&g_handle[_chl], (uint8_t *)_buf, _size))
	{
		return -1;
	}

	while (HAL_SPI_STATE_READY != HAL_SPI_GetState(&g_handle[_chl])) {}
		
	return 0;
}

int32_t spi_slave_init(const uint8_t _chl, const uint8_t _cpol, const uint8_t _cpha, const uint8_t _data_bits, const bool _lsb_first)
{
	g_handle[_chl].Init.Mode = SPI_MODE_SLAVE;
	return spi_init(_chl, _cpol, _cpha, _data_bits, _lsb_first);
}

int32_t spi_slave_deinit(const uint8_t _chl)
{
	return spi_master_deinit(_chl);
}

int32_t spi_slave_receive(const uint8_t _chl, uint8_t _buf[], const uint16_t _size)
{
	return spi_master_receive(_chl, _buf, _size);
}

int32_t spi_slave_send(const uint8_t _chl, const uint8_t _buf[], const uint16_t _size)
{
	return spi_master_send(_chl, _buf, _size);
}

/**
 * @name IRQ handlers
 * @{
 */

/**
 * SPI0 IRQ handler.
 */
void SPI0_IRQ_HANDLER(void)
{	
	HAL_SPI_IRQHandler(&g_handle[SPI_CH0]);
}

/**
 * SPI0 RX DMA IRQ handler.
 */
void SPI0_RX_DMA_IRQ_HANDLER(void)
{
	HAL_DMA_IRQHandler(g_handle[SPI_CH0].hdmarx);
}

/**
 * SPI0 TX DMA IRQ handler.
 */
void SPI0_TX_DMA_IRQ_HANDLER(void)
{
	HAL_DMA_IRQHandler(g_handle[SPI_CH0].hdmatx);
}

/** @} */ /* IRQ handlers */

/*******************************************************************************
 * Local functions
 ******************************************************************************/
int32_t spi_init(const uint8_t _chl, const uint8_t _cpol, const uint8_t _cpha, const uint8_t _data_bits, const bool _lsb_first)
{
	assert(SPI_CH0 >= _chl);
	
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	/* initialize the GPIOs */
	SPI_SCK_GPIO_CLK_ENABLE(_chl);
	GPIO_InitStructure.Pin = g_comm_config[_chl].sck_pin_;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStructure.Alternate = g_comm_config[_chl].gpio_af_;
	HAL_GPIO_Init(g_comm_config[_chl].sck_gpio_, &GPIO_InitStructure);

	SPI_MISO_GPIO_CLK_ENABLE(_chl);
	GPIO_InitStructure.Pin = g_comm_config[_chl].miso_pin_;
	HAL_GPIO_Init(g_comm_config[_chl].miso_gpio_, &GPIO_InitStructure);

	SPI_MOSI_GPIO_CLK_ENABLE(_chl);
	GPIO_InitStructure.Pin = g_comm_config[_chl].mosi_pin_;
	HAL_GPIO_Init(g_comm_config[_chl].mosi_gpio_, &GPIO_InitStructure);

	SPI_CS_GPIO_CLK_ENABLE(_chl);
	GPIO_InitStructure.Pin = g_comm_config[_chl].cs_pin_;
	HAL_GPIO_Init(g_comm_config[_chl].cs_gpio_, &GPIO_InitStructure);

	/* initialize the SPI */
	SPI_CLK_ENABLE(_chl);
	g_handle[_chl].Init.CLKPolarity = (SPI_CPOL_LOW == _cpol ? SPI_POLARITY_LOW : SPI_POLARITY_HIGH);
	g_handle[_chl].Init.CLKPhase    = (SPI_CPHA_1EDGE == _cpha ? SPI_PHASE_1EDGE : SPI_PHASE_2EDGE);
	g_handle[_chl].Init.DataSize    = (SPI_DATA_BITS_8 == _data_bits ? SPI_DATASIZE_8BIT : SPI_DATASIZE_16BIT);
	g_handle[_chl].Init.FirstBit    = _lsb_first ? SPI_FIRSTBIT_LSB : SPI_FIRSTBIT_MSB;
	HAL_SPI_Init(&g_handle[_chl]);
	
    /* initialize the DMA */
	SPI_DMA_CLK_ENABLE(_chl);
	HAL_DMA_Init(&g_rx_dma[_chl]);
    __HAL_LINKDMA(&g_handle[_chl], hdmarx, g_rx_dma[_chl]);
    HAL_DMA_Init(&g_tx_dma[_chl]);
    __HAL_LINKDMA(&g_handle[_chl], hdmatx, g_tx_dma[_chl]);
	
	/* initialize the NVIC */
	for (uint8_t i = 0; i < sizeof(g_comm_config[_chl].irqs_); i++)
	{
		HAL_NVIC_SetPriority(g_comm_config[_chl].irqs_[i], 0, 0);
		HAL_NVIC_EnableIRQ(g_comm_config[_chl].irqs_[i]);
	}

	return 0;
}
