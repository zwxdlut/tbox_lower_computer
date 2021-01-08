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
 * @name CAN module index.
 * @{
 */
#define CAN0_INDEX                              0
#define CAN1_INDEX                              1
/** @} */ // name CAN module index.

/**
 * @name CAN power modes.
 * @{
 */
#define CAN_PWR_MODE_SLEEP                      0
#define CAN_PWR_MODE_RUN                        1
/** @} */ // CAN power modes.

#define CAN_BUFFER_SIZE                         10 ///< CAN rx queue max size

/**
 * CAN message.
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
 * Initialize CAN module.
 *
 * @param [in] _index CAN index
 * @param [in] _filter_id_list Filter id list
 * @param [in] _filter_id_num Filter id number
 * @return Success(0) or failure(other values).
 */
int32_t can_init(const uint8_t _index, const uint32_t *_filter_id_list, const uint8_t _filter_id_num);

/**
 * Deinitialize CAN module.
 * 
 * @param [in] _index CAN index
 * @return Success(0) or failure(other values).
 */
int32_t can_deinit(const uint8_t _index);

/**
 * Receive CAN message.
 *
 * @param [in]  _index CAN index
 * @param [out] _id CAN ID
 * @param [out] _buf Receive buffer
 * @param [in]  _size Receive size
 * @return Received size.
 */
uint8_t can_receive(const uint8_t _index, uint32_t *const _id, uint8_t *const _buf,  const uint8_t _size);

/**
 * Transmit CAN message.
 *
 * @param [in] _index CAN index
 * @param [in] _id CAN ID
 * @param [in] _buf Transmit buffer
 * @param [in] _size Transmit size
 * @return Transmitted size.
 */
uint8_t can_transmit(const uint8_t _index, const uint32_t _id, const uint8_t *const _buf, const uint8_t _size);

/**
 * Transfer CAN power mode.
 *
 * @param [in] _index CAN index
 * @param [in] _mode Power mode:
 *  - CAN_PWR_MODE_SLEEP: Sleep
 *  - CAN_PWR_MODE_RUN: Run
 * @return Success(0) or failure(other values).
 */
int32_t can_pwr_mode_trans(const uint8_t _index, const uint8_t _mode);

#ifdef __cplusplus
}
#endif

#endif // __CAN_H__
