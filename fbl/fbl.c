#include <string.h>
#include <assert.h>

#include "board.h"
#include "uart.h"
#include "can.h"
#include "i2c.h"
#include "timer.h"
#include "flash_ctrl.h"
#include "diag/diag_server.h"
#include "resolve/gen_can_matrix.h"
#include "resolve/resolve.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#if defined S32K144
#if !defined __CORTEX_M
/**
 * @name CMSIS_CORE(transplant).
 * @{
 */
/* CONTROL Register Definitions */
#define CONTROL_FPCA_Pos                    2U                                            /*!< CONTROL: FPCA Position */
#define CONTROL_FPCA_Msk                   (1UL << CONTROL_FPCA_Pos)                      /*!< CONTROL: FPCA Mask */

#define CONTROL_SPSEL_Pos                   1U                                            /*!< CONTROL: SPSEL Position */
#define CONTROL_SPSEL_Msk                  (1UL << CONTROL_SPSEL_Pos)                     /*!< CONTROL: SPSEL Mask */

#define CONTROL_nPRIV_Pos                   0U                                            /*!< CONTROL: nPRIV Position */
#define CONTROL_nPRIV_Msk                  (1UL /*<< CONTROL_nPRIV_Pos*/)                 /*!< CONTROL: nPRIV Mask */

#if defined __CC_ARM
#define __STATIC_FORCEINLINE                static __forceinline
#elif defined __GNUC__
#define __STATIC_FORCEINLINE                __attribute__((always_inline)) static inline
#else
#error Unknown compiler!!!
#endif

__STATIC_FORCEINLINE void __set_MSP(uint32_t topOfProcStack);
__STATIC_FORCEINLINE uint32_t __get_CONTROL(void);
__STATIC_FORCEINLINE void __set_CONTROL(uint32_t control);

#define NVIC                                S32_NVIC
#define SysTick                             S32_SysTick
#define CTRL                                CSR
#define SCB                                 S32_SCB
#define SCB_ICSR_PENDSTCLR_Msk              S32_SCB_ICSR_PENDSTCLR_MASK
#define SCB_SHCSR_USGFAULTENA_Msk           S32_SCB_SHCSR_USGFAULTENA_MASK
#define SCB_SHCSR_BUSFAULTENA_Msk           S32_SCB_SHCSR_BUSFAULTENA_MASK
#define SCB_SHCSR_MEMFAULTENA_Msk           S32_SCB_SHCSR_MEMFAULTENA_MASK
/** @} */ /* CMSIS_CORE(transplant). */
#endif
#endif

#if defined __CC_ARM
void __svc( 0 ) EnablePrivilegedMode( void );
#elif defined __GNUC__
#define EnablePrivilegedMode() __asm("SVC #0")
#else
#error Unknown compiler!!!
#endif

/** 
 * @name Boot configuration.
 * @{
 */
#define FLASH_APP_BASE_ADDR   	                (FLASH_BASE_ADDR + 0x10000) /**< Application base address. */
#if defined FLASH_SECTOR_SIZE
#define CODE_BUFFER_SIZE                        FLASH_SECTOR_SIZE
#else
#define CODE_BUFFER_SIZE                        4096                        /**< 4k. */
#endif
/** @} */ /* Boot configuration. */

/** 
 * @name Transfer data protocal.
 * @{
 */	
#define TRANSFER_RE_SEND                        0x41
#define TRANSFER_ACK                            0x45
#define TRANSFER_BREAK                          0x46
#define TRANSFER_BLOCK_SIZE                     512
/** @} */ /* Transfer data protocal. */

/** 
 * @name Srec phrase index.
 * @{
 */
#define S_INDEX                                 0
#define TYPE_INDEX         		                1
#define BYTE_COUNT_INDEX                        2
#define ADDRESS_INDEX                           3
/** @} */ /* Srec phrase index. */

/** 
 * @name Diagnostic server.
 * @{
 */
static IsoTpLink      g_server_link_phy;                       /**< Server ISOTP link for physical addressing. */
static uint8_t        g_server_rx_buf_phy[DIAG_MAX_BUF_SIZE];  /**< Server receive buffer for physical addressing. */
static uint8_t        g_server_tx_buf_phy[DIAG_MAX_BUF_SIZE];  /**< Server transmit buffer for physical addressing. */
static IsoTpLink      g_server_link_func;                      /**< Server ISOTP link for functional addressing. */
static uint8_t        g_server_rx_buf_func[DIAG_MAX_BUF_SIZE]; /**< Server receive buffer for functional addressing. */
static uint8_t        g_server_tx_buf_func[DIAG_MAX_BUF_SIZE]; /**< Server transmit buffer for functional addressing. */
/** @} */ /* Diagnostic server. */

