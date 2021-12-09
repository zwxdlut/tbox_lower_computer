#include "can.h"

extern can_msg_t g_can_rx_queue[CAN_CH1 + 1][CAN_MSG_RX_QUEUE_MAX_LENGTH]; /* receiving queue */
extern uint8_t g_can_rx_queue_head[CAN_CH1 + 1]; /* receiving queue head */
extern uint8_t g_can_rx_queue_tail[CAN_CH1 + 1]; /* receiving queue tail */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/******************************************************************************
 * Local function prototypes
 ******************************************************************************/

/*******************************************************************************
 * Functions
 ******************************************************************************/
uint8_t can_receive(const uint8_t _chl, uint32_t *const _id, uint8_t _buf[],  const uint8_t _size)
{
	assert(CAN_CH1 >= _chl && NULL != _id && NULL != _buf);

	uint8_t size = 0;

	/* check if the rx queue is empty */
	if (g_can_rx_queue_head[_chl] != g_can_rx_queue_tail[_chl])
	{
		/* dequeue */
		*_id = g_can_rx_queue[_chl][g_can_rx_queue_head[_chl]].id_;
		size = _size > g_can_rx_queue[_chl][g_can_rx_queue_head[_chl]].dlc_ ? g_can_rx_queue[_chl][g_can_rx_queue_head[_chl]].dlc_ : _size;
		memcpy(_buf, g_can_rx_queue[_chl][g_can_rx_queue_head[_chl]].data_, size);
		g_can_rx_queue_head[_chl] = (g_can_rx_queue_head[_chl] + 1) % CAN_MSG_RX_QUEUE_MAX_LENGTH;
	}

	return size;
}

/*******************************************************************************
 * Local functions
 ******************************************************************************/
