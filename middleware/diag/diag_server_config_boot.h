#ifndef __DIAG_SERVER_CONFIG_BOOT_H__
#define __DIAG_SERVER_CONFIG_BOOT_H__

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/**
 * @name UDS service config.
 * @{
 *
 * Service id.
 * Support physical addressing.
 * Support functional addressing.
 * Support default session.
 * Support program session.
 * Support extend session.
 * Has sub-function.
 * Support parameter.
 * Security access level.
 * Sub-function number.
 */
#define UDS_CONFIG_DIAG_SESSION_CTRL          UDS_SID_DIAG_SESSION_CTRL        ,true  ,true  ,true  ,true  ,true  ,true  ,false ,UDS_SECURITY_ACCESS_NONE ,UDS_DIAG_SESSION_CTRL_SF_NUM
#define UDS_CONFIG_ECU_RESET                  UDS_SID_ECU_RESET                ,true  ,true  ,true  ,true  ,true  ,true  ,false ,UDS_SECURITY_ACCESS_NONE ,UDS_ECU_RESET_SF_NUM
#define UDS_CONFIG_CLEAR_DIAG_INFO            UDS_SID_CLEAR_DIAG_INFO          ,true  ,true  ,true  ,true  ,true  ,false ,true  ,UDS_SECURITY_ACCESS_NONE ,0u
#define UDS_CONFIG_READ_DTC_INFO				UDS_SID_READ_DTC_INFO            ,true  ,true  ,true  ,true  ,true  ,true  ,false ,UDS_SECURITY_ACCESS_NONE ,UDS_READ_DTC_INFO_SF_NUM
#define UDS_CONFIG_READ_DATA_BY_ID			UDS_SID_READ_DATA_BY_ID          ,true  ,true  ,true  ,true  ,true  ,false ,true  ,UDS_SECURITY_ACCESS_NONE ,0u
#define UDS_CONFIG_READ_MEM_BY_ADDR			UDS_SID_READ_MEM_BY_ADDR         ,false ,false ,false ,false ,false ,false ,true  ,UDS_SECURITY_ACCESS_NONE ,0u
#define UDS_CONFIG_READ_SCALING_DATA_BY_ID	UDS_SID_READ_SCALING_DATA_BY_ID  ,false ,false ,true  ,true  ,true  ,false ,true  ,UDS_SECURITY_ACCESS_NONE ,0u
#define UDS_CONFIG_SECURITY_ACCESS			UDS_SID_SECURITY_ACCESS          ,true  ,false ,false ,true  ,false ,true  ,false ,UDS_SECURITY_ACCESS_NONE ,UDS_SECURITY_ACCESS_SF_NUM
#define UDS_CONFIG_COM_CTRL					UDS_SID_COM_CTRL                 ,true  ,true  ,false ,false ,true  ,true  ,false ,UDS_SECURITY_ACCESS_NONE ,UDS_COM_CTRL_SF_NUM
#define UDS_CONFIG_READ_DATA_BY_PERIODIC_ID	UDS_SID_READ_DATA_BY_PERIODIC_ID ,false ,false ,false ,true  ,true  ,true  ,false ,UDS_SECURITY_ACCESS_NONE ,UDS_READ_DATA_BY_PERIODICL_ID_SF_NUM
#define UDS_CONFIG_DYN_DEFINE_DATA_ID			UDS_SID_DYN_DEFINE_DATA_ID       ,false ,false ,false ,true  ,true  ,true  ,false ,UDS_SECURITY_ACCESS_NONE ,UDS_DYN_DEFINE_DATA_ID_SF_NUM
#define UDS_CONFIG_WRITE_DATA_BY_ID			UDS_SID_WRITE_DATA_BY_ID         ,true  ,false ,false ,true  ,false ,false ,true  ,UDS_SECURITY_ACCESS_NONE ,0u
#define UDS_CONFIG_IO_CTRL_BY_ID				UDS_SID_IO_CTRL_BY_ID            ,true  ,false ,false ,true  ,true  ,false ,true  ,UDS_SECURITY_ACCESS_NONE ,0u
#define UDS_CONFIG_ROUTINE_CTRL				UDS_SID_ROUTINE_CTRL             ,true  ,false ,false ,true  ,true  ,true  ,false ,UDS_SECURITY_ACCESS_NONE ,UDS_ROUTINE_CTRL_SF_NUM
#define UDS_CONFIG_REQT_DOWNLOAD				UDS_SID_REQT_DOWNLOAD            ,true  ,false ,false ,true  ,false ,false ,true  ,UDS_SECURITY_ACCESS_NONE ,0u
#define UDS_CONFIG_REQT_UPLOAD				UDS_SID_REQT_UPLOAD              ,false ,false ,false ,true  ,false ,false ,true  ,UDS_SECURITY_ACCESS_NONE ,0u
#define UDS_CONFIG_TRANSFER_DATA				UDS_SID_TRANSFER_DATA            ,true  ,false ,false ,true  ,false ,false ,true  ,UDS_SECURITY_ACCESS_NONE ,0u
#define UDS_CONFIG_REQT_TRANSFER_EXIT			UDS_SID_REQT_TRANSFER_EXIT       ,true  ,false ,false ,true  ,false ,false ,true  ,UDS_SECURITY_ACCESS_NONE ,0u
#define UDS_CONFIG_WRITE_MEM_BY_ADDR			UDS_SID_WRITE_MEM_BY_ADDR        ,false ,false ,false ,false ,false ,false ,true  ,UDS_SECURITY_ACCESS_NONE ,0u
#define UDS_CONFIG_TESTER_PRESENT				UDS_SID_TESTER_PRESENT           ,true  ,true  ,true  ,true  ,true  ,true  ,false ,UDS_SECURITY_ACCESS_NONE ,UDS_TESTER_PRESENT_SF_NUM
#define UDS_CONFIG_ACCESS_TIMING_PARAM		UDS_SID_ACCESS_TIMING_PARAM      ,false ,false ,false ,false ,false ,true  ,false ,UDS_SECURITY_ACCESS_NONE ,0u
#define UDS_CONFIG_SECURED_DATA_TRANS			UDS_SID_SECURED_DATA_TRANS       ,false ,false ,false ,false ,false ,true  ,false ,UDS_SECURITY_ACCESS_NONE ,0u
#define UDS_CONFIG_CTRL_DTC_SETTING			UDS_SID_CTRL_DTC_SETTING         ,true  ,true  ,false ,false ,true  ,true  ,false ,UDS_SECURITY_ACCESS_NONE ,UDS_CTRL_DTC_SETTING_SF_NUM
#define UDS_CONFIG_RESP_ON_EVENT				UDS_SID_RESP_ON_EVENT            ,false ,false ,false ,false ,false ,true  ,false ,UDS_SECURITY_ACCESS_NONE ,0u
#define UDS_CONFIG_LINK_CTRL					UDS_SID_LINK_CTRL                ,false ,false ,false ,false ,false ,true  ,false ,UDS_SECURITY_ACCESS_NONE ,UDS_LINK_CTRL_SF_NUM
/** @} */ /* UDS service config. */