static bool           g_finished = false;                      /**< Flag indicate update finished. */
static uint8_t        g_code_buf[CODE_BUFFER_SIZE + 100];      /**< Code buffer for update from UART. */
static const uint32_t g_filter_id_list[] =                     /**< CAN filter id list. */
{
#if defined CAN0_MSG_ID_LIST
	CAN0_MSG_ID_LIST,
#endif
#if defined CAN1_MSG_ID_LIST
	CAN1_MSG_ID_LIST,
#endif
	FUNCTION_ID,
	SERVER_RX_ID,
};

/*******************************************************************************
 * Local Function prototypes
 ******************************************************************************/
/**
 * @name Callbacks.
 * @{
 */
static uint8_t server_receive_callback(uint32_t *const _id, uint8_t *const _buf, uint8_t _size);
static int32_t transmit_callback(const uint32_t _id, const uint8_t *const _buf, const uint8_t _size);
/** @} */ /* Callbacks. */

static void update_by_uart(void);
static void update_by_can(void);
static void jump_to_app(uint32_t *_addr);
//static void hexstr2bytes(const uint8_t *const _src, const uint16_t _len, uint8_t *const _dst);
//static void test_flash(void);

/*******************************************************************************
 * Functions
 ******************************************************************************/
//#pragma GCC push_options
//#pragma GCC optimize("O0")
/**
 * The flash boot loader function for the project.
 */
