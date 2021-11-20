/*
 * flash_ctrl_s32k1xx.c
 *
 *  Created on: 2018��10��17��
 *      Author: Administrator
 */

#include "flash_ctrl.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Declare a FLASH config struct which initialized by FlashInit, and will be used by all flash operations. */
static flash_ssd_config_t g_flash_ssd_cfg;

/*******************************************************************************
 * Local function prototypes
 ******************************************************************************/
void CCIF_Handler(void);

/* If target is flash, insert this macro to locate callback function into RAM. */
START_FUNCTION_DECLARATION_RAMSECTION
void CCIF_Callback(void)
END_FUNCTION_DECLARATION_RAMSECTION

/*******************************************************************************
 * Functions
 ******************************************************************************/
int32_t flash_ctrl_init(void)
{
    status_t ret;        /* Store the driver APIs return code */

#if (FEATURE_FLS_HAS_PROGRAM_PHRASE_CMD == 1u)
#ifndef FLASH_TARGET
    uint8_t unsecure_key[FTFx_PHRASE_SIZE] = {0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFEu, 0xFFu, 0xFFu, 0xFFu};
#endif
#else   /* FEATURE_FLASH_HAS_PROGRAM_LONGWORD_CMD */
    uint8_t unsecure_key[FTFx_LONGWORD_SIZE] = {0xFEu, 0xFFu, 0xFFu, 0xFFu};
#endif  /* FEATURE_FLS_HAS_PROGRAM_PHRASE_CMD */

    /* Disable cache to ensure that all flash operations will take effect instantly,
     * this is device dependent */
#ifndef FLASH_TARGET
#ifdef S32K144_SERIES
    MSCM->OCMDR[0u] |= MSCM_OCMDR_OCM1(0x3u);
    MSCM->OCMDR[1u] |= MSCM_OCMDR_OCM1(0x3u);
#endif /* S32K144_SERIES */
#endif /* FLASH_TARGET */

    /* Install interrupt for Flash Command Complete event */
    INT_SYS_InstallHandler(FTFC_IRQn, CCIF_Handler, (isr_t*) 0);
    INT_SYS_EnableIRQ(FTFC_IRQn);

    /* Enable global interrupt */
    INT_SYS_EnableIRQGlobal();

    /* Always initialize the driver before calling other functions */
    ret = FLASH_DRV_Init(&Flash_InitConfig0, &g_flash_ssd_cfg);
    DEV_ASSERT(STATUS_SUCCESS == ret);

#ifdef EVB
#if ((FEATURE_FLS_HAS_FLEX_NVM == 1u) & (FEATURE_FLS_HAS_FLEX_RAM == 1u))
    /* Config FlexRAM as EEPROM if it is currently used as traditional RAM */
    if (g_flash_ssd_cfg.EEESize == 0u)
    {
#ifndef FLASH_TARGET
        /* First, erase all Flash blocks if code is placed in RAM to ensure
         * the IFR region is blank before partitioning FLexNVM and FlexRAM */
        ret = FLASH_DRV_EraseAllBlock(&g_flash_ssd_cfg);
        DEV_ASSERT(STATUS_SUCCESS == ret);

        /* Verify the erase operation at margin level value of 1 */
        ret = FLASH_DRV_VerifyAllBlock(&g_flash_ssd_cfg, 1u);
        DEV_ASSERT(STATUS_SUCCESS == ret);

        /* Reprogram secure byte in Flash configuration field */
#if (FEATURE_FLS_HAS_PROGRAM_PHRASE_CMD == 1u)
        address = 0x408u;
        size = FTFx_PHRASE_SIZE;
#else   /* FEATURE_FLASH_HAS_PROGRAM_LONGWORD_CMD == 1u */
        address = 0x40Cu;
        size = FTFx_LONGWORD_SIZE;
#endif /* FEATURE_FLS_HAS_PROGRAM_PHRASE_CMD */
        ret = FLASH_DRV_Program(&g_flash_ssd_cfg, address, size, unsecure_key);
        DEV_ASSERT(STATUS_SUCCESS == ret);
#endif /* FLASH_TARGET */

        /* Configure FlexRAM as EEPROM and FlexNVM as EEPROM backup region,
         * DEFlashPartition will be failed if the IFR region isn't blank.
         * Refer to the device document for valid EEPROM Data Size Code
         * and FlexNVM Partition Code. For example on S32K144:
         * - EEEDataSizeCode = 0x02u: EEPROM size = 4 Kbytes
         * - DEPartitionCode = 0x08u: EEPROM backup size = 64 Kbytes */
        ret = FLASH_DRV_DEFlashPartition(&g_flash_ssd_cfg, 0x02u, 0x08u, 0x0u, false, true);
        DEV_ASSERT(STATUS_SUCCESS == ret);

        /* Re-initialize the driver to update the new EEPROM configuration */
        ret = FLASH_DRV_Init(&Flash_InitConfig0, &g_flash_ssd_cfg);
        DEV_ASSERT(STATUS_SUCCESS == ret);

        /* Make FlexRAM available for EEPROM */
        ret = FLASH_DRV_SetFlexRamFunction(&g_flash_ssd_cfg, EEE_ENABLE, 0x00u, NULL);
        DEV_ASSERT(STATUS_SUCCESS == ret);
    }
    else    /* FLexRAM is already configured as EEPROM */
    {
        /* Make FlexRAM available for EEPROM, make sure that FlexNVM and FlexRAM
         * are already partitioned successfully before */
        ret = FLASH_DRV_SetFlexRamFunction(&g_flash_ssd_cfg, EEE_ENABLE, 0x00u, NULL);
        DEV_ASSERT(STATUS_SUCCESS == ret);
    }
#endif /* (FEATURE_FLS_HAS_FLEX_NVM == 1u) & (FEATURE_FLS_HAS_FLEX_RAM == 1u) */
#endif

    return ret;
}