/**
 * @name UDS sub-function config.
 * @{
 *
 * Sub-function parameter value.
 * Support the sub-function.
 * Support default session.
 * Support program session.
 * Support extend session.
 * Security access level.
 */
/** Diagnostic session control sub-function list */
#define UDS_SF_CONFIG_DIAG_SESSION_CTRL \
{UDS_DIAG_SESSION_CTRL_SF_DEFAULT_MODE ,true  ,true  ,true  ,true  ,UDS_SECURITY_ACCESS_NONE},\
{UDS_DIAG_SESSION_CTRL_SF_PROGRAM_MODE ,true  ,false ,true  ,true  ,UDS_SECURITY_ACCESS_NONE},\
{UDS_DIAG_SESSION_CTRL_SF_EXTEND_MODE  ,true  ,true  ,false ,true  ,UDS_SECURITY_ACCESS_NONE},\
{UDS_DIAG_SESSION_CTRL_SF_SAFETY_MODE  ,false ,false ,false ,false ,UDS_SECURITY_ACCESS_NONE}

/** ECU reset sub-function list */
#define UDS_SF_CONFIG_ECU_RESET \
{UDS_ECU_RESET_SF_HARD       ,true  ,true ,true ,true ,UDS_SECURITY_ACCESS_NONE},\
{UDS_ECU_RESET_SF_KEY_OFF_ON ,false ,true ,true ,true ,UDS_SECURITY_ACCESS_NONE},\
{UDS_ECU_RESET_SF_SOFT       ,true  ,true ,true ,true ,UDS_SECURITY_ACCESS_NONE}

