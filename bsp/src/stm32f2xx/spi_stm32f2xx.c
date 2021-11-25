/*
 * spi_stm32f2xx.c
 *
 *  Created on: 2018年10月16日
 *      Author: Administrator
 */

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

static comm_config_t g_comm_config[SPI0_INDEX + 1] =
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

static SPI_HandleTypeDef g_handle[SPI0_INDEX + 1] = 
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

static DMA_HandleTypeDef g_rx_dma[SPI0_INDEX + 1] =
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

static DMA_HandleTypeDef g_tx_dma[SPI0_INDEX + 1] =
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
static int32_t spi_init(const uint8_t _index, const uint8_t _cpol, const uint8_t _cpha, const uint8_t _data_bits, const bool _lsb_first);

/*******************************************************************************
 * Functions
 ******************************************************************************/
int32_t spi_master_init(const uint8_t _index, const uint32_t _baudrate, const uint8_t _cpol, const uint8_t _cpha, const uint8_t _data_bits, const bool _lsb_first)
{
	uint32_t psr = 0;
	
	/* Calculate the prescaler */
	if (SPI1 == g_handle[_index].Instance)
	{
		psr = HAL_RCC_GetPCLK2Freq() / _baudrate;
	}
	else
	{
		psr = HAL_RCC_GetPCLK1Freq() / _baudrate;
	}

	if (3 >= psr)
	{
		g_handle[_index].Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	}
	else if (6 >= psr && 3 < psr)
	{
		g_handle[_index].Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
	}
	else if (12 >= psr && 6 < psr)
	{
		g_handle[_index].Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
	}
	else if (24 >= psr && 12 < psr)
	{
		g_handle[_index].Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
	}
	else if (48 >= psr && 24 < psr)
	{
		g_handle[_index].Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
	}
	else if (96 >= psr && 48 < psr)
	{
		g_handle[_index].Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
	}
	else if (192 >= psr && 96 < psr)
	{
		g_handle[_index].Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
	}
	else
	{
		g_handle[_index].Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	}
	
	g_handle[_index].Init.Mode = SPI_MODE_MASTER;
	
	return spi_init(_index, _cpol, _cpha, _data_bits, _lsb_first);
}

int32_t spi_master_deinit(const uint8_t _index)
{
	assert(SPI0_INDEX >= _index);

	for (uint8_t i = 0; i < sizeof(g_comm_config[_index].irqs_); i++)
	{
		HAL_NVIC_DisableIRQ(g_comm_config[_index].irqs_[i]);
	}

	HAL_DMA_DeInit(&g_rx_dma[_index]);
    HAL_DMA_DeInit(&g_tx_dma[_index]);
	SPI_DMA_CLK_DISABLE(_index);

	HAL_SPI_DeInit(&g_handle[_index]);
	SPI_CLK_DISABLE(_index);
	SPI_FORCE_RESET(_index);
	SPI_RELEASE_RESET(_index);

	HAL_GPIO_DeInit(g_comm_config[_index].sck_gpio_, g_comm_config[_index].sck_pin_);
	HAL_GPIO_DeInit(g_comm_config[_index].miso_gpio_, g_comm_config[_index].miso_pin_);
	HAL_GPIO_DeInit(g_comm_config[_index].mosi_gpio_, g_comm_config[_index].mosi_pin_);
	HAL_GPIO_DeInit(g_comm_config[_index].cs_gpio_, g_comm_config[_index].cs_pin_);

	return 0;
}

int32_t spi_master_receive(const uint8_t _index, uint8_t _buf[], const uint16_t _size)
{
	assert(SPI0_INDEX >= _index && NULL != _buf);
	
	if (HAL_OK != HAL_SPI_Receive_DMA(&g_handle[_index], _buf, _size))
	{
		return -1;
	}

	while (HAL_SPI_STATE_READY != HAL_SPI_GetState(&g_handle[_index])) {}
		
	return 0;
}

int32_t spi_master_send(const uint8_t _index, const uint8_t _buf[], const uint16_t _size)
{
	assert(SPI0_INDEX >= _index && NULL != _buf);
	
	if (HAL_OK != HAL_SPI_Transmit_DMA(&g_handle[_index], (uint8_t *)_buf, _size))
	{
		return -1;
	}

	while (HAL_SPI_STATE_READY != HAL_SPI_GetState(&g_handle[_index])) {}
		
	return 0;
}

