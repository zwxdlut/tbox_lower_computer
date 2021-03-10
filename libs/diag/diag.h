#ifndef __DIAG_H__
#define __DIAG_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*******************************************************************************
 *  Function prototypes
 ******************************************************************************/
/**
 * @brief Swap bytes.
 *
 * @param [in] _buf  Bytes buffer.
 * @param [in] _size Bytes size.
 */
void bytesswap(void *const _buf, const uint16_t _size);

#ifdef __cplusplus
}
#endif

#endif /* __DIAG_H__ */