/** Read DTC information sub-function list */
#define UDS_SF_CONFIG_READ_DTC_INFO \
{UDS_READ_DTC_INFO_SF_RPT_NUM_OF_DTC_BY_STS_MASK                  ,true  ,true ,true ,true ,UDS_SECURITY_ACCESS_NONE},\
{UDS_READ_DTC_INFO_SF_RPT_DTC_BY_STS_MASK                         ,true  ,true ,true ,true ,UDS_SECURITY_ACCESS_NONE},\
{UDS_READ_DTC_INFO_SF_RPT_DTC_SNAPSHOT_ID                         ,false ,true ,true ,true ,UDS_SECURITY_ACCESS_NONE},\
{UDS_READ_DTC_INFO_SF_RPT_DTC_SNAPSHOT_REC_BY_DTC_NUM             ,false ,true ,true ,true ,UDS_SECURITY_ACCESS_NONE},\
{UDS_READ_DTC_INFO_SF_RPT_DTC_SNAPSHOT_REC_BY_REC_NUM             ,false ,true ,true ,true ,UDS_SECURITY_ACCESS_NONE},\
{UDS_READ_DTC_INFO_SF_RPT_DTC_EXD_DATA_REC_BY_DTC_NUM             ,false ,true ,true ,true ,UDS_SECURITY_ACCESS_NONE},\
{UDS_READ_DTC_INFO_SF_RPT_NUM_OF_DTC_BY_SRV_MASK_REC              ,false ,true ,true ,true ,UDS_SECURITY_ACCESS_NONE},\
{UDS_READ_DTC_INFO_SF_RPT_DTC_BY_SRV_MASK_REC                     ,false ,true ,true ,true ,UDS_SECURITY_ACCESS_NONE},\
{UDS_READ_DTC_INFO_SF_RPT_SRV_INFO_OF_DTC                         ,false ,true ,true ,true ,UDS_SECURITY_ACCESS_NONE},\
{UDS_READ_DTC_INFO_SF_RPT_SUPPORTED_DTC                           ,true  ,true ,true ,true ,UDS_SECURITY_ACCESS_NONE},\
{UDS_READ_DTC_INFO_SF_RPT_FST_TEST_FAILED_DTC                     ,false ,true ,true ,true ,UDS_SECURITY_ACCESS_NONE},\
{UDS_READ_DTC_INFO_SF_RPT_FST_CONFIRMED_DTC                       ,false ,true ,true ,true ,UDS_SECURITY_ACCESS_NONE},\
{UDS_READ_DTC_INFO_SF_RPT_MOST_RECENT_TEST_FAILED_DTC             ,false ,true ,true ,true ,UDS_SECURITY_ACCESS_NONE},\
{UDS_READ_DTC_INFO_SF_RPT_MOST_RECENT_CONFIRMED_DTC               ,false ,true ,true ,true ,UDS_SECURITY_ACCESS_NONE},\
{UDS_READ_DTC_INFO_SF_RPT_MIR_MEM_DTC_BY_STS_MASK                 ,false ,true ,true ,true ,UDS_SECURITY_ACCESS_NONE},\
{UDS_READ_DTC_INFO_SF_RPT_MIR_MEM_DTC_EXD_DATA_REC_BY_DTC_NUM     ,false ,true ,true ,true ,UDS_SECURITY_ACCESS_NONE},\
{UDS_READ_DTC_INFO_SF_RPT_NUM_OF_MIR_MEM_DTC_BY_STS_MASK          ,false ,true ,true ,true ,UDS_SECURITY_ACCESS_NONE},\
{UDS_READ_DTC_INFO_SF_RPT_NUM_OF_EMIS_RELATED_OBD_DTC_BY_STS_MASK ,false ,true ,true ,true ,UDS_SECURITY_ACCESS_NONE},\
{UDS_READ_DTC_INFO_SF_RPT_EMIS_RELATED_OBD_DTC_BY_STS_MASK        ,false ,true ,true ,true ,UDS_SECURITY_ACCESS_NONE}

