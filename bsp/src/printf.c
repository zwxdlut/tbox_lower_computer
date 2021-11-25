/*
 * Copyright (C) 2008 Freescale Corporation.  All rights reserved.
 *
 */

/*    Routines
 *    --------
 *
 *        printf
 */

#include <ansi_parms.h>

#include <ewl_misra_types.h>
#include <critical_regions.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#if _EWL_WIDE_CHAR
#include <wchar.h>
#endif
#include <console_io.h>

#if _EWL_CONSOLE_SUPPORT
MISRA_EXCEPTION_RULE_16_1()
int_t _EWL_CDECL printf(const char_t * _EWL_RESTRICT format, ...)
{
    int_t result;
    va_list args;

    #if _EWL_WIDE_CHAR
    #if _EWL_C99_PRINTF_SCANF
    if (fwide(stdout, -1) >= 0)
    {
		MISRA_EXCEPTION_RULE_14_7()
        return(-1);
    }
    #endif /*_EWL_C99_PRINTF_SCANF */
    #endif /* _EWL_WIDE_CHAR && _EWL_C99 */

    va_start(args, format);

    __begin_critical_region(stdout_access);
    result = __pformatter(__FileWrite, (void *)stdout, format, args, 0);
    __end_critical_region(stdout_access);

    return(result);
}
#endif /* _EWL_CONSOLE_SUPPORT */
