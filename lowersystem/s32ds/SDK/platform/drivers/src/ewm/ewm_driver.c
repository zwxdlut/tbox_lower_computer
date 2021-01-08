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

/**
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 8.7, External could be made static.
 * The function is defined for use by application code.
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 11.6, A cast shall not be performed
 * between pointer to void and an arithmetic type.
 * The base addresses are provided as integers so they need to be cast to
 * pointers.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 11.4, A conversion should not be performed
 * between a pointer to an object and an integer type.
 * The base address from HAL functions is provided as integer so a conversion between
 * a pointer and an integer has to be performed.
 */

#include <stddef.h>
#include "ewm_hw_access.h"
#include "interrupt_manager.h"

/* @brief Local table used to store EWM base pointers */
static EWM_Type * const s_ewmBase[] = EWM_BASE_PTRS;

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @name External Watchdog Monitor Driver
 * @{
 */

/*FUNCTION**********************************************************************
 *
 * Function Name : EWM_DRV_Init
 * Description   : This function initializes the EWM instance to a specified
 * state
 *
 * Implements    : EWM_DRV_Init_Activity
 *END**************************************************************************/
status_t EWM_DRV_Init(uint32_t instance, const ewm_init_config_t * config)
{
    DEV_ASSERT(instance < EWM_INSTANCE_COUNT);
    DEV_ASSERT(config != NULL);

    /* Return status variable */
    status_t statusCode = STATUS_SUCCESS;
    /* Flag to store if the module is enabled */
    bool isModuleEnabled;
    uint8_t tempValue = 0U;
    /* Base pointer */
    EWM_Type * base = s_ewmBase[instance];

    /* Get the enablement status of the module */
    isModuleEnabled = EWM_IsEnabled(base);
    /* Check if the EWM instance is already enabled or if the windows values are not correct */
    if ((isModuleEnabled == true) || (config->compareHigh <= config->compareLow) ||
        (config->compareHigh > FEATURE_EWM_CMPH_MAX_VALUE))
    {
        /* If conditions are met change the status code to error */
        statusCode = STATUS_ERROR;
    }
    else
    {
        /* Set clock prescaler */
        EWM_SetPrescaler(base, config->prescaler);
        /* Set compare high and low values */
        EWM_SetCompareHigh(base, config->compareHigh);
        EWM_SetCompareLow(base, config->compareLow);

        /* Configure the Control register and enable the instance.
         * Set the values that are not affected by the input pin
         */
        tempValue = (uint8_t)((uint8_t)EWM_CTRL_EWMEN(1U) | EWM_CTRL_INTEN(config->interruptEnable ? 1U : 0U));

        /* Depending how the input pin is configured set the values into the
         * temporary variable
         */
        switch (config->assertLogic)
        {
            case EWM_IN_ASSERT_ON_LOGIC_ZERO:
                tempValue |= EWM_CTRL_INEN_MASK;    /* Input pin enabled, Input asserted on logic 0 */
                break;
            case EWM_IN_ASSERT_ON_LOGIC_ONE:
                tempValue |= (uint8_t)(EWM_CTRL_INEN_MASK | EWM_CTRL_ASSIN_MASK);    /* Input pin enabled, Input asserted on logic 1 */
                break;
            case EWM_IN_ASSERT_DISABLED:
            default:
                /* Input pin disabled */
                break;
        }

        /* Write the configuration into the Control register */
        EWM_SetControl(base, tempValue);
    }

    /* Return the status code */
    return statusCode;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : EWM_DRV_GetDefaultConfig
 * Description   : This function initializes the EWM configuration structure
 * to default values
 *
 * Implements    : EWM_DRV_GetDefaultConfig_Activity
 *END**************************************************************************/
void EWM_DRV_GetDefaultConfig(ewm_init_config_t * config)
{
    DEV_ASSERT(config != NULL);

    /* Disable interrupts */
    config->interruptEnable = false;
    /* Input pin enabled and configured to assert on logic 0 */
    config->assertLogic     = EWM_IN_ASSERT_ON_LOGIC_ZERO;
    /* Maximum prescaler */
    config->prescaler       = 255U;
    /* Maximum service window */
    config->compareHigh     = FEATURE_EWM_CMPH_MAX_VALUE;
    config->compareLow      = FEATURE_EWM_CMPL_MIN_VALUE;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : EWM_DRV_Refresh
 * Description   : This function refreshes the EWM instance counter
 *
 * Implements    : EWM_DRV_Refresh_Activity
 *END**************************************************************************/
void EWM_DRV_Refresh(uint32_t instance)
{
    DEV_ASSERT(instance < EWM_INSTANCE_COUNT);

    /* Base pointer */
    EWM_Type * base = s_ewmBase[instance];

    /* Disable global IRQ's to avoid disturbing the refresh process */
    INT_SYS_DisableIRQGlobal();
    /* Write the refresh values */
    EWM_Refresh(base);
    /* Re-enable all IRQ's */
    INT_SYS_EnableIRQGlobal();
}

/*FUNCTION**********************************************************************
 *
 * Function Name : EWM_DRV_GetInputPinAssertLogic
 * Description   : Get the Input pin assert logic
 *
 * Implements : EWM_DRV_GetInputPinAssertLogic_Activity
 *END**************************************************************************/
ewm_in_assert_logic_t EWM_DRV_GetInputPinAssertLogic(uint32_t instance)
{
    DEV_ASSERT(instance < EWM_INSTANCE_COUNT);

    /* Base pointer */
    const EWM_Type * const base = s_ewmBase[instance];
    /* Variable where to save the retrieved configuration */
    ewm_in_assert_logic_t returnValue;

    /* Check if input pin is enabled */
    if ((EWM_GetControl(base) & EWM_CTRL_INEN_MASK) != 0U)
    {
       /* If true get the assert logic into the returnValue variable */
        returnValue = (((EWM_GetControl(base) & EWM_CTRL_ASSIN_MASK) != 0U)
                       ? EWM_IN_ASSERT_ON_LOGIC_ONE : EWM_IN_ASSERT_ON_LOGIC_ZERO);
    }
    else
    {
        /* Pin is disabled, return the corresponding value */
        returnValue = EWM_IN_ASSERT_DISABLED;
    }

    return returnValue;
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