/** Security access sub-function list */
#define UDS_SF_CONFIG_SECURITY_ACCESS \
{UDS_SECURITY_ACCESS_SF_REQT_SEED_L1 ,false ,false ,true ,true  ,UDS_SECURITY_ACCESS_NONE},\
{UDS_SECURITY_ACCESS_SF_SEND_KEY_L1  ,false ,false ,true ,true  ,UDS_SECURITY_ACCESS_NONE},\
{UDS_SECURITY_ACCESS_SF_REQT_SEED_L2 ,false ,false ,true ,true  ,UDS_SECURITY_ACCESS_NONE},\
{UDS_SECURITY_ACCESS_SF_SEND_KEY_L2  ,false ,false ,true ,true  ,UDS_SECURITY_ACCESS_NONE},\
{UDS_SECURITY_ACCESS_SF_REQT_SEED_L3 ,false ,false ,true ,true  ,UDS_SECURITY_ACCESS_NONE},\
{UDS_SECURITY_ACCESS_SF_SEND_KEY_L3  ,false ,false ,true ,true  ,UDS_SECURITY_ACCESS_NONE},\
{UDS_SECURITY_ACCESS_SF_REQT_SEED_L4 ,true  ,false ,true ,false ,UDS_SECURITY_ACCESS_NONE},\
{UDS_SECURITY_ACCESS_SF_SEND_KEY_L4  ,true  ,false ,true ,false ,UDS_SECURITY_ACCESS_NONE}

/** Communication control sub-function list */ 
#define UDS_SF_CONFIG_COM_CTRL \
{UDS_COM_CTRL_SF_EN_RX_TX     ,true  ,false ,false ,true ,UDS_SECURITY_ACCESS_NONE},\
{UDS_COM_CTRL_SF_EN_RX_DIS_TX ,false ,false ,true  ,true ,UDS_SECURITY_ACCESS_NONE},\
{UDS_COM_CTRL_SF_DIS_RX_EN_TX ,false ,false ,true  ,true ,UDS_SECURITY_ACCESS_NONE},\
{UDS_COM_CTRL_SF_DIS_RX_TX    ,true  ,false ,false ,true ,UDS_SECURITY_ACCESS_NONE}

/** Read data by periodical identifier sub-function list */
#define UDS_SF_CONFIG_READ_DATA_BY_PRIODICL_ID \
{UDS_READ_DATA_BY_PERIODICL_ID_SF_SEND_AT_SLOW_RATE   ,false ,false ,true ,true ,UDS_SECURITY_ACCESS_NONE},\
{UDS_READ_DATA_BY_PERIODICL_ID_SF_SEND_AT_MEDIUM_RATE ,false ,false ,true ,true ,UDS_SECURITY_ACCESS_NONE},\
{UDS_READ_DATA_BY_PERIODICL_ID_SF_SEND_AT_FAST_RATE   ,false ,false ,true ,true ,UDS_SECURITY_ACCESS_NONE},\
{UDS_READ_DATA_BY_PERIODICL_ID_SF_STOP_SENDING        ,false ,false ,true ,true ,UDS_SECURITY_ACCESS_NONE}