void fbl(void)
{
	uint8_t  reset_type = 0xFF;

	/* Initialize system, GPIO, UART... */
	sys_init();
	gpio_init();
	i2c_master_init(I2C0_INDEX, 400000, false);
	timer_init(TIMER0_INDEX, 10);
	flash_ctrl_init();

	GPIO_WRITE_PIN(LED0_GPIO, LED0_PIN, LED_ON);
	GPIO_WRITE_PIN(LED1_GPIO, LED1_PIN, LED_ON);
	GPIO_WRITE_PIN(LED2_GPIO, LED2_PIN, LED_ON);
	debug("FBL ENTRY\n");

	/* Check initialization flag */
	eeprom_read(EEPROM_ADDR_INIT, &reset_type, EEPROM_SIZE_INIT);
	print_buf("FBL READ EEPROM", EEPROM_ADDR_INIT, &reset_type, EEPROM_SIZE_INIT);
	if(1 != reset_type)
	{
		uint32_t ecumd = ECUMD;
		uint8_t  fp1[EEPROM_SIZE_FP1] = {FP1};
		uint8_t  fp2[EEPROM_SIZE_FP2] = {FP2};
		uint8_t  eol[EEPROM_SIZE_EOL] = {EOL};
		uint8_t  dtc[EEPROM_SIZE_DTC];

		/* Initialize read/write data */
		eeprom_write(EEPROM_ADDR_SSECUHWVN, (uint8_t*)SSECUHWVN, strlen(SSECUHWVN));
		print_buf("FBL WRITE EEPROM", EEPROM_ADDR_SSECUHWVN, (uint8_t*)SSECUHWVN, strlen(SSECUHWVN));
		eeprom_write(EEPROM_ADDR_VIN, (uint8_t*)VIN, strlen(VIN));
		print_buf("FBL WRITE EEPROM", EEPROM_ADDR_VIN, (uint8_t*)VIN, strlen(VIN));
		eeprom_write(EEPROM_ADDR_ECUMD, (uint8_t*)(&ecumd), EEPROM_SIZE_ECUMD);
		print_buf("FBL WRITE EEPROM", EEPROM_ADDR_ECUMD, (uint8_t*)(&ecumd), EEPROM_SIZE_ECUMD);
		eeprom_write(EEPROM_ADDR_TBTN, (uint8_t*)TBTN, strlen(TBTN));
		print_buf("FBL WRITE EEPROM", EEPROM_ADDR_TBTN, (uint8_t*)TBTN, strlen(TBTN));
		eeprom_write(EEPROM_ADDR_IMEI, (uint8_t*)IMEI, strlen(IMEI));
		print_buf("FBL WRITE EEPROM", EEPROM_ADDR_IMEI, (uint8_t*)IMEI, strlen(IMEI));
		eeprom_write(EEPROM_ADDR_ICCID, (uint8_t*)ICCID, strlen(ICCID));
		print_buf("FBL WRITE EEPROM", EEPROM_ADDR_ICCID, (uint8_t*)ICCID, strlen(ICCID));
		eeprom_write(EEPROM_ADDR_PK, (uint8_t*)PK, strlen(PK));
		print_buf("FBL WRITE EEPROM", EEPROM_ADDR_PK, (uint8_t*)PK, strlen(PK));
		eeprom_write(EEPROM_ADDR_FP1, fp1, EEPROM_SIZE_FP1);
		print_buf("FBL WRITE EEPROM", EEPROM_ADDR_FP1, fp1, EEPROM_SIZE_FP1);
		eeprom_write(EEPROM_ADDR_FP2, fp2, EEPROM_SIZE_FP2);
		print_buf("FBL WRITE EEPROM", EEPROM_ADDR_FP2, fp2, EEPROM_SIZE_FP2);
		eeprom_write(EEPROM_ADDR_EOL, eol, EEPROM_SIZE_EOL);
		print_buf("FBL WRITE EEPROM", EEPROM_ADDR_EOL, eol, EEPROM_SIZE_EOL);

		/* Initialize DTC */
		eeprom_write(EEPROM_ADDR_DTC, dtc, EEPROM_SIZE_DTC);
		print_buf("FBL WRITE EEPROM", EEPROM_ADDR_DTC, dtc, EEPROM_SIZE_DTC);

		/* Initialize reset type */
		reset_type = 0xFF;
		eeprom_write(EEPROM_ADDR_RESET_TYPE, &reset_type, EEPROM_SIZE_RESET_TYPE);
		print_buf("FBL WRITE EEPROM", EEPROM_ADDR_RESET_TYPE, &reset_type, EEPROM_SIZE_RESET_TYPE);

		/* Write initialization flag */
		reset_type = 1;
		eeprom_write(EEPROM_ADDR_INIT, &reset_type, EEPROM_SIZE_INIT);
		print_buf("FBL WRITE EEPROM", EEPROM_ADDR_INIT, &reset_type, EEPROM_SIZE_INIT);
	}

	/* Check reset type */
	reset_type = 0xFF;
	eeprom_read(EEPROM_ADDR_RESET_TYPE, &reset_type, EEPROM_SIZE_RESET_TYPE);
	print_buf("FBL READ EEPROM", EEPROM_ADDR_RESET_TYPE, &reset_type, EEPROM_SIZE_RESET_TYPE);
	if('U' == reset_type)
		update_by_uart();
	else if('C' == reset_type)
	{
		reset_type = 0xFF;
		eeprom_write(EEPROM_ADDR_RESET_TYPE, &reset_type, EEPROM_SIZE_RESET_TYPE);
		print_buf("FBL WRITE EEPROM", EEPROM_ADDR_RESET_TYPE, &reset_type, EEPROM_SIZE_RESET_TYPE);
		update_by_can();
	}
	else
	{
#if defined MX_TB
		/* Power on upper computer(EC20) */
		GPIO_WRITE_PIN(UC_POWER_GPIO, UC_POWER_PIN, 1);
		/* Delay 600ms */
		for(uint8_t i = 0; i < 60; i++)
		{
			delay(10);
		}
	    GPIO_WRITE_PIN(UC_POWER_GPIO, UC_POWER_PIN, 0);
#endif
	}

	debug("FBL jump to application\n");
	jump_to_app((uint32_t *)FLASH_APP_BASE_ADDR);
}
//#pragma GCC pop_options

#if defined S32K144
#if !defined __CORTEX_M
/**
 * @brief   Set Main Stack Pointer.
 *
 * @details Assigns the given value to the Main Stack Pointer (MSP).
 * @param   [in] topOfMainStack  Main Stack Pointer value to set.
 */
__STATIC_FORCEINLINE void __set_MSP(uint32_t topOfMainStack)
{
	__asm volatile ("MSR msp, %0" : : "r" (topOfMainStack) : );
}

/**
 * @brief   Get Control Register.
 *
 * @details Returns the content of the Control Register.
 * @return  Control Register value.
 */
__STATIC_FORCEINLINE uint32_t __get_CONTROL(void)
{
	uint32_t result;

	__asm volatile ("MRS %0, control" : "=r" (result) );
	return(result);
}

