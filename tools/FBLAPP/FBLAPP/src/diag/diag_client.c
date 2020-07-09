#include <string.h>
#include <assert.h>

#include "diag_client.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*******************************************************************************
 * Local Function prototypes
 ******************************************************************************/
static void diag_client_app_indication(IsoTpLink *const _link, const uint8_t *const _buf, const uint16_t _size);

/*******************************************************************************
 * Functions
 ******************************************************************************/
void diag_client_init(IsoTpLink *const _link, const uint32_t _tx_id, const uint32_t _rx_id,
					  uint8_t *const _tx_buf, const uint16_t _tx_buf_size,
                      uint8_t *const _rx_buf, const uint16_t _rx_buf_size,
                      const send_callback_t _tx_callback,
                      const receive_callback_t _rx_callback,
                      const sys_time_callback_t _sys_time_callback,
                      const debug_callback_t _debug_callback)
{
	assert(NULL != _link);

	/* Initialize network layer */
	isotp_init_link(_link, _tx_id, _rx_id, _tx_buf, _tx_buf_size, _rx_buf, _rx_buf_size
		, _tx_callback, _rx_callback, diag_client_app_indication, _sys_time_callback, _debug_callback);
}

void diag_client_deinit(IsoTpLink *const _link)
{
	(void)_link;
}

void diag_client_request(IsoTpLink* const _link, const uint8_t* const _buf, const uint16_t _size)
{
	assert(NULL != _link && NULL != _buf);

	/* Transmit to network layer */
	isotp_send_poll(_link, _buf, _size);
}

uint16_t diag_client_poll_response(IsoTpLink* const _link, uint8_t* const _buf, const uint16_t _size)
{
	assert(NULL != _link && NULL != _buf);

	uint32_t id = 0;
	uint8_t  size = 0;
	uint8_t  buf[8];
	uint16_t out_size = 0;

	/* Poll response */
	while (1)
	{
		if (0 == (size = _link->receive_callback(&id, buf, sizeof(buf))) && CLIENT_RX_ID == id)
			continue;
		isotp_indication(_link, buf, size);
		if (ISOTP_RET_OK == isotp_receive(_link, _buf, _size, &out_size))
			break;
	}

	return out_size;
}

/*******************************************************************************
 * Local Functions
 ******************************************************************************/
 /**
  * @brief Indicate diagnostic client application layer data.
  *
  * @param [in] _link ISOTP link.
  * @param [in] _buf  Data buffer.
  * @param [in] _size Data size.
  */
static void diag_client_app_indication(IsoTpLink* const _link, const uint8_t* const _buf, const uint16_t _size)
{
}
