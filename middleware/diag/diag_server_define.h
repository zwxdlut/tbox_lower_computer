#ifndef __DIAG_SERVER_DEFINE_H__
#define __DIAG_SERVER_DEFINE_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/**
 * @name Diagnostic server address.
 * @{
 */
#define SERVER_RX_ID                                                                 0x729 /**< Server receive ID */
#define SERVER_TX_ID                                                                 0x7A9 /**< Server transmit ID. */
/** @} */ /* Diagnostic server address. */

/**
 * @name Server time.
 * @{
 */
#define P2CAN_SERVER                                                                 50    /**< P2CAN_Server in milliseconds. */
#define P2ECAN_SERVER                                                                5000  /**< P2*CAN_Server in milliseconds. */
#define S3_SERVER                                                                    5000  /**< S3_Server in milliseconds. */
/** @} */ /* Server time. */

/**
 * @name Security access request seed.
 * @{
 */
#define SECURITY_ACCESS_REQ_SEED_MAX_TRY_TIME                                        3
#define SECURITY_ACCESS_REQ_SEED_TIME_DELAY                                          10000 /**< Milliseconds. */
/** @} */ /* Security access request seed. */

/**
 * @name Battery voltage.
 * @{
 */
#define BAT_VOL_UNDER_THRESHOLD                                                      90   /**< Battery voltage(0.1V) under threshold. */
#define BAT_VOL_OVER_THRESHOLD                                                       160  /**< Battery voltage(0.1V) over threshold. */
#define BAT_VOL_RECOVER_TIME                                                         5000 /**< Battery voltage(0.1V) under/over recover time(in milliseconds). */
/** @} */ /* Battery voltage. */

/**
 * @name Message id.
 * @{
 */
#define MSG_NO_SUPPORTED                                                             0xFFFF
#define MSG_ACC_ID                                                                   0x233
#define MSG_EMS_ID                                                                   0x3F5
#define MSG_ESP_ID                                                                   0x218
#define MSG_SRS_ID                                                                   0x318
#define MSG_TPMS_ID                                                                  0x390
#define MSG_BCM_ID                                                                   0x3A0
#define MSG_AC_ID                                                                    0x3B0
#define MSG_PEPS_ID                                                                  0x090
#define MSG_MP5_ID                                                                   MSG_NO_SUPPORTED
#define MSG_VCU_ID0                                                                  0x118
#define MSG_VCU_ID1                                                                  0x119
#define MSG_VCU_ID2                                                                  0x126
#define MSG_VCU_ID3                                                                  0x128
#define MSG_BMS_ID0                                                                  0x140
#define MSG_BMS_ID1                                                                  0x142
#define MSG_BMS_ID2                                                                  0x143
#define MSG_BMS_ID3                                                                  0x148
#define MSG_BMS_ID4                                                                  0x160
#define MSG_BMS_ID5                                                                  0x162
#define MSG_BMS_ID6                                                                  0x164
#define MSG_BMS_ID7                                                                  0x166
#define MSG_BMS_ID8                                                                  0x168
#define MSG_BMS_ID9                                                                  0x170
#define MSG_BMS_ID10                                                                 0x172
#define MSG_BMS_ID11                                                                 0x174
#define MSG_BMS_ID12                                                                 0x176
#define MSG_BMS_ID13                                                                 0x178
#define MSG_BMS_ID14                                                                 0x180
#define MSG_BMS_ID15                                                                 0x182
#define MSG_BMS_ID16                                                                 0x184
#define MSG_BMS_ID17                                                                 0x186
#define MSG_BMS_ID18                                                                 0x188
#define MSG_BMS_ID19                                                                 0x190
#define MSG_BMS_ID20                                                                 0x192
#define MSG_BMS_ID21                                                                 0x194
#define MSG_BMS_ID22                                                                 0x196
#define MSG_BMS_ID23                                                                 0x198
#define MSG_BMS_ID24                                                                 0x1A0
#define MSG_BMS_ID25                                                                 0x1A2
#define MSG_BMS_ID26                                                                 0x1A4
#define MSG_BMS_ID27                                                                 0x1A6
#define MSG_BMS_ID28                                                                 0x1A8
#define MSG_BMS_ID29                                                                 0x1A9
#define MSG_BMS_ID30                                                                 0x1AA
#define MSG_BMS_ID31                                                                 0x1AB
#define MSG_BMS_ID32                                                                 0x1AC
#define MSG_BMS_ID33                                                                 0x1AD
#define MSG_BMS_ID34                                                                 0x1AE
#define MSG_BMS_ID35                                                                 0x1AF
#define MSG_BMS_ID36                                                                 0x1B0
#define MSG_BMS_ID37                                                                 0x1B2
#define MSG_BMS_ID38                                                                 0x1B4
#define MSG_BMS_ID39                                                                 0x1B6
#define MSG_BMS_ID40                                                                 0x1B8
#define MSG_MCU_ID0                                                                  0x130
#define MSG_MCU_ID1                                                                  0x132
#define MSG_MCU_ID2                                                                  0x181
#define MSG_OBC_ID                                                                   0x183
#define MSG_DCDC_ID                                                                  0x171
#define MSG_AC_COMP_ID                                                               MSG_NO_SUPPORTED
/** @} */ /* Message id. */

/**
 * @name Message type's number.
 * @{
 */
#define MSG_NUM_ACC                                                                  1
#define MSG_NUM_EMS                                                                  1
#define MSG_NUM_ABS_ESP                                                              1
#define MSG_NUM_SRS                                                                  1
#define MSG_NUM_TPMS                                                                 1
#define MSG_NUM_BCM                                                                  1
#define MSG_NUM_AC                                                                   1
#define MSG_NUM_PEPS                                                                 1
#define MSG_NUM_MP5                                                                  1
#define MSG_NUM_VCU                                                                  4
#define MSG_NUM_BMS                                                                  41
#define MSG_NUM_MCU                                                                  3
#define MSG_NUM_OBC                                                                  1
#define MSG_NUM_DCDC                                                                 1
#define MSG_NUM_AC_COMP                                                              1
#define MSG_TYPE_NUM                                                                 15
/** @} */ /* type's number. */

/**
 * @name Message lost time in milliseconds.
 * @{
 */
#define LOST_TIME_ACC                                                                500
#define LOST_TIME_EMS                                                                2000
#define LOST_TIME_ABS_ESP                                                            200
#define LOST_TIME_SRS                                                                1000
#define LOST_TIME_TPMS                                                               500
#define LOST_TIME_BCM                                                                500
#define LOST_TIME_AC                                                                 1000
#define LOST_TIME_PEPS                                                               1000
#define LOST_TIME_MP5                                                                1000
#define LOST_TIME_VCU                                                                1000
#define LOST_TIME_BMS                                                                1000
#define LOST_TIME_MCU                                                                1000
#define LOST_TIME_OBC                                                                1000
#define LOST_TIME_DCDC                                                               1000
#define LOST_TIME_AC_COMP                                                            1000
/** @} */ /* Message lost time in milliseconds. */

/**
 * @name recovery time in milliseconds.
 * @{
 */
#define RECOVERY_TIME_ACC                                                            4000
#define RECOVERY_TIME_EMS                                                            4000
#define RECOVERY_TIME_ABS_ESP                                                        4000
#define RECOVERY_TIME_SRS                                                            4000
#define RECOVERY_TIME_TPMS                                                           4000
#define RECOVERY_TIME_BCM                                                            4000
#define RECOVERY_TIME_AC                                                             4000
#define RECOVERY_TIME_PEPS                                                           4000
#define RECOVERY_TIME_MP5                                                            4000
#define RECOVERY_TIME_VCU                                                            4000
#define RECOVERY_TIME_BMS                                                            4000
#define RECOVERY_TIME_MCU                                                            4000
#define RECOVERY_TIME_OBC                                                            4000
#define RECOVERY_TIME_DCDC                                                           4000
#define RECOVERY_TIME_AC_COMP                                                        4000
/** @} */ /* Message recovery time in milliseconds. */

/**
 * @name Read/Write initialization value.
 * @{
 */
#define TBTN                                                                         "002169800248948"
#define IMEI                                                                         "841561654149859"
#define ICCID                                                                        "8986007711125122585"
#define PK                                                                           "8415616541498595"
#define ESK                                                                          "123456"
#define CIP                                                                          0x00, 0x00, 0x00, 0x9F, 0x00, 0x00, 0x00, 0x90, 0x00, 0x00, 0x00, 0x85, 0xDF, 0x50, 0x50
#define LIP                                                                          0x00, 0x00, 0x00, 0x9F, 0x00, 0x00, 0x00, 0x90, 0x00, 0x00, 0x00, 0x85, 0xDF, 0x50, 0x50
#define EIP                                                                          0x00, 0x00, 0x00, 0x9F, 0x00, 0x00, 0x00, 0x90, 0x00, 0x00, 0x00, 0x85, 0xDF, 0x50, 0x50
#define FP1                                                                          0
#define FP2                                                                          0
#define EOL                                                                          0
#define BSI                                                                          "T010042000000006"
#define VMSPN                                                                        "XXA1234567"
#define SSID                                                                         "MXNAVI"
#define ECUMD                                                                        0x20200101
#define ECUSN                                                                        ""
#define VIN                                                                          ""
#define SSECUHWVN                                                                    "T010042000000005"
#define SSECUSWVN                                                                    "T0100420101020004"
#define SNOET                                                                        "TBOX"
/** @} */ /* Read/Write data initialization value. */

/**
 * @name Car status.
 * @{
 */
#define CAR_STATUS_RUNNING                                                           0x01
#define CAR_STATUS_STOPPED                                                           0x02
#define CAR_STATUS_OTHER                                                             0x03
/** @} */ /* Car status. */

/**
 * @name Flash defines.
 * @{
 */
#if defined S32K144
#define MEM_BASE_ADDR 			                                                     0x00000000
#define MEM_SECTOR_SIZE                                                              4096       /**< 4k. */
#define MEM_TOTAL_SIZE                                                               0x80000    /**< 512k. */
#elif defined STM32F10X_CL
#define MEM_BASE_ADDR 			                                                     0x08000000
#define MEM_SECTOR_SIZE                                                              2048       /**< 2k. */
#define MEM_TOTAL_SIZE                                                               0x40000    /**< 256k. */
#elif defined STM32F205xx
#define MEM_BASE_ADDR 			                                                     0x08000000
#define MEM_SECTOR_SIZE                                                              4096       /**< 4k. */
#define MEM_TOTAL_SIZE                                                               0x80000    /**< 512k. */
#else
#error #error Mcu type not defined!!!
#endif
#define MEM_APP_BASE_ADDR   	                                                     (MEM_BASE_ADDR + 10000) /**< Application base address. */
/** @} */ /* Flash defines. */

/**
 * @name Request download/upload block.
 * @{
 */
#define LENGTN_OF_BLOCK_LENGTH                                                        2
#define BLOCK_LENGTH                                                                  512
/** @} */ /* Request download/upload block. */

/**
 * @name Transfer data type.
 * @{
 */
#define TRANSFER_TYPE_INVALID                                                        0
#define TRANSFER_TYPE_DOWNLOAD                                                       1
#define TRANSFER_TYPE_UPLOAD                                                         2
/** @} */ /* Transfer data type. */

#ifdef __cplusplus
}
#endif

#endif /* __DIAG_SERVER_DEFINE_H__ */