int32_t spi_slave_init(const uint8_t _index, const uint8_t _cpol, const uint8_t _cpha, const uint8_t _data_bits, const bool _lsb_first)
{
	g_handle[_index].Init.Mode = SPI_MODE_SLAVE;
	return spi_init(_index, _cpol, _cpha, _data_bits, _lsb_first);
}

int32_t spi_slave_deinit(const uint8_t _index)
{
	return spi_master_deinit(_index);
}

int32_t spi_slave_receive(const uint8_t _index, uint8_t _buf[], const uint16_t _size)
{
	return spi_master_receive(_index, _buf, _size);
}

int32_t spi_slave_send(const uint8_t _index, const uint8_t _buf[], const uint16_t _size)
{
	return spi_master_send(_index, _buf, _size);
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
	HAL_SPI_IRQHandler(&g_handle[SPI0_INDEX]);
}

/**
 * SPI0 RX DMA IRQ handler.
 */
void SPI0_RX_DMA_IRQ_HANDLER(void)
{
	HAL_DMA_IRQHandler(g_handle[SPI0_INDEX].hdmarx);
}

/**
 * SPI0 TX DMA IRQ handler.
 */
void SPI0_TX_DMA_IRQ_HANDLER(void)
{
	HAL_DMA_IRQHandler(g_handle[SPI0_INDEX].hdmatx);
}

/** @} */ /* IRQ handlers */

/*******************************************************************************
 * Local functions
 ******************************************************************************/
int32_t spi_init(const uint8_t _index, const uint8_t _cpol, const uint8_t _cpha, const uint8_t _data_bits, const bool _lsb_first)
{
	assert(SPI0_INDEX >= _index);
	
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	/* Initialize the GPIOs */
	SPI_SCK_GPIO_CLK_ENABLE(_index);
	GPIO_InitStructure.Pin = g_comm_config[_index].sck_pin_;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStructure.Alternate = g_comm_config[_index].gpio_af_;
	HAL_GPIO_Init(g_comm_config[_index].sck_gpio_, &GPIO_InitStructure);

	SPI_MISO_GPIO_CLK_ENABLE(_index);
	GPIO_InitStructure.Pin = g_comm_config[_index].miso_pin_;
	HAL_GPIO_Init(g_comm_config[_index].miso_gpio_, &GPIO_InitStructure);

	SPI_MOSI_GPIO_CLK_ENABLE(_index);
	GPIO_InitStructure.Pin = g_comm_config[_index].mosi_pin_;
	HAL_GPIO_Init(g_comm_config[_index].mosi_gpio_, &GPIO_InitStructure);

	SPI_CS_GPIO_CLK_ENABLE(_index);
	GPIO_InitStructure.Pin = g_comm_config[_index].cs_pin_;
	HAL_GPIO_Init(g_comm_config[_index].cs_gpio_, &GPIO_InitStructure);

	/* Initialize the SPI */
	SPI_CLK_ENABLE(_index);
	g_handle[_index].Init.CLKPolarity = (SPI_CPOL_LOW == _cpol ? SPI_POLARITY_LOW : SPI_POLARITY_HIGH);
	g_handle[_index].Init.CLKPhase    = (SPI_CPHA_1EDGE == _cpha ? SPI_PHASE_1EDGE : SPI_PHASE_2EDGE);
	g_handle[_index].Init.DataSize    = (SPI_DATA_BITS_8 == _data_bits ? SPI_DATASIZE_8BIT : SPI_DATASIZE_16BIT);
	g_handle[_index].Init.FirstBit    = _lsb_first ? SPI_FIRSTBIT_LSB : SPI_FIRSTBIT_MSB;
	HAL_SPI_Init(&g_handle[_index]);
	
    /* Initialize the DMA */
	SPI_DMA_CLK_ENABLE(_index);
	HAL_DMA_Init(&g_rx_dma[_index]);
    __HAL_LINKDMA(&g_handle[_index], hdmarx, g_rx_dma[_index]);
    HAL_DMA_Init(&g_tx_dma[_index]);
    __HAL_LINKDMA(&g_handle[_index], hdmatx, g_tx_dma[_index]);
	
	/* Initialize the NVIC */
	for (uint8_t i = 0; i < sizeof(g_comm_config[_index].irqs_); i++)
	{
		HAL_NVIC_SetPriority(g_comm_config[_index].irqs_[i], 0, 0);
		HAL_NVIC_EnableIRQ(g_comm_config[_index].irqs_[i]);
	}

	return 0;
}
