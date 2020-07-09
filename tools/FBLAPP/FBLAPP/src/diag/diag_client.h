#ifndef __DIAG_CLIENT_H__
#define __DIAG_CLIENT_H__

#include <stdint.h>

#include "isotp/isotp.h"
#include "diag_define.h"
#include "diag.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/**
 * @defgroup Diagnostic client address.
 * @{
 */
#define CLIENT_RX_ID                                                                 0x7A9 /**< Client receive ID */
#define CLIENT_TX_ID                                                                 0x729 /**< Client transmit ID. */
/** @} */ /* End of group Diagnostic client address. */

/**
 * @defgroup Flash defines.
 * @{
 */
#if defined S32K144
#define MEM_BASE_ADDR 			                                                     0x00000000
#define MEM_SECTOR_SIZE                                                              4096       /**< 4k. */
#define MEM_TOTAL_SIZE                                                               0x80000    /**< 512k. */
#elif defined STM32F10X_CL
#define MEM_BASE_ADDR 			                                                     0x08000000
#define MEM_SECTOR_SIZE                                                              2048       /**< 2k. */
#define MEM_TOTAL_SIZE                                                               0x40000    /**< 256k. */
#elif defined STM32F205xx
#define MEM_BASE_ADDR 			                                                     0x08000000
#define MEM_SECTOR_SIZE                                                              4096       /**< 4k. */
#define MEM_TOTAL_SIZE                                                               0x80000    /**< 512k. */
#elif WIN32
#else
#error #error Mcu type not defined!!!
#endif
#define CLIENT_FLASH_BASE_ADDR   	                                                 0x08010000 /**< Client flash base address. */
#define CLIENT_FLASH_TOTAL_SIZE   	                                                 0x70000    /**< Client flash total size. */
/** @} */ /* End of group Flash defines. */

/*******************************************************************************
 * Function prototypes
 ******************************************************************************/
/**
 * @brief Initialize diagnostic client.
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
 * @param [in] _sys_time_callback Callback function to get system time in milliseconds.
 * @param [in] _debug_callback    Callback function for debug.
 */
void diag_client_init(IsoTpLink *const _link, const uint32_t _tx_id, const uint32_t _rx_id,
					  uint8_t *const _tx_buf, const uint16_t _tx_buf_size,
                      uint8_t *const _rx_buf, const uint16_t _rx_buf_size,
                      const send_callback_t _tx_callback,
                      const receive_callback_t _rx_callback,
                      const sys_time_callback_t _sys_time_callback,
                      const debug_callback_t _debug_callback);

/**
 * @brief De-initialize diagnostic client.
 *
 * @param [in] _link ISOTP link.
 */
void diag_client_deinit(IsoTpLink *const _link);

/**
 * @brief  Diagnostic client request.
 *
 * @param  [in] _link ISOTP link.
 * @param  [in] _buf  Request data buffer.
 * @param  [in] _size Request data size.
 */
void diag_client_request(IsoTpLink* const _link, const uint8_t* const _buf, const uint16_t _size);

/**
 * @brief  Diagnostic client poll response.
 *
 * @param  [in]  _link ISOTP link.
 * @param  [out] _buf  Response data buffer.
 * @param  [in]  _size Response data size.
 * @return Responsed data size.
 */
uint16_t diag_client_poll_response(IsoTpLink* const _link, uint8_t* const _buf, const uint16_t _size);

#ifdef __cplusplus
}
#endif

#endif /* __DIAG_CLIENT_H__ */
