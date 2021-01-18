/*
 * can.h
 *
 *  Created on: 2018��8��21��
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
 * @name The CAN channel indexes
 * @{
 */
#define CAN0_INDEX                              0
#define CAN1_INDEX                              1
/** @} */ // The CAN channel indexes

/**
 * @name The CAN power modes
 * @{
 */
#define CAN_PWR_MODE_SLEEP                      0
#define CAN_PWR_MODE_RUN                        1
/** @} */ // The CAN power modes

#define CAN_BUFFER_SIZE                         10 ///< the CAN RX queue max size

/**
 * The CAN message structure
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
 * @param [in] _index the CAN channel index
 * @param [in] _filter_id_list the CAN filter id list
 * @param [in] _filter_id_num the CAN filter id number
 * @return 0(success) or other values(failure)
 */
int32_t can_init(const uint8_t _index, const uint32_t *_filter_id_list, const uint8_t _filter_id_num);

/**
 * De-initialize the CAN.
 * 
 * @param [in] _index the CAN channel index
 * @return 0(success) or other values(failure)
 */
int32_t can_deinit(const uint8_t _index);

/**
 * Receive a CAN message.
 *
 * @param [in]  _index the CAN channel index
 * @param [out] _id the received CAN ID 
 * @param [out] _buf the buffer to receive to
 * @param [in]  _size the size to receive
 * @return the received size
 */
uint8_t can_receive(const uint8_t _index, uint32_t *const _id, uint8_t *const _buf,  const uint8_t _size);

/**
 * Transmit a CAN message.
 *
 * @param [in] _index the CAN channel index
 * @param [in] _id the transmitted CAN ID
 * @param [in] _buf the buffer to transmit from
 * @param [in] _size the size to transmit
 * @return the transmitted size
 */
uint8_t can_transmit(const uint8_t _index, const uint32_t _id, const uint8_t *const _buf, const uint8_t _size);

/**
 * Transfer the CAN power mode.
 *
 * @param [in] _index the CAN channel index
 * @param [in] _mode the CAN power mode to transfer to:
 * <ul>
 * <li>{@link #CAN_PWR_MODE_SLEEP} sleep</li>
 * <li>{@link #CAN_PWR_MODE_RUN} run</li>
 * </ul>
 * @return 0(success) or other values(failure)
 */
int32_t can_pwr_mode_trans(const uint8_t _index, const uint8_t _mode);

#ifdef __cplusplus
}
#endif

#endif // __CAN_H__
