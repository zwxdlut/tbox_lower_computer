/*
 * Copyright (c) 2014 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
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
 * @lpi2c_irq.c
 *
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 5.1, Identifier clash
 * The supported compilers use more than 31 significant characters for identifiers.
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 5.2, Identifier clash
 * The supported compilers use more than 31 significant characters for identifiers.
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 5.4, Identifier clash
 * The supported compilers use more than 31 significant characters for identifiers.
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 5.5, Identifier clash
 * The supported compilers use more than 31 significant characters for identifiers.
 *
 * @section [global]
 * Violates MISRA 2012 Required Rule 8.4, A compatible declaration shall be
 * visible when an object or function with external linkage is defined.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 8.7, External could be made static.
 * Function is defined for usage by application code.
 */

#include "device_registers.h"
#include "lpi2c_driver.h"
#include "lpi2c_irq.h"

/*******************************************************************************
 * Code
 ******************************************************************************/

#if defined (S32K11x_SERIES)
/* Implementation of LPI2C0 master and slave handler named in startup code. */
void LPI2C0_Master_Slave_IRQHandler(void)
{
	LPI2C_DRV_ModuleIRQHandler(0);
}

#else
#if (LPI2C_INSTANCE_COUNT > 0u)
/* Implementation of LPI2C0 master handler named in startup code. */
void LPI2C0_Master_IRQHandler(void)
{
	LPI2C_DRV_MasterIRQHandler(0);
}

/* Implementation of LPI2C0 slave handler named in startup code. */
void LPI2C0_Slave_IRQHandler(void)
{
	LPI2C_DRV_SlaveIRQHandler(0);
}

#if(LPI2C_INSTANCE_COUNT == 2u)

/* Implementation of LPI2C1 master handler named in startup code. */
void LPI2C1_Master_IRQHandler(void)
{
	LPI2C_DRV_MasterIRQHandler(1);
}

/* Implementation of LPI2C1 slave handler named in startup code. */
void LPI2C1_Slave_IRQHandler(void)
{
	LPI2C_DRV_SlaveIRQHandler(1);
}

#endif

#endif

#endif
/*******************************************************************************
 * EOF
 ******************************************************************************/