/** Dynamically define data identifier sub-function list */
#define UDS_SF_CONFIG_DYN_DEFINE_DATA_BY_ID \
{UDS_DYN_DEFINE_DATA_ID_SF_DEFINE_BY_ID              ,false ,true ,true ,true ,UDS_SECURITY_ACCESS_NONE},\
{UDS_DYN_DEFINE_DATA_ID_SF_CLEAR_DYN_DEFINED_DATA_ID ,false ,true ,true ,true ,UDS_SECURITY_ACCESS_NONE}

/** Routine control sub-function list */
#define UDS_SF_CONFIG_ROUTINE_CTRL \
{UDS_ROUTINE_CTRL_SF_START        ,true  ,false ,true ,true ,UDS_SECURITY_ACCESS_NONE},\
{UDS_ROUTINE_CTRL_SF_STOP         ,false ,false ,true ,true ,UDS_SECURITY_ACCESS_NONE},\
{UDS_ROUTINE_CTRL_SF_REQT_RESULTS ,false ,false ,true ,true ,UDS_SECURITY_ACCESS_NONE}

/** Routine control identifier configuration */
#define ROUTINE_CTRL_ID_CONFIG \
{HWORD_HIGH_BYTE(UDS_RID_ERASE_MEM)            ,HWORD_LOW_BYTE(UDS_RID_ERASE_MEM)            ,false ,true  ,false ,UDS_SECURITY_ACCESS_NONE},\
{HWORD_HIGH_BYTE(UDS_RID_CHECK_PROM_DPED)      ,HWORD_LOW_BYTE(UDS_RID_CHECK_PROM_DPED)      ,false ,true  ,false ,UDS_SECURITY_ACCESS_NONE},\
{HWORD_HIGH_BYTE(UDS_RID_ERASE_MIRROR_MEM_DTCS),HWORD_LOW_BYTE(UDS_RID_ERASE_MIRROR_MEM_DTCS),false ,false ,false ,UDS_SECURITY_ACCESS_NONE},\
{HWORD_HIGH_BYTE(UDS_RID_CHECK_PROM_INTY)      ,HWORD_LOW_BYTE(UDS_RID_CHECK_PROM_INTY)      ,false ,true  ,false ,UDS_SECURITY_ACCESS_NONE},\
{HWORD_HIGH_BYTE(UDS_RID_CHECK_PROM_PREC)      ,HWORD_LOW_BYTE(UDS_RID_CHECK_PROM_PREC)      ,false ,false ,true  ,UDS_SECURITY_ACCESS_NONE},\
{HWORD_HIGH_BYTE(UDS_RID_CLEAN_TBOX_EOL)       ,HWORD_LOW_BYTE(UDS_RID_CLEAN_TBOX_EOL)       ,true  ,false ,false ,UDS_SECURITY_ACCESS_L1}

/* Tester present sub-function list */
#define UDS_SF_TESTER_PRESENT \
{UDS_TESTER_PRESENT_SF_ZERO ,true ,true ,true ,true ,UDS_SECURITY_ACCESS_NONE}

/** Control DTC setting sub-function list */
#define UDS_SF_CONFIG_CTRL_DTC_SETTING \
{UDS_CTRL_DTC_SETTING_SF_ON  ,true ,false ,false ,true ,UDS_SECURITY_ACCESS_NONE},\
{UDS_CTRL_DTC_SETTING_SF_OFF ,true ,false ,false ,true ,UDS_SECURITY_ACCESS_NONE}

/** Link control sub-function list */
#define UDS_SF_CONFIG_LINK_CTRL \
{UDS_LINK_CTRL_SF_VERIFY_BAUD_TRANS_WITH_FIXED_BAUD ,false ,false ,true ,true ,UDS_SECURITY_ACCESS_NONE},\
{UDS_LINK_CTRL_SF_VERIFY_BAUD_TRANS_WITH_SPEC_BAUD  ,false ,false ,true ,true ,UDS_SECURITY_ACCESS_NONE},\
{UDS_LINK_CTRL_SF_TRANS_BAUD                        ,false ,false ,true ,true ,UDS_SECURITY_ACCESS_NONE}
/** @} */ /* UDS sub-function config. */