/**
 * @brief   Set Control Register.
 *
 * @details Writes the given value to the Control Register.
 * @param   [in] control  Control Register value to set.
 */
__STATIC_FORCEINLINE void __set_CONTROL(uint32_t control)
{
	__asm volatile ("MSR control, %0" : : "r" (control) : "memory");
}
#endif
#endif

/**
 * @brief SVC handler main.
 */
void SVC_Handler_Main( unsigned int *svc_args )
{
  unsigned int svc_number;

  /*
  * Stack contains:
  * r0, r1, r2, r3, r12, r14, the return address and xPSR
  * First argument (r0) is svc_args[0]
  */
  svc_number = ( ( char * )svc_args[ 6 ] )[ -2 ] ;
  switch( svc_number )
  {
    case 0:  /* EnablePrivilegedMode */
      __set_CONTROL( __get_CONTROL( ) & ~CONTROL_nPRIV_Msk ) ;
      break;
    default:    /* unknown SVC */
      break;
  }
}

/**
 * @brief SVC handler.
 */
#if defined __CC_ARM
__asm void SVC_Handler(void)
{
  IMPORT SVC_Handler_Main
  TST lr, #4
  ITE EQ
  MRSEQ r0, MSP
  MRSNE r0, PSP
  B SVC_Handler_Main
}
#elif defined __GNUC__
void SVC_Handler(void)
{
  __asm(
    ".global SVC_Handler_Main\n"
    "TST lr, #4\n"
    "ITE EQ\n"
    "MRSEQ r0, MSP\n"
    "MRSNE r0, PSP\n"
    "B SVC_Handler_Main\n"
  ) ;
}
#else
#error Unknown compiler!!!
#endif

/**
 * @brief Overwrite timer IRQ callback.
 *
 * @param _index Timer index.
 */
void timer_irq_callback(const uint8_t _index)
{
	if(TIMER0_INDEX == _index)
	{
		static uint32_t count = 0;
		count++;
		if(count >= 500)
		{
			timer_stop(TIMER0_INDEX);
			count = 0u;
			g_finished = true;
		}
	}
}

/**
 * @name Overwrite diagnostic server callbacks.
 * @{
 */
/**
 * @brief Overwrite reset system callback¡£
 */
void diag_server_sys_reset_callback(void)
{
	sys_reset();
}

/**
 * @brief  Overwrite erase continuous sectors callback.
 *
 * @param  [in] _addr Sector start address.
 * @param  [in] _size Erase size aligned to sector.
 * @return Success(0) or failure(other values).
 */
int32_t  diag_server_flash_erase_sectors_callback(const uint32_t _addr, const uint32_t _size)
{
	return flash_ctrl_erase_sector(_addr, _size);
}

/**
 * @brief  Overwrite verify continuous sectors callback.
 *
 * @param  [in] _addr Sector start address.
 * @param  [in] _size Erase size aligned to sector.
 * @return Success(0) or failure(other values).
 */
int32_t  diag_server_flash_verify_sectors_callback(const uint32_t _addr, const uint32_t _size)
{
	return flash_ctrl_verify_sector(_addr, _size);
}

/**
 * @brief  Overwrite program flash memory callback.
 *
 * @param  [in] _addr Program start address.
 * @param  [in] _size Program size.
 * @param  [in] _buf  Program buffer.
 * @return Success(0) or failure(other values).
 */
int32_t  diag_server_flash_program_callback(const uint32_t _addr, const uint32_t _size, const uint8_t *const _buf)
{
	return flash_ctrl_program(_addr, _size, _buf);
}

/**
 * @brief  Overwrite verify programed flash memory callback.
 *
 * @param  [in] _addr Verify start address.
 * @param  [in] _size Verify size.
 * @param  [in] _buf  Verify buffer.
 * @return Success(0) or failure(other values).
 */
int32_t  diag_server_flash_program_verify_callback(const uint32_t _addr, const uint32_t _size, const uint8_t *const _buf)
{
	return flash_ctrl_program_verify(_addr, _size, _buf);
}
/** @} */ /* Overwrite diagnostic server callbacks. */

/*******************************************************************************
 * Local Functions
 ******************************************************************************/
/**
 * @brief  Receive callback function.
 *
 * @param  [out] _id   ID.
 * @param  [out] _buf  Receive buffer.
 * @param  [in]  _size Receive size.
 * @return Received size.
 */
