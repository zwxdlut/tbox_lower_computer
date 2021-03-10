#include "diag_server.h"

#include <string.h>
#include <assert.h>

#if defined USING_OS_FREERTOS
#include "FreeRTOS.h"
#include "semphr.h"
#endif

#ifdef BOOT
#include "diag_server_config_boot.h"
#else
#include "diag_server_config_app.h"
#endif
#include "i2c.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/**
 * @name Callbacks.
 * @{
 */
static sys_time_callback_t g_sys_time_callback;
static debug_callback_t    g_debug_callback;
/** @} */ /* Callbacks. */

/**
 * @name Application layer timer parameter.
 * @{
 */
static bool     g_s3_server_timer_started = false;
static uint32_t g_s3_server_timer = 0;
/** @} */ /* Application layer timer parameter. */

/**
 * @name Sub-function defines.
 * @{
 */
/**
 * @brief Sub-function.
 */
typedef struct
{
	uint8_t param_;
	uint8_t spd_ : 1;
	uint8_t dft_ses_spd_ : 1;
	uint8_t pro_ses_spd_ : 1;
	uint8_t exd_ses_spd_ : 1;
	uint8_t sec_acc_lev_;
}sub_function_t;

static sub_function_t g_sf_diag_session_ctrl[]         = {UDS_SF_CONFIG_DIAG_SESSION_CTRL};
static sub_function_t g_sf_ecu_reset[]                 = {UDS_SF_CONFIG_ECU_RESET};
static sub_function_t g_sf_dtc_info[]                  = {UDS_SF_CONFIG_READ_DTC_INFO};
static sub_function_t g_sf_security_access[]           = {UDS_SF_CONFIG_SECURITY_ACCESS};
static sub_function_t g_sf_comm_ctrl[]                 = {UDS_SF_CONFIG_COM_CTRL};
static sub_function_t g_sf_read_data_by_periodicl_id[] = {UDS_SF_CONFIG_READ_DATA_BY_PRIODICL_ID};
static sub_function_t g_sf_dyn_define_data_id[]        = {UDS_SF_CONFIG_DYN_DEFINE_DATA_BY_ID};
static sub_function_t g_sf_routine_ctrl[]              = {UDS_SF_CONFIG_ROUTINE_CTRL};
static sub_function_t g_sf_tester_present[]            = {UDS_SF_TESTER_PRESENT};
static sub_function_t g_sf_ctrl_dtc_setting[]          = {UDS_SF_CONFIG_CTRL_DTC_SETTING};
static sub_function_t g_sf_link_ctrl_setting[]         = {UDS_SF_CONFIG_LINK_CTRL};
/** @} */ /* Sub-function defines */

/**
 * @name Services defines.
 * @{
 */
/**
 * @brief Service.
 */
typedef struct
{
	uint8_t        sid_;
	uint8_t        phys_addr_spd_ : 1;
	uint8_t        func_addr_spd_ : 1;
	uint8_t        dft_ses_spd_ : 1;
	uint8_t        pro_ses_spd_ : 1;
	uint8_t        exd_ses_spd_ : 1;
	uint8_t        with_sf_ : 1;
	uint8_t        param_spd_ : 1;
	uint8_t        sec_acc_lev_;
	uint8_t        sf_num_;
	sub_function_t *sf_;
}service_t;

static service_t g_service[] =
{
	{UDS_CONFIG_DIAG_SESSION_CTRL        ,g_sf_diag_session_ctrl},
	{UDS_CONFIG_ECU_RESET                ,g_sf_ecu_reset},
	{UDS_CONFIG_CLEAR_DIAG_INFO          ,NULL},
	{UDS_CONFIG_READ_DTC_INFO			 ,g_sf_dtc_info},
	{UDS_CONFIG_READ_DATA_BY_ID			 ,NULL},
	{UDS_CONFIG_READ_MEM_BY_ADDR		 ,NULL},
	{UDS_CONFIG_READ_SCALING_DATA_BY_ID	 ,NULL},
	{UDS_CONFIG_SECURITY_ACCESS			 ,g_sf_security_access},
	{UDS_CONFIG_COM_CTRL				 ,g_sf_comm_ctrl},
	{UDS_CONFIG_READ_DATA_BY_PERIODIC_ID ,g_sf_read_data_by_periodicl_id},
	{UDS_CONFIG_DYN_DEFINE_DATA_ID	     ,g_sf_dyn_define_data_id},
	{UDS_CONFIG_WRITE_DATA_BY_ID	     ,NULL},
	{UDS_CONFIG_IO_CTRL_BY_ID		     ,NULL},
	{UDS_CONFIG_ROUTINE_CTRL		     ,g_sf_routine_ctrl},
	{UDS_CONFIG_REQT_DOWNLOAD			 ,NULL},
	{UDS_CONFIG_REQT_UPLOAD				 ,NULL},
	{UDS_CONFIG_TRANSFER_DATA			 ,NULL},
	{UDS_CONFIG_REQT_TRANSFER_EXIT		 ,NULL},
	{UDS_CONFIG_WRITE_MEM_BY_ADDR		 ,NULL},
	{UDS_CONFIG_TESTER_PRESENT			 ,g_sf_tester_present},
	{UDS_CONFIG_ACCESS_TIMING_PARAM		 ,NULL},
	{UDS_CONFIG_SECURED_DATA_TRANS		 ,NULL},
	{UDS_CONFIG_CTRL_DTC_SETTING		 ,g_sf_ctrl_dtc_setting},
	{UDS_CONFIG_RESP_ON_EVENT			 ,NULL},
	{UDS_CONFIG_LINK_CTRL				 ,g_sf_link_ctrl_setting}
};
/** @} */ /* Services defines. */

/**
 * @name Diagnostic session control.
 * @{
 */
static uint8_t g_session_mode = UDS_DIAG_SESSION_CTRL_SF_DEFAULT_MODE;
/** @} */ /* Diagnostic session control. */

/**
 * @name DTC.
 * @{
 */
/**
 * @brief DTC information.
 */
typedef struct
{
	uint8_t high_byte_;
	uint8_t mid_byte_;
	uint8_t low_byte_;
	uint8_t status_;
}dtc_info_t;

static dtc_info_t g_dtc_info[] =
{
	{WORD_BYTE2(DTC_BAT_UNDER_VOL    ), WORD_BYTE1(DTC_BAT_UNDER_VOL    ), WORD_BYTE0(DTC_BAT_UNDER_VOL    ), 0},
	{WORD_BYTE2(DTC_BAT_OVER_VOL     ), WORD_BYTE1(DTC_BAT_OVER_VOL     ), WORD_BYTE0(DTC_BAT_OVER_VOL     ), 0},
	{WORD_BYTE2(DTC_SIM_UNPRESENT	 ), WORD_BYTE1(DTC_SIM_UNPRESENT    ), WORD_BYTE0(DTC_SIM_UNPRESENT	   ), 0}, 
	{WORD_BYTE2(DTC_GPS_ANTENNA_LOST ), WORD_BYTE1(DTC_GPS_ANTENNA_LOST ), WORD_BYTE0(DTC_GPS_ANTENNA_LOST ), 0}, 
	{WORD_BYTE2(DTC_BAT_INVALID_VOL  ), WORD_BYTE1(DTC_BAT_INVALID_VOL  ), WORD_BYTE0(DTC_BAT_INVALID_VOL  ), 0}, 
	{WORD_BYTE2(DTC_NM_LIMP		     ), WORD_BYTE1(DTC_NM_LIMP	        ), WORD_BYTE0(DTC_NM_LIMP		   ), 0}, 
	{WORD_BYTE2(DTC_COMM_ACC_LOST	 ), WORD_BYTE1(DTC_COMM_ACC_LOST	), WORD_BYTE0(DTC_COMM_ACC_LOST    ), 0}, 
	{WORD_BYTE2(DTC_COMM_EMS_LOST	 ), WORD_BYTE1(DTC_COMM_EMS_LOST	), WORD_BYTE0(DTC_COMM_EMS_LOST	   ), 0}, 
	{WORD_BYTE2(DTC_COMM_ABS_ESP_LOST), WORD_BYTE1(DTC_COMM_ABS_ESP_LOST), WORD_BYTE0(DTC_COMM_ABS_ESP_LOST), 0}, 
	{WORD_BYTE2(DTC_COMM_SRS_LOST	 ), WORD_BYTE1(DTC_COMM_SRS_LOST	), WORD_BYTE0(DTC_COMM_SRS_LOST	   ), 0}, 
	{WORD_BYTE2(DTC_COMM_TPMS_LOST   ), WORD_BYTE1(DTC_COMM_TPMS_LOST   ), WORD_BYTE0(DTC_COMM_TPMS_LOST   ), 0}, 
	{WORD_BYTE2(DTC_COMM_BCM_LOST	 ), WORD_BYTE1(DTC_COMM_BCM_LOST	), WORD_BYTE0(DTC_COMM_BCM_LOST	   ), 0}, 
	{WORD_BYTE2(DTC_COMM_AC_LOST	 ), WORD_BYTE1(DTC_COMM_AC_LOST	    ), WORD_BYTE0(DTC_COMM_AC_LOST	   ), 0}, 
	{WORD_BYTE2(DTC_COMM_PEPS_LOST   ), WORD_BYTE1(DTC_COMM_PEPS_LOST   ), WORD_BYTE0(DTC_COMM_PEPS_LOST   ), 0}, 
	{WORD_BYTE2(DTC_COMM_MP5_LOST	 ), WORD_BYTE1(DTC_COMM_MP5_LOST	), WORD_BYTE0(DTC_COMM_MP5_LOST	   ), 0}, 
	{WORD_BYTE2(DTC_COMM_VCU_LOST	 ), WORD_BYTE1(DTC_COMM_VCU_LOST	), WORD_BYTE0(DTC_COMM_VCU_LOST	   ), 0}, 
	{WORD_BYTE2(DTC_COMM_BMS_LOST	 ), WORD_BYTE1(DTC_COMM_BMS_LOST	), WORD_BYTE0(DTC_COMM_BMS_LOST	   ), 0}, 
	{WORD_BYTE2(DTC_COMM_MCU_LOST	 ), WORD_BYTE1(DTC_COMM_MCU_LOST	), WORD_BYTE0(DTC_COMM_MCU_LOST	   ), 0}, 
	{WORD_BYTE2(DTC_COMM_OBC_LOST	 ), WORD_BYTE1(DTC_COMM_OBC_LOST	), WORD_BYTE0(DTC_COMM_OBC_LOST	   ), 0}, 
	{WORD_BYTE2(DTC_COMM_DCDC_LOST   ), WORD_BYTE1(DTC_COMM_DCDC_LOST   ), WORD_BYTE0(DTC_COMM_DCDC_LOST   ), 0}, 
	{WORD_BYTE2(DTC_COMM_AC_COMP_LOST), WORD_BYTE1(DTC_COMM_AC_COMP_LOST), WORD_BYTE0(DTC_COMM_AC_COMP_LOST), 0}
};

/**
 * @brief Message time.
 */
typedef struct
{
	uint32_t   lost_time_;
	uint32_t   recovery_time_;
	uint16_t   dtc_info_index_;
	uint32_t   msg_time_num_;
	uint32_t   *msg_time_;
	bool       is_lost_;
}msg_time_t;

static uint32_t g_acc_time[MSG_NUM_ACC];
static uint32_t g_ems_time[MSG_NUM_EMS];
static uint32_t g_abs_esp_time[MSG_NUM_ABS_ESP];
static uint32_t g_srs_time[MSG_NUM_SRS];
static uint32_t g_tpms_time[MSG_NUM_TPMS];
static uint32_t g_bcm_time[MSG_NUM_BCM];
static uint32_t g_ac_time[MSG_NUM_AC];
static uint32_t g_peps_time[MSG_NUM_PEPS];
static uint32_t g_mp5_time[MSG_NUM_MP5];
static uint32_t g_vcu_time[MSG_NUM_VCU];
static uint32_t g_bms_time[MSG_NUM_BMS];
static uint32_t g_mcu_time[MSG_NUM_MCU];
static uint32_t g_obc_time[MSG_NUM_OBC];
static uint32_t g_dcdc_time[MSG_NUM_DCDC];
static uint32_t g_ac_comp_time[MSG_NUM_AC_COMP];

static msg_time_t g_msg_time[] =
{
	{LOST_TIME_ACC     ,RECOVERY_TIME_ACC     ,6  ,MSG_NUM_ACC     ,g_acc_time     ,false},
	{LOST_TIME_EMS     ,RECOVERY_TIME_EMS     ,7  ,MSG_NUM_EMS     ,g_ems_time     ,false},
	{LOST_TIME_ABS_ESP ,RECOVERY_TIME_ABS_ESP ,8  ,MSG_NUM_ABS_ESP ,g_abs_esp_time ,false},
	{LOST_TIME_SRS     ,RECOVERY_TIME_SRS     ,9  ,MSG_NUM_SRS     ,g_srs_time     ,false},
	{LOST_TIME_TPMS    ,RECOVERY_TIME_TPMS    ,10 ,MSG_NUM_TPMS    ,g_tpms_time    ,false},
	{LOST_TIME_BCM     ,RECOVERY_TIME_BCM     ,11 ,MSG_NUM_BCM     ,g_bcm_time     ,false},
	{LOST_TIME_AC      ,RECOVERY_TIME_AC      ,12 ,MSG_NUM_AC      ,g_ac_time      ,false},
	{LOST_TIME_PEPS    ,RECOVERY_TIME_PEPS    ,13 ,MSG_NUM_PEPS    ,g_peps_time    ,false},
	{LOST_TIME_MP5     ,RECOVERY_TIME_MP5     ,14 ,MSG_NUM_MP5     ,g_mp5_time     ,false},
	{LOST_TIME_VCU     ,RECOVERY_TIME_VCU     ,15 ,MSG_NUM_VCU     ,g_vcu_time     ,false},
	{LOST_TIME_BMS     ,RECOVERY_TIME_BMS     ,16 ,MSG_NUM_BMS     ,g_bms_time     ,false},
	{LOST_TIME_MCU     ,RECOVERY_TIME_MCU     ,17 ,MSG_NUM_MCU     ,g_mcu_time     ,false},
	{LOST_TIME_OBC     ,RECOVERY_TIME_OBC     ,18 ,MSG_NUM_OBC     ,g_obc_time     ,false},
	{LOST_TIME_DCDC    ,RECOVERY_TIME_DCDC    ,19 ,MSG_NUM_DCDC    ,g_dcdc_time    ,false},
	{LOST_TIME_AC_COMP ,RECOVERY_TIME_AC_COMP ,20 ,MSG_NUM_AC_COMP ,g_ac_comp_time ,false}
};
/** @} */ /* DTC. */

/**
 * @name DTC Snapshot.
 * @{
 */
/**
 * @brief Data.
 */
typedef struct
{
	uint8_t  size_;
	uint8_t  *buf_;
}data_t;

/**
 * @brief Data with identifier.
 */
typedef struct
{
	uint8_t id_msb_;
	uint8_t id_lsb_;
	data_t  data_;
}did_t;

/**
 * @brief DTC snapshot record.
 */
typedef struct
{
	uint8_t did_num_;
	did_t  *did_;
}dtc_snapshot_t;

static uint16_t g_dtc_snapshot_vs   = 0;
static uint16_t g_dtc_snapshot_ecuv = 120;

static did_t g_dtc_snapshot01_did[] =
{
	{HWORD_HIGH_BYTE(VSDID)   ,HWORD_LOW_BYTE(VSDID)   ,{VS_LEN   ,(uint8_t*)(&g_dtc_snapshot_vs)}},
	{HWORD_HIGH_BYTE(ECUVDID) ,HWORD_LOW_BYTE(ECUVDID) ,{ECUV_LEN ,(uint8_t*)(&g_dtc_snapshot_ecuv)}}
};

static dtc_snapshot_t g_dtc_snapshot[] =
{
	{DTC_SNAPSHOT01_DID_NUM, g_dtc_snapshot01_did}
};
/** @} */ /* DTC Snapshot. */

/**
 * @name DTC extend data.
 * @{
 */
static uint8_t g_dtc_extend_data01[] = {0, 0};

static data_t g_dtc_extend_data[] =
{
	{DTC_EXTEND_DATA01_LEN, g_dtc_extend_data01}
};
/** @} */ /* DTC extend data. */

#if defined USING_OS_FREERTOS
static SemaphoreHandle_t g_dtc_mutex = NULL;
#endif

/**
 * @name Read/Write data by identifier.
 * @{
 */
/**
 * @brief Read/Write data.
 */
typedef struct
{
	uint8_t  dft_ses_writable_ : 1;
	uint8_t  dft_ses_readable_ : 1;
	uint8_t  pro_ses_writable_ : 1;
	uint8_t  pro_ses_readable_ : 1;
	uint8_t  exd_ses_writable_ : 1;
	uint8_t  exd_ses_readable_ : 1;
	uint8_t  sec_acc_lev_;
	did_t    did_;
}rw_did_t;

static uint8_t  g_vmspn[VMSPN_LEN]         = VMSPN;
static uint8_t  g_ssid[SSID_LEN]           = SSID;
static uint8_t  g_snoet[SNOET_LEN]         = SNOET;
static uint8_t  g_ssecuhwvn[SSECUHWVN_LEN] = SSECUHWVN;
static uint8_t  g_ssecuswvn[SSECUSWVN_LEN] = SSECUSWVN;
static uint8_t  g_ecusn[ECUSN_LEN]         = ECUSN;
static uint8_t  g_vin[VIN_LEN]             = VIN;
static uint32_t g_ecumd                    = ECUMD;
static uint8_t  g_bsi[BSI_LEN]             = BSI;
static uint8_t  g_epsv                     = 120;
static uint8_t  g_tbtn[TBTN_LEN]           = TBTN;
static uint8_t  g_imei[IMEI_LEN]           = IMEI;
static uint8_t  g_iccid[ICCID_LEN]         = ICCID;
static uint8_t  g_pk[PK_LEN]               = PK;
static uint8_t  g_esk[ESK_LEN]             = ESK;
static uint8_t  g_cip[IP_LEN]              = {CIP};
static uint8_t  g_lip[IP_LEN]              = {LIP};
static uint8_t  g_eip[IP_LEN]              = {EIP};
static uint8_t  g_fp1[FP1_LEN]             = {FP1};
static uint8_t  g_fp2[FP2_LEN]             = {FP2};
static uint8_t  g_eol[EOL_LEN]             = {EOL};

static rw_did_t g_rw_did[] =
{
	{RW_DATA_CONFIG_VMSPN     ,{HWORD_HIGH_BYTE(VMSPNDID)     ,HWORD_LOW_BYTE(VMSPNDID)     ,{VMSPN_LEN     ,g_vmspn}}},
	{RW_DATA_CONFIG_SSID      ,{HWORD_HIGH_BYTE(SSIDDID)      ,HWORD_LOW_BYTE(SSIDDID)      ,{SSID_LEN      ,g_ssid}}},
	{RW_DATA_CONFIG_SNOET     ,{HWORD_HIGH_BYTE(SNOETDID)     ,HWORD_LOW_BYTE(SNOETDID)     ,{SNOET_LEN     ,g_snoet}}},
	{RW_DATA_CONFIG_SSECUHWVN ,{HWORD_HIGH_BYTE(SSECUHWVNDID) ,HWORD_LOW_BYTE(SSECUHWVNDID) ,{SSECUHWVN_LEN ,g_ssecuhwvn}}},
	{RW_DATA_CONFIG_SSECUSWVN ,{HWORD_HIGH_BYTE(SSECUSWVNDID) ,HWORD_LOW_BYTE(SSECUSWVNDID) ,{SSECUSWVN_LEN ,g_ssecuswvn}}},
	{RW_DATA_CONFIG_ECUSN     ,{HWORD_HIGH_BYTE(ECUSNDID)     ,HWORD_LOW_BYTE(ECUSNDID)     ,{ECUSN_LEN     ,g_ecusn}}},
	{RW_DATA_CONFIG_VIN       ,{HWORD_HIGH_BYTE(VINDID)       ,HWORD_LOW_BYTE(VINDID)       ,{VIN_LEN       ,g_vin}}},
	{RW_DATA_CONFIG_ADS       ,{HWORD_HIGH_BYTE(ADSDID)       ,HWORD_LOW_BYTE(ADSDID)       ,{ADS_LEN       ,&g_session_mode}}},
	{RW_DATA_CONFIG_ECUMD     ,{HWORD_HIGH_BYTE(ECUMDDID)     ,HWORD_LOW_BYTE(ECUMDDID)     ,{ECUMD_LEN     ,(uint8_t*)(&g_ecumd)}}},
	{RW_DATA_CONFIG_BSI       ,{HWORD_HIGH_BYTE(BSIDID)       ,HWORD_LOW_BYTE(BSIDID)       ,{BSI_LEN	    ,g_bsi}}},
	{RW_DATA_CONFIG_EPSV      ,{HWORD_HIGH_BYTE(EPSVDID)      ,HWORD_LOW_BYTE(EPSVDID)      ,{EPSV_LEN      ,&g_epsv}}},
	{RW_DATA_CONFIG_TBTN      ,{HWORD_HIGH_BYTE(TBTNDID)      ,HWORD_LOW_BYTE(TBTNDID)      ,{TBTN_LEN      ,g_tbtn}}},
	{RW_DATA_CONFIG_IMEI      ,{HWORD_HIGH_BYTE(IMEIDID)      ,HWORD_LOW_BYTE(IMEIDID)      ,{IMEI_LEN      ,g_imei}}},
	{RW_DATA_CONFIG_ICCID     ,{HWORD_HIGH_BYTE(ICCIDDID)     ,HWORD_LOW_BYTE(ICCIDDID)     ,{ICCID_LEN     ,g_iccid}}},
	{RW_DATA_CONFIG_PK        ,{HWORD_HIGH_BYTE(PKDID)        ,HWORD_LOW_BYTE(PKDID)        ,{PK_LEN        ,g_pk}}},
	{RW_DATA_CONFIG_EW        ,{HWORD_HIGH_BYTE(EWDID)        ,HWORD_LOW_BYTE(EWDID)        ,{ESK_LEN       ,g_esk}}},
	{RW_DATA_CONFIG_ED        ,{HWORD_HIGH_BYTE(EDDID)        ,HWORD_LOW_BYTE(EDDID)        ,{ESK_LEN       ,g_esk}}},
	{RW_DATA_CONFIG_CIP       ,{HWORD_HIGH_BYTE(CIPDID)       ,HWORD_LOW_BYTE(CIPDID)       ,{IP_LEN        ,g_cip}}},
	{RW_DATA_CONFIG_LIP       ,{HWORD_HIGH_BYTE(LIPDID)       ,HWORD_LOW_BYTE(LIPDID)       ,{IP_LEN        ,g_lip}}},
	{RW_DATA_CONFIG_EIP       ,{HWORD_HIGH_BYTE(EIPDID )      ,HWORD_LOW_BYTE(EIPDID)       ,{IP_LEN        ,g_eip}}},
	{RW_DATA_CONFIG_FP1       ,{HWORD_HIGH_BYTE(FP1DID)       ,HWORD_LOW_BYTE(FP1DID)       ,{FP1_LEN       ,g_fp1}}},
	{RW_DATA_CONFIG_FP2       ,{HWORD_HIGH_BYTE(FP2DID)       ,HWORD_LOW_BYTE(FP2DID)       ,{FP2_LEN       ,g_fp2}}},
	{RW_DATA_CONFIG_EOL       ,{HWORD_HIGH_BYTE(EOLDID)       ,HWORD_LOW_BYTE(EOLDID)       ,{EOL_LEN       ,g_eol}}}
};

#if defined USING_OS_FREERTOS
static SemaphoreHandle_t g_rw_data_mutex = NULL;
#endif
/** @} */ /* Read/Write data by identifier. */

/**
 * @name Security access.
 * @{
 */
/**
 * @brief Security access level.
 */
typedef struct
{
	uint32_t seed_;
	uint8_t  seed_req_ : 1;
	uint8_t  locked_ : 1;
	uint8_t  try_count_ : 2;
	uint8_t  timer_active_ : 1;
	uint32_t reach_time_;
}security_access_level_t;

static security_access_level_t g_sal[] =
{
	{0, false, true, 0, false, 0},
	{0, false, true, 0, false, 0},
	{0, false, true, 0, false, 0},
	{0, false, true, 0, false, 0}
};
/** @} */ /* Security access. */

/**
 * @name Communication control.
 * @{
 */
static uint8_t g_comm_ctrl_rx = (COMM_MSG_TYPE_MASK_NCM | COMM_MSG_TYPE_MASK_NWMCM);
static uint8_t g_comm_ctrl_tx = (COMM_MSG_TYPE_MASK_NCM | COMM_MSG_TYPE_MASK_NWMCM);
#if defined USING_OS_FREERTOS
static SemaphoreHandle_t g_comm_ctrl_mutex = NULL;
#endif
/** @} */ /* Communication control. */

/**
 * @name Routine control.
 * @{
 */
/**
 * @brief Routine control.
 */
typedef struct
{
	uint8_t id_msb_;
	uint8_t id_lsb_;
	uint8_t dft_ses_spd_ : 1;
	uint8_t pro_ses_spd_ : 1;
	uint8_t exd_ses_spd_ : 1;
	uint8_t sec_acc_lev_;

}routine_ctrl_t;

static routine_ctrl_t g_routine_ctrl[] = {ROUTINE_CTRL_ID_CONFIG};
/** @} */ /* Routine control. */

/**
 * @name Transfer data.
 * @{
 */
static uint32_t g_transfer_addr = 0;
static uint32_t g_transfer_size = 0;
static uint32_t g_transfered_size = 0;
static uint8_t  g_transfer_type = TRANSFER_TYPE_INVALID;
static uint8_t  g_block_count = 0;
/** @} */ /* Transfer data. */

/**
 * @name Control DTC setting.
 * @{
 */
static bool g_dtc_ctrl = true;
/** @} */ /* Transfer data. */

/*******************************************************************************
 * Local Function prototypes
 ******************************************************************************/
static void diag_server_enter_default_session(void);
static void diag_server_app_indication(IsoTpLink *const _link, const uint8_t *const _buf, const uint16_t _size);
static uint8_t diag_server_request_handler(const uint8_t _sid, const uint8_t *const _param, const uint16_t _len, uint8_t *const _out_param, uint16_t *const _out_len);
static bool diag_server_is_spd_in_active_session_mode(const bool _dft_ses_spd, const bool _pro_ses_spd, const bool _exd_ses_spd);
static bool diag_server_is_security_access_level_locked(const uint8_t _level);
static uint16_t diag_server_get_did_index(const uint16_t _did);
static uint32_t diag_server_get_did_address(const uint16_t _did);

/**
 * @name Diagnostic request handlers.
 * @{
 */
static uint8_t request_session_ctrl_handler(const uint8_t *const _param, const uint16_t _len, uint8_t *const _out_param, uint16_t *const _out_len);
static uint8_t request_ecu_reset_handler(const uint8_t *const _param, const uint16_t _len, uint8_t *const _out_param, uint16_t *const _out_len);
static uint8_t request_clear_diag_info_handler(const uint8_t *const _param, const uint16_t _len, uint8_t *const _out_param, uint16_t *const _out_len);
static uint8_t request_read_dtc_info_handler(const uint8_t *const _param, const uint16_t _len, uint8_t *const _out_param, uint16_t *const _out_len);
static uint8_t request_read_by_did_handler(const uint8_t *const _param, const uint16_t _len, uint8_t *const _out_param, uint16_t *const _out_len);
static uint8_t request_write_by_did_handler(const uint8_t *const _param, const uint16_t _len, uint8_t *const _out_param, uint16_t *const _out_len);
static uint8_t request_security_access_handler(const uint8_t *const _param, const uint16_t _len, uint8_t *const _out_param, uint16_t *const _out_len);
static uint8_t request_comm_ctrl_handler(const uint8_t *const _param, const uint16_t _len, uint8_t *const _out_param, uint16_t *const _out_len);
static uint8_t request_routine_ctrl_handler(const uint8_t *const _param, const uint16_t _len, uint8_t *const _out_param, uint16_t *const _out_len);
static uint8_t request_download_handler(const uint8_t *const _param, const uint16_t _len, uint8_t *const _out_param, uint16_t *const _out_len);
static uint8_t request_transfer_data_handler(const uint8_t *const _param, const uint16_t _len, uint8_t *const _out_param, uint16_t *const _out_len);
static uint8_t request_transfer_exit_handler(const uint8_t *const _param, const uint16_t _len, uint8_t *const _out_param, uint16_t *const _out_len);
static uint8_t request_ctrl_dtc_setting_handler(const uint8_t *const _param, const uint16_t _len, uint8_t *const _out_param, uint16_t *const _out_len);
/** @} */ /* Diagnostic request handlers. */

/*******************************************************************************
 * Functions
 ******************************************************************************/
void diag_server_init(IsoTpLink *const _link, const uint32_t _tx_id, const uint32_t _rx_id,
					  uint8_t *const _tx_buf, const uint16_t _tx_buf_size,
                      uint8_t *const _rx_buf, const uint16_t _rx_buf_size,
                      const send_callback_t _tx_callback,
                      const receive_callback_t _rx_callback,
					  const sys_time_callback_t _sys_time_callback,
					  const debug_callback_t _debug_callback)
{
	/* Initialize network layer */
	isotp_init_link(_link, _tx_id, _rx_id, _tx_buf, _tx_buf_size, _rx_buf, _rx_buf_size,
			        _tx_callback, _rx_callback, diag_server_app_indication, _sys_time_callback, _debug_callback);

	/* Initialize application layer */
#if defined USING_OS_FREERTOS
	g_comm_ctrl_mutex = xSemaphoreCreateMutex();
	g_dtc_mutex = xSemaphoreCreateMutex();
	g_rw_data_mutex = xSemaphoreCreateMutex();
#endif
	g_sys_time_callback = _sys_time_callback;
	g_debug_callback = _debug_callback;
	diag_server_enter_default_session();
}

void diag_server_deinit(IsoTpLink *const _link)
{
	(void)_link;
#if defined USING_OS_FREERTOS
	vSemaphoreDelete(g_comm_ctrl_mutex);
	vSemaphoreDelete(g_dtc_mutex);
	vSemaphoreDelete(g_rw_data_mutex);
#endif
}

void diag_server_data_init(void)
{
#if defined USING_OS_FREERTOS
	xSemaphoreTake( g_rw_data_mutex, portMAX_DELAY );
#endif
	/* Initialize read/write data */
	for(uint8_t i = 0; i < RW_DID_NUM; i++)
	{
		uint32_t addr = diag_server_get_did_address(*(uint16_t*)(&g_rw_did[i].did_));

		if(EEPROM_TOTAL_SIZE != addr)
		{
			eeprom_read(addr, g_rw_did[i].did_.data_.buf_, g_rw_did[i].did_.data_.size_);
			g_debug_callback("EERPROM READ(0x%X,%d): ", addr, g_rw_did[i].did_.data_.size_);
			for(uint8_t k = 0; k < g_rw_did[i].did_.data_.size_; k++)
			{
				g_debug_callback("%02X ", g_rw_did[i].did_.data_.buf_[k]);
			}
			g_debug_callback("\n");
		}
	}
#if defined USING_OS_FREERTOS
	xSemaphoreGive( g_rw_data_mutex );
#endif
#if defined USING_OS_FREERTOS
	xSemaphoreTake( g_dtc_mutex, portMAX_DELAY );
#endif
	/* Initialize DTC */
	for(uint8_t i = 0; i < DTC_NUM; i++)
	{
		eeprom_read(EEPROM_ADDR_DTC + i, &g_dtc_info[i].status_,1);
		g_debug_callback("EERPROM READ(0x%X,1): %02X\n", EEPROM_ADDR_DTC + i, g_dtc_info[i].status_);
	}
#if defined USING_OS_FREERTOS
	xSemaphoreGive( g_dtc_mutex );
#endif
}

void diag_server_indication(IsoTpLink *const _link, const uint8_t *const _buf, const uint8_t _size)
{
	assert(NULL != _link && NULL != _buf);

	uint8_t type = _buf[0] >> 4;

	/* Check S3_Server timer */
	if((ISOTP_PCI_TYPE_SINGLE == type || ISOTP_PCI_TYPE_SINGLE == type) &&
			UDS_DIAG_SESSION_CTRL_SF_DEFAULT_MODE != g_session_mode &&
			g_s3_server_timer_started)
	{
		if(g_sys_time_callback() >= g_s3_server_timer + S3_SERVER)
			diag_server_enter_default_session();
		g_s3_server_timer_started = false;
	}

	isotp_indication(_link, _buf, _size);
}

bool diag_server_comm_ctrl_rx_enabled(const uint8_t _mask)
{
	bool ret = true;
	
#if defined USING_OS_FREERTOS
	xSemaphoreTake( g_comm_ctrl_mutex, portMAX_DELAY );
#endif
	ret = ((g_comm_ctrl_rx & _mask) ? true : false);
#if defined USING_OS_FREERTOS
	xSemaphoreGive( g_comm_ctrl_mutex );
#endif
	
	return ret;
}

bool diag_server_comm_ctrl_tx_enabled(const uint8_t _mask)
{
	bool ret = true;
	
#if defined USING_OS_FREERTOS
	xSemaphoreTake( g_comm_ctrl_mutex, portMAX_DELAY );
#endif
	ret = ((g_comm_ctrl_tx & _mask) ? true : false);
#if defined USING_OS_FREERTOS
	xSemaphoreGive( g_comm_ctrl_mutex );
#endif

	return ret;
}

void diag_server_read_data_by_id(const uint16_t _did, uint8_t *const _buf, const uint16_t _size)
{
	uint16_t did;
	uint16_t i = 0;

	did = _did << 8;
	did |= (_did >> 8);
#if defined USING_OS_FREERTOS
	xSemaphoreTake( g_rw_data_mutex, portMAX_DELAY );
#endif
	i = diag_server_get_did_index(did);
	if(RW_DID_NUM != i)
		memcpy(_buf, g_rw_did[i].did_.data_.buf_, _size > g_rw_did[i].did_.data_.size_ ? g_rw_did[i].did_.data_.size_ : _size);
#if defined USING_OS_FREERTOS
	xSemaphoreGive( g_rw_data_mutex );
#endif
}

void diag_server_on_msg(const uint32_t _msg_id)
{
	uint32_t cur_time = g_sys_time_callback();

#if defined USING_OS_FREERTOS
	xSemaphoreTake( g_dtc_mutex, portMAX_DELAY );
#endif
	switch (_msg_id)
	{
	case MSG_ACC_ID:
		g_acc_time[0] = cur_time;
		break;
	case MSG_EMS_ID:
		g_ems_time[0] = cur_time;
		break;
	case MSG_ESP_ID:
		g_abs_esp_time[0] = cur_time;
		break;
	case MSG_SRS_ID:
		g_srs_time[0] = cur_time;
		break;
	case MSG_TPMS_ID:
		g_tpms_time[0] = cur_time;
		break;
	case MSG_BCM_ID:
		g_bcm_time[0] = cur_time;
		break;
	case MSG_AC_ID:
		g_ac_time[0] = cur_time;
		break;
	case MSG_PEPS_ID:
		g_peps_time[0] = cur_time;
		break;
	case MSG_VCU_ID0:
		g_vcu_time[0] = cur_time;
		break;
	case MSG_VCU_ID1:
		g_vcu_time[1] = cur_time;
		break;
	case MSG_VCU_ID2:
		g_vcu_time[2] = cur_time;
		break;
	case MSG_VCU_ID3:
		g_vcu_time[3] = cur_time;
		break;
	case MSG_BMS_ID0:
		g_bms_time[0] = cur_time;
		break;
	case MSG_BMS_ID1:
		g_bms_time[1] = cur_time;
		break;
	case MSG_BMS_ID2:
		g_bms_time[2] = cur_time;
		break;
	case MSG_BMS_ID3:
		g_bms_time[3] = cur_time;
		break;
	case MSG_BMS_ID4:
		g_bms_time[4] = cur_time;
		break;
	case MSG_BMS_ID5:
		g_bms_time[5] = cur_time;
		break;
	case MSG_BMS_ID6:
		g_bms_time[6] = cur_time;
		break;
	case MSG_BMS_ID7:
		g_bms_time[7] = cur_time;
		break;
	case MSG_BMS_ID8:
		g_bms_time[8] = cur_time;
		break;
	case MSG_BMS_ID9:
		g_bms_time[9] = cur_time;
		break;
	case MSG_BMS_ID10:
		g_bms_time[10] = cur_time;
		break;
	case MSG_BMS_ID11:
		g_bms_time[11] = cur_time;
		break;
	case MSG_BMS_ID12:
		g_bms_time[12] = cur_time;
		break;
	case MSG_BMS_ID13:
		g_bms_time[13] = cur_time;
		break;
	case MSG_BMS_ID14:
		g_bms_time[14] = cur_time;
		break;
	case MSG_BMS_ID15:
		g_bms_time[15] = cur_time;
		break;
	case MSG_BMS_ID16:
		g_bms_time[16] = cur_time;
		break;
	case MSG_BMS_ID17:
		g_bms_time[17] = cur_time;
		break;
	case MSG_BMS_ID18:
		g_bms_time[18] = cur_time;
		break;
	case MSG_BMS_ID19:
		g_bms_time[19] = cur_time;
		break;
	case MSG_BMS_ID20:
		g_bms_time[20] = cur_time;
		break;
	case MSG_BMS_ID21:
		g_bms_time[21] = cur_time;
		break;
	case MSG_BMS_ID22:
		g_bms_time[22] = cur_time;
		break;
	case MSG_BMS_ID23:
		g_bms_time[23] = cur_time;
		break;
	case MSG_BMS_ID24:
		g_bms_time[24] = cur_time;
		break;
	case MSG_BMS_ID25:
		g_bms_time[25] = cur_time;
		break;
	case MSG_BMS_ID26:
		g_bms_time[26] = cur_time;
		break;
	case MSG_BMS_ID27:
		g_bms_time[27] = cur_time;
		break;
	case MSG_BMS_ID28:
		g_bms_time[28] = cur_time;
		break;
	case MSG_BMS_ID29:
		g_bms_time[29] = cur_time;
		break;
	case MSG_BMS_ID30:
		g_bms_time[30] = cur_time;
		break;
	case MSG_BMS_ID31:
		g_bms_time[31] = cur_time;
		break;
	case MSG_BMS_ID32:
		g_bms_time[32] = cur_time;
		break;
	case MSG_BMS_ID33:
		g_bms_time[33] = cur_time;
		break;
	case MSG_BMS_ID34:
		g_bms_time[34] = cur_time;
		break;
	case MSG_BMS_ID35:
		g_bms_time[35] = cur_time;
		break;
	case MSG_BMS_ID36:
		g_bms_time[36] = cur_time;
		break;
	case MSG_BMS_ID37:
		g_bms_time[37] = cur_time;
		break;
	case MSG_BMS_ID38:
		g_bms_time[38] = cur_time;
		break;
	case MSG_BMS_ID39:
		g_bms_time[39] = cur_time;
		break;
	case MSG_BMS_ID40:
		g_bms_time[40] = cur_time;
		break;
	case MSG_MCU_ID0:
		g_mcu_time[0] = cur_time;
		break;
	case MSG_MCU_ID1:
		g_mcu_time[1] = cur_time;
		break;
	case MSG_MCU_ID2:
		g_mcu_time[2] = cur_time;
		break;
	case MSG_OBC_ID:
		g_obc_time[0] = cur_time;
		break;
	case MSG_DCDC_ID:
		g_dcdc_time[0] = cur_time;
		break;
	default:
		break;
	}
#if defined USING_OS_FREERTOS
	xSemaphoreGive( g_dtc_mutex );
#endif
}

void diag_server_update_dtc(void)
{
	if(g_dtc_ctrl)
		return;

	static uint32_t reach_time = 0;
	static bool     timer_active = false;
	static bool     fault = false;
	uint32_t        pwr_vol = 0;

#if defined USING_OS_FREERTOS
	xSemaphoreTake( g_dtc_mutex, portMAX_DELAY );
#endif
	/* Detect battery voltage */
	pwr_vol = diag_server_get_pwr_vol_callback();
	if (BAT_VOL_UNDER_THRESHOLD > pwr_vol)
	{
		g_dtc_info[0].status_ |= DTC_STATUS_MASK_TF;
		fault = true;
		g_dtc_snapshot_vs = diag_server_get_vehicle_speed_callback();
		g_dtc_snapshot_ecuv = pwr_vol;
		eeprom_write(EEPROM_ADDR_DTC, &g_dtc_info[0].status_, 1);
	}
	else if (BAT_VOL_OVER_THRESHOLD < pwr_vol)
	{
		g_dtc_info[1].status_ |= DTC_STATUS_MASK_TF;
		fault = true;
		g_dtc_snapshot_vs = diag_server_get_vehicle_speed_callback();
		g_dtc_snapshot_ecuv = pwr_vol;
		eeprom_write(EEPROM_ADDR_DTC + 1, &g_dtc_info[1].status_, 1);
	}
	else
	{
		if (fault)
		{
			if (!timer_active)
			{
				reach_time = g_sys_time_callback() + BAT_VOL_RECOVER_TIME;
				timer_active = true;
			}
			else
			{
				if (g_sys_time_callback() >= reach_time)
				{
					g_dtc_info[0].status_ &= ~DTC_STATUS_MASK_TF;
					g_dtc_info[1].status_ &= ~DTC_STATUS_MASK_TF;
					timer_active = false;
					fault = false;
					eeprom_write(EEPROM_ADDR_DTC, &g_dtc_info[0].status_, 1);
					eeprom_write(EEPROM_ADDR_DTC + 1, &g_dtc_info[1].status_, 1);
				}
			}
		}
	}

	/* Detect message communication */
	for (uint16_t i = 0; i < MSG_TYPE_NUM; i++)
	{
		if (!g_msg_time[i].is_lost_)
		{
			uint32_t time = 0u;
			for (uint16_t j = 0; j < g_msg_time[i].msg_time_num_; j++)
				if (time < g_msg_time[i].msg_time_[j])
					time = g_msg_time[i].msg_time_[j];
			if ((g_msg_time[i].lost_time_ + time) <= g_sys_time_callback())
			{
				g_msg_time[i].is_lost_ = true;
				g_dtc_info[g_msg_time[i].dtc_info_index_].status_ |= DTC_STATUS_MASK_TF;
				g_dtc_snapshot_vs = diag_server_get_vehicle_speed_callback();
				g_dtc_snapshot_ecuv = pwr_vol;
				eeprom_write(EEPROM_ADDR_DTC + g_msg_time[i].dtc_info_index_, &g_dtc_info[g_msg_time[i].dtc_info_index_].status_, 1);
			}
		}
		else
		{
			uint32_t time = 0xFFFFFFFF;
			for (uint16_t j = 0; j < g_msg_time[i].msg_time_num_; j++)
				if (time > g_msg_time[i].msg_time_[j])
					time = g_msg_time[i].msg_time_[j];
			if ((g_msg_time[i].recovery_time_ + time) > g_sys_time_callback())
			{
				g_msg_time[i].is_lost_ = false;
				g_dtc_info[g_msg_time[i].dtc_info_index_].status_ &= ~DTC_STATUS_MASK_TF;
				eeprom_write(EEPROM_ADDR_DTC + g_msg_time[i].dtc_info_index_, &g_dtc_info[g_msg_time[i].dtc_info_index_].status_, 1);
			}
		}
	}
#if defined USING_OS_FREERTOS
	xSemaphoreGive( g_dtc_mutex );
#endif
}

/**
 * @name Diagnostic server callbacks default implementation.
 * @{
 */
void     __attribute__((weak)) diag_server_sys_reset_callback(void){}
uint32_t __attribute__((weak)) diag_server_get_pwr_vol_callback(void){return 120;}
uint32_t __attribute__((weak)) diag_server_get_vehicle_speed_callback(void){return 0;}
uint32_t __attribute__((weak)) diag_server_get_car_status_callback(void){return CAR_STATUS_OTHER;}
int32_t  __attribute__((weak)) diag_server_flash_erase_sectors_callback(const uint32_t _addr, const uint32_t _size){return 0;}
int32_t  __attribute__((weak)) diag_server_flash_verify_sectors_callback(const uint32_t _addr, const uint32_t _size){return 0;}
int32_t  __attribute__((weak)) diag_server_flash_program_callback(const uint32_t _addr, const uint32_t _size, const uint8_t *const _buf){return 0;}
int32_t  __attribute__((weak)) diag_server_flash_program_verify_callback(const uint32_t _addr, const uint32_t _size, const uint8_t *const _buf){return 0;}
/** @} */ /* Diagnostic server callbacks default implementation. */

/*******************************************************************************
 * Local Functions
 ******************************************************************************/
static void diag_server_enter_default_session(void)
{
	/* Initialize diagnostic session to default mode */
	g_session_mode = UDS_DIAG_SESSION_CTRL_SF_DEFAULT_MODE;

	/* Initialize security access */
	for (uint16_t i = 0; i < UDS_SECURITY_ACCESS_NONE; i++)
	{
		g_sal[i].seed_         = 0;
		g_sal[i].seed_req_  = true;
		g_sal[i].locked_       = true;
		g_sal[i].try_count_    = 0;
		g_sal[i].timer_active_ = false;
		g_sal[i].reach_time_   = 0;
	}

	/* Recover communication */
#if defined USING_OS_FREERTOS
	xSemaphoreTake(g_comm_ctrl_mutex, portMAX_DELAY);
#endif
	g_comm_ctrl_rx = (COMM_MSG_TYPE_MASK_NCM | COMM_MSG_TYPE_MASK_NWMCM);
	g_comm_ctrl_tx = (COMM_MSG_TYPE_MASK_NCM | COMM_MSG_TYPE_MASK_NWMCM);
#if defined USING_OS_FREERTOS
	xSemaphoreGive(g_comm_ctrl_mutex);
#endif
	/* Control DTC setting on */
#if defined USING_OS_FREERTOS
	xSemaphoreTake(g_dtc_mutex, portMAX_DELAY);
#endif
	g_dtc_ctrl = true;
#if defined USING_OS_FREERTOS
	xSemaphoreGive(g_dtc_mutex);
#endif
}

/**
 * @brief Indicate diagnostic server application layer data.
 *
 * @param [in] _link  ISOTP link.
 * @param [in] _buf   Data buffer.
 * @param [in] _size  Data size.
 */
static void diag_server_app_indication(IsoTpLink *const _link, const uint8_t *const _buf, const uint16_t _size)
{
	assert(NULL != _link && NULL != _buf);

	int32_t        nrc = UDS_NRC_PR;
	service_t      *service = NULL;
	static uint8_t buf[DIAG_MAX_BUF_SIZE];
	uint16_t       size = 0;

	/* TODO: If server is busy, negative response UDS_NRC_BRR(0x021)*/
	/* Seek service */
	for (uint8_t i = 0; i < UDS_SERVICE_NUM; i++)
	{
		if (_buf[0] == g_service[i].sid_)
		{
			service = &g_service[i];
			break;
		}
	}

	/* General handling */
	if (NULL != service)
	{
		if ((FUNCTION_ID != _link->receive_arbitration_id && service->phys_addr_spd_)
			|| (FUNCTION_ID ==_link->receive_arbitration_id && service->func_addr_spd_))
		{
			if (diag_server_is_spd_in_active_session_mode(service->dft_ses_spd_, service->pro_ses_spd_, service->exd_ses_spd_))
			{
				if (!diag_server_is_security_access_level_locked(service->sec_acc_lev_))
				{
					if (service->with_sf_)
					{
						if (NULL != _buf + 1 && 2 <= _size)
						{
							sub_function_t *sf = NULL;
							for (uint8_t i = 0; i < service->sf_num_; i++)
							{
								if ((service->sf_)[i].param_ == UDS_SF_PARAM_VAL(_buf[1]))
								{
									sf = &((service->sf_)[i]);
									break;
								}
							}

							if (NULL != sf && sf->spd_)
							{
								if (diag_server_is_spd_in_active_session_mode(sf->dft_ses_spd_, sf->pro_ses_spd_, sf->exd_ses_spd_))
								{
									if (!diag_server_is_security_access_level_locked(sf->sec_acc_lev_))
									{
										nrc = diag_server_request_handler(_buf[0], _buf + 1, _size - 1, buf + 1, &size);
										if (UDS_NRC_PR == nrc && (0u != UDS_SF_PARAM_SPRMIB(_buf[1]))
											/* No response */
											nrc = UDS_NRC_NO_RESP;
									}
									else
										nrc = UDS_NRC_SAD;
								}
								else
									nrc = UDS_NRC_SFNSIAS;
							}
							else
							{
								if (FUNCTION_ID != _link->receive_arbitration_id)
									nrc = UDS_NRC_SFNS;
								else
									/* No response */
									nrc = UDS_NRC_NO_RESP;
							}
						}
						else
							nrc = UDS_NRC_INLOIF;
					}
					else
					{
						if (service->param_spd_)
							nrc = diag_server_request_handler(_buf[0], _buf + 1, _size - 1, buf + 1, &size);
						else
						{
							if (FUNCTION_ID != _link->receive_arbitration_id)
								nrc = UDS_NRC_ROOR;
							else
								/* No response */
								nrc = UDS_NRC_NO_RESP;
						}
					}
				}
				else
					nrc = UDS_NRC_SAD;

			}
			else
				nrc = UDS_NRC_SNSIAS;
		}
		else
		{
			if (FUNCTION_ID != _link->receive_arbitration_id)
				nrc = UDS_NRC_SNS;
			else
				/* No response */
				nrc = UDS_NRC_NO_RESP;
		}
	}
	else
		nrc = UDS_NRC_SNS;

	/* Response */
	if (UDS_NRC_PR == nrc)
	{
		buf[0] = UDS_SID_PR(_buf[0]);
		isotp_send_poll(_link, buf, size + 1);
		if(UDS_DIAG_SESSION_CTRL_SF_DEFAULT_MODE != g_session_mode)
		{
			g_s3_server_timer_started = true;
			g_s3_server_timer = g_sys_time_callback();
		}
	}
	else if(UDS_NRC_NO_RESP == nrc)
	{
		if(UDS_DIAG_SESSION_CTRL_SF_DEFAULT_MODE != g_session_mode)
		{
			g_s3_server_timer_started = true;
			g_s3_server_timer = g_sys_time_callback();
		}
	}
	else
	{
		buf[0] = UDS_SID_NR;
		buf[1] = _buf[0];
		buf[2] = nrc;
		isotp_send_poll(_link, buf, 3);
	}

	/* ECU reset */
	if (UDS_SID_ECU_RESET == _buf[0] && (UDS_NRC_PR == nrc || UDS_NRC_NO_RESP == nrc))
	{
		if(UDS_ECU_RESET_SF_SOFT == UDS_SF_PARAM_VAL(_buf[1]))
			eeprom_write(EEPROM_ADDR_RESET_TYPE, (uint8_t *)"C", EEPROM_SIZE_RESET_TYPE);
		diag_server_sys_reset_callback();
	}
}

static uint8_t diag_server_request_handler(const uint8_t _sid, const uint8_t *const _param, const uint16_t _len, uint8_t *const _out_param, uint16_t *const _out_len)
{
	assert(NULL != _param && NULL != _out_param && NULL !=  _out_len);

	uint8_t ret = UDS_NRC_PR;

	switch (_sid)
	{
	case UDS_SID_DIAG_SESSION_CTRL:
		ret = request_session_ctrl_handler(_param, _len, _out_param, _out_len);
		break;
	case UDS_SID_ECU_RESET:
		ret = request_ecu_reset_handler(_param, _len, _out_param, _out_len);
		break;
	case UDS_SID_CLEAR_DIAG_INFO:
		ret = request_clear_diag_info_handler(_param, _len, _out_param, _out_len);
		break;
	case UDS_SID_READ_DTC_INFO:
		ret = request_read_dtc_info_handler(_param, _len, _out_param, _out_len);
		break;
	case UDS_SID_READ_DATA_BY_ID:
		ret = request_read_by_did_handler(_param, _len, _out_param, _out_len);
		break;
	case UDS_SID_READ_MEM_BY_ADDR:
		ret = UDS_NRC_SNS;
		break;
	case UDS_SID_READ_SCALING_DATA_BY_ID:
		ret = UDS_NRC_SNS;
		break;
	case UDS_SID_SECURITY_ACCESS:
		ret = request_security_access_handler(_param, _len, _out_param, _out_len);
		break;
	case UDS_SID_COM_CTRL:
		ret = request_comm_ctrl_handler(_param, _len, _out_param, _out_len);
		break;
	case UDS_SID_READ_DATA_BY_PERIODIC_ID:
		ret = UDS_NRC_SNS;
		break;
	case UDS_SID_DYN_DEFINE_DATA_ID:
		ret = UDS_NRC_SNS;
		break;
	case UDS_SID_WRITE_DATA_BY_ID:
		ret = request_write_by_did_handler(_param, _len, _out_param, _out_len);
		break;
	case UDS_SID_IO_CTRL_BY_ID:
		ret = UDS_NRC_SNS;
		break;
	case UDS_SID_ROUTINE_CTRL:
		ret = request_routine_ctrl_handler(_param, _len, _out_param, _out_len);
		break;
	case UDS_SID_REQT_DOWNLOAD:
		ret = request_download_handler(_param, _len, _out_param, _out_len);
		break;
	case UDS_SID_REQT_UPLOAD:
		ret = UDS_NRC_SNS;
		break;
	case UDS_SID_TRANSFER_DATA:
		ret = request_transfer_data_handler(_param, _len, _out_param, _out_len);
		break;
	case UDS_SID_REQT_TRANSFER_EXIT:
		ret = request_transfer_exit_handler(_param, _len, _out_param, _out_len);
		break;
	case UDS_SID_WRITE_MEM_BY_ADDR:
		ret = UDS_NRC_SNS;
		break;
	case UDS_SID_TESTER_PRESENT:
		*_out_len = 1u;
		_out_param[0] = UDS_TESTER_PRESENT_SF_ZERO;
		break;
	case UDS_SID_ACCESS_TIMING_PARAM:
		ret = UDS_NRC_SNS;
		break;
	case UDS_SID_SECURED_DATA_TRANS:
		ret = UDS_NRC_SNS;
		break;
	case UDS_SID_CTRL_DTC_SETTING:
		ret = request_ctrl_dtc_setting_handler(_param, _len, _out_param, _out_len);
		break;
	case UDS_SID_RESP_ON_EVENT:
		ret = UDS_NRC_SNS;
		break;
	case UDS_SID_LINK_CTRL:
		ret = UDS_NRC_SNS;
		break;
	default:
		ret = UDS_NRC_SNS;
		break;
	}

	return ret;
}

static bool diag_server_is_spd_in_active_session_mode(const bool _dft_ses_spd, const bool _pro_ses_spd, const bool _exd_ses_spd)
{
	if ((UDS_DIAG_SESSION_CTRL_SF_DEFAULT_MODE == g_session_mode && _dft_ses_spd)
		|| (UDS_DIAG_SESSION_CTRL_SF_PROGRAM_MODE == g_session_mode && _pro_ses_spd)
		|| (UDS_DIAG_SESSION_CTRL_SF_EXTEND_MODE == g_session_mode && _exd_ses_spd))
	{
		return true;
	}

	return false;
}

static bool diag_server_is_security_access_level_locked(const uint8_t _level)
{
	if (UDS_SECURITY_ACCESS_NONE <= _level)
		return false;
	else
		return g_sal[_level].locked_;
}

static uint16_t diag_server_get_did_index(const uint16_t _did)
{
	for(uint16_t i = 0; i < RW_DID_NUM; i++)
	{
		if(_did == *((uint16_t*)(&g_rw_did[i].did_)))
			return i;
	}

	return RW_DID_NUM;
}

static uint32_t diag_server_get_did_address(const uint16_t _did)
{
	uint32_t addr = EEPROM_TOTAL_SIZE;
	uint16_t did;

	did = _did << 8;
	did |= (_did >> 8);

	if(SSECUHWVNDID == did)
		addr = EEPROM_ADDR_SSECUHWVN;
	else if(VINDID == did)
		addr = EEPROM_ADDR_VIN;
	else if(ECUMDDID == did)
		addr = EEPROM_ADDR_ECUMD;
	else if(TBTNDID == did)
		addr = EEPROM_ADDR_TBTN;
	else if(IMEIDID == did)
		addr = EEPROM_ADDR_IMEI;
	else if(ICCIDDID == did)
		addr = EEPROM_ADDR_ICCID;
	else if(PKDID == did)
		addr = EEPROM_ADDR_PK;
	else if(FP1DID == did)
		addr = EEPROM_ADDR_FP1;
	else if(FP2DID == did)
		addr = EEPROM_ADDR_FP2;
	else if(EOLDID == did)
		addr = EEPROM_ADDR_EOL;

	return addr;
}

static uint8_t request_session_ctrl_handler(const uint8_t *const _param, const uint16_t _len, uint8_t *const _out_param, uint16_t *const _out_len)
{
	uint8_t ret = UDS_NRC_PR;
	uint8_t sf_param = UDS_SF_PARAM_VAL(_param[0]);

	/* Transfer session mode */
	switch (g_session_mode)
	{
	case UDS_DIAG_SESSION_CTRL_SF_DEFAULT_MODE:
		switch (sf_param)
		{
		case UDS_DIAG_SESSION_CTRL_SF_DEFAULT_MODE:
			break;
		case UDS_DIAG_SESSION_CTRL_SF_PROGRAM_MODE:
			ret = UDS_NRC_SFNSIAS;
			break;
		case UDS_DIAG_SESSION_CTRL_SF_EXTEND_MODE:
			g_session_mode = UDS_DIAG_SESSION_CTRL_SF_EXTEND_MODE;
			break;
		default:
			ret = UDS_NRC_SFNS;
			break;
		}
		break;
	case UDS_DIAG_SESSION_CTRL_SF_PROGRAM_MODE:
		switch (sf_param)
		{
		case UDS_DIAG_SESSION_CTRL_SF_DEFAULT_MODE:
			diag_server_enter_default_session();
			break;
		case UDS_DIAG_SESSION_CTRL_SF_PROGRAM_MODE:
			break;
		case UDS_DIAG_SESSION_CTRL_SF_EXTEND_MODE:
			g_session_mode = UDS_DIAG_SESSION_CTRL_SF_EXTEND_MODE;
			break;
		default:
			ret = UDS_NRC_SFNS;
			break;
		}
		break;
	case UDS_DIAG_SESSION_CTRL_SF_EXTEND_MODE:
		switch (sf_param)
		{
		case UDS_DIAG_SESSION_CTRL_SF_DEFAULT_MODE:
			diag_server_enter_default_session();
			break;
		case UDS_DIAG_SESSION_CTRL_SF_PROGRAM_MODE:
			g_session_mode = UDS_DIAG_SESSION_CTRL_SF_PROGRAM_MODE;
			break;
		case UDS_DIAG_SESSION_CTRL_SF_EXTEND_MODE:
			break;
		default:
			ret = UDS_NRC_SFNS;
			break;
		}
		break;
	default:
		ret = UDS_NRC_SFNS;
		break;
	}

	/* Write out parameters */
	if (UDS_NRC_PR == ret)
	{
		*_out_len = 5u;
		_out_param[0] = sf_param;
		_out_param[1] = HWORD_HIGH_BYTE(P2CAN_SERVER);
		_out_param[2] = HWORD_LOW_BYTE(P2CAN_SERVER);
		_out_param[3] = HWORD_HIGH_BYTE(P2ECAN_SERVER / 10);
		_out_param[4] = HWORD_LOW_BYTE(P2ECAN_SERVER / 10);
	}

	return ret;
}

static uint8_t request_ecu_reset_handler(const uint8_t *const _param, const uint16_t _len, uint8_t *const _out_param, uint16_t *const _out_len)
{
	uint8_t ret = UDS_NRC_PR;
	uint8_t sf_param = UDS_SF_PARAM_VAL(_param[0]);

	switch (sf_param)
	{
	case UDS_ECU_RESET_SF_HARD:
		*_out_len = 1u;
		_out_param[0] = sf_param;
		break;
	case UDS_ECU_RESET_SF_KEY_OFF_ON:
		ret = UDS_NRC_SFNS;
		break;
	case UDS_ECU_RESET_SF_SOFT:
		*_out_len = 1u;
		_out_param[0] = sf_param;
		break;
	default:
		ret = UDS_NRC_SFNS;
		break;
	}

	return ret;
}

static uint8_t request_clear_diag_info_handler(const uint8_t *const _param, const uint16_t _len, uint8_t *const _out_param, uint16_t *const _out_len)
{
	if (3 > _len)
		return UDS_NRC_INLOIF;

	uint32_t group = 0;

	group = _param[0] << 16;
	group |= _param[1] << 8;
	group |= _param[2];

	if(DTC_PG > group)
		return UDS_NRC_ROOR;

	uint32_t group_end = 0;

	if(DTC_PG == group)
		group_end = DTC_CG;
	else if(DTC_CG == group)
		group_end = DTC_BG;
	else if(DTC_BG == group)
		group_end = DTC_NCG;
	else if(DTC_NCG == group)
		group_end = DTC_AG;
	else
		group_end = group;

#if defined USING_OS_FREERTOS
	xSemaphoreTake( g_dtc_mutex, portMAX_DELAY );
#endif
	for (uint16_t i = 0; i < DTC_NUM; i++)
	{
		if(DTC_AG == group)
		{
			g_dtc_info[i].status_ = 0;
			if(0 != eeprom_write(EEPROM_ADDR_DTC + i, &g_dtc_info[i].status_, 1))
				return UDS_NRC_GPF;
		}
		else
		{
			uint32_t dtc = 0u;

			dtc = g_dtc_info[i].high_byte_ << 16;
			dtc |= g_dtc_info[i].mid_byte_ << 8;
			dtc |= g_dtc_info[i].low_byte_;

			if(group < dtc && dtc < group_end)
			{
				g_dtc_info[i].status_ = 0;
				if(0 != eeprom_write(EEPROM_ADDR_DTC + i, &g_dtc_info[i].status_, 1))
					return UDS_NRC_GPF;
			}
			else if(group == dtc && group_end == dtc)
			{
				g_dtc_info[i].status_ = 0;
				if(0 != eeprom_write(EEPROM_ADDR_DTC + i, &g_dtc_info[i].status_, 1))
					return UDS_NRC_GPF;
				break;
			}
		}
	}
#if defined USING_OS_FREERTOS
	xSemaphoreGive( g_dtc_mutex );
#endif
	*_out_len = 0;

	return UDS_NRC_PR;
}

static uint8_t request_read_dtc_info_handler(const uint8_t *const _param, const uint16_t _len, uint8_t *const _out_param, uint16_t *const _out_len)
{
	uint8_t ret = UDS_NRC_PR;
	uint8_t sf_param = UDS_SF_PARAM_VAL(_param[0]);
	uint8_t num = 0;
	bool    is_snapshot = false;

#if defined USING_OS_FREERTOS
	xSemaphoreTake( g_dtc_mutex, portMAX_DELAY );
#endif
	switch (sf_param)
	{
	case UDS_READ_DTC_INFO_SF_RPT_NUM_OF_DTC_BY_STS_MASK:
		if (2 <= _len)
		{
			uint16_t count = 0;

			for (uint16_t i = 0; i < DTC_NUM; i++)
			{
				if (0 != (_param[1] & DTC_STATUS_AVA_MASK & g_dtc_info[i].status_))
					count++;
			}

			*_out_len = 5;
			_out_param[0] = sf_param;
			_out_param[1] = DTC_STATUS_AVA_MASK;
			_out_param[2] = ISO_14229_1_DTC_FORMAT;
			_out_param[4] = (uint8_t)(count >> 8);
			_out_param[5] = (uint8_t)count;
		}
		else
			ret = UDS_NRC_INLOIF;
		break;
	case UDS_READ_DTC_INFO_SF_RPT_DTC_BY_STS_MASK:
		if (2 <= _len)
		{
			*_out_len = 2;
			_out_param[0] = sf_param;
			_out_param[1] = DTC_STATUS_AVA_MASK;

			for (uint16_t i = 0; i < DTC_NUM; i++)
			{
				if (0 != (_param[1] & DTC_STATUS_AVA_MASK & g_dtc_info[i].status_))
				{
					memcpy(_out_param + (*_out_len), &(g_dtc_info[i]), 4);
					(*_out_len) += 4;
				}
			}
		}
		else
			ret = UDS_NRC_INLOIF;
		break;
	case UDS_READ_DTC_INFO_SF_RPT_DTC_SNAPSHOT_REC_BY_DTC_NUM:
		num = DTC_SNAPSHOT_NUM;
		is_snapshot = true;
	case UDS_READ_DTC_INFO_SF_RPT_DTC_EXD_DATA_REC_BY_DTC_NUM:
		num = DTC_EXTEND_DATA_NUM;
		is_snapshot = false;

		/* Read routine */
		if (5 <= _len)
		{
			uint16_t i = 0;

			for (i = 0; i < DTC_NUM; i++)
			{
				if (0 == memcmp(_param + 1u, &(g_dtc_info[i]), 3))
					break;
			}

			if (DTC_NUM != i && (0xFF == _param[4] || num > _param[4]))
			{
				*_out_len = 5;
				_out_param[0] = sf_param;
				memcpy(_out_param, &g_dtc_info[i], 4);

				for (i = 0; i < num; i++)
				{
					uint8_t index = i;

					if (0xFF != _param[4])
					{
						index = _param[4];
						i = num;
					}

					if (is_snapshot)
					{
						_out_param[(*_out_len)++] = index;
						_out_param[(*_out_len)++] = g_dtc_snapshot[index].did_num_;
						for (uint16_t j = 0; j < g_dtc_snapshot[index].did_num_; j++)
						{
							memcpy(_out_param + (*_out_len), &(g_dtc_snapshot[index].did_[j]), 2u);
							(*_out_len) += 2u;
							memcpy(_out_param + (*_out_len), g_dtc_snapshot[index].did_[j].data_.buf_, g_dtc_snapshot[index].did_[j].data_.size_);
							(*_out_len) += g_dtc_snapshot[index].did_[j].data_.size_;
						}
					}
					else
					{
						_out_param[(*_out_len)++] = index;
						memcpy(_out_param + (*_out_len), g_dtc_extend_data[index].buf_, g_dtc_extend_data[index].size_);
						(*_out_len) += g_dtc_extend_data[index].size_;
					}
				}
			}
			else
				ret = UDS_NRC_ROOR;
		}
		else
			ret = UDS_NRC_INLOIF;
		break;
	case UDS_READ_DTC_INFO_SF_RPT_SUPPORTED_DTC:
		*_out_len = 2 + 4 * DTC_NUM;
		_out_param[0] = sf_param;
		_out_param[1] = DTC_STATUS_AVA_MASK;
		memcpy(_out_param + 2, g_dtc_info, 4 * DTC_NUM);
		break;
	default:
		ret = UDS_NRC_SFNS;
		break;
	}
#if defined USING_OS_FREERTOS
	xSemaphoreGive( g_dtc_mutex );
#endif

	return ret;
}

static uint8_t request_read_by_did_handler(const uint8_t *const _param, const uint16_t _len, uint8_t *const _out_param, uint16_t *const _out_len)
{
	if (2 > _len || 0 != _len % 2)
		return UDS_NRC_INLOIF;

	uint8_t  nrc = UDS_NRC_PR;
	uint16_t i = 0;
	
#if defined USING_OS_FREERTOS
	xSemaphoreTake( g_rw_data_mutex, portMAX_DELAY );
#endif
	*_out_len = 0;
	while(i < _len)
	{
		uint16_t k = diag_server_get_did_index(*((uint16_t*)(_param + i)));

		if(RW_DID_NUM != k)
		{
			if(!diag_server_is_spd_in_active_session_mode(g_rw_did[k].dft_ses_readable_, g_rw_did[k].pro_ses_readable_
				, g_rw_did[k].exd_ses_readable_))
				break;
			if (diag_server_is_security_access_level_locked(g_rw_did[k].sec_acc_lev_))
				break;
			memcpy(_out_param + *_out_len, _param + i, 2);
			memcpy(_out_param + *_out_len + 2, g_rw_did[k].did_.data_.buf_, g_rw_did[k].did_.data_.size_);
			*_out_len += 2 + g_rw_did[k].did_.data_.size_;
		}

		if(DIAG_MAX_BUF_SIZE < *_out_len + 1)
		{
			nrc = UDS_NRC_RTL;
			break;
		}

		i += 2;
	}

	if(0 == *_out_len)
		nrc = UDS_NRC_ROOR;
#if defined USING_OS_FREERTOS
	xSemaphoreGive( g_rw_data_mutex );
#endif
	
	return nrc;
}

static uint8_t request_write_by_did_handler(const uint8_t *const _param, const uint16_t _len, uint8_t *const _out_param, uint16_t *const _out_len)
{
	if (2 > _len)
		return UDS_NRC_INLOIF;

	uint8_t  nrc = UDS_NRC_ROOR;
	uint16_t i = 0;
	
#if defined USING_OS_FREERTOS
	xSemaphoreTake(g_rw_data_mutex, portMAX_DELAY);
#endif
	i = diag_server_get_did_index(*((uint16_t*)_param));
	if(RW_DID_NUM != i)
	{
    	if (!diag_server_is_spd_in_active_session_mode(g_rw_did[i].dft_ses_writable_, g_rw_did[i].pro_ses_writable_
			, g_rw_did[i].exd_ses_writable_))
			nrc = UDS_NRC_ROOR;
		else if(g_rw_did[i].did_.data_.size_ > (_len - 2))
			nrc = UDS_NRC_INLOIF;
		else if(diag_server_is_security_access_level_locked(g_rw_did[i].sec_acc_lev_))
			nrc = UDS_NRC_SAD;
		else
		{
			uint32_t addr = diag_server_get_did_address(*((uint16_t*)_param));

			if(EEPROM_TOTAL_SIZE != addr && 0 != eeprom_write(addr, _param + 2, g_rw_did[i].did_.data_.size_))
				nrc = UDS_NRC_GPF;
			else
			{
				memcpy(g_rw_did[i].did_.data_.buf_, _param + 2, g_rw_did[i].did_.data_.size_);
				*_out_len = 2;
				memcpy(_out_param, _param, 2);
				nrc = UDS_NRC_PR;
			}
		}
	}
#if defined USING_OS_FREERTOS
	xSemaphoreGive(g_rw_data_mutex);
#endif

	return nrc;
}

static uint8_t request_security_access_handler(const uint8_t *const _param, const uint16_t _len, uint8_t *const _out_param, uint16_t *const _out_len)
{
	uint8_t ret = UDS_NRC_PR;
	uint8_t sf_param = UDS_SF_PARAM_VAL(_param[0]);
	uint8_t index = 0;
	bool    req_seed = false;

	switch (sf_param)
	{
	case UDS_SECURITY_ACCESS_SF_REQT_SEED_L1:
		req_seed = true;
	case UDS_SECURITY_ACCESS_SF_SEND_KEY_L1:
		index = UDS_SECURITY_ACCESS_L1;
		break;
	case UDS_SECURITY_ACCESS_SF_REQT_SEED_L2:
		req_seed = true;
	case UDS_SECURITY_ACCESS_SF_SEND_KEY_L2:
		index = UDS_SECURITY_ACCESS_L2;
		break;
	case UDS_SECURITY_ACCESS_SF_REQT_SEED_L3:
		req_seed = true;
	case UDS_SECURITY_ACCESS_SF_SEND_KEY_L3:
		index = UDS_SECURITY_ACCESS_L3;
		break;
	case UDS_SECURITY_ACCESS_SF_REQT_SEED_L4:
		req_seed = true;
	case UDS_SECURITY_ACCESS_SF_SEND_KEY_L4:
		index = UDS_SECURITY_ACCESS_L4;
		break;
	default:
		ret = UDS_NRC_SFNS;
		break;
	}

	if (UDS_NRC_PR != ret)
		return ret;

	if (req_seed)
	{
		if (!g_sal[index].timer_active_)
		{
			if (SECURITY_ACCESS_REQ_SEED_MAX_TRY_TIME <= g_sal[index].try_count_)
			{
				g_sal[index].timer_active_ = true;
				g_sal[index].reach_time_   = g_sys_time_callback() + SECURITY_ACCESS_REQ_SEED_TIME_DELAY;
				g_sal[index].seed_req_  = false;
				ret = UDS_NRC_ENOA;
			}
		}
		else
		{
			if (g_sal[index].reach_time_ > g_sys_time_callback())
			{
				g_sal[index].seed_req_ = false;
				ret = UDS_NRC_RTDNE;
			}
			else
			{
				g_sal[index].try_count_--;
				g_sal[index].timer_active_ = false;
			}
		}

		if (UDS_NRC_PR != ret)
			return ret;

		*_out_len = 5;
		_out_param[0] = sf_param;
		if (g_sal[index].locked_)
		{
			g_sal[index].seed_ = g_sys_time_callback();
			g_sal[index].seed_req_ = true;
			_out_param[1] = WORD_BYTE0(g_sal[index].seed_);
			_out_param[2] = WORD_BYTE1(g_sal[index].seed_);
			_out_param[3] = WORD_BYTE2(g_sal[index].seed_);
			_out_param[4] = WORD_BYTE3(g_sal[index].seed_);
		}
		else
		{
			g_sal[index].seed_req_ = false;
			_out_param[1] = 0;
			_out_param[2] = 0;
			_out_param[3] = 0;
			_out_param[4] = 0;
		}
	}
	else
	{
		if (g_sal[index].seed_req_)
		{
			if (5 <= _len)
			{
				uint32_t key = 1 + g_sal[index].seed_;
				if (WORD_BYTE0(key) == _param[1] && WORD_BYTE1(key) == _param[2]
					&& WORD_BYTE2(key) == _param[3] && WORD_BYTE3(key) == _param[4])
				{
					g_sal[UDS_SECURITY_ACCESS_L1].locked_ = true;
					g_sal[UDS_SECURITY_ACCESS_L2].locked_ = true;
					g_sal[UDS_SECURITY_ACCESS_L3].locked_ = true;
					g_sal[UDS_SECURITY_ACCESS_L4].locked_ = true;
					g_sal[index].locked_ = false;
					g_sal[index].try_count_ = 0u;
					*_out_len = 1u;
					_out_param[0] = sf_param;
				}
				else
				{
					g_sal[index].try_count_++;
					ret = UDS_NRC_IK;
				}
			}
			else
				ret = UDS_NRC_INLOIF;
			g_sal[index].seed_req_ = false;
		}
		else
			ret = UDS_NRC_RSE;
	}

	return ret;
}

static uint8_t request_comm_ctrl_handler(const uint8_t *const _param, const uint16_t _len, uint8_t *const _out_param, uint16_t *const _out_len)
{
	if (2 > _len)
		return UDS_NRC_INLOIF;

	if (0 != diag_server_get_vehicle_speed_callback() || CAR_STATUS_STOPPED != diag_server_get_car_status_callback())
		return UDS_NRC_CNC;

	uint8_t ret = UDS_NRC_PR;
	uint8_t sf_param = UDS_SF_PARAM_VAL(_param[0]);

#if defined USING_OS_FREERTOS
	xSemaphoreTake(g_comm_ctrl_mutex, portMAX_DELAY);
#endif
	switch (sf_param)
	{
	case UDS_COM_CTRL_SF_EN_RX_TX:
		g_comm_ctrl_rx |= _param[1];
		g_comm_ctrl_tx |= _param[1];
		break;
	case UDS_COM_CTRL_SF_DIS_RX_TX:
		g_comm_ctrl_rx &= ~_param[1];
		g_comm_ctrl_tx &= ~_param[1];
		break;
	case UDS_COM_CTRL_SF_EN_RX_DIS_TX:
		g_comm_ctrl_rx |= _param[1];
		g_comm_ctrl_tx &= ~_param[1];
	case UDS_COM_CTRL_SF_DIS_RX_EN_TX:
		g_comm_ctrl_rx &= ~_param[1];
		g_comm_ctrl_tx |= _param[1];
	default:
		ret = UDS_NRC_SFNS;
		break;
	}
#if defined USING_OS_FREERTOS
	xSemaphoreGive(g_comm_ctrl_mutex);
#endif
	if (UDS_NRC_PR == ret)
	{
		*_out_len = 1;
		_out_param[0] = sf_param;
	}

	return ret;
}

static uint8_t request_routine_ctrl_handler(const uint8_t *const _param, const uint16_t _len, uint8_t *const _out_param, uint16_t *const _out_len)
{
	uint8_t  ret = UDS_NRC_PR;
	uint8_t  sf_param = 0;
	uint16_t rid = 0;

	if (3 > _len)
		return UDS_NRC_INLOIF;

	ret = UDS_NRC_ROOR;
	for(uint8_t i = 0; i < UDS_RID_NUM; i++)
	{
		if(0 == memcmp(&_param[1], &g_routine_ctrl[i], 2))
		{
			if(!diag_server_is_spd_in_active_session_mode(g_routine_ctrl[i].dft_ses_spd_, g_routine_ctrl[i].pro_ses_spd_
					, g_routine_ctrl[i].exd_ses_spd_))
				ret = UDS_NRC_ROOR;
			else if(diag_server_is_security_access_level_locked(g_routine_ctrl[i].sec_acc_lev_))
				ret = UDS_NRC_SAD;
			else
				ret = UDS_NRC_PR;
			break;
		}
	}

	if(UDS_NRC_PR != ret)
		return ret;

	sf_param = UDS_SF_PARAM_VAL(_param[0]);
	rid = _param[1] << 8;
	rid |= _param[2];

	switch (sf_param)
	{
	case UDS_ROUTINE_CTRL_SF_START:
		switch (rid)
		{
		case UDS_RID_ERASE_MEM:
		{
			uint8_t  size_len = 0;
			uint8_t  addr_len = 0;

			if (6 > _len)
				return UDS_NRC_INLOIF;

			size_len = _param[3] >> 4;
			addr_len = 0xF & _param[3];

			if (4 + size_len + addr_len > _len)
				return UDS_NRC_INLOIF;

			uint32_t addr = 0;
			uint32_t size = 0;

			memcpy(&addr, _param + 4, addr_len);
			bytesswap(&addr, addr_len);
			if (MEM_APP_BASE_ADDR > addr || MEM_BASE_ADDR + MEM_TOTAL_SIZE < addr)
				return UDS_NRC_ROOR;

			memcpy(&size, _param + 4 + addr_len, size_len);
			bytesswap(&size, size_len);
			if (MEM_TOTAL_SIZE - (MEM_APP_BASE_ADDR - MEM_BASE_ADDR) < size)
				return UDS_NRC_ROOR;
			if(0 != size % MEM_SECTOR_SIZE)
				return UDS_NRC_GPF;

			/* Erase and verify pages */
			diag_server_flash_erase_sectors_callback(addr, size);
			diag_server_flash_verify_sectors_callback(addr, size);

			*_out_len = 4;
			memcpy(_out_param, _param, 3);
			_out_param[3] = ROUTINE_STATUS_CORRECT;
			break;
		}
		case UDS_RID_CHECK_PROM_DPED:
		{
			uint32_t check_sum = 0;

			if (7 > _len)
				return UDS_NRC_INLOIF;

			*_out_len = 4;
			memcpy(&check_sum, _param + 3, 4);
			bytesswap(&check_sum, 4);
			memcpy(_out_param, _param, 3);
			_out_param[3] = ROUTINE_STATUS_CORRECT;
			break;
		}
		case UDS_RID_CLEAN_TBOX_EOL:
			*_out_len = 3;
			memcpy(_out_param, _param, 3);
			break;
		case UDS_RID_ERASE_MIRROR_MEM_DTCS:
		case UDS_RID_CHECK_PROM_INTY:
		case UDS_RID_CHECK_PROM_PREC:
		default:
			ret = UDS_NRC_SFNS;
			break;
		}
		break;
	case UDS_ROUTINE_CTRL_SF_STOP:
		ret = UDS_NRC_SFNS;
		break;
	case UDS_ROUTINE_CTRL_SF_REQT_RESULTS:
		switch (rid)
		{
		case UDS_RID_CLEAN_TBOX_EOL:
			*_out_len = 4;
			memcpy(_out_param, _param, 4);
			_out_param[3] = ROUTINE_STATUS_CORRECT;
			break;
		case UDS_RID_ERASE_MEM:
		case UDS_RID_CHECK_PROM_DPED:
		case UDS_RID_ERASE_MIRROR_MEM_DTCS:
		case UDS_RID_CHECK_PROM_INTY:
		case UDS_RID_CHECK_PROM_PREC:
		default:
			ret = UDS_NRC_SFNS;
			break;
		}
		break;
	default:
		ret = UDS_NRC_SFNS;
		break;
	}

	return ret;
}

static uint8_t request_download_handler(const uint8_t *const _param, const uint16_t _len, uint8_t *const _out_param, uint16_t *const _out_len)
{
	uint8_t  size_len = 0;
	uint8_t  addr_len = 0;
	uint32_t block_size = BLOCK_LENGTH;

	if (4 > _len)
		return UDS_NRC_INLOIF;

	size_len = _param[1] >> 4;
	addr_len = 0xF & _param[1];

	if (2 + size_len + addr_len > _len)
		return UDS_NRC_INLOIF;
 
	g_transfer_addr = 0;
	memcpy(&g_transfer_addr, _param + 2, addr_len);
	bytesswap(&g_transfer_addr, addr_len);

	if (MEM_APP_BASE_ADDR > g_transfer_addr || MEM_BASE_ADDR + MEM_TOTAL_SIZE < g_transfer_addr)
		return UDS_NRC_ROOR;

	g_transfer_size = 0;
	memcpy(&g_transfer_size, _param + 2 + addr_len, size_len);
	bytesswap(&g_transfer_size, size_len);

	if (MEM_TOTAL_SIZE - (MEM_APP_BASE_ADDR - MEM_BASE_ADDR) < g_transfer_size)
		return UDS_NRC_ROOR;

	g_transfer_type = TRANSFER_TYPE_DOWNLOAD;
	g_transfered_size = 0;
	g_block_count = 0;
	*_out_len = 1 + LENGTN_OF_BLOCK_LENGTH;
	_out_param[0] = LENGTN_OF_BLOCK_LENGTH << 4;
	memcpy(_out_param + 1, &block_size, LENGTN_OF_BLOCK_LENGTH);
	bytesswap(_out_param + 1, LENGTN_OF_BLOCK_LENGTH);

	return UDS_NRC_PR;
}

static uint8_t request_transfer_data_handler(const uint8_t *const _param, const uint16_t _len, uint8_t *const _out_param, uint16_t *const _out_len)
{
	if (2 > _len)
		return UDS_NRC_INLOIF;

	if (TRANSFER_TYPE_INVALID == g_transfer_type || g_transfered_size >= g_transfer_size)
		return UDS_NRC_RSE;

	if (_len - 1 > BLOCK_LENGTH || g_transfered_size + _len - 1 > g_transfer_size)
		return UDS_NRC_TDS;

	if (g_block_count != _param[0] && g_block_count + 1 != _param[0])
		return UDS_NRC_WBSC;

	if(g_block_count == _param[0])
		g_transfered_size -= BLOCK_LENGTH;

	uint8_t buf[BLOCK_LENGTH];

	memset(buf, 0xFF, BLOCK_LENGTH);
	memcpy(buf, _param + 1, _len - 1);

	/* Program and verify */
	diag_server_flash_program_callback(g_transfer_addr + g_transfered_size, BLOCK_LENGTH, buf);
	diag_server_flash_program_verify_callback(g_transfer_addr + g_transfered_size, BLOCK_LENGTH, buf);

	g_transfered_size += _len - 1;
	g_block_count = _param[0];
	*_out_len = 2;
	_out_param[0] = g_block_count;
	_out_param[1] = 0;

	return UDS_NRC_PR;
}

static uint8_t request_transfer_exit_handler(const uint8_t *const _param, const uint16_t _len, uint8_t *const _out_param, uint16_t *const _out_len)
{
	if (TRANSFER_TYPE_INVALID == g_transfer_type || g_transfered_size < g_transfer_size)
		return UDS_NRC_RSE;

	g_transfer_type = TRANSFER_TYPE_INVALID;
	g_transfer_addr = 0;
	g_transfer_size = 0;
	g_transfered_size = 0;
	g_block_count = 0;
	*_out_len = 0;

	return UDS_NRC_PR;
}

static uint8_t request_ctrl_dtc_setting_handler(const uint8_t *const _param, const uint16_t _len, uint8_t *const _out_param, uint16_t *const _out_len)
{
	uint8_t sf_param = UDS_SF_PARAM_VAL(_param[0]);

	if(UDS_CTRL_DTC_SETTING_SF_ON == sf_param)
		g_dtc_ctrl = true;
	else if(UDS_CTRL_DTC_SETTING_SF_OFF == sf_param)
		g_dtc_ctrl = false;
	else
		return UDS_NRC_SFNS;

	*_out_len = 1;
	_out_param[0] = sf_param;

	return UDS_NRC_PR;
}
