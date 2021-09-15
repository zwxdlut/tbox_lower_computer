#include <string.h>
#include <assert.h>

/* Kernel includes */
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "timers.h"

#include "board.h"
#include "uart.h"
#include "can.h"
#include "i2c.h"
#include "diag/diag_server.h"
#include "diag/diag_client.h"
#include "resolve/resolve.h"
#include "resolve/gen_can_matrix.h"
#include "bitfield/bitfield.h"
#include "nm/osek_nm.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#if defined UDEBUG
#define MINIMAL_STACK_SIZE                      (2 * configMINIMAL_STACK_SIZE)
#else
#define MINIMAL_STACK_SIZE                      configMINIMAL_STACK_SIZE
#endif

#define SLEEP_TIME                              30000 	                     /**< Sleep detect time threshold in milliseconds. */
#define TX_UPSTREAM_INTERVAL                    1000 	                     /**< Transmit upstream data interval in milliseconds. */
#define HEARTBEAT_INTERVAL                      20000                        /**< Heartbeat time interval in milliseconds. */

/**
 * @name Transfer data protocal.
 * @{
 */
#define TRANSFER_RE_SEND                        0x41
#define TRANSFER_ACK                            0x45
#define TRANSFER_BREAK                          0x46
/** @} */ /* Transfer data protocal. */

/**
 * @name Diagnostic server.
 * @{
 */
static IsoTpLink         g_server_link_phy;                                 /**< Server ISOTP link for physical addressing. */
static uint8_t           g_server_rx_buf_phy[DIAG_MAX_BUF_SIZE];            /**< Server Receive buffer for physical addressing. */
static uint8_t           g_server_tx_buf_phy[DIAG_MAX_BUF_SIZE];            /**< Server Transmit buffer for physical addressing. */
static IsoTpLink         g_server_link_func;                                /**< Server ISOTP link for functional addressing. */
static uint8_t           g_server_rx_buf_func[DIAG_MAX_BUF_SIZE];           /**< Server Receive buffer for functional addressing. */
static uint8_t           g_server_tx_buf_func[DIAG_MAX_BUF_SIZE];           /**< Server Transmit buffer for functional addressing. */
static QueueHandle_t     g_server_rx_queue = NULL;                          /**< Server receive queue */
/** @} */ /* Diagnostic server. */

/**
 * @name Diagnostic client.
 * @{
 */
static IsoTpLink         g_client_link;                                     /**< Client ISOTP link for physical addressing. */
static uint8_t           g_client_rx_buf[DIAG_MAX_BUF_SIZE];                /**< Client Receive buffer for physical addressing. */
static uint8_t           g_client_tx_buf[DIAG_MAX_BUF_SIZE];                /**< Client Transmit buffer for physical addressing. */
static QueueHandle_t     g_client_rx_queue = NULL;                          /**< Client receive queue */
/** @} */ /* Diagnostic client. */

static uint32_t          g_sleep_time = 0;                                   /**< Sleep detection time. */
static SemaphoreHandle_t g_sleep_mutex = NULL;                            	 /**< Sleep detection mutex. */
static bool              g_heartbeat_actived = false;                        /**< Heartbeat activated flag. */
static uint32_t          g_heartbeat_time = 0;                               /**< Heartbeat time. */
static SemaphoreHandle_t g_heartbeat_mutex = NULL;                           /**< Heartbeat mutex. */
static TimerHandle_t     g_tx_timer;                                         /**< Transmit timer. */
static SemaphoreHandle_t g_tx_timer_mutex;                                   /**< Transmit timer mutex. */
static const uint32_t    g_filter_id_list[] =                                /**< CAN filter id list. */
{
#if defined CAN0_MSG_ID_LIST
	CAN0_MSG_ID_LIST,
#endif
#if defined CAN1_MSG_ID_LIST
	CAN1_MSG_ID_LIST,
#endif
	FUNCTION_ID,
	SERVER_RX_ID,
	CLIENT_RX_ID
};

/*******************************************************************************
 * Local Function prototypes
 ******************************************************************************/
/** 
 * @name Tasks.
 * @{
 */
static void daemon_task( void *pvParameters );
static void can_receive_task( void *pvParameters );
static void downstream_receive_task( void *pvParameters );
static void diag_server_task( void *pvParameters );
static void normal_task( void *pvParameters );
/** @} */ /* Tasks. */

/** 
 * @name Callbacks.
 * @{
 */