static uint8_t server_receive_callback(uint32_t *const _id, uint8_t *const _buf, uint8_t _size)
{
	assert(NULL != _id && NULL != _buf);

	uint8_t size = 0;

	if(0 < (size = can_receive(CAN0_INDEX, _id, _buf, _size)))
		print_buf("CAN RX", *_id, _buf, size);

	return size;
}

/**
 * @brief  Transmit callback function.
 *
 * @param  [in] _id   ID.
 * @param  [in] _buf  Transmit buffer.
 * @param  [in] _size Transmit size.
 * @return Success(0) or failure(other values).
 */
static int32_t transmit_callback(const uint32_t _id, const uint8_t *const _buf, const uint8_t _size)
{
	assert(NULL != _buf);

	if(SERVER_TX_ID != _id && !diag_server_comm_ctrl_tx_enabled(COMM_MSG_TYPE_MASK_NCM))
		return -1;
	if(0 == can_send(CAN0_INDEX, _id, _buf, _size))
		return -1;
	print_buf("CAN TX", _id, _buf, _size);

	return 0;
}

/**
 * @brief Update by UART.
 */
static void update_by_uart(void)
{
	uint32_t address = 0;
	
	/* Initialize UART */
	uart_init(UART0_INDEX, 115200, UART_DATA_BITS_8, UART_STOP_BITS_1, UART_PARITY_MODE_NONE);

	/* Request update */
	*((uint16_t*)g_code_buf) = UPDATE_NOTIFY_ID;
	*((uint16_t*)(g_code_buf + ID_LENGTH)) = TRANSFER_BLOCK_SIZE;
	uart_send_with_header(UART0_INDEX, g_code_buf, ID_LENGTH + 2);
	print_buf("FBL UART TX", 0, g_code_buf, ID_LENGTH + 2);

	/* Start waiting for transfer data */
	address = FLASH_APP_BASE_ADDR;
	memset(g_code_buf, 0xFF, sizeof(g_code_buf));
	debug("FBL start waiting for transfer data...\n");
	timer_start(TIMER0_INDEX);
	
	while(!g_finished)
	{
		static uint32_t addr = 0;
		uint8_t         buf[UART_BUFFER_SIZE];
		uint16_t        size = 0;

		/* Receive data from UART */
		if(0 == (size = uart_receive_with_header_poll(UART0_INDEX, buf, sizeof(buf))))
			continue;
		debug("FBL UART RX %d bytes\n", size);

		/* Check if the received data is transfer data */
		if(TRANSFER_DATA_ID != *((uint16_t*)buf))
			continue;

		timer_stop(TIMER0_INDEX);
		g_finished = false;
		memcpy(g_code_buf + addr,  buf + ID_LENGTH, size - ID_LENGTH);
		addr += size - ID_LENGTH;

		if(CODE_BUFFER_SIZE <= addr)
		{
			if(flash_ctrl_is_sector_aligned(address))
			{
				/* Erase and verify sectors */
				flash_ctrl_erase_sector(address, CODE_BUFFER_SIZE);
				flash_ctrl_verify_sector(address, CODE_BUFFER_SIZE);
			}

			/* Program and verify */
			flash_ctrl_program(address, CODE_BUFFER_SIZE, g_code_buf);
			flash_ctrl_program_verify(address, CODE_BUFFER_SIZE, g_code_buf);
			address += CODE_BUFFER_SIZE;
			addr -= CODE_BUFFER_SIZE;
			memset(g_code_buf, 0xFF, CODE_BUFFER_SIZE);
			memcpy(g_code_buf, g_code_buf + CODE_BUFFER_SIZE, addr);
		}
		else if(ID_LENGTH + TRANSFER_BLOCK_SIZE > size )
		{
			if(flash_ctrl_is_sector_aligned(address))
			{
				/* Erase and verify sectors */
				flash_ctrl_erase_sector(address, CODE_BUFFER_SIZE);
				flash_ctrl_verify_sector(address, CODE_BUFFER_SIZE);
			}
			
			/* Program and verify */
			flash_ctrl_program(address, CODE_BUFFER_SIZE, g_code_buf);
			flash_ctrl_program_verify(address, CODE_BUFFER_SIZE, g_code_buf);
			g_finished = true;
			uint8_t reset_type = 0xFF;
			eeprom_write(EEPROM_ADDR_RESET_TYPE, &reset_type, EEPROM_SIZE_RESET_TYPE);
			print_buf("FBL WRITE EEPROM", EEPROM_ADDR_RESET_TYPE, &reset_type, EEPROM_SIZE_RESET_TYPE);
		}
#if 0 
		/* Receiving srec string */
		static bool     phrase_started = false;
		uint32_t        addr = 0;
		uint32_t        addr_len = 0;
		uint8_t         check_sum = 0;
		uint8_t         buf[100];
		uint8_t         temp_buf[100];
		uint16_t        size = 0;

		/* Receive data from uart */
		if(0 == (size = uart_receive_with_header_poll(UART0_INDEX, temp_buf, sizeof(temp_buf))))
			continue;

		/* Check if the received data is phrase */
		if(TRANSFER_DATA_ID != *((uint16_t*)(temp_buf)) || 'S' != temp_buf[ID_LENGTH + S_INDEX])
			continue;

		timer_stop(TIMER0_INDEX);
		g_finished = false;

		/* Extract phrase */
		buf[S_INDEX] = temp_buf[ID_LENGTH + S_INDEX];
		buf[TYPE_INDEX] = temp_buf[ID_LENGTH + TYPE_INDEX];
		size -= ID_LENGTH + BYTE_COUNT_INDEX;
		hexstr2bytes((char*)(temp_buf + ID_LENGTH + BYTE_COUNT_INDEX), size, buf + BYTE_COUNT_INDEX);
		size = size / 2 + BYTE_COUNT_INDEX;
		print_buf("FBL UART RX", 0, buf, size);

		/* Calculate check sum */
		for(uint8_t i = BYTE_COUNT_INDEX; i < BYTE_COUNT_INDEX + buf[BYTE_COUNT_INDEX]; i++)
			check_sum += buf[i];

		/* Check */
		check_sum = ~check_sum;
		if(check_sum != buf[BYTE_COUNT_INDEX + buf[BYTE_COUNT_INDEX]])
		{
			/* Request re-send*/
			*((uint16_t*)buf) = TRANSFER_DATA_ID;
			temp_buf[ID_LENGTH] = TRANSFER_RE_SEND;
			uart_transmit_with_header(UART0_INDEX, temp_buf, ID_LENGTH + 1);
			print_buf("FBL UART TX", 0, temp_buf, ID_LENGTH + 1);
			continue;
		}

		/* Program routine */
		switch(buf[TYPE_INDEX])
		{
			case '0':
				phrase_started = false;
				address = 0;
				memset(g_code_buf, 0xFFu, sizeof(g_code_buf));
				break;
			case '1':
				addr_len = 2;
				goto CONTINUE;
			case '2':
				addr_len = 3;
				goto CONTINUE;
			case '3':
				addr_len = 4;
CONTINUE:
				for(uint8_t i = 0; i < addr_len; i++)
				{
					addr |= buf[ADDRESS_INDEX + i] << ((addr_len - i - 1) * 8);
				}

				if(!phrase_started)
				{
					address = addr;
					phrase_started = true;
				}

				if(CODE_BUFFER_SIZE <= (addr - address))
				{
					if(flash_ctrl_is_sector_aligned(address))
					{
						/* Erase and verify sectors */
						flash_ctrl_erase_sector(address, CODE_BUFFER_SIZE);
						flash_ctrl_verify_sector(address, CODE_BUFFER_SIZE);
					}
					
					/* Program and verify */
					flash_ctrl_program(address, CODE_BUFFER_SIZE, g_code_buf);
					flash_ctrl_program_verify(address, CODE_BUFFER_SIZE, g_code_buf);
					address += CODE_BUFFER_SIZE;
					memset(g_code_buf, 0xFF, CODE_BUFFER_SIZE);
					memcpy(g_code_buf, g_code_buf + CODE_BUFFER_SIZE, addr - address);
				}
				
				memcpy(g_code_buf + (addr - address),  buf + ADDRESS_INDEX + addr_len, buf[BYTE_COUNT_INDEX] - addr_len - 1);
				break;
			case '7':
			case '8':
			case '9':
				if(flash_ctrl_is_sector_aligned(address))
				{
					/* Erase and verify sectors */
					flash_ctrl_erase_sector(address, CODE_BUFFER_SIZE);
					flash_ctrl_verify_sector(address, CODE_BUFFER_SIZE);
					
				}
				
				/* Program and verify */
				flash_ctrl_program(address, CODE_BUFFER_SIZE, g_code_buf);
				flash_ctrl_program_verify(address, CODE_BUFFER_SIZE, g_code_buf);
				phrase_started = false;
				address = 0;
				memset(g_code_buf, 0xFF, sizeof(g_code_buf));
				g_finished = true;
				uint8_t reset_type = 0xFF;
				eeprom_write(EEPROM_ADDR_RESET_TYPE, &reset_type, EEPROM_SIZE_RESET_TYPE);
				print_buf("FBL WRITE EEPROM", EEPROM_ADDR_RESET_TYPE, &reset_type, EEPROM_SIZE_RESET_TYPE);
				break;
			default:
				break;
		}
#endif
		GPIO_TOGGLE_PIN(LED0_GPIO, LED0_PIN);
		GPIO_TOGGLE_PIN(LED1_GPIO, LED1_PIN);
		GPIO_TOGGLE_PIN(LED2_GPIO, LED2_PIN);

		/* ACK */
		*((uint16_t*)buf) = TRANSFER_DATA_ID;
		buf[ID_LENGTH] = TRANSFER_ACK;
		uart_send_with_header(UART0_INDEX, buf, ID_LENGTH + 1);
		print_buf("FBL UART TX", 0, buf, ID_LENGTH + 1);
	}
	
	uart_deinit(UART0_INDEX);
}

