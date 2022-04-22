/**
 * @file   gen_resolve.h
 * @author Auto generated
 * @note   Created on 2022/04/22
 */

#ifndef __GEN_RESOLVE_H__
#define __GEN_RESOLVE_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * Definitions
 ******************************************************************************/

/******************************************************************************
 * Function prototypes
 ******************************************************************************/
/**
 * @brief  Resolve message to upstream.
 *
 * @param  [in] _id   Message ID.
 * @param  [in] _data Message data.
 * @param  [in] _len  Message data length.
 */
void gen_resolve_msg_to_upstream(const uint32_t _id, const uint8_t *const _data, const uint8_t _len);

/**
 * @brief  Resolve downstream to message.
 *
 * @param  [in] _id   Downstream ID.
 * @param  [in] _data Downstream data.
 * @param  [in] _len  Downstream data length.
 */
void gen_resolve_downstream_to_msg(const uint16_t _id, const uint8_t *const _data, const uint16_t _len);

/**
 * @name Callback functions.
 * @{
 */
/**
 * @brief Resolve message to upstream callback function.
 *
 * @param [in] _signal_start_bit Message signal start bit.
 * @param [in] _signal_bit_len   Message signal item bit length.
 * @param [in] _data             Message data.
 * @param [in] _len              Messge  data length.
 * @param [in] _from_list        Covert from table list.
 * @param [in] _to_list          Covert to table list.
 * @param [in] _list_num         Table list number.
 * @param [in] _resolution       Covert resolution.
 * @param [in] _offset           Covert offset.
 * @param [in] _item_start_bit   Upstream item start bit.
 * @param [in] _item_bit_len     Upstream item bit length.
 */
void gen_resolve_msg_to_upstream_callback(const uint8_t _signal_start_bit,
                                          const uint8_t _signal_bit_len,
                                          const uint8_t *const _data,
                                          const uint8_t _len,
                                          const uint32_t *const _from_list,
                                          const uint32_t *const _to_list,
                                          const uint8_t _list_num,
                                          const float _resolution,
                                          const int32_t _offset,
                                          const uint16_t _item_start_bit,
                                          const uint8_t _item_bit_len);

/**
 * @brief Resolve downstream to message callback function.
 *
 * @param _item_start_bit   Downstream item start bit.
 * @param _item_bit_len     Downstream item bit length.
 * @param _data             Downstream data.
 * @param _len              Downstream data length.
 * @param _from_list        Covert from table list.
 * @param _to_list          Covert to table list.
 * @param _list_num         Table list number.
 * @param _resolution       Covert resolution.
 * @param _offset           Covert offset.
 * @param _id               Message ID.
 * @param _signal_start_bit Message signal start bit.
 * @param _signal_bit_len   Message signal bit length.
 */
void gen_resolve_downstream_to_msg_callback(const uint8_t _item_start_bit,
                                            const uint8_t _item_bit_len,
                                            const uint8_t *const _data,
                                            const uint16_t _len,
                                            const uint32_t *const _from_list,
                                            const uint32_t *const _to_list,
                                            const uint8_t _list_num,
                                            const float _resolution,
                                            const int32_t _offset,
                                            const uint32_t _id,
                                            const uint8_t _signal_start_bit,
                                            const uint8_t _signal_bit_len);
/** @} */ /* Callback functions. */

#ifdef __cplusplus
}
#endif

#endif /* __GEN_RESOLVE_H__ */