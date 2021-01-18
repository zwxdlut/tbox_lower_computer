/*
 * i2c_s32k1xx.c
 *
 *  Created on: 2019��1��9��
 *      Author: Administrator
 */

#include "i2c.h"

/******************************************************************************
 * Definitions
 ******************************************************************************/
#if defined USING_OS_FREERTOS
SemaphoreHandle_t g_i2c_mutex[I2C0_INDEX + 1] = {NULL}; // the RX/TX mutex
#endif

typedef struct
{
	PORT_Type    *port_;
	uint8_t      scl_pin_;
	uint8_t      sda_pin_;
	port_mux_t   gpio_af_;
	IRQn_Type    irqs_[1];
} comm_config_t;

static comm_config_t g_comm_config[I2C0_INDEX + 1] =
{
	{
		.port_    = I2C0_PORT,
		.scl_pin_ = I2C0_SCL_PIN,
		.sda_pin_ = I2C0_SDA_PIN,
		.gpio_af_ = I2C0_GPIO_AF,
		.irqs_    = {I2C0_IRQ}
	}
};

static uint8_t g_handle[I2C0_INDEX + 1] =
{
#if defined INST_LPI2C0
	INST_LPI2C0,
#else
	0xFF
#endif
};

static lpi2c_master_user_config_t *g_config[I2C0_INDEX + 1] =
{
#if defined INST_LPI2C0
	&lpi2c0_MasterConfig0,
#else
	NULL
#endif
};

static lpi2c_master_state_t g_state[I2C0_INDEX + 1];

/******************************************************************************
 * Local function prototypes
 ******************************************************************************/
/******************************************************************************
 * Functions
 ******************************************************************************/
int32_t i2c_master_init(const uint8_t _index, const uint32_t _baudrate, const bool _is_10bit_addr)
{
	assert(I2C0_INDEX >= _index);

#if defined USING_OS_FREERTOS
	g_i2c_mutex[_index] = xSemaphoreCreateMutex();
#endif

	// initialize the GPIOs
	PINS_DRV_SetMuxModeSel(g_comm_config[_index].port_, g_comm_config[_index].scl_pin_, g_comm_config[_index].gpio_af_);
	PINS_DRV_SetMuxModeSel(g_comm_config[_index].port_, g_comm_config[_index].sda_pin_, g_comm_config[_index].gpio_af_);

    /* Initialize LPI2C Master configuration:
       - Slave address 0x50
       - Fast operating mode, 400 KHz SCL frequency
       - See LPI2C components for configuration details */
	g_config[_index]->baudRate    = _baudrate;
	g_config[_index]->is10bitAddr = _is_10bit_addr;
	LPI2C_DRV_MasterInit(g_handle[_index], g_config[_index], &g_state[_index]);

#if defined USING_OS_FREERTOS
	/* The interrupt calls an interrupt safe API function - so its priority must
	 * be equal to or lower than configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY. */
	for (uint8_t i = 0; i < sizeof(g_comm_config[_index].irqs_) && NotAvail_IRQn != g_comm_config[_index].irqs_[i]; i++)
	{
		INT_SYS_SetPriority( g_comm_config[_index].irqs_[i], configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
	}	
#endif

	return 0;
}

int32_t i2c_master_deinit(const uint8_t _index)
{
	assert(I2C0_INDEX >= _index);

	LPI2C_DRV_MasterDeinit(g_handle[_index]);
	PINS_DRV_SetMuxModeSel(g_comm_config[_index].port_, g_comm_config[_index].scl_pin_, PORT_PIN_DISABLED);
	PINS_DRV_SetMuxModeSel(g_comm_config[_index].port_, g_comm_config[_index].sda_pin_, PORT_PIN_DISABLED);
	
#if defined USING_OS_FREERTOS
	vSemaphoreDelete(g_i2c_mutex[_index]);
#endif

	return 0;
}

int32_t i2c_master_receive(const uint8_t _index, const uint16_t _addr, uint8_t *const _buf, const uint16_t _size, const bool _stop)
{
	assert(I2C0_INDEX >= _index && NULL != _buf);

	LPI2C_DRV_MasterSetSlaveAddr(g_handle[_index], _addr, g_config[_index]->is10bitAddr);

	if (STATUS_SUCCESS != LPI2C_DRV_MasterReceiveData(g_handle[_index], _buf, _size > 256 ? 256 : _size, _stop))
	{
		return -1;
	}

	uint32_t bytes = 0;
	status_t status;

	while (STATUS_BUSY == (status = LPI2C_DRV_MasterGetTransferStatus(g_handle[_index], &bytes))) {}

	if (STATUS_SUCCESS != status)
	{
		return -1;
	}

	return 0;
}

int32_t i2c_master_transmit(const uint8_t _index, const uint16_t _addr, const uint8_t *const _buf, const uint16_t _size, const bool _stop)
{
	assert(I2C0_INDEX >= _index && NULL != _buf);

	LPI2C_DRV_MasterSetSlaveAddr(g_handle[_index], _addr, g_config[_index]->is10bitAddr);

	if (STATUS_SUCCESS != LPI2C_DRV_MasterSendData(g_handle[_index], _buf, _size, _stop))
	{
		return -1;
	}
		
	uint32_t bytes = 0;
	status_t status;

	while (STATUS_BUSY == (status = LPI2C_DRV_MasterGetTransferStatus(g_handle[_index], &bytes))) {}

	if (STATUS_SUCCESS != status)
	{
		return -1;
	}
		
	OSIF_TimeDelay(10);

	return 0;
}

/******************************************************************************
 * Local functions
 ******************************************************************************/
