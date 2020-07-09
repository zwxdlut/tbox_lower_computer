#ifndef __DIAG_DEFINE_H__
#define __DIAG_DEFINE_H__

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DIAG_MAX_BUF_SIZE                                                            1024  /**< Message data buffer size. */

#define FUNCTION_ID                                                                  0x7DF /**< Functional ID. */

/**
 * @name Word/Half word operation.
 * @{
 */
#define HWORD_LOW_BYTE(HWORD)                                                        ( 0x00FFu & (HWORD))
#define HWORD_HIGH_BYTE(HWORD)                                                       ((0xFF00u & (HWORD)) >> 8)
#define WORD_BYTE0(WORD)                                                             ( 0x000000FFu & (WORD))
#define WORD_BYTE1(WORD)                                                             ((0x0000FF00u & (WORD)) >> 8)
#define WORD_BYTE2(WORD)                                                             ((0x00FF0000u & (WORD)) >> 16)
#define WORD_BYTE3(WORD)                                                             ((0xFF000000u & (WORD)) >> 24)
/** @} */ /* Word/Half word operator. */

/**
 * @name OBD-II Mode.
 * @{
 */
#define OBD_MODE_SHOW_CURRENT_DATA                                                   0x01
#define OBD_MODE_SHOW_FREEZE_FRAME                                                   0x02
#define OBD_MODE_READ_DTC                                                            0x03
#define OBD_MODE_CLEAR_DTC                                                           0x04
#define OBD_MODE_TEST_RESULTS_NON_CAN                                                0x05
#define OBD_MODE_TEST_RESULTS_CAN                                                    0x06
#define OBD_MODE_READ_PENDING_DTC                                                    0x07
#define OBD_MODE_CONTROL_OPERATIONS                                                  0x08
#define OBD_MODE_VEHICLE_INFORMATION                                                 0x09
#define OBD_MODE_READ_PERM_DTC                                                       0x0A
/** @} */ /* OBD-II Mode. */

/**
 * @name UDS service id and sub-function parameter operation.
 * @{
 */
#define UDS_SID_PR(SID)                                                              ((SID) + 0x40) /**< Make positive response service id. */
#define UDS_SF_PARAM_VAL(PARAM)                                                      (UDS_SID_NR & (PARAM))
#define UDS_SF_PARAM_SPRMIB(PARAM)                                                   ((~UDS_SID_NR) & (PARAM)))
#define UDS_MAKE_SF_PARAM_SPR(VALUE)                                                 (~UDS_SID_NR | (VALUE))
#define UDS_MAKE_SF_PARAM_PR(VALUE)                                                  (UDS_SID_NR & (VALUE))
/** @} */ /* UDS service id and sub-function parameter operation. */

/**
 * @name UDS SID.
 * @{
 */
#define UDS_SID_DIAG_SESSION_CTRL                                                    0x10
#define UDS_SID_ECU_RESET                                                            0x11
#define UDS_SID_CLEAR_DIAG_INFO                                                      0x14
#define UDS_SID_READ_DTC_INFO                                                        0x19
#define UDS_SID_READ_DATA_BY_ID                                                      0x22
#define UDS_SID_READ_MEM_BY_ADDR                                                     0x23
#define UDS_SID_READ_SCALING_DATA_BY_ID                                              0x24
#define UDS_SID_SECURITY_ACCESS                                                      0x27
#define UDS_SID_COM_CTRL                                                             0x28
#define UDS_SID_READ_DATA_BY_PERIODIC_ID                                             0x2A
#define UDS_SID_DYN_DEFINE_DATA_ID                                                   0x2C
#define UDS_SID_WRITE_DATA_BY_ID                                                     0x2E
#define UDS_SID_IO_CTRL_BY_ID                                                        0x2F
#define UDS_SID_ROUTINE_CTRL                                                         0x31
#define UDS_SID_REQT_DOWNLOAD                                                        0x34
#define UDS_SID_REQT_UPLOAD                                                          0x35
#define UDS_SID_TRANSFER_DATA                                                        0x36
#define UDS_SID_REQT_TRANSFER_EXIT                                                   0x37
#define UDS_SID_WRITE_MEM_BY_ADDR                                                    0x3D
#define UDS_SID_TESTER_PRESENT                                                       0x3E
#define UDS_SID_ACCESS_TIMING_PARAM                                                  0x83
#define UDS_SID_SECURED_DATA_TRANS                                                   0x84
#define UDS_SID_CTRL_DTC_SETTING                                                     0x85
#define UDS_SID_RESP_ON_EVENT                                                        0x86
#define UDS_SID_LINK_CTRL                                                            0x87
#define UDS_SID_NR                                                                   0x7F
#define UDS_SERVICE_NUM                                                              25u
/** @} */ /* UDS SID. */