static void timer_callback( TimerHandle_t xTimer );
static uint8_t server_receive_callback(uint32_t *const _id, uint8_t *const _buf, const uint8_t _size);
static uint8_t client_receive_callback(uint32_t *const _id, uint8_t *const _buf, const uint8_t _size);
static int32_t transmit_callback(const uint32_t _id, const uint8_t *const _buf, const uint8_t _size);
/** @} */ /* Callbacks. */

static void init( void );

/*******************************************************************************
 * Functions
 ******************************************************************************/
void rtos_start( void )
{
	/* Initialization */
	init();
			
	/* Start the four tasks as described in the comments at the top of this file */
	xTaskCreate( daemon_task, "daemon", MINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3, NULL );
	xTaskCreate( can_receive_task, "can0_receive", MINIMAL_STACK_SIZE, ( void * ) CAN0_INDEX, tskIDLE_PRIORITY + 2, NULL );
	xTaskCreate( can_receive_task, "can1_receive", MINIMAL_STACK_SIZE, ( void * ) CAN1_INDEX, tskIDLE_PRIORITY + 2, NULL );
	xTaskCreate( downstream_receive_task, "downstream_receive", MINIMAL_STACK_SIZE, ( void * ) UART0_INDEX, tskIDLE_PRIORITY + 2, NULL );
	xTaskCreate( diag_server_task, "diag_server", MINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL );
	xTaskCreate( normal_task, "normal", MINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
	
	/* Create transmit timer */
	g_tx_timer
		= xTimerCreate( "tx_timer",              /* A text name, purely to help debugging. */
		    pdMS_TO_TICKS(10),                   /* The timer period. */
			pdTRUE,						         /* This is a repeatedly timer, so xAutoReload is set to pdTRUE. */
			( void * ) 0,					     /* The ID is not used, so can be set to anything. */
			timer_callback                       /* The callback function . */
		);

	/* Start the tasks and timer running */
	xTimerReset(g_tx_timer, 0);
	vTaskStartScheduler();

	/* If all is well, the scheduler will now be running, and the following line
	will never be reached.  If the following line does execute, then there was
	insufficient FreeRTOS heap memory available for the idle and/or timer tasks
	to be created.  See the memory management section on the FreeRTOS web site
	for more details. */
	for( ;; );
}

/**
 * @name Overwrite diagnostic server callbacks.
 * @{
 */
/**
 * @brief Overwrite reset system callback。
 */
void diag_server_sys_reset_callback(void)
{
	sys_reset();
}

/**
 * @brief  Overwrite get power voltage callback。
 *
 * @return Power voltage in 0.1V.
 */
uint32_t diag_server_get_pwr_vol_callback(void)
{
	return (1.6 * resolve_get_upstream_item(BATTERY_VOL_START_BIT, BATTERY_VOL_BIT_LEN));
}

/**
 * @brief  Overwrite get vehicle speed callback。
 *
 * @return Vehicle speed in 0.1km/h。
 */
uint32_t diag_server_get_vehicle_speed_callback(void)
{
	return resolve_get_upstream_item(SPEED_START_BIT, SPEED_BIT_LEN);
}

/**
 * @brief  Overwrite get car status callback。
 *
 * @return Car status:
 * @retval CAR_STATUS_RUNNING: Running.
 * @retval CAR_STATUS_STOPPED: Stopped.
 * @retval CAR_STATUS_OTHER:   Other.
 */
uint32_t diag_server_get_car_status_callback(void)
{
	return resolve_get_upstream_item(CAR_STATUS_START_BIT, CAR_STATUS_BIT_LEN);
}
/** @} */ /* Overwrite diagnostic server callbacks. */

/*******************************************************************************
 * Local Functions
 ******************************************************************************/
/**
 * @brief   Daemon task.
 *
 * @details This task refreshes watch dog, check and performs sleep routine.
 *
 * @param   [in] pvParameters Parameters.
 */
static void daemon_task( void *pvParameters )
{
	(void)pvParameters;

	wdog_enable();
	
	while(1)
	{
		vTaskDelay(pdMS_TO_TICKS(1));
		wdog_refresh();

		/* Detect heartbeat */
		if(g_heartbeat_actived && (xTaskGetTickCount() * portTICK_PERIOD_MS - g_heartbeat_time >= HEARTBEAT_INTERVAL))
		{
#if defined MX_TB
			debug("Reset upper computer!\r\n");
			GPIO_WRITE_PIN(UC_RESET_GPIO, UC_RESET_PIN, 1);
			/* Delay 300ms */
			for(uint8_t i = 0; i < 30; i++)
			{
				vTaskDelay(pdMS_TO_TICKS(10));
				wdog_refresh();
			}
			GPIO_WRITE_PIN(UC_RESET_GPIO, UC_RESET_PIN, 0);
			xSemaphoreTake( g_heartbeat_mutex, portMAX_DELAY );
			g_heartbeat_actived = false;
			xSemaphoreGive( g_heartbeat_mutex );
#endif
		}

		/* Detect sleep */
		if(xTaskGetTickCount() * portTICK_PERIOD_MS - g_sleep_time >= SLEEP_TIME)
		{
			uint8_t reset_type = 0;

			/* Stop transmit timer */
			xSemaphoreTake( g_tx_timer_mutex, portMAX_DELAY );
			xTimerStop(g_tx_timer, 0);
			xSemaphoreGive( g_tx_timer_mutex );
			/* Check update */
			eeprom_read(EEPROM_ADDR_RESET_TYPE, &reset_type, EEPROM_SIZE_RESET_TYPE);
			print_buf("READ EEPROM", EEPROM_ADDR_RESET_TYPE, &reset_type, EEPROM_SIZE_RESET_TYPE);
			if('U' == reset_type)
			{
				/* Waiting until UART sending finished */
				for(uint16_t i = 0; i < 100; i++)
				{
					vTaskDelay(pdMS_TO_TICKS(10));
					wdog_refresh();
				}
				sys_reset();
			}
			debug("Go to sleep...\r\n");
			GPIO_WRITE_PIN(LED0_GPIO, LED0_PIN, LED_OFF);
			GPIO_WRITE_PIN(LED1_GPIO, LED1_PIN, LED_OFF);
			GPIO_WRITE_PIN(LED2_GPIO, LED2_PIN, LED_ON);
			/* Update heartbeat time */
			xSemaphoreTake( g_heartbeat_mutex, portMAX_DELAY );
			if(g_heartbeat_actived)
				g_heartbeat_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
			xSemaphoreGive( g_heartbeat_mutex );
#if defined MX_TB
			/* Control upper computer sleep */
			GPIO_WRITE_PIN(UC_WAKEUP_GPIO, UC_WAKEUP_PIN, 1);
			/* Waiting upper computer sleep */
			for(uint16_t i = 0; i < 200; i++)
			{
				vTaskDelay(pdMS_TO_TICKS(10));
				wdog_refresh();
			}
#endif
			accr_sys_mode_trans(ACCR_SYSMOD_STANDBY);
			can_pwr_mode_trans(CAN0_INDEX, CAN_PWR_MODE_SLEEP);
			can_pwr_mode_trans(CAN1_INDEX, CAN_PWR_MODE_SLEEP);
			pwr_mode_trans(PWR_MODE_DEEPSLEEP);
			can_pwr_mode_trans(CAN0_INDEX, CAN_PWR_MODE_RUN);
			can_pwr_mode_trans(CAN1_INDEX, CAN_PWR_MODE_RUN);
			accr_sys_mode_trans(ACCR_SYSMOD_ACTIVE);
#if defined MX_TB
			/* Wake up upper computer */
			GPIO_WRITE_PIN(UC_WAKEUP_GPIO, UC_WAKEUP_PIN, 0);
#endif
			/* Update heartbeat time */
			xSemaphoreTake( g_heartbeat_mutex, portMAX_DELAY );
			if(g_heartbeat_actived)
				g_heartbeat_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
			xSemaphoreGive( g_heartbeat_mutex );
			/* Update sleep time */
			xSemaphoreTake( g_sleep_mutex, portMAX_DELAY );
			g_sleep_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
			xSemaphoreGive( g_sleep_mutex );
			GPIO_WRITE_PIN(LED2_GPIO, LED2_PIN, LED_OFF);
			wdog_refresh();
			debug("Wake up\r\n");
			/* Reset transmit timer */
			xSemaphoreTake( g_tx_timer_mutex, portMAX_DELAY );
			xTimerReset(g_tx_timer, 0);
			xSemaphoreGive( g_tx_timer_mutex );
		}
	}
}

/**
 * @brief   CAN receive task.
 *
 * @details This task receives and handles CAN message.
 *
 * @param   [in] pvParameters Parameters.
 */
static void can_receive_task( void *pvParameters )
{
	uint32_t id = 0;
	uint8_t  buf[8];
	uint8_t  size = 0;

	while(1)
	{
		vTaskDelay(pdMS_TO_TICKS(1));

		if(0 == (size = can_receive((uint32_t)pvParameters, &id, buf, sizeof(buf))))
			continue;
		xSemaphoreTake( g_sleep_mutex, portMAX_DELAY );
		g_sleep_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
		xSemaphoreGive( g_sleep_mutex );

		print_buf("CAN RX", id, buf, size);

		/* Toggle LED */
		if(CAN0_INDEX == (uint32_t)pvParameters)
		{
			static uint32_t count = 0;
			if(count >= 100)
			{
				GPIO_TOGGLE_PIN(LED0_GPIO, LED0_PIN);
				count = 0;
			}
			count++;
		}
		else
		{
			static uint32_t count = 0;
			if(count >= 100)
			{
				GPIO_TOGGLE_PIN(LED1_GPIO, LED1_PIN);
				count = 0;
			}
			count++;
		}

		if(SERVER_RX_ID == id || FUNCTION_ID == id)
		{
			can_msg_t msg;

			msg.id_  = id;
			msg.dlc_ = size;
			memcpy(msg.data_, buf, size);
			xQueueSend( g_server_rx_queue, &msg, 0 );
		}
		else if(CLIENT_RX_ID == id)
		{
			can_msg_t msg;

			msg.id_  = id;
			msg.dlc_ = size;
			memcpy(msg.data_, buf, size);
			xQueueSend( g_client_rx_queue, &msg, 0 );
		}
		else if(TBOX_NM_ID_BASE_ADDRESS == (id & TBOX_NM_ID_BASE_ADDRESS) && diag_server_comm_ctrl_rx_enabled(COMM_MSG_TYPE_MASK_NWMCM))
			osek_nm_indication(id, buf);
		else
		{
			if(!diag_server_comm_ctrl_rx_enabled(COMM_MSG_TYPE_MASK_NCM))
			    continue;
			/* Notify message received */
			diag_server_on_msg(id);
			/* Resolve message */
			resolve_msg_to_upstream(id, buf, size);
		}
	}
}

/**
 * @brief   Downstream receive task.
 *
 * @details This task receives and handles downstream.
 *
 * @param   [in] pvParameters Parameters.
 */
static void downstream_receive_task( void *pvParameters )
{
	static uint8_t  buf[UART_RX_BUFFER_SIZE];
	uint16_t size = 0u;
	uint32_t fw_size = 0;
	uint32_t transfered_size = 0;
	uint16_t block_count = 0;
	uint32_t duration = 0;

	while(1)
	{
		vTaskDelay(pdMS_TO_TICKS(1));

		if(0 == (size = uart_receive_with_format_polling((uint32_t)pvParameters, buf, sizeof(buf))))
			continue;
		print_buf("UART RX", 0, buf, size);

		if(UPDATE_NOTIFY_ID == *((uint16_t*)buf))
		{
			uint8_t swvn[SSECUSWVN_LEN];
			diag_server_read_data_by_id(SSECUSWVNDID, swvn, SSECUSWVN_LEN);
			if(0 == memcmp(buf + ID_LENGTH, swvn, (size - ID_LENGTH) < strlen((char*)swvn) ? (size - ID_LENGTH) : strlen((char*)swvn)))
			{
				eeprom_write(EEPROM_ADDR_RESET_TYPE, (uint8_t *)"U", EEPROM_SIZE_RESET_TYPE);
				print_buf("WRITE EEPROM", EEPROM_ADDR_RESET_TYPE, (uint8_t *)"U", EEPROM_SIZE_RESET_TYPE);
			}
		}
		else if(REQ_SW_NO_ID == *((uint16_t*)buf))
		{
			diag_server_read_data_by_id(SSECUSWVNDID, buf + ID_LENGTH, SSECUSWVN_LEN);
			uart_send_with_format((uint32_t)pvParameters ,buf, ID_LENGTH + SSECUSWVN_LEN);
			print_buf("UART TX", 0, buf, ID_LENGTH + SSECUSWVN_LEN);
		}
		else if(HEARTBEAT_ID == *((uint16_t*)buf))
		{
			xSemaphoreTake( g_heartbeat_mutex, portMAX_DELAY );
			if(!g_heartbeat_actived)
				g_heartbeat_actived = true;
			g_heartbeat_time = xTaskGetTickCount() * portTICK_PERIOD_MS;
			xSemaphoreGive( g_heartbeat_mutex );
		}
		else if(UPDATE_ECU_NOTIFY_ID == *((uint16_t*)buf))
		{
			uint32_t block_size = 0;

			/* Stop transmit timer */
			xSemaphoreTake( g_tx_timer_mutex, portMAX_DELAY );
			xTimerStop(g_tx_timer, 0);
			xSemaphoreGive( g_tx_timer_mutex );

			fw_size = *((uint32_t*)(buf + ID_LENGTH + ECU_TYPE0_BIT_LEN / 8));

			/* ECU reset */
			buf[0] = UDS_SID_ECU_RESET;
			buf[1] = UDS_ECU_RESET_SF_SOFT;
			diag_client_request(&g_client_link, buf, 2);
			diag_client_poll_response(&g_client_link, buf, sizeof(buf));

			vTaskDelay(pdMS_TO_TICKS(1000));

			/* Tester present */
			buf[0] = UDS_SID_TESTER_PRESENT;
			buf[1] = UDS_TESTER_PRESENT_SF_ZERO;
			diag_client_request(&g_client_link, buf, 2);
			diag_client_poll_response(&g_client_link, buf, sizeof(buf));

			/* Enter extend session */
			buf[0] = UDS_SID_DIAG_SESSION_CTRL;
			buf[1] = UDS_DIAG_SESSION_CTRL_SF_EXTEND_MODE;
			diag_client_request(&g_client_link, buf, 2);
			diag_client_poll_response(&g_client_link, buf, sizeof(buf));

			/* Control DTC setting */
			buf[0] = UDS_SID_CTRL_DTC_SETTING;
			buf[1] = UDS_CTRL_DTC_SETTING_SF_OFF;
			diag_client_request(&g_client_link, buf, 2);
			diag_client_poll_response(&g_client_link, buf, sizeof(buf));

			/* Communication control */
			buf[0] = UDS_SID_COM_CTRL;
			buf[1] = UDS_COM_CTRL_SF_DIS_RX_TX;
			buf[2] = COMM_MSG_TYPE_MASK_NCM | COMM_MSG_TYPE_MASK_NWMCM;
			diag_client_request(&g_client_link, buf, 3);
			diag_client_poll_response(&g_client_link, buf, sizeof(buf));

			/* Enter program session */
			buf[0] = UDS_SID_DIAG_SESSION_CTRL;
			buf[1] = UDS_DIAG_SESSION_CTRL_SF_PROGRAM_MODE;
			diag_client_request(&g_client_link, buf, 2);
			diag_client_poll_response(&g_client_link, buf, sizeof(buf));

			/* Erase memory */
			buf[0] = UDS_SID_ROUTINE_CTRL;
			buf[1] = UDS_ROUTINE_CTRL_SF_START;
			buf[2] = UDS_RID_ERASE_MEM >> 8;
			buf[3] = (uint8_t)UDS_RID_ERASE_MEM;
			buf[4] = 0x44;
			*((uint32_t*)(buf + 5)) = CLIENT_FLASH_BASE_ADDR;
			bytesswap(buf + 5, 4);
			*((uint32_t*)(buf + 9)) = CLIENT_FLASH_TOTAL_SIZE;
			bytesswap(buf + 9, 4);
			debug("Erasing...\r\n");
			duration = xTaskGetTickCount() * portTICK_PERIOD_MS;
			diag_client_request(&g_client_link, buf, 13);
			diag_client_poll_response(&g_client_link, buf, sizeof(buf));
			debug("Erase done(%d ms)!\r\n", (xTaskGetTickCount() * portTICK_PERIOD_MS - duration));

			/* Request download */
			buf[0] = UDS_SID_REQT_DOWNLOAD;
			buf[1] = 0;
			buf[2] = 0x44;
			*((uint32_t*)(buf + 3)) = CLIENT_FLASH_BASE_ADDR;
			bytesswap(buf + 3, 4);
			*((uint32_t*)(buf + 7)) = fw_size;
			bytesswap(buf + 7, 4);
			debug("Programing...\r\n");
			duration = xTaskGetTickCount() * portTICK_PERIOD_MS;
			diag_client_request(&g_client_link, buf, 11);
			diag_client_poll_response(&g_client_link, buf, sizeof(buf));
			memcpy(&block_size, buf + 2, buf[1] >> 4);
			bytesswap(&block_size, buf[1] >> 4);

			/* ACK */
			*((uint16_t*)buf) = UPDATE_ECU_NOTIFY_ID;
			buf[ID_LENGTH] = 0;
			*((uint16_t*)(buf + ID_LENGTH + ECU_TYPE0_BIT_LEN / 8)) = block_size;
			uart_send_with_format((uint32_t)pvParameters ,buf, ID_LENGTH + ECU_TYPE0_BIT_LEN / 8 + 2);
			print_buf("UART TX", 0, buf, ID_LENGTH + ECU_TYPE0_BIT_LEN / 8 + 2);
			transfered_size = 0;
		}
		else if(TRANSFER_ECU_DATA_ID == *((uint16_t*)buf) && 0!= fw_size)
		{
			block_count = *((uint16_t*)(buf + ID_LENGTH + ECU_TYPE1_BIT_LEN / 8));

			/* Transfer data */
			buf[ID_LENGTH + ECU_TYPE1_BIT_LEN / 8] = UDS_SID_TRANSFER_DATA;
			buf[ID_LENGTH + ECU_TYPE1_BIT_LEN / 8 + 1] = (block_count + 1) % 256;
			diag_client_request(&g_client_link, buf + ID_LENGTH + ECU_TYPE1_BIT_LEN / 8, size - ID_LENGTH - TRANSFER_ECU_DATA_LEN + 2);
			diag_client_poll_response(&g_client_link, buf, sizeof(buf));

			transfered_size += size - ID_LENGTH - TRANSFER_ECU_DATA_LEN;
			if(transfered_size >= fw_size)
			{
				/* Request transfer exit */
				buf[0] = UDS_SID_REQT_TRANSFER_EXIT;
				diag_client_request(&g_client_link, buf, 1);
				diag_client_poll_response(&g_client_link, buf, sizeof(buf));

				debug("Programe done(%d ms)!\r\n", (xTaskGetTickCount() * portTICK_PERIOD_MS - duration));

				/* Check program dependence */
				buf[0] = UDS_SID_ROUTINE_CTRL;
				buf[1] = UDS_ROUTINE_CTRL_SF_START;
				buf[2] = UDS_RID_CHECK_PROM_DPED >> 8;
				buf[3] = (uint8_t)UDS_RID_CHECK_PROM_DPED;
				diag_client_request(&g_client_link, buf, 8);
				diag_client_poll_response(&g_client_link, buf, sizeof(buf));

				/* ECU reset */
				buf[0] = UDS_SID_ECU_RESET;
				buf[1] = UDS_ECU_RESET_SF_HARD;
				diag_client_request(&g_client_link, buf, 2);
				diag_client_poll_response(&g_client_link, buf, sizeof(buf));

				vTaskDelay(pdMS_TO_TICKS(1000));
				fw_size = 0;

				/* Reset transmit timer */
				xSemaphoreTake( g_tx_timer_mutex, portMAX_DELAY );
				xTimerReset(g_tx_timer, 0);
				xSemaphoreGive( g_tx_timer_mutex );
			}

			/* ACK */
			*((uint16_t*)buf) = TRANSFER_ECU_DATA_ID;
			buf[ID_LENGTH] = 0;
			*((uint16_t*)(buf + ID_LENGTH + ECU_TYPE1_BIT_LEN / 8)) = block_count;
			buf[ID_LENGTH + TRANSFER_ECU_DATA_LEN] = TRANSFER_ACK;
			uart_send_with_format((uint32_t)pvParameters ,buf, ID_LENGTH + TRANSFER_ECU_DATA_LEN + 1);
			print_buf("UART TX", 0, buf, ID_LENGTH + TRANSFER_ECU_DATA_LEN + 1);
		}
		else if(REQ_ECU_SW_NO_ID == *((uint16_t*)buf))
		{
			/* Read data by identifier */
			buf[0] = UDS_SID_READ_DATA_BY_ID;
			buf[1] = SSECUSWVNDID >> 8;
			buf[2] = (uint8_t)SSECUSWVNDID;
			diag_client_request(&g_client_link, buf, 3);
			diag_client_poll_response(&g_client_link, buf, sizeof(buf));

			/* ACK */
			*((uint16_t*)buf) = REQ_ECU_SW_NO_ID;
			buf[ID_LENGTH] = 0;
			uart_send_with_format((uint32_t)pvParameters, buf, ID_LENGTH + REQ_ECU_SW_NO_LEN + SSECUSWVN_LEN);
			print_buf("UART TX", 0, buf, ID_LENGTH + REQ_ECU_SW_NO_LEN + SSECUSWVN_LEN);
		}
		else
			resolve_downstream_to_msg(*((uint16_t*)buf), buf + ID_LENGTH, size - ID_LENGTH);
	}
}

/**
 * @brief   Diagnostic server task.
 *
 * @details This task is diagnostic server.
 *
 * @param   [in] pvParameters Parameters.
 */
static void diag_server_task( void *pvParameters )
{
	(void)pvParameters;

	can_msg_t msg;

	diag_server_data_init();

	while(1)
	{
		xQueueReceive( g_server_rx_queue, &msg, portMAX_DELAY );

		if(SERVER_RX_ID == msg.id_)
			/* Diagnostic message physical addressing */
			diag_server_indication(&g_server_link_phy, msg.data_, msg.dlc_);
		else
			/* Diagnostic message functional addressing */
			diag_server_indication(&g_server_link_func, msg.data_, msg.dlc_);
	}
}

/**
 * @brief   Normal task.
 *
 * @details This task gets and handles accelerometer X, Y, Z-axis sample data, updates DTC.
 *
 * @param   [in] pvParameters Parameters.
 */
static void normal_task( void *pvParameters )
{
	(void)pvParameters;

#if 0
	/* Reset then initialize and wake up accelerometer */
	accr_reset();
	vTaskDelay(pdMS_TO_TICKS(10));
	accr_init(ACCR_INT_DRDY_MASK);
	accr_sys_mode_trans(ACCR_SYSMOD_ACTIVE);
#endif

	while(1)
	{
		vTaskDelay(pdMS_TO_TICKS(1));
#if 0
		/* Accelerometer detection */
		uint8_t int_src = 0u;

		/* Get interrupt source */
		int_src = accr_get_int_src();

		/* Detect data ready */
		if(ACCR_INT_DRDY_MASK & int_src)
		{
			uint8_t buf[6];
			char    info[50];

			/* Get X, Y, Z-axis sample data */
			accr_get_xyz_sample(buf, sizeof(buf));
			print_buf("ACCELERATOR SAMPLE DATA", 0u, buf, sizeof(buf));

			/* Convert X, Y, Z-axis sample data to physical data */
			strcpy(info, "ACCELERATOR PHYSICAL DATA(mg): ");
			for(uint8_t i = 0; i < 3; i++)
			{
				int16_t temp = 0u;

				temp = get_bitfield(buf + 2 * i, 2, 0, 12);
				if(0x800u & temp)
					temp = -(0xFFF & (- temp));
				temp = (2 * 1000 * temp) / 1024;
				sprintf(info + strlen(info), "%d ", temp);
			}
			strcat(info, "\r\n");
			debug(info);
		}
#endif
		/* Update DTC */
		diag_server_update_dtc();
	}
}

/**
 * @brief   Timer callback function.
 *
 * @details This function transmits upstream data, checks heartbeat and  prints debug information.
 *
 * @param   [in] pvParameters Parameters.
 */
static void timer_callback( TimerHandle_t xTimer )
{
	configASSERT( xTimer );

    if(xTimer == g_tx_timer)
    {
    	static uint32_t upstream_count = 0;

    	/* Transmit network management message */
		if(diag_server_comm_ctrl_tx_enabled(COMM_MSG_TYPE_MASK_NWMCM))
			osek_nm_time_callback();

    	/* Check upstream count and then transmit */
    	if(TX_UPSTREAM_INTERVAL / 10 <= upstream_count)
    	{
        	uint16_t id = GB32960_ID;
        	static uint8_t  buf[ID_LENGTH + GB32960_DATA_LEN];
        	static uint16_t len = 0;

    		memcpy(buf, &id, ID_LENGTH);
    		len = resolve_get_upstream(buf + ID_LENGTH, GB32960_DATA_LEN);
#if defined MX_TB
    		/* Correct car status */
    		if(0 == GPIO_READ_PIN(IGN_GPIO, IGN_PIN))
    			set_bitfield(0x01, CAR_STATUS_START_BIT, CAR_STATUS_BIT_LEN, buf + ID_LENGTH, GB32960_DATA_LEN);
#endif
    		uart_send_with_format(UART0_INDEX ,buf, ID_LENGTH + len);
    		debug("UART TX GB32960 %d bytes\r\n", len);
    		upstream_count = 0;
    	}
    	upstream_count++;
    }
}

/**
 * @brief  Server receive callback function.
 *
 * @param  [out] _id   ID.
 * @param  [out] _buf  Receive buffer.
 * @param  [in]  _size Receive size.
 * @return Received size.
 */
static uint8_t server_receive_callback(uint32_t *const _id, uint8_t *const _buf, uint8_t _size)
{
	assert(NULL != _id && NULL != _buf);

	can_msg_t msg;
	uint8_t   size = 0;

	xQueueReceive( g_server_rx_queue, &msg, portMAX_DELAY );
	*_id = msg.id_;
	size = msg.dlc_ > _size ? _size : msg.dlc_;
	memcpy(_buf, msg.data_, size);

	return size;
}

/**
 * @brief  Client receive callback function.
 *
 * @param  [out] _id   ID.
 * @param  [out] _buf  Receive buffer.
 * @param  [in]  _size Receive size.
 * @return Received size.
 */
static uint8_t client_receive_callback(uint32_t *const _id, uint8_t *const _buf, const uint8_t _size)
{
	assert(NULL != _id && NULL != _buf);

	can_msg_t msg;
	uint8_t   size = 0;

	xQueueReceive( g_client_rx_queue, &msg, portMAX_DELAY );
	*_id = msg.id_;
	size = msg.dlc_ > _size ? _size : msg.dlc_;
	memcpy(_buf, msg.data_, size);

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
 * @brief Initialize modules.
 */
static void init( void )
{
	/* Initialize system, GPIO, UART... */
	sys_init();
    gpio_init();
    uart_init(UART0_INDEX, 115200, UART_DATA_BITS_8, UART_STOP_BITS_1, UART_PARITY_MODE_NONE);
    can_init(CAN0_INDEX, g_filter_id_list, sizeof(g_filter_id_list) / sizeof(uint32_t));
    can_init(CAN1_INDEX, g_filter_id_list, sizeof(g_filter_id_list) / sizeof(uint32_t));
    i2c_master_init(I2C0_INDEX, 400000, false);

	/* Initialize resolve related */
	resolve_init(transmit_callback, delay);

	/* Initialize diagnostic server */
	diag_server_init(&g_server_link_phy, SERVER_TX_ID, SERVER_RX_ID,
			         g_server_tx_buf_phy, sizeof(g_server_tx_buf_phy),
		             g_server_rx_buf_phy, sizeof(g_server_rx_buf_phy),
			         transmit_callback, server_receive_callback, sys_time, debug);
	diag_server_init(&g_server_link_func, SERVER_TX_ID, FUNCTION_ID,
			         g_server_tx_buf_func, sizeof(g_server_tx_buf_func),
			         g_server_rx_buf_func, sizeof(g_server_rx_buf_func),
			         transmit_callback, server_receive_callback, sys_time, debug);
	g_server_rx_queue = xQueueCreate( 10, sizeof( can_msg_t ) );

	/* Initialize diagnostic client */
	diag_client_init(&g_client_link, CLIENT_TX_ID, CLIENT_RX_ID,
			         g_client_tx_buf, sizeof(g_client_tx_buf),
			         g_client_rx_buf, sizeof(g_client_rx_buf),
			         transmit_callback, client_receive_callback, sys_time, debug);
	g_client_rx_queue = xQueueCreate( 10, sizeof( can_msg_t ) );

	g_sleep_mutex = xSemaphoreCreateMutex();
	g_heartbeat_mutex = xSemaphoreCreateMutex();
	g_tx_timer_mutex = xSemaphoreCreateMutex();
}

void vApplicationMallocFailedHook( void )
{
	/* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	taskDISABLE_INTERRUPTS();
	for( ;; )
	{
		vTaskDelay(pdMS_TO_TICKS(1000));
		GPIO_TOGGLE_PIN(LED2_GPIO, LED2_PIN);
	}
}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for( ;; )
	{
		vTaskDelay(pdMS_TO_TICKS(1000));
		GPIO_TOGGLE_PIN(LED2_GPIO, LED2_PIN);
	}
}

void vApplicationIdleHook( void )
{
    volatile size_t xFreeHeapSpace;

	/* This function is called on each cycle of the idle task.  In this case it
	does nothing useful, other than report the amount of FreeRTOS heap that
	remains unallocated. */
	xFreeHeapSpace = xPortGetFreeHeapSize();

	if( xFreeHeapSpace > 100 )
	{
		/* By now, the kernel has allocated everything it is going to, so
		if there is a lot of heap remaining unallocated then
		the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		reduced accordingly. */
	}
}

/* The Blinky build configuration does not include run time stats gathering,
however, the Full and Blinky build configurations share a FreeRTOSConfig.h
file.  Therefore, dummy run time stats functions need to be defined to keep the
linker happy. */
void vMainConfigureTimerForRunTimeStats( void ) {}
unsigned long ulMainGetRunTimeCounterValue( void ) { return 0UL; }

/* A tick hook is used by the "Full" build configuration.  The Full and blinky
build configurations share a FreeRTOSConfig.h header file, so this simple build
configuration also has to define a tick hook - even though it does not actually
use it for anything. */
void vApplicationTickHook( void ) {}