int32_t flash_ctrl_deinit(void)
{
	INT_SYS_DisableIRQ(FTFC_IRQn);
    
	return 0;
}

int32_t flash_ctrl_erase_sector(const uint32_t _addr, const uint32_t _size)
{
    status_t ret;        /* Store the driver APIs return code */
    flash_callback_t pCallBack;

    /* Set callback function before a long time consuming flash operation
     * (ex: erasing) to let the application code do other tasks while flash
     * in operation. In this case we use it to enable interrupt for
     * Flash Command Complete event */
    pCallBack = (flash_callback_t)CCIF_Callback;
    g_flash_ssd_cfg.CallBack = pCallBack;

    INT_SYS_DisableIRQGlobal();
    /* Erase PFlash sector */
    ret = FLASH_DRV_EraseSector(&g_flash_ssd_cfg, _addr, _size);
    INT_SYS_EnableIRQGlobal();

    /* Disable Callback */
    g_flash_ssd_cfg.CallBack = NULL_CALLBACK;

    DEV_ASSERT(STATUS_SUCCESS == ret);

    return ret;
}

int32_t flash_ctrl_verify_sector(const uint32_t _addr, const uint32_t _size)
{
    status_t ret;        /* Store the driver APIs return code */

    INT_SYS_DisableIRQGlobal();
    /* Verify the erase operation at margin level value of 1, user read */
    ret = FLASH_DRV_VerifySection(&g_flash_ssd_cfg, _addr, _size / FTFx_DPHRASE_SIZE, 1u);
    INT_SYS_EnableIRQGlobal();

    DEV_ASSERT(STATUS_SUCCESS == ret);

    return ret;
}

bool flash_ctrl_is_sector_aligned(const uint32_t _addr)
{
	return (0 == (_addr - FLASH_BASE_ADDR) % FLASH_SECTOR_SIZE );
}

int32_t flash_ctrl_program(const uint32_t _addr, const uint32_t _size, const uint8_t _buf[])
{
    status_t ret;        /* Store the driver APIs return code */

    INT_SYS_DisableIRQGlobal();
    ret = FLASH_DRV_Program(&g_flash_ssd_cfg, _addr, _size, _buf);
    INT_SYS_EnableIRQGlobal();

    DEV_ASSERT(STATUS_SUCCESS == ret);

    return ret;
}