/**
 * @name UDS NRC.
 * @{
 */
#define UDS_NRC_PR                                                                   0x00
#define UDS_NRC_GR                                                                   0x10
#define UDS_NRC_SNS                                                                  0x11
#define UDS_NRC_SFNS                                                                 0x12
#define UDS_NRC_INLOIF                                                               0x13
#define UDS_NRC_RTL                                                                  0x14
#define UDS_NRC_BRR                                                                  0x21
#define UDS_NRC_CNC                                                                  0x22
#define UDS_NRC_RSE                                                                  0x24
#define UDS_NRC_NRFSC                                                                0x25
#define UDS_NRC_ROOR                                                                 0x31
#define UDS_NRC_SAD                                                                  0x33
#define UDS_NRC_IK                                                                   0x35
#define UDS_NRC_ENOA                                                                 0x36
#define UDS_NRC_RTDNE                                                                0x37
#define UDS_NRC_TDS                                                                  0x71
#define UDS_NRC_GPF                                                                  0x72
#define UDS_NRC_WBSC                                                                 0x73
#define UDS_NRC_RCRRP                                                                0x78
#define UDS_NRC_SFNSIAS                                                              0x7E
#define UDS_NRC_SNSIAS                                                               0x7F
#define UDS_NRC_NO_RESP                                                              0xFF
/** @} */ /* UDS NRC. */

/**
 * @name Diagnostic session control sub-function.
 * @{
 */
#define UDS_DIAG_SESSION_CTRL_SF_DEFAULT_MODE                                        0x01
#define UDS_DIAG_SESSION_CTRL_SF_PROGRAM_MODE                                        0x02
#define UDS_DIAG_SESSION_CTRL_SF_EXTEND_MODE                                         0x03
#define UDS_DIAG_SESSION_CTRL_SF_SAFETY_MODE                                         0x04
#define UDS_DIAG_SESSION_CTRL_SF_NUM                                                 4
/** @} */ /* Diagnostic session control sub-function. */

/**
 * @name ECU Reset sub-function.
 * @{
 */
#define UDS_ECU_RESET_SF_HARD                                                        0x01
#define UDS_ECU_RESET_SF_KEY_OFF_ON                                                  0x02
#define UDS_ECU_RESET_SF_SOFT                                                        0x03
#define UDS_ECU_RESET_SF_NUM                                                         3u
/** @} */ /* ECU Reset sub-function. */

/**
 * @name Read DTC information sub-function.
 * @{
 */
#define UDS_READ_DTC_INFO_SF_RPT_NUM_OF_DTC_BY_STS_MASK                              0x01
#define UDS_READ_DTC_INFO_SF_RPT_DTC_BY_STS_MASK                                     0x02
#define UDS_READ_DTC_INFO_SF_RPT_DTC_SNAPSHOT_ID                                     0x03
#define UDS_READ_DTC_INFO_SF_RPT_DTC_SNAPSHOT_REC_BY_DTC_NUM                         0x04
#define UDS_READ_DTC_INFO_SF_RPT_DTC_SNAPSHOT_REC_BY_REC_NUM                         0x05
#define UDS_READ_DTC_INFO_SF_RPT_DTC_EXD_DATA_REC_BY_DTC_NUM                         0x06
#define UDS_READ_DTC_INFO_SF_RPT_NUM_OF_DTC_BY_SRV_MASK_REC                          0x07
#define UDS_READ_DTC_INFO_SF_RPT_DTC_BY_SRV_MASK_REC                                 0x08
#define UDS_READ_DTC_INFO_SF_RPT_SRV_INFO_OF_DTC                                     0x09
#define UDS_READ_DTC_INFO_SF_RPT_SUPPORTED_DTC                                       0x0A
#define UDS_READ_DTC_INFO_SF_RPT_FST_TEST_FAILED_DTC                                 0x0B
#define UDS_READ_DTC_INFO_SF_RPT_FST_CONFIRMED_DTC                                   0x0C
#define UDS_READ_DTC_INFO_SF_RPT_MOST_RECENT_TEST_FAILED_DTC                         0x0D
#define UDS_READ_DTC_INFO_SF_RPT_MOST_RECENT_CONFIRMED_DTC                           0x0E
#define UDS_READ_DTC_INFO_SF_RPT_MIR_MEM_DTC_BY_STS_MASK                             0x0F
#define UDS_READ_DTC_INFO_SF_RPT_MIR_MEM_DTC_EXD_DATA_REC_BY_DTC_NUM                 0x10
#define UDS_READ_DTC_INFO_SF_RPT_NUM_OF_MIR_MEM_DTC_BY_STS_MASK                      0x11
#define UDS_READ_DTC_INFO_SF_RPT_NUM_OF_EMIS_RELATED_OBD_DTC_BY_STS_MASK             0x12
#define UDS_READ_DTC_INFO_SF_RPT_EMIS_RELATED_OBD_DTC_BY_STS_MASK                    0x13
#define UDS_READ_DTC_INFO_SF_NUM                                                     19
/** @} */ /* Read DTC information sub-function. */

