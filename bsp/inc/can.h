#ifndef __CAN_H__
#define __CAN_H__

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/**
 * @name CAN channel numbers
 * @{
 */
#define CAN_CH0                                 0
#define CAN_CH1                                 1
/** @} */ /* CAN channel numbers */

/**
 * @name CAN power modes
 * @{
 */
#define CAN_PWR_MODE_SLEEP                      0
#define CAN_PWR_MODE_RUN                        1
/** @} */ /* CAN power modes */

#define CAN_MSG_RX_QUEUE_MAX_LENGTH             10 /**< the max length of CAN receiving queue */

/**
 * CAN message structure.
 */
typedef struct
{
	uint32_t id_;
	uint8_t  dlc_;
	uint8_t  data_[8];
} can_msg_t;

/*******************************************************************************
 * Function prototypes
 ******************************************************************************/
/**
 * Initialize a CAN channel.
 *
 * @param [in] _chl the CAN channel number
 * @param [in] _filter_id_list the CAN filter id list
 * @param [in] _filter_id_num the number of the ids in the list
 * @return 0(success) or other values(failure)
 */
int32_t can_init(const uint8_t _chl, const uint32_t *_filter_id_list, const uint8_t _filter_id_num);

/**
 * De-initialize a CAN channel.
 * 
 * @param [in] _chl the CAN channel number
 * @return 0(success) or other values(failure)
 */
int32_t can_deinit(const uint8_t _chl);

/**
 * Receive a CAN message.
 *
 * @param [in] _chl the CAN channel number
 * @param [out] _id the received CAN id
 * @param [out] _buf the buffer to receive
 * @param [in]  _size the size to receive
 * @return the receivied CAN data size
 */
uint8_t can_receive(const uint8_t _chl, uint32_t *const _id, uint8_t _buf[],  const uint8_t _size);

/**
 * Send a CAN message.
 *
 * @param [in] _chl the CAN channel number
 * @param [in] _id the sent CAN id
 * @param [in] _buf the buffer to send
 * @param [in] _size the size to send
 * @return the sent CAN data size
 */
uint8_t can_send(const uint8_t _chl, const uint32_t _id, const uint8_t _buf[], const uint8_t _size);

/**
 * Transfer the CAN power mode.
 *
 * @param [in] _chl the CAN channel number
 * @param [in] _mode the CAN power mode is transfered to:
 * <ul>
 * <li>{@link CAN_PWR_MODE_SLEEP}</li>
 * <li>{@link CAN_PWR_MODE_RUN}</li>
 * </ul>
 * @return 0(success) or other values(failure)
 */
int32_t can_pwr_mode_trans(const uint8_t _chl, const uint8_t _mode);

#ifdef __cplusplus
}
#endif

#endif // __CAN_H__
