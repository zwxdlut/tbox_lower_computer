/*
 * can.c
 *
 *  Created on: 2018��8��21��
 *      Author: Administrator
 */

#include "can.h"

extern can_msg_t g_can_rx_queue[CAN1_INDEX + 1][CAN_BUFFER_SIZE]; // Rx ring queue
extern uint8_t g_can_rx_queue_head[CAN1_INDEX + 1]; // Rx queue head
extern uint8_t g_can_rx_queue_tail[CAN1_INDEX + 1]; // Rx queue tail

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/******************************************************************************
 * Local Function prototypes
 ******************************************************************************/
/*******************************************************************************
 * Functions
 ******************************************************************************/
uint8_t can_receive(const uint8_t _index, uint32_t *const _id, uint8_t *const _buf,  const uint8_t _size)
{
	assert(CAN1_INDEX >= _index && NULL != _id && NULL != _buf);

	uint8_t size = 0;

	// Rx queue is not empty
	if (g_can_rx_queue_head[_index] != g_can_rx_queue_tail[_index])
	{
		// Pop rx queue
		*_id = g_can_rx_queue[_index][g_can_rx_queue_head[_index]].id_;
		size = _size > g_can_rx_queue[_index][g_can_rx_queue_head[_index]].dlc_ ? g_can_rx_queue[_index][g_can_rx_queue_head[_index]].dlc_ : _size;
		memcpy(_buf, g_can_rx_queue[_index][g_can_rx_queue_head[_index]].data_, size);
		g_can_rx_queue_head[_index] = (g_can_rx_queue_head[_index] + 1) % CAN_BUFFER_SIZE;
	}

	return size;
}

/*******************************************************************************
 * Local Functions
 ******************************************************************************/
