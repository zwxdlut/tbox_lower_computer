/*
 * i2c.h
 *
 *  Created on: 2019��1��9��
 *      Author: Administrator
 */

#ifndef __I2C_H__
#define __I2C_H__

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * Definitions
 ******************************************************************************/
/**
 * @name The I2C channel indexes
 * @{
 */
#define I2C0_INDEX       		                0
/** @} */ // The I2C channel indexes

/** 
 * @name The EEPROM configuration
 * 
 * Two-wire serial EEPROM AT24C02 2K(256 * 8 bit) 32 pages of 8 bytes each.
 * 
 * @{
 */
#define EEPROM_ADDR              			     0x50 ///< The device address, 7bit without R/W bit.
#define EEPROM_PAGE_SIZE                         8 ///< the page size in bytes
#define EEPROM_TOTAL_SIZE                        256 ///< the total size in bytes
#define EEPROM_ADDR_RESET_TYPE                   0x00
#define EEPROM_SIZE_RESET_TYPE                   1
#define EEPROM_ADDR_SSECUHWVN                    (EEPROM_ADDR_RESET_TYPE + EEPROM_SIZE_RESET_TYPE)
#define EEPROM_SIZE_SSECUHWVN                    16
#define EEPROM_ADDR_VIN                          (EEPROM_ADDR_SSECUHWVN + EEPROM_SIZE_SSECUHWVN)
#define EEPROM_SIZE_VIN                          17
#define EEPROM_ADDR_ECUMD                        (EEPROM_ADDR_VIN + EEPROM_SIZE_VIN)
#define EEPROM_SIZE_ECUMD                        4
#define EEPROM_ADDR_TBTN                         (EEPROM_ADDR_ECUMD + EEPROM_SIZE_ECUMD)
#define EEPROM_SIZE_TBTN                         15
#define EEPROM_ADDR_IMEI                         (EEPROM_ADDR_TBTN + EEPROM_SIZE_TBTN)
#define EEPROM_SIZE_IMEI                         15
#define EEPROM_ADDR_ICCID                        (EEPROM_ADDR_IMEI + EEPROM_SIZE_IMEI)
#define EEPROM_SIZE_ICCID                        20
#define EEPROM_ADDR_PK                           (EEPROM_ADDR_ICCID + EEPROM_SIZE_ICCID)
#define EEPROM_SIZE_PK                           16
#define EEPROM_ADDR_FP1                          (EEPROM_ADDR_PK + EEPROM_SIZE_PK)
#define EEPROM_SIZE_FP1                          9
#define EEPROM_ADDR_FP2                          (EEPROM_ADDR_FP1 + EEPROM_SIZE_FP1)
#define EEPROM_SIZE_FP2                          20
#define EEPROM_ADDR_EOL                          (EEPROM_ADDR_FP2 + EEPROM_SIZE_FP2)
#define EEPROM_SIZE_EOL                          56
#define EEPROM_ADDR_DTC                          (EEPROM_ADDR_EOL + EEPROM_SIZE_EOL)
#define EEPROM_SIZE_DTC                          21
#define EEPROM_ADDR_INIT                         EEPROM_TOTAL_SIZE - 1
#define EEPROM_SIZE_INIT                         1
/** @} */ // The EEPROM configuration

/** 
 * @name the accelerometer configuration.
 * 
 * 3-axis, 12-bit/8-bit digital accelerometer MMA8452Q.
 * 
 * @{
 */
#define ACCR_ADDR                       		0x1C ///< the device address, 7bit without R/W bit.
#define ACCR_STATUS_REG	                        0x00
#define ACCR_OUT_X_MSB_REG	                    0x01
#define ACCR_OUT_X_LSB_REG	                    0x02
#define ACCR_OUT_Y_MSB_REG	                    0x03
#define ACCR_OUT_Y_LSB_REG	                    0x04
#define ACCR_OUT_Z_MSB_REG	                    0x05
#define ACCR_OUT_Z_LSB_REG	                    0x06
//#define ACCR_RESERVED_REG	                    0x07
//#define ACCR_RESERVED_REG	                    0x08
#define ACCR_SYSMOD_REG	                        0x0B
#define ACCR_INT_SOURCE_REG	                    0x0C
#define ACCR_WHO_AM_I_REG	                    0x0D
#define ACCR_XYZ_DATA_CFG_REG	                0x0E
#define ACCR_P_FILTER_CUTOFF_REG	            0x0F
#define ACCR_PL_STATUS_REG	                    0x10
#define ACCR_PL_CFG_REG	                        0x11
#define ACCR_PL_COUNT_REG	                    0x12
#define ACCR_PL_BF_ZCOMP_REG	                0x13
#define ACCR_P_L_THS_REG_REG	                0x14
#define ACCR_FF_MT_CFG_REG	                    0x15
#define ACCR_FF_MT_SRC_REG	                    0x16
#define ACCR_FF_MT_THS_REG	                    0x17
#define ACCR_FF_MT_COUNT_REG	                0x18
//#define ACCR_RESERVED_REG	                    0x19-0x1C
#define ACCR_TRANSIENT_CFG_REG	                0x1D
#define ACCR_TRANSIENT_SRC_REG	                0x1E
#define ACCR_TRANSIENT_THS_REG	                0x1F
#define ACCR_RANSIENT_COUNT_REG	                0x20
#define ACCR_PULSE_CFG_REG		                0x21
#define ACCR_PULSE_SRC_REG		                0x22
#define ACCR_PULSE_THSX_REG		                0x23
#define ACCR_PULSE_THSY_REG		                0x24
#define ACCR_PULSE_THSZ_REG		                0x25
#define ACCR_PULSE_TMLT_REG		                0x26
#define ACCR_PULSE_LTCY_REG		                0x27
#define ACCR_PULSE_WIND_REG		                0x28
#define ACCR_ASLP_COUNT_REG		                0x29
#define ACCR_CTRL_REG1_REG		                0x2A
#define ACCR_CTRL_REG2_REG		                0x2B
#define ACCR_CTRL_REG3_REG		                0x2C
#define ACCR_CTRL_REG4_REG		                0x2D
#define ACCR_CTRL_REG5_REG		                0x2E
#define ACCR_OFF_X_REG			                0x2F
#define ACCR_OFF_Y_REG			                0x30
#define ACCR_OFF_Z_REG			                0x31
//#define ACCR_RESERVED_REG		                0x40-0x7F
/** @} */ // the accelerometer configuration.

