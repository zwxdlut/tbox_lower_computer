#include "spi.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
typedef struct
{
	PORT_Type *sck_port_;
	uint8_t sck_pin_;
	PORT_Type *sdi_port_;
	uint8_t sdi_pin_;
	PORT_Type *sdo_port_;
	uint8_t sdo_pin_;
	PORT_Type *cs_port_;
	uint8_t cs_pin_;
	port_mux_t gpio_af_;
	IRQn_Type irqs_[1];
} comm_config_t;

static comm_config_t g_comm_config[SPI_CH0 + 1] =
{
	{
		.sck_port_ = SPI0_SCK_PORT,
		.sck_pin_  = SPI0_SCK_PIN,
		.sdi_port_ = SPI0_SDI_PORT,
		.sdi_pin_  = SPI0_SDI_PIN,
		.sdo_port_ = SPI0_SDO_PORT,
		.sdo_pin_  = SPI0_SDO_PIN,
		.cs_port_  = SPI0_CS_PORT,
		.cs_pin_   = SPI0_CS_PIN,
		.gpio_af_  = SPI0_GPIO_AF,
		.irqs_     = {SPI0_IRQ},
	}
};

static uint8_t g_handle[SPI_CH0 + 1] =
{
#if defined SPI0
	SPI0,
#else
	0xFF,
#endif
};

static lpspi_master_config_t *g_master_config[SPI_CH0 + 1] =
{
#if defined SPI0
	&spi0_MasterConfig0,
#else
	NULL,
#endif
};

static lpspi_slave_config_t *g_slave_config[SPI_CH0 + 1] =
{
#if defined SPI0
	&spi0_SlaveConfig0,
#else
	NULL,
#endif
};

static lpspi_state_t *g_state[SPI_CH0 + 1] =
{
#if defined SPI0
	&spi0State,
#else
	NULL,
#endif
};

/*******************************************************************************
 * Local function prototypes
 ******************************************************************************/
int32_t spi_init(const uint8_t _chl);
int32_t spi_deinit(const uint8_t _chl);

/*******************************************************************************
 * Functions
 ******************************************************************************/
int32_t spi_master_init(const uint8_t _chl, const uint32_t _baudrate, const uint8_t _cpol, const uint8_t _cpha, const uint8_t _data_bits, const bool _lsb_first)
{
    spi_init(_chl);

	/* initialize the master SPI */
    g_master_config[_chl]->bitsPerSec = _baudrate;
	g_master_config[_chl]->clkPolarity = (SPI_CPOL_LOW == _cpol ? LPSPI_SCK_ACTIVE_HIGH : LPSPI_SCK_ACTIVE_LOW);
	g_master_config[_chl]->clkPhase = (SPI_CPHA_1EDGE == _cpha ? LPSPI_CLOCK_PHASE_1ST_EDGE : LPSPI_CLOCK_PHASE_2ND_EDGE);
	g_master_config[_chl]->bitcount = _data_bits;
	g_master_config[_chl]->lsbFirst = _lsb_first;
	LPSPI_DRV_MasterInit(g_handle[_chl], g_state[_chl], g_master_config[_chl]);
	LPSPI_DRV_MasterSetDelay(g_handle[_chl], 1, 1, 1);

	return 0;
}

int32_t spi_master_deinit(const uint8_t _chl)
{
	LPSPI_DRV_MasterDeinit(g_handle[_chl]);

	return spi_deinit(_chl);
}

int32_t spi_master_receive(const uint8_t _chl, uint8_t _buf[], const uint16_t _size)
{
	assert(SPI_CH0 >= _chl && NULL != _buf);

    if (STATUS_SUCCESS != LPSPI_DRV_MasterTransfer(g_handle[_chl], NULL, _buf, _size))
	{
		return -1;
	}

    status_t status;

    while (STATUS_BUSY == (status = LPSPI_DRV_MasterGetTransferStatus(g_handle[_chl], NULL))) {}

	if (STATUS_SUCCESS != status)
	{
		return -1;
	}

	return 0;
}

int32_t spi_master_send(const uint8_t _chl, const uint8_t _buf[], const uint16_t _size)
{
	assert(SPI_CH0 >= _chl && NULL != _buf);

    if (STATUS_SUCCESS != LPSPI_DRV_MasterTransfer(g_handle[_chl], _buf, NULL, _size))
    	return -1;

    status_t status;

    while (STATUS_BUSY == (status = LPSPI_DRV_MasterGetTransferStatus(g_handle[_chl], NULL))) {}

	if (STATUS_SUCCESS != status)
	{
		return -1;
	}

	return 0;
}

