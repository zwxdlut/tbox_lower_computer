/*
 * can.h
 *
 *  Created on: 2018年8月21日
 *      Author: Administrator
 */

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
 * @name CAN channel indexes
 * @{
 */
#define CAN0_INDEX                              0
#define CAN1_INDEX                              1
/** @} */ /* CAN channel indexes */

/**
 * @name CAN power modes
 * @{
 */
#define CAN_PWR_MODE_SLEEP                      0
#define CAN_PWR_MODE_RUN                        1
/** @} */ /* CAN power modes */

#define CAN_MSG_RX_QUEUE_MAX_LENGTH             10 /**< The max length of CAN receiving queue */

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
 * Initialize the CAN.
 *
 * @param [in] _index The CAN channel index
 * @param [in] _filter_id_list The CAN filter id list
 * @param [in] _filter_id_num The number of the ids in the list
 * @return 0(success) or other values(failure).
 */
int32_t can_init(const uint8_t _index, const uint32_t *_filter_id_list, const uint8_t _filter_id_num);

/**
 * De-initialize the CAN.
 * 
 * @param [in] _index The CAN channel index
 * @return 0(success) or other values(failure).
 */
int32_t can_deinit(const uint8_t _index);

/**
 * Receive a CAN message.
 *
 * @param [in] _index The CAN channel index
 * @param [out] _id The received CAN id
 * @param [out] _buf The buffer to receive
 * @param [in]  _size The size to receive
 * @return The receivied CAN data size.
 */
uint8_t can_receive(const uint8_t _index, uint32_t *const _id, uint8_t _buf[],  const uint8_t _size);

/**
 * Send a CAN message.
 *
 * @param [in] _index The CAN channel index
 * @param [in] _id The sent CAN id
 * @param [in] _buf The buffer to send
 * @param [in] _size The size to send
 * @return The sent CAN data size.
 */
uint8_t can_send(const uint8_t _index, const uint32_t _id, const uint8_t _buf[], const uint8_t _size);

/**
 * Transfer the CAN power mode.
 *
 * @param [in] _index The CAN channel index
 * @param [in] _mode The CAN power mode is transfered to:
 * <ul>
 * <li>{@link CAN_PWR_MODE_SLEEP}</li>
 * <li>{@link CAN_PWR_MODE_RUN}</li>
 * </ul>
 * @return 0(success) or other values(failure).
 */
int32_t can_pwr_mode_trans(const uint8_t _index, const uint8_t _mode);

#ifdef __cplusplus
}
#endif

#endif // __CAN_H__