/** 
 * @name The accelerometer system modes.
 * @{
 */
#define ACCR_SYSMOD_STANDBY                     0u
#define ACCR_SYSMOD_ACTIVE                      1u
/** @} */ // The accelerometer system modes.

/** 
 * @name The accelerometer interrupt source mask.
 * @{
 */
#define ACCR_INT_NONE_MASK                      0x00
#define ACCR_INT_ASLP_MASK                      0x80
#define ACCR_INT_TRANS_MASK                     0x20
#define ACCR_INT_LNDPRT_MASK                    0x10
#define ACCR_INT_PULSE_MASK                     0x08
#define ACCR_INT_FF_MT_MASK                     0x04
#define ACCR_INT_DRDY_MASK                      0x01
/** @} */ // The accelerometer interrupt source mask.

/******************************************************************************
 * Function prototypes
 ******************************************************************************/
/**
 * Initialize the I2C bus controller as master.
 *
 * @param [in] _index the I2C channel index
 * @param [in] _baudrate the baud rate
 * @param [in] _is_10bit_addr if 7-bit or 10-bit slave address
 * @return 0(success) or other values(failure)
 */
int32_t i2c_master_init(const uint8_t _index, const uint32_t _baudrate, const bool _is_10bit_addr);

/**
 * De-initialize the I2C bus controller as master.
 *
 * @param [in] _index the I2C channel index
 * @return 0(success) or other values(failure)
 */
int32_t i2c_master_deinit(const uint8_t _index);

/**
 * Receive data from the specified slave device.
 *
 * @param [in] _index the I2C channel index
 * @param [in] _addr the slave device address(7 bit without R/W bit)
 * @param [out] _buf the buffer to receive to
 * @param [in] _size the size to receive
 * @param [in] _stop if generate stop condition after sending
 * @return 0(success) or other values(failure)
 */
int32_t i2c_master_receive(const uint8_t _index, const uint16_t _addr, uint8_t *const _buf, const uint16_t _size, const bool _stop);

/**
 * Transmit data to the specified slave device.
 *
 * @param [in] _index the I2C channel index
 * @param [in] _addr the slave device address(7 bit without R/W bit)
 * @param [in] _buf the buffer to transmit from
 * @param [in] _size the size to transmit
 * @param [in] _stop if generate stop condition after sending
 * @return 0(success) or other values(failure)
 */
int32_t i2c_master_transmit(const uint8_t _index, const uint16_t _addr, const uint8_t *const _buf, const uint16_t _size, const bool _stop);

/**
 * Read data from the EEPROM.
 *
 * @param [in] _addr the start address to read from
 * @param [out] _buf the buffer to read to
 * @param [in] _size the size to read
 * @return 0(success) or other values(failure)
 */
int32_t eeprom_read(const uint8_t _addr, uint8_t *const _buf, const uint16_t _size);

/**
 * Write data to the EEPROM.
 *
 * @param [in] _addr the start address to write to
 * @param [in] _buf the buffer to write from
 * @param [in] _size the size to write
 * @return 0(success) or other values(failure)
 */
int32_t eeprom_write(const uint8_t _addr, const uint8_t *const _buf, const uint16_t _size);

/**
 * Reset the accelerometer.
 *
 * @return 0(success) or other values(failure)
 */
int32_t accr_reset(void);

/**
 * Initialize the accelerometer
 *
 * @param [in] _int_src the interrupt source which which "OR" operation by ACCR_INT_XXX_MASK
 * @return 0(success) or other values(failure)
 */
int32_t accr_init(const uint8_t _int_src);

/**
 * De-initialize the accelerometer.
 *
 * @return 0(success) or other values(failure)
 */
int32_t accr_deinit(void);

/**
 * Transfer the accelerometer system mode.
 *
 * @param [in] _mode the system mode to transfer to:
 * <ul>
 * <li>{@link #ACCR_SYSMOD_STANDBY} standby</li>
 * <li>{@link #ACCR_SYSMOD_ACTIVE} active</li>
 * </ul>
 * @return 0(success) or other values(failure)
 */
int32_t accr_sys_mode_trans(const uint8_t _mode);

/**
 * Get the interrupt source.
 *
 * @return the interrupt source which combination of ACCR_INT_XXX
 */
uint8_t accr_get_int_src(void);

/**
 * Get the X, Y, Z-axis sample data(MSB).
 *
 * @param [in] _buf the buffer to get to
 * @param [in] _size the size to get
 * @return 0(success) or other values(failure)
 */
int32_t accr_get_xyz_sample(uint8_t *const _buf, const uint8_t _size);

#ifdef __cplusplus
}
#endif

#endif // __I2C_H__