/**
 * @name Security access sub-function.
 * @{
 */
#define UDS_SECURITY_ACCESS_SF_REQT_SEED_L1                                          0x01
#define UDS_SECURITY_ACCESS_SF_SEND_KEY_L1                                           0x02
#define UDS_SECURITY_ACCESS_SF_REQT_SEED_L2                                          0x03
#define UDS_SECURITY_ACCESS_SF_SEND_KEY_L2                                           0x04
#define UDS_SECURITY_ACCESS_SF_REQT_SEED_L3                                          0x05
#define UDS_SECURITY_ACCESS_SF_SEND_KEY_L3                                           0x06
#define UDS_SECURITY_ACCESS_SF_REQT_SEED_L4                                          0x09
#define UDS_SECURITY_ACCESS_SF_SEND_KEY_L4                                           0x0A
#define UDS_SECURITY_ACCESS_SF_NUM                                                   8
/** @} */ /* Security access sub-function. */

/**
 * @name Communication control sub-function.
 * @{
 */
#define UDS_COM_CTRL_SF_EN_RX_TX                                                     0x00
#define UDS_COM_CTRL_SF_EN_RX_DIS_TX                                                 0x01
#define UDS_COM_CTRL_SF_DIS_RX_EN_TX                                                 0x02
#define UDS_COM_CTRL_SF_DIS_RX_TX                                                    0x03
#define UDS_COM_CTRL_SF_NUM                                                          4
/** @} */ /* Communication control sub-function. */

/**
 * @name Read data by periodical identifier sub-function.
 * @{
 */
#define UDS_READ_DATA_BY_PERIODICL_ID_SF_SEND_AT_SLOW_RATE                           0x01
#define UDS_READ_DATA_BY_PERIODICL_ID_SF_SEND_AT_MEDIUM_RATE                         0x02
#define UDS_READ_DATA_BY_PERIODICL_ID_SF_SEND_AT_FAST_RATE                           0x03
#define UDS_READ_DATA_BY_PERIODICL_ID_SF_STOP_SENDING                                0x04
#define UDS_READ_DATA_BY_PERIODICL_ID_SF_NUM                                         4
/** @} */ /* Read data by periodical identifier sub-function. */

/**
 * @name Dynamically define data identifier sub-function.
 * @{
 */
#define UDS_DYN_DEFINE_DATA_ID_SF_DEFINE_BY_ID                                       0x01
#define UDS_DYN_DEFINE_DATA_ID_SF_CLEAR_DYN_DEFINED_DATA_ID                          0x03
#define UDS_DYN_DEFINE_DATA_ID_SF_NUM                                                2
/** @} */ /* Dynamically define data identifier sub-function. */

/**
 * @name Routine control sub-function.
 * @{
 */
#define UDS_ROUTINE_CTRL_SF_START                                                   0x01
#define UDS_ROUTINE_CTRL_SF_STOP                                                    0x02
#define UDS_ROUTINE_CTRL_SF_REQT_RESULTS                                            0x03
#define UDS_ROUTINE_CTRL_SF_NUM                                                     3
/** @} */ /* Test present sub-function. */

/**
 * @name Test present sub-function.
 * @{
 */
#define UDS_TESTER_PRESENT_SF_ZERO                                                   0x00
#define UDS_TESTER_PRESENT_SF_NUM                                                    1
/** @} */ /* Test present sub-function. */