int32_t spi_slave_init(const uint8_t _chl, const uint8_t _cpol, const uint8_t _cpha, const uint8_t _data_bits, const bool _lsb_first)
{
    spi_init(_chl);

	/* initialize the slave SPI */
	g_slave_config[_chl]->clkPolarity = (SPI_CPOL_LOW == _cpol ? LPSPI_SCK_ACTIVE_HIGH : LPSPI_SCK_ACTIVE_LOW);
	g_slave_config[_chl]->clkPhase = (SPI_CPHA_1EDGE == _cpha ? LPSPI_CLOCK_PHASE_1ST_EDGE : LPSPI_CLOCK_PHASE_2ND_EDGE);
	g_slave_config[_chl]->bitcount = _data_bits;
	g_slave_config[_chl]->lsbFirst = _lsb_first;
	LPSPI_DRV_SlaveInit(g_handle[_chl], g_state[_chl], g_slave_config[_chl]);

	return 0;
}

int32_t spi_slave_deinit(const uint8_t _chl)
{
	LPSPI_DRV_SlaveDeinit(g_handle[_chl]);
	return spi_deinit(_chl);
}

int32_t spi_slave_receive(const uint8_t _chl, uint8_t _buf[], const uint16_t _size)
{
	assert(SPI_CH0 >= _chl && NULL != _buf);

    if (STATUS_SUCCESS != LPSPI_DRV_SlaveTransfer(g_handle[_chl], NULL, _buf, _size))
	{
		return -1;
	}

    status_t status;

    while (STATUS_BUSY == (status = LPSPI_DRV_SlaveGetTransferStatus(g_handle[_chl], NULL))) {}

	if (STATUS_SUCCESS != status)
	{
		return -1;
	}

	return 0;
}

int32_t spi_slave_send(const uint8_t _chl, const uint8_t _buf[], const uint16_t _size)
{
	assert(SPI_CH0 >= _chl && NULL != _buf);

    if (STATUS_SUCCESS != LPSPI_DRV_SlaveTransfer(g_handle[_chl], _buf, NULL, _size))
	{
		return -1;
	}

    status_t status;

    while (STATUS_BUSY == (status = LPSPI_DRV_SlaveGetTransferStatus(g_handle[_chl], NULL))) {}

	if (STATUS_SUCCESS != status)
	{
		return -1;
	}

	return 0;
}

/*******************************************************************************
 * Local functions
 ******************************************************************************/
int32_t spi_init(const uint8_t _chl)
{
	assert(SPI_CH0 >= _chl);

    EDMA_DRV_Init(&dmaController1_State, &dmaController1_InitConfig0, edmaChnStateArray, edmaChnConfigArray, EDMA_CONFIGURED_CHANNELS_COUNT);

	/* initialize the GPIOs */
	PINS_DRV_SetMuxModeSel(g_comm_config[_chl].sck_port_, g_comm_config[_chl].sck_pin_, g_comm_config[_chl].gpio_af_);
	PINS_DRV_SetMuxModeSel(g_comm_config[_chl].sdi_port_, g_comm_config[_chl].sdi_pin_, g_comm_config[_chl].gpio_af_);
	PINS_DRV_SetMuxModeSel(g_comm_config[_chl].sdo_port_, g_comm_config[_chl].sdo_pin_, g_comm_config[_chl].gpio_af_);
	PINS_DRV_SetMuxModeSel(g_comm_config[_chl].cs_port_, g_comm_config[_chl].cs_pin_, g_comm_config[_chl].gpio_af_);

#if defined USING_OS_FREERTOS
	/* The interrupt calls an interrupt safe API function - so its priority must
	   be equal to or lower than configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY */
	for (uint8_t i = 0; i < sizeof(g_comm_config[_chl].irqs_) && NotAvail_IRQn != g_comm_config[_chl].irqs_[i]; i++)
	{
		INT_SYS_SetPriority( g_comm_config[_chl].irqs_[i], configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
	}
#endif

	return 0;
}

int32_t spi_deinit(const uint8_t _chl)
{
	assert(SPI_CH0 >= _chl);

	PINS_DRV_SetMuxModeSel(g_comm_config[_chl].sck_port_, g_comm_config[_chl].sck_pin_, PORT_PIN_DISABLED);
	PINS_DRV_SetMuxModeSel(g_comm_config[_chl].sdi_port_, g_comm_config[_chl].sdi_pin_, PORT_PIN_DISABLED);
	PINS_DRV_SetMuxModeSel(g_comm_config[_chl].sdo_port_, g_comm_config[_chl].sdo_pin_, PORT_PIN_DISABLED);
	PINS_DRV_SetMuxModeSel(g_comm_config[_chl].cs_port_, g_comm_config[_chl].cs_pin_, PORT_PIN_DISABLED);
	
	return 0;
}