/**
 * @brief Update by CAN.
 */
static void update_by_can(void)
{
	uint32_t id = 0;
	uint8_t  buf[8];
	uint8_t  size = 0;
	
	/* Initialize CAN */
    can_init(CAN0_INDEX, g_filter_id_list, sizeof(g_filter_id_list) / sizeof(uint32_t));

	/* Initialize diagnostic server */
	diag_server_init(&g_server_link_phy, SERVER_TX_ID, SERVER_RX_ID,
			         g_server_tx_buf_phy, sizeof(g_server_tx_buf_phy),
		             g_server_rx_buf_phy, sizeof(g_server_rx_buf_phy),
			         transmit_callback, server_receive_callback, sys_time, debug);
	diag_server_init(&g_server_link_func, SERVER_TX_ID, FUNCTION_ID,
			         g_server_tx_buf_func, sizeof(g_server_tx_buf_func),
			         g_server_rx_buf_func, sizeof(g_server_rx_buf_func),
			         transmit_callback, server_receive_callback, sys_time, debug);
 	diag_server_data_init();

	timer_start(TIMER0_INDEX);

	while(!g_finished)
	{
		if(0 == (size = can_receive(CAN0_INDEX, &id, buf, sizeof(buf))))
			continue;
		print_buf("CAN RX", id, buf, size);
		
		if(SERVER_RX_ID == id)
		{
			timer_stop(TIMER0_INDEX);
			g_finished = false;
			/* Diagnostic message physical addressing */
			diag_server_indication(&g_server_link_phy, buf, size);
		}
		else if(FUNCTION_ID == id)
		{
			timer_stop(TIMER0_INDEX);
			g_finished = false;
			/* Diagnostic message functional addressing */
			diag_server_indication(&g_server_link_func, buf, size);
		}
		else
		{
			if(!diag_server_comm_ctrl_rx_enabled(COMM_MSG_TYPE_MASK_NCM))
			    continue;
			/* Notify DTC message received */
			diag_server_on_msg(id);
		}

		GPIO_TOGGLE_PIN(LED0_GPIO, LED0_PIN);
		GPIO_TOGGLE_PIN(LED1_GPIO, LED1_PIN);
		GPIO_TOGGLE_PIN(LED2_GPIO, LED2_PIN);
	}

	diag_server_deinit(&g_server_link_phy);
	diag_server_deinit(&g_server_link_func);
	can_deinit(CAN0_INDEX);
}