/**
 * @name Control DTC setting sub-function.
 * @{
 */
#define UDS_CTRL_DTC_SETTING_SF_ON                                                   0x01
#define UDS_CTRL_DTC_SETTING_SF_OFF                                                  0x02
#define UDS_CTRL_DTC_SETTING_SF_NUM                                                  2
/** @} */ /* Control DTC setting sub-function. */

/**
 * @name Link control sub-function.
 * @{
 */
#define UDS_LINK_CTRL_SF_VERIFY_BAUD_TRANS_WITH_FIXED_BAUD                           0x01
#define UDS_LINK_CTRL_SF_VERIFY_BAUD_TRANS_WITH_SPEC_BAUD                            0x02
#define UDS_LINK_CTRL_SF_TRANS_BAUD                                                  0x03
#define UDS_LINK_CTRL_SF_NUM                                                         3
/** @} */ /* Link control sub-function. */

/**
 * @name DTC format identifier.
 * @{
 */
#define SAE_J2012_DA_DTC_FORMAT                                                      0x00
#define ISO_14229_1_DTC_FORMAT                                                       0x01
#define SAE_J1939_73_DTC_FORMAT									                     0x02
#define ISO_11992_4_DTC_FORMAT									                     0x03
#define SAE_J2012_DA_DTC_FORMAT_04       					                         0x04
/** @} */ /* DTC format identifier. */

/**
 * @name DTC group.
 * @{
 */
#define DTC_ERS                                                                      0x000000
#define DTC_PG                                                                       0x100000
#define DTC_CG                                                                       0x400000
#define DTC_BG                                                                       0x800000
#define DTC_NCG                                                                      0xC00000
#define DTC_AG                                                                       0xFFFFFF
/** @} */ /* DTC group. */

/**
 * @name DTC status mask.
 * @{
 */
#define DTC_STATUS_MASK_TF                                                           0x01
#define DTC_STATUS_MASK_TFTMC                                                        0x02
#define DTC_STATUS_MASK_PDTC                                                         0x04
#define DTC_STATUS_MASK_CDTC                                                         0x08
#define DTC_STATUS_MASK_TNCSLC                                                       0x10
#define DTC_STATUS_MASK_TFSLC                                                        0x20
#define DTC_STATUS_MASK_TNCTMC                                                       0x40
#define DTC_STATUS_MASK_WIR                                                          0x80
/** @} */ /* DTC status mask. */

/**
 * @name DTC.
 * @{
 */
#define DTC_BAT_UNDER_VOL                                                            0x911716
#define DTC_BAT_OVER_VOL                                                             0x911717
#define DTC_SIM_UNPRESENT                                                            0x940076
#define DTC_GPS_ANTENNA_LOST                                                         0x940113
#define DTC_BAT_INVALID_VOL                                                          0x940114
#define DTC_NM_LIMP                                                                  0xE20D00
#define DTC_COMM_ACC_LOST                                                            0xC10400
#define DTC_COMM_EMS_LOST                                                            0xC10000
#define DTC_COMM_ABS_ESP_LOST                                                        0xC12100
#define DTC_COMM_SRS_LOST                                                            0xD20000
#define DTC_COMM_TPMS_LOST                                                           0xC12700
#define DTC_COMM_BCM_LOST                                                            0xC14000
#define DTC_COMM_AC_LOST                                                             0xC16400
#define DTC_COMM_PEPS_LOST                                                           0xC21400
#define DTC_COMM_MP5_LOST                                                            0xC16300
#define DTC_COMM_VCU_LOST                                                            0xC25900
#define DTC_COMM_BMS_LOST                                                            0xC11100
#define DTC_COMM_MCU_LOST                                                            0xC11000
#define DTC_COMM_OBC_LOST                                                            0xC19B00
#define DTC_COMM_DCDC_LOST                                                           0xC29800
#define DTC_COMM_AC_COMP_LOST                                                        0xC10F00
#define DTC_NUM                                                                      21
/** @} */ /* DTC. */

/**
 * @name DTC snapshot data identifier.
 * @{
 */
#define VSDID                                                                        0xDF01
#define ECUVDID                                                                      0xDF02
/** @} */ /* DTC snapshot data identifier. */

/**
 * @name DTC snapshot data length.
 * @{
 */