int32_t flash_ctrl_program_verify(const uint32_t _addr, const uint32_t _size, const uint8_t _buf[])
{
    status_t ret;        /* Store the driver APIs return code */
    uint32_t fail_addr;

    INT_SYS_DisableIRQGlobal();
    /* Verify the program operation at margin level value of 1, user margin */
    ret = FLASH_DRV_ProgramCheck(&g_flash_ssd_cfg, _addr, _size, _buf, &fail_addr, 1u);
    INT_SYS_EnableIRQGlobal();

    DEV_ASSERT(STATUS_SUCCESS == ret);

    return ret;
}

int32_t flash_ctrl_write_e2(const uint32_t _addr, const uint32_t _size, const uint8_t _buf[])
{
    status_t ret = STATUS_SUCCESS;        /* Store the driver APIs return code */

#ifdef EVB
    /* Try to write data to EEPROM if FlexRAM is configured as EEPROM */
    if (g_flash_ssd_cfg.EEESize != 0u)
    {
        ret = FLASH_DRV_EEEWrite(&g_flash_ssd_cfg, _addr + g_flash_ssd_cfg.EERAMBase, _size, _buf);
        DEV_ASSERT(STATUS_SUCCESS == ret);

        /* Verify the written data */
        uint32_t i = 0;
        while (i < _size )
        {
        	if (*((uint8_t*)(_buf + i)) != *((uint8_t*)(_addr + g_flash_ssd_cfg.EERAMBase + i)))
        	{
        		DEV_ASSERT(false);
        	}
        	i++;
        }
    }
    else
    {
#if (FEATURE_FLS_HAS_FLEX_NVM == 1u)
    	uint32_t fail_addr;
    	uint32_t _addr;
    	uint32_t size;

        /* Erase a sector in DFlash */
    	_addr = g_flash_ssd_cfg.DFlashBase;
        size = 2 * FEATURE_FLS_DF_BLOCK_SECTOR_SIZE;
        ret = FLASH_DRV_EraseSector(&g_flash_ssd_cfg, _addr, size);
        DEV_ASSERT(STATUS_SUCCESS == ret);

        /* Verify the erase operation at margin level value of 1, user read */
        ret = FLASH_DRV_VerifySection(&g_flash_ssd_cfg, _addr, size / FTFx_PHRASE_SIZE, 1u);
        DEV_ASSERT(STATUS_SUCCESS == ret);

        /* Write some data to the erased DFlash sector */
        _addr = g_flash_ssd_cfg.DFlashBase + _addr - g_flash_ssd_cfg.EERAMBase;
        size = _size;
        ret = FLASH_DRV_Program(&g_flash_ssd_cfg, _addr, size, _buf);
        DEV_ASSERT(STATUS_SUCCESS == ret);

        /* Verify the program operation at margin level value of 1, user margin */
        ret = FLASH_DRV_ProgramCheck(&g_flash_ssd_cfg, _addr, size, _buf, &fail_addr, 1u);
        DEV_ASSERT(STATUS_SUCCESS == ret);
#endif /* FEATURE_FLS_HAS_FLEX_NVM */
    }
#else
    (void)_addr;
    (void)_size;
    (void)_buf;
#endif

    return ret;
}

/*******************************************************************************
 * Local function prototypes
 ******************************************************************************/
/**
 * IRQ handler for Flash Command Complete event.
 */
void CCIF_Handler(void)
{
    /* Disable Flash Command Complete interrupt */
    FTFx_FCNFG &= (~FTFx_FCNFG_CCIE_MASK);

    return;
}

/**
 * Callback function for Flash operations.
 */
START_FUNCTION_DEFINITION_RAMSECTION
void CCIF_Callback(void)
{
    /* Enable interrupt for Flash Command Complete */
    if ((FTFx_FCNFG & FTFx_FCNFG_CCIE_MASK) == 0u)
    {
        FTFx_FCNFG |= FTFx_FCNFG_CCIE_MASK;
    }
}
END_FUNCTION_DEFINITION_RAMSECTION

