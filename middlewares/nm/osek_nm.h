#ifndef __OSEK_NM_H__
#define __OSEK_NM_H__

#include "osek_nm_define.h"

#ifdef __cplusplus
extern "C" {
#endif

void osek_nm_init(void);
void osek_nm_uninit(void);
void osek_nm_time_callback(void);
void osek_nm_indication(const char id, const unsigned char* const buf);

#ifdef __cplusplus
}
#endif
#endif

