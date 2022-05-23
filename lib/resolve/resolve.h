/*
 * resolve.h
 *
 *  Created on: 2018Äê8ÔÂ30ÈÕ
 *      Author: Administrator
 */

#ifndef __RESOLVE_H__
#define __RESOLVE_H__

#include <stdint.h>

#include "gen_upstream.h"
#include "gen_downstream.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define ID_LENGTH                                               2

/**
 * @brief Transmit callback function type.
 */
typedef int32_t (*resolve_transmit_callback_t)(const uint32_t _id, const uint8_t *const _data, const uint8_t _len);

/**
 * @brief Delay milliseconds callback function type.
 */
typedef void (*resolve_delay_callback_t)(const uint32_t _ms);

/*******************************************************************************
 * Function prototypes
 ******************************************************************************/
/**
 * @brief Initialize the resolver.
 *
 * @param [in] _transmit_callback CAN transmit callback function.
 * @param [in] _delay_callcack    Delay milliseconds callback function.
 */
void resolve_init(const resolve_transmit_callback_t _transmit_callback, const resolve_delay_callback_t _delay_callcack);

/**
 * @brief Deinitialize the resolver.
 */
void resolve_deinit(void);

/**
 * @brief Resolve message to upstream.
 *
 * @param [in] _id   Message ID
 * @param [in] _data Message data.
 * @param [in] _len  Message data length.
 */
void resolve_msg_to_upstream(const uint32_t _id, const uint8_t* const _data, const uint8_t _len);

/**
 * @brief Resolve downstream to message.
 *
 * @param [in] _id   Downstream ID.
 * @param [in] _data Downstream data.
 * @param [in] _len  Downstream data length.
 */
void resolve_downstream_to_msg(const uint16_t _id, const uint8_t* const _data, const uint16_t _len);

/**
 * @brief  Get upstream.
 *
 * @param  [out] _data Upstream data.
 * @param  [in]  _len  Upstream data length.
 * @return Got upstream data length.
 */
uint16_t resolve_get_upstream(uint8_t *const _data, const uint16_t _len);

/**
 * @brief  Get upstream item.
 *
 * @param  [in] _start_bit Item start bit.
 * @param  [in] _bit_len   Item bit length.
 * @return Item.
 */
uint32_t resolve_get_upstream_item(const uint16_t _start_bit, const uint16_t _bit_len);

/**
 * @brief Set upstream item.
 *
 * @param [in] _start_bit Item start bit.
 * @param [in] _bit_len   Item bit length.
 * @param [in] _item      Item.
 */
void resolve_set_upstream_item(const uint16_t _start_bit, const uint16_t _bit_len, const uint32_t _item);

#ifdef __cplusplus
}
#endif

#endif /* __RESOLVE_H__ */
