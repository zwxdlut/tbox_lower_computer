/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * THIS SOFTWARE IS PROVIDED BY NXP "AS IS" AND ANY EXPRESSED OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL NXP OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

/*!
 * @file ewm_hw_access.h
 */

#ifndef EWM_HW_ACCESS_H
#define EWM_HW_ACCESS_H

#include "ewm_driver.h"

/*!
 * @brief External Watchdog Monitor Hardware Access.
 * This is provides low-level access to all hardware features of the EWM.
 * @{
 */

/*******************************************************************************
* Definitions
******************************************************************************/


/*******************************************************************************
 * API
 ******************************************************************************/
/*!
 * @name External Watchdog Module HW
 * @{
 */

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Refresh EWM
 *
 * This method needs to be called within the window period
 * specified by the Compare Low and Compare High registers
 *
 * @param[in] base EWM base pointer
 */
static inline void EWM_Refresh(EWM_Type * const base)
{
    /* Write first byte of the service key */
    base->SERV = FEATURE_EWM_KEY_FIRST_BYTE;
    /* Write second byte of the service key */
    base->SERV = FEATURE_EWM_KEY_SECOND_BYTE;
}

/*!
 * @brief Get the EWM enable bit.
 *
 * @param[in] base EWM base pointer
 * @return The state of the device enable bit:
 *      -   false - EWM disabled
 *      -   true  - EWM enabled
 */
static inline bool EWM_IsEnabled(const EWM_Type * const base)
{
    return ((base->CTRL & EWM_CTRL_EWMEN_MASK) >> EWM_CTRL_EWMEN_SHIFT) != 0U;
}

/*!
 * @brief Set the Control Value.
 *
 * This register can be only written once after a CPU reset.
 *
 * @param[in] base  EWM base pointer
 * @param[in] value Value to write into Control register
 */
static inline void EWM_SetControl(EWM_Type * const base, uint8_t value)
{
    base->CTRL = value;
}

/*!
 * @brief Get the Control register Value.
 *
 * @param[in] base EWM base pointer
 * @return Value stored in Control register
 */
static inline uint8_t EWM_GetControl(const EWM_Type * const base)
{
    return base->CTRL;
}

/*!
 * @brief Set the Compare Low Value.
 *
 * This register can be only written once after a CPU reset.
 * The user must make sure that the Compare High is greater than Compare Low value
 *
 * @param[in] base  EWM base pointer
 * @param[in] value Value to write into Compare Low register
 */
static inline void EWM_SetCompareLow(EWM_Type * const base, uint8_t value)
{
    base->CMPL = value;
}


/*!
 * @brief Set the Compare High Value.
 *
 * This register can be only written once after a CPU reset.
 * The user must make sure that the Compare High is greater than Compare Low value
 * Note: The maximum Compare High value is 0xFE
 *
 * @param[in] base  EWM base pointer
 * @param[in] value Value to write into Compare High register
 */
static inline void EWM_SetCompareHigh(EWM_Type * const base, uint8_t value)
{
    base->CMPH = value;
}


/*!
 * @brief Set the Clock Prescaler Value.
 *
 * This register can be only written once after a CPU reset and
 * it must be written before enabling the EWM
 *
 * @param[in] base  EWM base pointer
 * @param[in] value Prescaler Value
 */
static inline void EWM_SetPrescaler(EWM_Type * const base, uint8_t value)
{
    base->CLKPRESCALER = value;
}


/*! @}*/

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif /* EWM_HW_ACCESS_H */
/*******************************************************************************
 * EOF
 ******************************************************************************/