#define DTC_STATUS_AVA_MASK 0x49 /**< DTC status available mask. */

/**
 * @name Read/Write data by identifier config.
 * @{
 *
 * Default session writable.
 * Default session readable.
 * Program session writable.
 * Program session readable.
 * Extend session writable.
 * Extend session readable.
 * Security access level.
 */
#define RW_DATA_CONFIG_VMSPN     false ,false ,false ,false ,false ,false ,UDS_SECURITY_ACCESS_NONE
#define RW_DATA_CONFIG_SSID      false ,false ,false ,false ,false ,false ,UDS_SECURITY_ACCESS_NONE
#define RW_DATA_CONFIG_SNOET     false ,false ,false ,false ,false ,false ,UDS_SECURITY_ACCESS_NONE
#define RW_DATA_CONFIG_SSECUHWVN false ,false ,false ,false ,false ,false ,UDS_SECURITY_ACCESS_NONE
#define RW_DATA_CONFIG_SSECUSWVN false ,false ,false ,false ,false ,false ,UDS_SECURITY_ACCESS_NONE
#define RW_DATA_CONFIG_ECUSN     false ,false ,false ,false ,false ,false ,UDS_SECURITY_ACCESS_NONE
#define RW_DATA_CONFIG_VIN       false ,false ,false ,false ,false ,false ,UDS_SECURITY_ACCESS_L1
#define RW_DATA_CONFIG_ADS       false ,false ,false ,false ,false ,false ,UDS_SECURITY_ACCESS_NONE
#define RW_DATA_CONFIG_ECUMD     false ,false ,false ,false ,false ,false ,UDS_SECURITY_ACCESS_NONE
#define RW_DATA_CONFIG_BSI       false ,false ,false ,false ,false ,false ,UDS_SECURITY_ACCESS_NONE
#define RW_DATA_CONFIG_EPSV      false ,false ,false ,false ,false ,false ,UDS_SECURITY_ACCESS_NONE
#define RW_DATA_CONFIG_TBTN      false ,false ,false ,false ,false ,false ,UDS_SECURITY_ACCESS_L3
#define RW_DATA_CONFIG_IMEI      false ,false ,false ,false ,false ,false ,UDS_SECURITY_ACCESS_L3
#define RW_DATA_CONFIG_ICCID     false ,false ,false ,false ,false ,false ,UDS_SECURITY_ACCESS_L3
#define RW_DATA_CONFIG_PK        false ,false ,false ,false ,false ,false ,UDS_SECURITY_ACCESS_L1
#define RW_DATA_CONFIG_EW        false ,false ,false ,false ,false ,false ,UDS_SECURITY_ACCESS_L1
#define RW_DATA_CONFIG_ED        false ,false ,false ,false ,false ,false ,UDS_SECURITY_ACCESS_L1
#define RW_DATA_CONFIG_CIP       false ,false ,false ,false ,false ,false ,UDS_SECURITY_ACCESS_L3
#define RW_DATA_CONFIG_LIP       false ,false ,false ,false ,false ,false ,UDS_SECURITY_ACCESS_L3
#define RW_DATA_CONFIG_EIP       false ,false ,false ,false ,false ,false ,UDS_SECURITY_ACCESS_L3
#define RW_DATA_CONFIG_FP1       false ,false ,false ,false ,false ,false ,UDS_SECURITY_ACCESS_L2
#define RW_DATA_CONFIG_FP2       false ,false ,false ,false ,false ,false ,UDS_SECURITY_ACCESS_L2
#define RW_DATA_CONFIG_EOL       false ,false ,false ,false ,false ,false ,UDS_SECURITY_ACCESS_L2
/** @} */ /* Read/Write data identifier config. */

#ifdef __cplusplus
}
#endif

#endif /* __DIAG_SERVER_CONFIG_BOOT_H__ */
