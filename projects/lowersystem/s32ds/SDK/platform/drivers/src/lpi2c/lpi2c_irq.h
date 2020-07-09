/* 
 * Copyright 2018 NXP
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

#ifndef LPI2C_IRQ_H__
#define LPI2C_IRQ_H__

#include "device_registers.h"

/*******************************************************************************
 * Code
 ******************************************************************************/

#if defined (S32K11x_SERIES)

/* LPI2C0 master and slave handler named in startup code. */
void LPI2C0_Master_Slave_IRQHandler(void);

#else

#if (LPI2C_INSTANCE_COUNT > 0u)
/* LPI2C0 master handler named in startup code. */
void LPI2C0_Master_IRQHandler(void);

/* LPI2C0 slave handler named in startup code. */
void LPI2C0_Slave_IRQHandler(void);

#if(LPI2C_INSTANCE_COUNT == 2u)

/* LPI2C1 master handler named in startup code. */
void LPI2C1_Master_IRQHandler(void);

/* LPI2C1 slave handler named in startup code. */
void LPI2C1_Slave_IRQHandler(void);

#endif /* LPI2C_INSTANCE_COUNT == 2u */

#endif /* LPI2C_INSTANCE_COUNT > 0u */

#endif /* endif cpu defined */

#endif /* LPI2C_IRQ_H__ */
/*******************************************************************************
 * EOF
 ******************************************************************************/
