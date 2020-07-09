#pragma once

#ifndef __CAN_COMM_H__
#define __CAN_COMM_H__

#include <windows.h>
#include <string>
#include <mutex>

#include "ECanVci.h"

/* CAN device type */
#define USBCAN_I						        USBCAN1
#define USBCAN_II                               USBCAN2
										        
/* CAN device index */					        
#define CAN_DEV_IDX0					        0
#define CAN_DEV_IDX1                            1
										        
/* CAN channel number */				        
#define CAN_CHL0                                0
#define CAN_CHL1                                1

/** 
 * @brief CAN comunication class.
 */
class can
{
public:
	/**
	 * @brief  Constructor.
	 *
	 * @param  [in] _dev_type  Device type.
	 * @param  [in] _dev_idx   Device index.
	 */
	can(const uint8_t _dev_type, const uint8_t _dev_idx);

	/** 
	 * @brief Destructor. 
	 */
	~can(void);

	/**
	 * @brief  Open device.
	 *
	 * @return Success(0) or failure(other values).
	 */
	int32_t open(void);

	/**
	 * @brief  Close device.
	 *
	 * @return Success(0) or failure(other values).
	 */
	int32_t close(void);

	/**
	 * @brief  Initialize channel.
	 *
	 * @param  [in] _chl   Channel number.
	 * @return Success(0) or failure(other values).
	 */
	int32_t init(const uint8_t _chl);

	/**
	 * @brief  Receive frame.
	 *
	 * @param  [in]  _chl   Channel number.
	 * @param  [out] _id    CAN ID.
	 * @param  [out] _buf   Receive buffer.
	 * @param  [in]  _size  Receive size.
	 * @return Received size.
	 */
	uint8_t receive(const uint8_t _chl, uint32_t* const _id, uint8_t* const _buf, const uint8_t _size);

	/**
	 * @brief  Transmit frame.
	 *
	 * @param  [in] _chl   Channel number.
	 * @param  [in] _id    CAN ID.
	 * @param  [in] _buf   Transmit buffer.
	 * @param  [in] _size  Transmit size.
	 * @return Transmitted size.
	 */
	uint8_t transmit(const uint8_t _chl, const uint32_t _id, const uint8_t *const _buf, const uint8_t _size);

private:
	uint8_t    dev_type_;
	uint8_t    dev_idx_;
	std::mutex tx_mutex_;
};

#endif //__CAN_COMM_H__