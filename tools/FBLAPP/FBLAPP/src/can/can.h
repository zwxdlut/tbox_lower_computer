#pragma once

#ifndef __CAN_COMM_H__
#define __CAN_COMM_H__

#include <windows.h>
#include <string>
#include <mutex>

#include "ECanVci.h"

/**
 * @name CAN device types
 * @{
 */
#define USBCAN_I						        USBCAN1
#define USBCAN_II                               USBCAN2
 /** @} */ // CAN device types

/**
 * @name CAN device indexes
 * @{
 */
#define CAN_DEV_IDX0					        0
#define CAN_DEV_IDX1                            1
 /** @} */ // CAN device indexes
	
/**
 * @name CAN channel numbers
 * @{
 */
#define CAN_CHL0                                0
#define CAN_CHL1                                1
 /** @} */ // CAN channel numbers

/** 
 * CAN communication class.
 */
class can
{
public:
	/**
	 * Constructor.
	 *
	 * @param [in] _dev_type the device type
	 * @param [in] _dev_idx the device index
	 */
	can(const uint8_t _dev_type, const uint8_t _dev_idx);

	/** 
	 * Destructor.
	 */
	~can(void);

	/**
	 * Open the device.
	 *
	 * @return 0(success) or other values(failure)
	 */
	int32_t open(void);

	/**
	 * Close the device.
	 *
	 * @return 0(success) or other values(failure)
	 */
	int32_t close(void);

	/**
	 * Initialize a CAN channel.
	 *
	 * @param [in] _chl the CAN channel number
	 * @return 0(success) or other values(failure)
	 */
	int32_t init(const uint8_t _chl);

	/**
	 * Receive a CAN message.
	 *
	 * @param [in]  _index the CAN channel number
	 * @param [out] _id the received CAN id
	 * @param [out] _buf the buffer to receive to
	 * @param [in]  _size the size to receive
	 * @return the received CAN data size
	 */
	uint8_t receive(const uint8_t _chl, uint32_t* const _id, uint8_t _buf[], const uint8_t _size);

	/**
	 * Send a CAN message.
	 *
	 * @param [in] _index the CAN channel number
	 * @param [in] _id the sent CAN id
	 * @param [in] _buf the buffer to send from
	 * @param [in] _size the size to send
	 * @return the sent CAN data size
	 */
	uint8_t send(const uint8_t _chl, const uint32_t _id, const uint8_t _buf[], const uint8_t _size);

private:
	uint8_t dev_type_;
	uint8_t dev_idx_;
	std::mutex tx_mutex_;
};

#endif //__CAN_COMM_H__