#define VS_LEN                                                                       2
#define ECUV_LEN                                                                     2
/** @} */ /* DTC snapshot data length. */

/**
 * @name DTC snapshot and data number.
 * @{
 */
#define DTC_SNAPSHOT01_DID_NUM                                                      2
#define DTC_SNAPSHOT_NUM                                                            1
/** @} */ /* DTC snapshot and data number. */

/**
 * @name DTC extend data number and length.
 * @{
 */
#define DTC_EXTEND_DATA01_LEN                                                        2
#define DTC_EXTEND_DATA_NUM                                                          1
/** @} */ /* DTC extend data number and length. */

/**
 * @name Read/Write data identifier.
 * @{
 */
#define TBTNDID                                                                      0x1201
#define IMEIDID                                                                      0x1202
#define ICCIDDID                                                                     0x1203
#define PKDID                                                                        0x1204
#define EWDID                                                                        0x1205
#define EDDID                                                                        0x1206
#define CIPDID                                                                       0x120B
#define LIPDID                                                                       0x120C
#define EIPDID                                                                       0x120D
#define EPSVDID                                                                      0xDF00
#define FP1DID                                                                       0xF15A
#define FP2DID                                                                       0xF15B
#define EOLDID                                                                       0xF120
#define BSIDID                                                                       0xF180
#define ADSDID                                                                       0xF186
#define VMSPNDID                                                                     0xF187
#define SSIDDID                                                                      0xF18A
#define ECUMDDID                                                                     0xF18B
#define ECUSNDID                                                                     0xF18C
#define VINDID                                                                       0xF190
#define SSECUHWVNDID                                                                 0xF193
#define SSECUSWVNDID                                                                 0xF195
#define SNOETDID                                                                     0xF197
#define RW_DID_NUM                                                                   23
/** @} */ /* Read/Write data identifier. */

/**
 * @name Read/Write data length.
 * @{
 */
#define TBTN_LEN                                                                     15
#define IMEI_LEN                                                                     15
#define ICCID_LEN                                                                    20
#define PK_LEN                                                                       16
#define ESK_LEN                                                                      6
#define IP_LEN                                                                       18
#define EPSV_LEN                                                                     1
#define FP1_LEN                                                                      9
#define FP2_LEN                                                                      20
#define EOL_LEN                                                                      56
#define BSI_LEN    									 	                             16
#define ADS_LEN                                                                      1
#define VMSPN_LEN                                                                    10
#define SSID_LEN                                                                     10
#define ECUMD_LEN	                                                                 4
#define ECUSN_LEN                                                                    29
#define VIN_LEN                                                                      17
#define SSECUHWVN_LEN                                                                16
#define SSECUSWVN_LEN                                                                19
#define SNOET_LEN                                                                    10
/** @} */ /* Read/Write data length. */

/**
 * @name Security access level.
 * @{
 */
#define UDS_SECURITY_ACCESS_L1                                                       0x00
#define UDS_SECURITY_ACCESS_L2                                                       0x01
#define UDS_SECURITY_ACCESS_L3                                                       0x02
#define UDS_SECURITY_ACCESS_L4                                                       0x03
#define UDS_SECURITY_ACCESS_NONE                                                     0x04
/** @} */ /* Security access level. */

/**
 * @name Communication control message type mask.
 * @{
 */
#define COMM_MSG_TYPE_MASK_NCM                                                       0x01
#define COMM_MSG_TYPE_MASK_NWMCM                                                     0x02
/** @} */ /*  Communication control message type mask. */

/**
 * @name Routine identifier.
 * @{
 */
#define UDS_RID_ERASE_MEM                                                            0xFF00
#define UDS_RID_CHECK_PROM_DPED                                                      0xFF01
#define UDS_RID_ERASE_MIRROR_MEM_DTCS                                                0xFF02
#define UDS_RID_CHECK_PROM_INTY                                                      0x0202
#define UDS_RID_CHECK_PROM_PREC                                                      0x0203
#define UDS_RID_CLEAN_TBOX_EOL                                                       0xF002
#define UDS_RID_NUM                                                                  6
/** @} */ /* Routine identifier. */

/**
 * @name Routine status.
 * @{
 */
#define ROUTINE_STATUS_CORRECT                                                      0
#define ROUTINE_STATUS_INCORRECT                                                    1
/** @} */ /* Routine status. */

#ifdef __cplusplus
}
#endif

#endif /* __DIAG_DEFINE_H__ */
