#ifndef __DIAG_SERVER_H__
#define __DIAG_SERVER_H__

#include <stdint.h>
#include <stdbool.h>

#include "diag_server_define.h"
#include "diag_define.h"
#include "diag.h"
#include "isotp/isotp.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*******************************************************************************
 * Function prototypes
 ******************************************************************************/
/**
 * @brief Initialize diagnostic server.
 *
 * @param [in] _link              ISOTP link.
 * @param [in] _tx_id             Transmit address information.
 * @param [in] _rx_id             Receive address information.
 * @param [in] _tx_buf            Transmit buffer.
 * @param [in] _tx_buf_size       Transmit buffer size.
 * @param [in] _rx_buf            Receive buffer.
 * @param [in] _rx_buf_size       Receive buffer size.
 * @param [in] _tx_callback       Callback function to transmit data to link layer.
 * @param [in] _rx_callback       Callback function to receive from link layer.
 * @param [in] _sys_time_callback Get system time in milliseconds.
 * @param [in] _debug_callback    Print debug info.
 */
void diag_server_init(IsoTpLink *const _link, const uint32_t _tx_id, const uint32_t _rx_id,
					  uint8_t *const _tx_buf, const uint16_t _tx_buf_size,
                      uint8_t *const _rx_buf, const uint16_t _rx_buf_size,
                      const send_callback_t _tx_callback,
                      const receive_callback_t _rx_callback,
					  const sys_time_callback_t _sys_time_callback,
					  const debug_callback_t _debug_callback);

/**
 * @brief De-initialize diagnostic server.
 *
 * @param [in] _link ISOTP link.
 */
void diag_server_deinit(IsoTpLink *const _link);

/**
 * @brief Initialize data from memory.
 */
void diag_server_data_init(void);

/**
 * @brief Indicate diagnostic server link layer data.
 *
 * @param [in] _link  ISOTP link.
 * @param [in] _buf   Data buffer.
 * @param [in] _size  Data size.
 */
void diag_server_indication(IsoTpLink *const _link, const uint8_t *const _buf, const uint8_t _size);

/**
 * @brief  Check if communication control receive enabled with specific message type.
 *
 * @param  [in] _mask Message type mask(COM_TYPE_MASK_XXX).
 * @return True(enabled) or false(disabled).
 */
bool diag_server_comm_ctrl_rx_enabled(const uint8_t _mask);

/**
 * @brief  Check if communication control send enabled with specific message type.
 *
 * @param  [in] _mask Message type mask(COM_TYPE_MASK_XXX).
 * @return True(enabled) or false(disabled).
 */
bool diag_server_comm_ctrl_tx_enabled(const uint8_t _mask);

/**
 * @brief Read data by identifier.
 *
 * @param [in] _did  Data identifier.
 * @param [in] _buf  Read buffer.
 * @param [in] _size Read size.
 */
void diag_server_read_data_by_id(const uint16_t _did, uint8_t *const _buf, const uint16_t _size);

/**
 * @brief Notify message received.
 *
 * @param [in] _msg_id Message id.
 */
void diag_server_on_msg(const uint32_t _msg_id);

/**
 * @brief Detect and update DTC.
 */
void diag_server_update_dtc(void);

/**
 * @name Diagnostic server callbacks.
 * @{
 * @brief User should implement them.
 */
/**
 * @brief Reset system callback.
 */
void diag_server_sys_reset_callback(void);

/**
 * @brief  Get power voltage callback.
 *
 * @return Power voltage in 0.1V.
 */
uint32_t diag_server_get_pwr_vol_callback(void);

/**
 * @brief  Get vehicle speed callback.
 *
 * @return Vehicle speed in 0.1km/h.
 */
uint32_t diag_server_get_vehicle_speed_callback(void);

/**
 * @brief  Get car status callback.
 *
 * @return Car status:
 * @retval CAR_STATUS_RUNNING: Running.
 * @retval CAR_STATUS_STOPPED: Stopped.
 * @retval CAR_STATUS_OTHER:   Other.
 */
uint32_t diag_server_get_car_status_callback(void);

/**
 * @brief  Erase continuous sectors callback.
 *
 * @param  [in] _addr Sector start address.
 * @param  [in] _size Erase size aligned to sector.
 * @return Success(0) or failure(other values).
 */
int32_t  diag_server_flash_erase_sectors_callback(const uint32_t _addr, const uint32_t _size);

/**
 * @brief  Verify continuous sectors callback.
 *
 * @param  [in] _addr Sector start address.
 * @param  [in] _size Erase size aligned to sector.
 * @return Success(0) or failure(other values).
 */
int32_t  diag_server_flash_verify_sectors_callback(const uint32_t _addr, const uint32_t _size);

/**
 * @brief  Program flash memory callback.
 *
 * @param  [in] _addr Program start address.
 * @param  [in] _size Program size.
 * @param  [in] _buf  Program buffer.
 * @return Success(0) or failure(other values).
 */
int32_t  diag_server_flash_program_callback(const uint32_t _addr, const uint32_t _size, const uint8_t *const _buf);

/**
 * @brief  Verify programed flash memory callback.
 *
 * @param  [in] _addr Verify start address.
 * @param  [in] _size Verify size.
 * @param  [in] _buf  Verify buffer.
 * @return Success(0) or failure(other values).
 */
int32_t  diag_server_flash_program_verify_callback(const uint32_t _addr, const uint32_t _size, const uint8_t *const _buf);
/** @} */ /* Diagnostic server callbacks. */

#ifdef __cplusplus
}
#endif

#endif /* __DIAG_SERVER_H__ */