/**
 * @brief Jump to application.
 *
 * @param  [in] _buf Application address.
 */
static void jump_to_app(uint32_t *_addr)
{
	/* Make sure, the CPU is in privileged mode. */
	if( CONTROL_nPRIV_Msk & __get_CONTROL() )
		EnablePrivilegedMode();

	/* Disable all enabled interrupts in NVIC. */
	NVIC->ICER[0] = 0xFFFFFFFF ;
	NVIC->ICER[1] = 0xFFFFFFFF ;
	NVIC->ICER[2] = 0xFFFFFFFF ;
	NVIC->ICER[3] = 0xFFFFFFFF ;
	NVIC->ICER[4] = 0xFFFFFFFF ;
	NVIC->ICER[5] = 0xFFFFFFFF ;
	NVIC->ICER[6] = 0xFFFFFFFF ;
	NVIC->ICER[7] = 0xFFFFFFFF ;

	/* Disable all enabled peripherals which might generate interrupt requests,
	 * and clear all pending interrupt flags in those peripherals. */
	flash_ctrl_deinit();
	timer_deinit(TIMER0_INDEX);
	i2c_master_deinit(I2C0_INDEX);
	gpio_deinit();

	/* Clear all pending interrupt requests in NVIC. */
	NVIC->ICPR[ 0 ] = 0xFFFFFFFF ;
	NVIC->ICPR[ 1 ] = 0xFFFFFFFF ;
	NVIC->ICPR[ 2 ] = 0xFFFFFFFF ;
	NVIC->ICPR[ 3 ] = 0xFFFFFFFF ;
	NVIC->ICPR[ 4 ] = 0xFFFFFFFF ;
	NVIC->ICPR[ 5 ] = 0xFFFFFFFF ;
	NVIC->ICPR[ 6 ] = 0xFFFFFFFF ;
	NVIC->ICPR[ 7 ] = 0xFFFFFFFF ;

	/* Disable SysTick and clear its exception pending bit, if it is used in the fbl, e. g. by the RTX. */
	SysTick->CTRL = 0 ;
	SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk ;

	/* Disable individual fault handlers if the fbl used them. */
	SCB->SHCSR &= ~( SCB_SHCSR_USGFAULTENA_Msk | \
					 SCB_SHCSR_BUSFAULTENA_Msk | \
					 SCB_SHCSR_MEMFAULTENA_Msk ) ;

	/* Activate the MSP, if the core is found to currently run with the PSP. */
	if( CONTROL_SPSEL_Msk & __get_CONTROL( ) )
	{
		/* MSP is not active */
		__set_CONTROL( __get_CONTROL( ) & ~CONTROL_SPSEL_Msk ) ;
	}

	/* Load the vector table address of the user application into SCB->VTOR register.
	 * Make sure the address meets the alignment requirements. */
	SCB->VTOR = ( uint32_t )_addr ;

	/* Set the MSP to the value found in the user application vector table. */
	__set_MSP( _addr[ 0 ] ) ;

	/* Set the PC to the reset vector value of the user application via a function call. */
	( ( void ( * )( void ) )_addr[ 1 ] )( ) ;
}

#if 0
/**
 * @brief Convert hex string to bytes.
 *
 * @param [in]  _src Hex string.
 * @param [in]  _len Hex string length.
 * @param [out] _dst Bytes buffer.
 */
static void hexstr2bytes(const uint8_t *const _src, const uint16_t _len, uint8_t *const _dst)
{
	uint8_t high_byte = 0;
	uint8_t low_byte = 0;

	for (uint16_t i = 0; i < _len; i += 2)
	{
		high_byte = toupper(_src[i]);
		if (i + 1 < _size)
			low_byte = toupper(_src[i + 1]);
		else
			low_byte = 0;

		if (high_byte > 0x39)
			high_byte -= 0x37;
		else
			high_byte -= 0x30;

		if (low_byte > 0x39)
			low_byte -= 0x37;
		else
			low_byte -= 0x30;

		_dst[i / 2] = (high_byte << 4) | low_byte;
	}
}

/**
 * @brief Test flash.
 */
static void test_flash(void)
{
	uint8_t buf[64];
	
	assert(0 == flash_ctrl_init());
	
	/* Erase and verify sectors */
	assert(0 == flash_ctrl_erase_sector(FLASH_APP_BASE_ADDR, CODE_BUFFER_SIZE));
	assert(0 == flash_ctrl_verify_sector(FLASH_APP_BASE_ADDR, CODE_BUFFER_SIZE));
	
	/*  Program and verify */
	memset(buf, 0xAA, sizeof(buf));
	assert(0 == flash_ctrl_program(FLASH_APP_BASE_ADDR, sizeof(buf), buf));
	assert(0 == flash_ctrl_program_verify(FLASH_APP_BASE_ADDR, sizeof(buf), buf));

    /* Write EEPROM */
	memset(buf, 0xBB, 4);
    assert(0 == flash_ctrl_write_e2(0, 4, buf));

    assert(0 == flash_ctrl_deinit());
}
#endif
