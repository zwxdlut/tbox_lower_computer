/**
 * @file   gen_upstream.h
 * @author Auto generated
 * @note   Created on 2020/5/28 
 */

#ifndef __GEN_UPSTREAM_H__
#define __GEN_UPSTREAM_H__

#include <stdint.h>

/******************************************************************************
 * Definitions
 ******************************************************************************/
/**
 * @name GB32960.
 * @{
 */
#define GB32960_ID                                              0x8000    /**< GB32960 */
#define CAR_STATUS_START_BIT                                    0         /**< 车辆状态. */
#define CAR_STATUS_BIT_LEN                                      8
#define CAR_STATUS_MIN                                          0x01
#define CAR_STATUS_MAX                                          0x03
#define CHARGING_STATUS_START_BIT                               8         /**< 充电状态. */
#define CHARGING_STATUS_BIT_LEN                                 8
#define CHARGING_STATUS_MIN                                     0x01
#define CHARGING_STATUS_MAX                                     0x04
#define RUNNING_MODE_START_BIT                                  16        /**< 运行模式. */
#define RUNNING_MODE_BIT_LEN                                    8
#define RUNNING_MODE_MIN                                        0x01
#define RUNNING_MODE_MAX                                        0x03
#define SPEED_START_BIT                                         24        /**< 车速. */
#define SPEED_BIT_LEN                                           16
#define SPEED_MIN                                               0
#define SPEED_MAX                                               2200
#define ODO_START_BIT                                           40        /**< 总里程. */
#define ODO_BIT_LEN                                             32
#define ODO_MIN                                                 0
#define ODO_MAX                                                 9999999
#define TOTAL_VOL_START_BIT                                     72        /**< 总电压. */
#define TOTAL_VOL_BIT_LEN                                       16
#define TOTAL_VOL_MIN                                           0
#define TOTAL_VOL_MAX                                           10000
#define TOTAL_CUR_START_BIT                                     88        /**< 总电流. */
#define TOTAL_CUR_BIT_LEN                                       16
#define TOTAL_CUR_MIN                                           0
#define TOTAL_CUR_MAX                                           20000
#define SOC_START_BIT                                           104       /**< SOC. */
#define SOC_BIT_LEN                                             8
#define SOC_MIN                                                 0
#define SOC_MAX                                                 100
#define DCDC_STATUS_START_BIT                                   112       /**< DC/DC状态. */
#define DCDC_STATUS_BIT_LEN                                     8
#define DCDC_STATUS_MIN                                         0x01
#define DCDC_STATUS_MAX                                         0x02
#define SHIFT_START_BIT                                         120       /**< 档位. */
#define SHIFT_BIT_LEN                                           8
#define SHIFT_MIN                                               0x00
#define SHIFT_MAX                                               0x3F
#define INSULATION_RESISTANCE_START_BIT                         128       /**< 绝缘电阻. */
#define INSULATION_RESISTANCE_BIT_LEN                           16
#define INSULATION_RESISTANCE_MIN                               0
#define INSULATION_RESISTANCE_MAX                               60000
#define ACCELERATROR_PEDAL_POS_START_BIT                        144       /**< 加速踏板位置. */
#define ACCELERATROR_PEDAL_POS_BIT_LEN                          8
#define ACCELERATROR_PEDAL_POS_MIN                              0
#define ACCELERATROR_PEDAL_POS_MAX                              100
#define BRAKE_PEDAL_POS_START_BIT                               152       /**< 制动踏板位置. */
#define BRAKE_PEDAL_POS_BIT_LEN                                 8
#define BRAKE_PEDAL_POS_MIN                                     0
#define BRAKE_PEDAL_POS_MAX                                     100
#define MOTOR_NUM_START_BIT                                     160       /**< 驱动电机个数. */
#define MOTOR_NUM_BIT_LEN                                       8
#define MOTOR_NUM_MIN                                           1
#define MOTOR_NUM_MAX                                           1
#define MOTOR_STATUS_START_BIT                                  168       /**< 驱动电机状态. */
#define MOTOR_STATUS_BIT_LEN                                    8
#define MOTOR_STATUS_MIN                                        0x01
#define MOTOR_STATUS_MAX                                        0x04
#define MOTOR_MCU_TEM_START_BIT                                 176       /**< 驱动电机控制器温度. */
#define MOTOR_MCU_TEM_BIT_LEN                                   8
#define MOTOR_MCU_TEM_MIN                                       0
#define MOTOR_MCU_TEM_MAX                                       250
#define MOTOR_SPEED_START_BIT                                   184       /**< 驱动电机转速. */
#define MOTOR_SPEED_BIT_LEN                                     16
#define MOTOR_SPEED_MIN                                         0
#define MOTOR_SPEED_MAX                                         65531
#define MOTOR_TORQUE_START_BIT                                  200       /**< 驱动电机转矩. */
#define MOTOR_TORQUE_BIT_LEN                                    16
#define MOTOR_TORQUE_MIN                                        0
#define MOTOR_TORQUE_MAX                                        65531
#define MOTOR_TEM_START_BIT                                     216       /**< 驱动电机温度. */
#define MOTOR_TEM_BIT_LEN                                       8
#define MOTOR_TEM_MIN                                           0
#define MOTOR_TEM_MAX                                           250
#define MOTOR_MCU_VOL_START_BIT                                 224       /**< 驱动电机控制器输入电压. */
#define MOTOR_MCU_VOL_BIT_LEN                                   16
#define MOTOR_MCU_VOL_MIN                                       0
#define MOTOR_MCU_VOL_MAX                                       60000
#define MOTOR_MCU_CUR_START_BIT                                 240       /**< 驱动电机控制器母线电流. */
#define MOTOR_MCU_CUR_BIT_LEN                                   16
#define MOTOR_MCU_CUR_MIN                                       0
#define MOTOR_MCU_CUR_MAX                                       20000
#define FUEL_BAT_VOL_START_BIT                                  256       /**< 燃料电池电压. */
#define FUEL_BAT_VOL_BIT_LEN                                    16
#define FUEL_BAT_VOL_MIN                                        0
#define FUEL_BAT_VOL_MAX                                        20000
#define FUEL_BAT_CUR_START_BIT                                  272       /**< 燃料电池电流. */
#define FUEL_BAT_CUR_BIT_LEN                                    16
#define FUEL_BAT_CUR_MIN                                        0
#define FUEL_BAT_CUR_MAX                                        20000
#define FUEL_BAT_CONSUME_RATE_START_BIT                         288       /**< 燃料消耗率. */
#define FUEL_BAT_CONSUME_RATE_BIT_LEN                           16
#define FUEL_BAT_CONSUME_RATE_MIN                               0
#define FUEL_BAT_CONSUME_RATE_MAX                               60000
#define FUEL_BAT_TEM_PROB_NUM_START_BIT                         304       /**< 燃料电池温度探针个数. */
#define FUEL_BAT_TEM_PROB_NUM_BIT_LEN                           16
#define FUEL_BAT_TEM_PROB_NUM_MIN                               0
#define FUEL_BAT_TEM_PROB_NUM_MAX                               0
#define ARGON_SYS_MAX_TEM_START_BIT                             320       /**< 氢系统中最高温度. */
#define ARGON_SYS_MAX_TEM_BIT_LEN                               16
#define ARGON_SYS_MAX_TEM_MIN                                   0
#define ARGON_SYS_MAX_TEM_MAX                                   2400
#define ARGON_SYS_MAX_TEM_PROB_NO_START_BIT                     336       /**< 氢系统中最高温度探针序号. */
#define ARGON_SYS_MAX_TEM_PROB_NO_BIT_LEN                       8
#define ARGON_SYS_MAX_TEM_PROB_NO_MIN                           1
#define ARGON_SYS_MAX_TEM_PROB_NO_MAX                           252
#define HYDROGEN_MAX_DENSITY_START_BIT                          344       /**< 氢气最高浓度. */
#define HYDROGEN_MAX_DENSITY_BIT_LEN                            16
#define HYDROGEN_MAX_DENSITY_MIN                                0
#define HYDROGEN_MAX_DENSITY_MAX                                60000
#define HYDROGEN_MAX_DENSITY_SENSOR_NO_START_BIT                360       /**< 氢气最高浓度传感器代号. */
#define HYDROGEN_MAX_DENSITY_SENSOR_NO_BIT_LEN                  8
#define HYDROGEN_MAX_DENSITY_SENSOR_NO_MIN                      1
#define HYDROGEN_MAX_DENSITY_SENSOR_NO_MAX                      252
#define HYDROGEN_MAX_PRESSURE_START_BIT                         368       /**< 氢气最高压力. */
#define HYDROGEN_MAX_PRESSURE_BIT_LEN                           16
#define HYDROGEN_MAX_PRESSURE_MIN                               0
#define HYDROGEN_MAX_PRESSURE_MAX                               1000
#define HYDROGEN_MAX_PRESSURE_SENSOR_NO_START_BIT               384       /**< 氢气最高压力传感器代号. */
#define HYDROGEN_MAX_PRESSURE_SENSOR_NO_BIT_LEN                 8
#define HYDROGEN_MAX_PRESSURE_SENSOR_NO_MIN                     1
#define HYDROGEN_MAX_PRESSURE_SENSOR_NO_MAX                     252
#define HIGH_PRESSURE_DCDC_STATUS_START_BIT                     392       /**< 高压DC/DC状态. */
#define HIGH_PRESSURE_DCDC_STATUS_BIT_LEN                       8
#define HIGH_PRESSURE_DCDC_STATUS_MIN                           0x01
#define HIGH_PRESSURE_DCDC_STATUS_MAX                           0x02
#define ENGINE_STATUS_START_BIT                                 400       /**< 发动机状态. */
#define ENGINE_STATUS_BIT_LEN                                   8
#define ENGINE_STATUS_MIN                                       0x01
#define ENGINE_STATUS_MAX                                       0x02
#define CRANKSHAFT_SPEED_START_BIT                              408       /**< 曲轴转速. */
#define CRANKSHAFT_SPEED_BIT_LEN                                16
#define CRANKSHAFT_SPEED_MIN                                    0
#define CRANKSHAFT_SPEED_MAX                                    60000
#define FUEL_CONSUME_RATE_START_BIT                             424       /**< 燃料消耗率. */
#define FUEL_CONSUME_RATE_BIT_LEN                               16
#define FUEL_CONSUME_RATE_MIN                                   0
#define FUEL_CONSUME_RATE_MAX                                   60000
#define HIGHEST_VOL_CELL_NO_START_BIT                           440       /**< 最高电压电池单体代号. */
#define HIGHEST_VOL_CELL_NO_BIT_LEN                             8
#define HIGHEST_VOL_CELL_NO_MIN                                 1
#define HIGHEST_VOL_CELL_NO_MAX                                 250
#define HIGHEST_CELL_VOL_START_BIT                              448       /**< 电池单体电压最高值. */
#define HIGHEST_CELL_VOL_BIT_LEN                                16
#define HIGHEST_CELL_VOL_MIN                                    0
#define HIGHEST_CELL_VOL_MAX                                    15000
#define LOWEST_VOL_CELL_NO_START_BIT                            464       /**< 最低电压电池单体代号. */
#define LOWEST_VOL_CELL_NO_BIT_LEN                              8
#define LOWEST_VOL_CELL_NO_MIN                                  1
#define LOWEST_VOL_CELL_NO_MAX                                  250
#define LOWEST_CELL_VOL_START_BIT                               472       /**< 电池单体电压最低值. */
#define LOWEST_CELL_VOL_BIT_LEN                                 16
#define LOWEST_CELL_VOL_MIN                                     0
#define LOWEST_CELL_VOL_MAX                                     15000
#define HIGHEST_TEM_PROBE_NO_START_BIT                          488       /**< 最高温度探针序号. */
#define HIGHEST_TEM_PROBE_NO_BIT_LEN                            8
#define HIGHEST_TEM_PROBE_NO_MIN                                1
#define HIGHEST_TEM_PROBE_NO_MAX                                250
#define HIGHEST_PROBE_TEM_START_BIT                             496       /**< 最高温度值. */
#define HIGHEST_PROBE_TEM_BIT_LEN                               8
#define HIGHEST_PROBE_TEM_MIN                                   0
#define HIGHEST_PROBE_TEM_MAX                                   250
#define LOWEST_TEM_PROBE_NO_START_BIT                           504       /**< 最低温度探针序号. */
#define LOWEST_TEM_PROBE_NO_BIT_LEN                             8
#define LOWEST_TEM_PROBE_NO_MIN                                 1
#define LOWEST_TEM_PROBE_NO_MAX                                 250
#define LOWEST_PROBE_TEM_START_BIT                              512       /**< 最低温度值. */
#define LOWEST_PROBE_TEM_BIT_LEN                                8
#define LOWEST_PROBE_TEM_MIN                                    0
#define LOWEST_PROBE_TEM_MAX                                    250
#define ALARM_HIGHEST_LEVEL_START_BIT                           520       /**< 最高报警等级. */
#define ALARM_HIGHEST_LEVEL_BIT_LEN                             8
#define ALARM_HIGHEST_LEVEL_MIN                                 0x00
#define ALARM_HIGHEST_LEVEL_MAX                                 0x03
#define ALRARM_RESERVE_START_BIT                                528       /**< 通用报警标志预留. */
#define ALRARM_RESERVE_BIT_LEN                                  13
#define ALRARM_RESERVE_MIN                                      0
#define ALRARM_RESERVE_MAX                                      1
#define ALARM_TEM_DIFF_START_BIT                                541       /**< 温度差异报警. */
#define ALARM_TEM_DIFF_BIT_LEN                                  1
#define ALARM_TEM_DIFF_MIN                                      0
#define ALARM_TEM_DIFF_MAX                                      1
#define ALARM_BAT_OVER_TEM_START_BIT                            542       /**< 电池高温报警. */
#define ALARM_BAT_OVER_TEM_BIT_LEN                              1
#define ALARM_BAT_OVER_TEM_MIN                                  0
#define ALARM_BAT_OVER_TEM_MAX                                  1
#define ALARM_BAT_OVER_VOL_START_BIT                            543       /**< 车载储能装置类型过压报警. */
#define ALARM_BAT_OVER_VOL_BIT_LEN                              1
#define ALARM_BAT_OVER_VOL_MIN                                  0
#define ALARM_BAT_OVER_VOL_MAX                                  1
#define ALARM_BAT_UNDER_VOL_START_BIT                           544       /**< 车载储能装置类型欠压报警. */
#define ALARM_BAT_UNDER_VOL_BIT_LEN                             1
#define ALARM_BAT_UNDER_VOL_MIN                                 0
#define ALARM_BAT_UNDER_VOL_MAX                                 1
#define ALARM_SOC_LOW_START_BIT                                 545       /**< SOC低报警. */
#define ALARM_SOC_LOW_BIT_LEN                                   1
#define ALARM_SOC_LOW_MIN                                       0
#define ALARM_SOC_LOW_MAX                                       1
#define ALARM_CELL_OVER_VOL_START_BIT                           546       /**< 单体电池过压报警. */
#define ALARM_CELL_OVER_VOL_BIT_LEN                             1
#define ALARM_CELL_OVER_VOL_MIN                                 0
#define ALARM_CELL_OVER_VOL_MAX                                 1
#define ALARM_CELL_UNDER_VOL_START_BIT                          547       /**< 单体电池欠压报警. */
#define ALARM_CELL_UNDER_VOL_BIT_LEN                            1
#define ALARM_CELL_UNDER_VOL_MIN                                0
#define ALARM_CELL_UNDER_VOL_MAX                                1
#define ALARM_SOC_HIGH_START_BIT                                548       /**< SOC过高报警. */
#define ALARM_SOC_HIGH_BIT_LEN                                  1
#define ALARM_SOC_HIGH_MIN                                      0
#define ALARM_SOC_HIGH_MAX                                      1
#define ALARM_SOC_JUMP_START_BIT                                549       /**< SOC跳变报警. */
#define ALARM_SOC_JUMP_BIT_LEN                                  1
#define ALARM_SOC_JUMP_MIN                                      0
#define ALARM_SOC_JUMP_MAX                                      1
#define ALARM_BAT_NOT_MATCH_START_BIT                           550       /**< 可充电储能系统不匹配报警. */
#define ALARM_BAT_NOT_MATCH_BIT_LEN                             1
#define ALARM_BAT_NOT_MATCH_MIN                                 0
#define ALARM_BAT_NOT_MATCH_MAX                                 1
#define ALARM_CELL_CONS_DIFF_START_BIT                          551       /**< 单体电池一致性差报警. */
#define ALARM_CELL_CONS_DIFF_BIT_LEN                            1
#define ALARM_CELL_CONS_DIFF_MIN                                0
#define ALARM_CELL_CONS_DIFF_MAX                                1
#define ALARM_INSULATION_START_BIT                              552       /**< 绝缘报警. */
#define ALARM_INSULATION_BIT_LEN                                1
#define ALARM_INSULATION_MIN                                    0
#define ALARM_INSULATION_MAX                                    1
#define ALARM_DCDC_TEM_START_BIT                                553       /**< DCDC温度报警. */
#define ALARM_DCDC_TEM_BIT_LEN                                  1
#define ALARM_DCDC_TEM_MIN                                      0
#define ALARM_DCDC_TEM_MAX                                      1
#define ALARM_BRAKE_START_BIT                                   554       /**< 制动系统报警. */
#define ALARM_BRAKE_BIT_LEN                                     1
#define ALARM_BRAKE_MIN                                         0
#define ALARM_BRAKE_MAX                                         1
#define ALARM_DCDC_STATUS_START_BIT                             555       /**< DCDC状态报警. */
#define ALARM_DCDC_STATUS_BIT_LEN                               1
#define ALARM_DCDC_STATUS_MIN                                   0
#define ALARM_DCDC_STATUS_MAX                                   1
#define ALARM_MOTOR_MCU_TEM_START_BIT                           556       /**< 驱动电机控制器温度报警. */
#define ALARM_MOTOR_MCU_TEM_BIT_LEN                             1
#define ALARM_MOTOR_MCU_TEM_MIN                                 0
#define ALARM_MOTOR_MCU_TEM_MAX                                 1
#define ALARM_HIGH_VOL_LOCK_LOOP_START_BIT                      557       /**< 高压互锁状态报警. */
#define ALARM_HIGH_VOL_LOCK_LOOP_BIT_LEN                        1
#define ALARM_HIGH_VOL_LOCK_LOOP_MIN                            0
#define ALARM_HIGH_VOL_LOCK_LOOP_MAX                            1
#define ALARM_MOTOR_TEM_START_BIT                               558       /**< 驱动电机温度报警. */
#define ALARM_MOTOR_TEM_BIT_LEN                                 1
#define ALARM_MOTOR_TEM_MIN                                     0
#define ALARM_MOTOR_TEM_MAX                                     1
#define ALARM_BAT_OVER_CHARGE_START_BIT                         559       /**< 车载储能装置类型过充. */
#define ALARM_BAT_OVER_CHARGE_BIT_LEN                           1
#define ALARM_BAT_OVER_CHARGE_MIN                               0
#define ALARM_BAT_OVER_CHARGE_MAX                               1
#define CELL_NUM_START_BIT                                      560       /**< 单体电池总数. */
#define CELL_NUM_BIT_LEN                                        16
#define CELL_NUM_MIN                                            0
#define CELL_NUM_MAX                                            120
#define CELL_VOL_001_START_BIT                                  576       /**< 单体电池001电压. */
#define CELL_VOL_001_BIT_LEN                                    16
#define CELL_VOL_001_MIN                                        0
#define CELL_VOL_001_MAX                                        60000
#define CELL_VOL_002_START_BIT                                  592       /**< 单体电池002电压. */
#define CELL_VOL_002_BIT_LEN                                    16
#define CELL_VOL_002_MIN                                        0
#define CELL_VOL_002_MAX                                        60000
#define CELL_VOL_003_START_BIT                                  608       /**< 单体电池003电压. */
#define CELL_VOL_003_BIT_LEN                                    16
#define CELL_VOL_003_MIN                                        0
#define CELL_VOL_003_MAX                                        60000
#define CELL_VOL_004_START_BIT                                  624       /**< 单体电池004电压. */
#define CELL_VOL_004_BIT_LEN                                    16
#define CELL_VOL_004_MIN                                        0
#define CELL_VOL_004_MAX                                        60000
#define CELL_VOL_005_START_BIT                                  640       /**< 单体电池005电压. */
#define CELL_VOL_005_BIT_LEN                                    16
#define CELL_VOL_005_MIN                                        0
#define CELL_VOL_005_MAX                                        60000
#define CELL_VOL_006_START_BIT                                  656       /**< 单体电池006电压. */
#define CELL_VOL_006_BIT_LEN                                    16
#define CELL_VOL_006_MIN                                        0
#define CELL_VOL_006_MAX                                        60000
#define CELL_VOL_007_START_BIT                                  672       /**< 单体电池007电压. */
#define CELL_VOL_007_BIT_LEN                                    16
#define CELL_VOL_007_MIN                                        0
#define CELL_VOL_007_MAX                                        60000
#define CELL_VOL_008_START_BIT                                  688       /**< 单体电池008电压. */
#define CELL_VOL_008_BIT_LEN                                    16
#define CELL_VOL_008_MIN                                        0
#define CELL_VOL_008_MAX                                        60000
#define CELL_VOL_009_START_BIT                                  704       /**< 单体电池009电压. */
#define CELL_VOL_009_BIT_LEN                                    16
#define CELL_VOL_009_MIN                                        0
#define CELL_VOL_009_MAX                                        60000
#define CELL_VOL_010_START_BIT                                  720       /**< 单体电池010电压. */
#define CELL_VOL_010_BIT_LEN                                    16
#define CELL_VOL_010_MIN                                        0
#define CELL_VOL_010_MAX                                        60000
#define CELL_VOL_011_START_BIT                                  736       /**< 单体电池011电压. */
#define CELL_VOL_011_BIT_LEN                                    16
#define CELL_VOL_011_MIN                                        0
#define CELL_VOL_011_MAX                                        60000
#define CELL_VOL_012_START_BIT                                  752       /**< 单体电池012电压. */
#define CELL_VOL_012_BIT_LEN                                    16
#define CELL_VOL_012_MIN                                        0
#define CELL_VOL_012_MAX                                        60000
#define CELL_VOL_013_START_BIT                                  768       /**< 单体电池013电压. */
#define CELL_VOL_013_BIT_LEN                                    16
#define CELL_VOL_013_MIN                                        0
#define CELL_VOL_013_MAX                                        60000
#define CELL_VOL_014_START_BIT                                  784       /**< 单体电池014电压. */
#define CELL_VOL_014_BIT_LEN                                    16
#define CELL_VOL_014_MIN                                        0
#define CELL_VOL_014_MAX                                        60000
#define CELL_VOL_015_START_BIT                                  800       /**< 单体电池015电压. */
#define CELL_VOL_015_BIT_LEN                                    16
#define CELL_VOL_015_MIN                                        0
#define CELL_VOL_015_MAX                                        60000
#define CELL_VOL_016_START_BIT                                  816       /**< 单体电池016电压. */
#define CELL_VOL_016_BIT_LEN                                    16
#define CELL_VOL_016_MIN                                        0
#define CELL_VOL_016_MAX                                        60000
#define CELL_VOL_017_START_BIT                                  832       /**< 单体电池017电压. */
#define CELL_VOL_017_BIT_LEN                                    16
#define CELL_VOL_017_MIN                                        0
#define CELL_VOL_017_MAX                                        60000
#define CELL_VOL_018_START_BIT                                  848       /**< 单体电池018电压. */
#define CELL_VOL_018_BIT_LEN                                    16
#define CELL_VOL_018_MIN                                        0
#define CELL_VOL_018_MAX                                        60000
#define CELL_VOL_019_START_BIT                                  864       /**< 单体电池019电压. */
#define CELL_VOL_019_BIT_LEN                                    16
#define CELL_VOL_019_MIN                                        0
#define CELL_VOL_019_MAX                                        60000
#define CELL_VOL_020_START_BIT                                  880       /**< 单体电池020电压. */
#define CELL_VOL_020_BIT_LEN                                    16
#define CELL_VOL_020_MIN                                        0
#define CELL_VOL_020_MAX                                        60000
#define CELL_VOL_021_START_BIT                                  896       /**< 单体电池021电压. */
#define CELL_VOL_021_BIT_LEN                                    16
#define CELL_VOL_021_MIN                                        0
#define CELL_VOL_021_MAX                                        60000
#define CELL_VOL_022_START_BIT                                  912       /**< 单体电池022电压. */
#define CELL_VOL_022_BIT_LEN                                    16
#define CELL_VOL_022_MIN                                        0
#define CELL_VOL_022_MAX                                        60000
#define CELL_VOL_023_START_BIT                                  928       /**< 单体电池023电压. */
#define CELL_VOL_023_BIT_LEN                                    16
#define CELL_VOL_023_MIN                                        0
#define CELL_VOL_023_MAX                                        60000
#define CELL_VOL_024_START_BIT                                  944       /**< 单体电池024电压. */
#define CELL_VOL_024_BIT_LEN                                    16
#define CELL_VOL_024_MIN                                        0
#define CELL_VOL_024_MAX                                        60000
#define CELL_VOL_025_START_BIT                                  960       /**< 单体电池025电压. */
#define CELL_VOL_025_BIT_LEN                                    16
#define CELL_VOL_025_MIN                                        0
#define CELL_VOL_025_MAX                                        60000
#define CELL_VOL_026_START_BIT                                  976       /**< 单体电池026电压. */
#define CELL_VOL_026_BIT_LEN                                    16
#define CELL_VOL_026_MIN                                        0
#define CELL_VOL_026_MAX                                        60000
#define CELL_VOL_027_START_BIT                                  992       /**< 单体电池027电压. */
#define CELL_VOL_027_BIT_LEN                                    16
#define CELL_VOL_027_MIN                                        0
#define CELL_VOL_027_MAX                                        60000
#define CELL_VOL_028_START_BIT                                  1008      /**< 单体电池028电压. */
#define CELL_VOL_028_BIT_LEN                                    16
#define CELL_VOL_028_MIN                                        0
#define CELL_VOL_028_MAX                                        60000
#define CELL_VOL_029_START_BIT                                  1024      /**< 单体电池029电压. */
#define CELL_VOL_029_BIT_LEN                                    16
#define CELL_VOL_029_MIN                                        0
#define CELL_VOL_029_MAX                                        60000
#define CELL_VOL_030_START_BIT                                  1040      /**< 单体电池030电压. */
#define CELL_VOL_030_BIT_LEN                                    16
#define CELL_VOL_030_MIN                                        0
#define CELL_VOL_030_MAX                                        60000
#define CELL_VOL_031_START_BIT                                  1056      /**< 单体电池031电压. */
#define CELL_VOL_031_BIT_LEN                                    16
#define CELL_VOL_031_MIN                                        0
#define CELL_VOL_031_MAX                                        60000
#define CELL_VOL_032_START_BIT                                  1072      /**< 单体电池032电压. */
#define CELL_VOL_032_BIT_LEN                                    16
#define CELL_VOL_032_MIN                                        0
#define CELL_VOL_032_MAX                                        60000
#define CELL_VOL_033_START_BIT                                  1088      /**< 单体电池033电压. */
#define CELL_VOL_033_BIT_LEN                                    16
#define CELL_VOL_033_MIN                                        0
#define CELL_VOL_033_MAX                                        60000
#define CELL_VOL_034_START_BIT                                  1104      /**< 单体电池034电压. */
#define CELL_VOL_034_BIT_LEN                                    16
#define CELL_VOL_034_MIN                                        0
#define CELL_VOL_034_MAX                                        60000
#define CELL_VOL_035_START_BIT                                  1120      /**< 单体电池035电压. */
#define CELL_VOL_035_BIT_LEN                                    16
#define CELL_VOL_035_MIN                                        0
#define CELL_VOL_035_MAX                                        60000
#define CELL_VOL_036_START_BIT                                  1136      /**< 单体电池036电压. */
#define CELL_VOL_036_BIT_LEN                                    16
#define CELL_VOL_036_MIN                                        0
#define CELL_VOL_036_MAX                                        60000
#define CELL_VOL_037_START_BIT                                  1152      /**< 单体电池037电压. */
#define CELL_VOL_037_BIT_LEN                                    16
#define CELL_VOL_037_MIN                                        0
#define CELL_VOL_037_MAX                                        60000
#define CELL_VOL_038_START_BIT                                  1168      /**< 单体电池038电压. */
#define CELL_VOL_038_BIT_LEN                                    16
#define CELL_VOL_038_MIN                                        0
#define CELL_VOL_038_MAX                                        60000
#define CELL_VOL_039_START_BIT                                  1184      /**< 单体电池039电压. */
#define CELL_VOL_039_BIT_LEN                                    16
#define CELL_VOL_039_MIN                                        0
#define CELL_VOL_039_MAX                                        60000
#define CELL_VOL_040_START_BIT                                  1200      /**< 单体电池040电压. */
#define CELL_VOL_040_BIT_LEN                                    16
#define CELL_VOL_040_MIN                                        0
#define CELL_VOL_040_MAX                                        60000
#define CELL_VOL_041_START_BIT                                  1216      /**< 单体电池041电压. */
#define CELL_VOL_041_BIT_LEN                                    16
#define CELL_VOL_041_MIN                                        0
#define CELL_VOL_041_MAX                                        60000
#define CELL_VOL_042_START_BIT                                  1232      /**< 单体电池042电压. */
#define CELL_VOL_042_BIT_LEN                                    16
#define CELL_VOL_042_MIN                                        0
#define CELL_VOL_042_MAX                                        60000
#define CELL_VOL_043_START_BIT                                  1248      /**< 单体电池043电压. */
#define CELL_VOL_043_BIT_LEN                                    16
#define CELL_VOL_043_MIN                                        0
#define CELL_VOL_043_MAX                                        60000
#define CELL_VOL_044_START_BIT                                  1264      /**< 单体电池044电压. */
#define CELL_VOL_044_BIT_LEN                                    16
#define CELL_VOL_044_MIN                                        0
#define CELL_VOL_044_MAX                                        60000
#define CELL_VOL_045_START_BIT                                  1280      /**< 单体电池045电压. */
#define CELL_VOL_045_BIT_LEN                                    16
#define CELL_VOL_045_MIN                                        0
#define CELL_VOL_045_MAX                                        60000
#define CELL_VOL_046_START_BIT                                  1296      /**< 单体电池046电压. */
#define CELL_VOL_046_BIT_LEN                                    16
#define CELL_VOL_046_MIN                                        0
#define CELL_VOL_046_MAX                                        60000
#define CELL_VOL_047_START_BIT                                  1312      /**< 单体电池047电压. */
#define CELL_VOL_047_BIT_LEN                                    16
#define CELL_VOL_047_MIN                                        0
#define CELL_VOL_047_MAX                                        60000
#define CELL_VOL_048_START_BIT                                  1328      /**< 单体电池048电压. */
#define CELL_VOL_048_BIT_LEN                                    16
#define CELL_VOL_048_MIN                                        0
#define CELL_VOL_048_MAX                                        60000
#define CELL_VOL_049_START_BIT                                  1344      /**< 单体电池049电压. */
#define CELL_VOL_049_BIT_LEN                                    16
#define CELL_VOL_049_MIN                                        0
#define CELL_VOL_049_MAX                                        60000
#define CELL_VOL_050_START_BIT                                  1360      /**< 单体电池050电压. */
#define CELL_VOL_050_BIT_LEN                                    16
#define CELL_VOL_050_MIN                                        0
#define CELL_VOL_050_MAX                                        60000
#define CELL_VOL_051_START_BIT                                  1376      /**< 单体电池051电压. */
#define CELL_VOL_051_BIT_LEN                                    16
#define CELL_VOL_051_MIN                                        0
#define CELL_VOL_051_MAX                                        60000
#define CELL_VOL_052_START_BIT                                  1392      /**< 单体电池052电压. */
#define CELL_VOL_052_BIT_LEN                                    16
#define CELL_VOL_052_MIN                                        0
#define CELL_VOL_052_MAX                                        60000
#define CELL_VOL_053_START_BIT                                  1408      /**< 单体电池053电压. */
#define CELL_VOL_053_BIT_LEN                                    16
#define CELL_VOL_053_MIN                                        0
#define CELL_VOL_053_MAX                                        60000
#define CELL_VOL_054_START_BIT                                  1424      /**< 单体电池054电压. */
#define CELL_VOL_054_BIT_LEN                                    16
#define CELL_VOL_054_MIN                                        0
#define CELL_VOL_054_MAX                                        60000
#define CELL_VOL_055_START_BIT                                  1440      /**< 单体电池055电压. */
#define CELL_VOL_055_BIT_LEN                                    16
#define CELL_VOL_055_MIN                                        0
#define CELL_VOL_055_MAX                                        60000
#define CELL_VOL_056_START_BIT                                  1456      /**< 单体电池056电压. */
#define CELL_VOL_056_BIT_LEN                                    16
#define CELL_VOL_056_MIN                                        0
#define CELL_VOL_056_MAX                                        60000
#define CELL_VOL_057_START_BIT                                  1472      /**< 单体电池057电压. */
#define CELL_VOL_057_BIT_LEN                                    16
#define CELL_VOL_057_MIN                                        0
#define CELL_VOL_057_MAX                                        60000
#define CELL_VOL_058_START_BIT                                  1488      /**< 单体电池058电压. */
#define CELL_VOL_058_BIT_LEN                                    16
#define CELL_VOL_058_MIN                                        0
#define CELL_VOL_058_MAX                                        60000
#define CELL_VOL_059_START_BIT                                  1504      /**< 单体电池059电压. */
#define CELL_VOL_059_BIT_LEN                                    16
#define CELL_VOL_059_MIN                                        0
#define CELL_VOL_059_MAX                                        60000
#define CELL_VOL_060_START_BIT                                  1520      /**< 单体电池060电压. */
#define CELL_VOL_060_BIT_LEN                                    16
#define CELL_VOL_060_MIN                                        0
#define CELL_VOL_060_MAX                                        60000
#define CELL_VOL_061_START_BIT                                  1536      /**< 单体电池061电压. */
#define CELL_VOL_061_BIT_LEN                                    16
#define CELL_VOL_061_MIN                                        0
#define CELL_VOL_061_MAX                                        60000
#define CELL_VOL_062_START_BIT                                  1552      /**< 单体电池062电压. */
#define CELL_VOL_062_BIT_LEN                                    16
#define CELL_VOL_062_MIN                                        0
#define CELL_VOL_062_MAX                                        60000
#define CELL_VOL_063_START_BIT                                  1568      /**< 单体电池063电压. */
#define CELL_VOL_063_BIT_LEN                                    16
#define CELL_VOL_063_MIN                                        0
#define CELL_VOL_063_MAX                                        60000
#define CELL_VOL_064_START_BIT                                  1584      /**< 单体电池064电压. */
#define CELL_VOL_064_BIT_LEN                                    16
#define CELL_VOL_064_MIN                                        0
#define CELL_VOL_064_MAX                                        60000
#define CELL_VOL_065_START_BIT                                  1600      /**< 单体电池065电压. */
#define CELL_VOL_065_BIT_LEN                                    16
#define CELL_VOL_065_MIN                                        0
#define CELL_VOL_065_MAX                                        60000
#define CELL_VOL_066_START_BIT                                  1616      /**< 单体电池066电压. */
#define CELL_VOL_066_BIT_LEN                                    16
#define CELL_VOL_066_MIN                                        0
#define CELL_VOL_066_MAX                                        60000
#define CELL_VOL_067_START_BIT                                  1632      /**< 单体电池067电压. */
#define CELL_VOL_067_BIT_LEN                                    16
#define CELL_VOL_067_MIN                                        0
#define CELL_VOL_067_MAX                                        60000
#define CELL_VOL_068_START_BIT                                  1648      /**< 单体电池068电压. */
#define CELL_VOL_068_BIT_LEN                                    16
#define CELL_VOL_068_MIN                                        0
#define CELL_VOL_068_MAX                                        60000
#define CELL_VOL_069_START_BIT                                  1664      /**< 单体电池069电压. */
#define CELL_VOL_069_BIT_LEN                                    16
#define CELL_VOL_069_MIN                                        0
#define CELL_VOL_069_MAX                                        60000
#define CELL_VOL_070_START_BIT                                  1680      /**< 单体电池070电压. */
#define CELL_VOL_070_BIT_LEN                                    16
#define CELL_VOL_070_MIN                                        0
#define CELL_VOL_070_MAX                                        60000
#define CELL_VOL_071_START_BIT                                  1696      /**< 单体电池071电压. */
#define CELL_VOL_071_BIT_LEN                                    16
#define CELL_VOL_071_MIN                                        0
#define CELL_VOL_071_MAX                                        60000
#define CELL_VOL_072_START_BIT                                  1712      /**< 单体电池072电压. */
#define CELL_VOL_072_BIT_LEN                                    16
#define CELL_VOL_072_MIN                                        0
#define CELL_VOL_072_MAX                                        60000
#define CELL_VOL_073_START_BIT                                  1728      /**< 单体电池073电压. */
#define CELL_VOL_073_BIT_LEN                                    16
#define CELL_VOL_073_MIN                                        0
#define CELL_VOL_073_MAX                                        60000
#define CELL_VOL_074_START_BIT                                  1744      /**< 单体电池074电压. */
#define CELL_VOL_074_BIT_LEN                                    16
#define CELL_VOL_074_MIN                                        0
#define CELL_VOL_074_MAX                                        60000
#define CELL_VOL_075_START_BIT                                  1760      /**< 单体电池075电压. */
#define CELL_VOL_075_BIT_LEN                                    16
#define CELL_VOL_075_MIN                                        0
#define CELL_VOL_075_MAX                                        60000
#define CELL_VOL_076_START_BIT                                  1776      /**< 单体电池076电压. */
#define CELL_VOL_076_BIT_LEN                                    16
#define CELL_VOL_076_MIN                                        0
#define CELL_VOL_076_MAX                                        60000
#define CELL_VOL_077_START_BIT                                  1792      /**< 单体电池077电压. */
#define CELL_VOL_077_BIT_LEN                                    16
#define CELL_VOL_077_MIN                                        0
#define CELL_VOL_077_MAX                                        60000
#define CELL_VOL_078_START_BIT                                  1808      /**< 单体电池078电压. */
#define CELL_VOL_078_BIT_LEN                                    16
#define CELL_VOL_078_MIN                                        0
#define CELL_VOL_078_MAX                                        60000
#define CELL_VOL_079_START_BIT                                  1824      /**< 单体电池079电压. */
#define CELL_VOL_079_BIT_LEN                                    16
#define CELL_VOL_079_MIN                                        0
#define CELL_VOL_079_MAX                                        60000
#define CELL_VOL_080_START_BIT                                  1840      /**< 单体电池080电压. */
#define CELL_VOL_080_BIT_LEN                                    16
#define CELL_VOL_080_MIN                                        0
#define CELL_VOL_080_MAX                                        60000
#define CELL_VOL_081_START_BIT                                  1856      /**< 单体电池081电压. */
#define CELL_VOL_081_BIT_LEN                                    16
#define CELL_VOL_081_MIN                                        0
#define CELL_VOL_081_MAX                                        60000
#define CELL_VOL_082_START_BIT                                  1872      /**< 单体电池082电压. */
#define CELL_VOL_082_BIT_LEN                                    16
#define CELL_VOL_082_MIN                                        0
#define CELL_VOL_082_MAX                                        60000
#define CELL_VOL_083_START_BIT                                  1888      /**< 单体电池083电压. */
#define CELL_VOL_083_BIT_LEN                                    16
#define CELL_VOL_083_MIN                                        0
#define CELL_VOL_083_MAX                                        60000
#define CELL_VOL_084_START_BIT                                  1904      /**< 单体电池084电压. */
#define CELL_VOL_084_BIT_LEN                                    16
#define CELL_VOL_084_MIN                                        0
#define CELL_VOL_084_MAX                                        60000
#define CELL_VOL_085_START_BIT                                  1920      /**< 单体电池085电压. */
#define CELL_VOL_085_BIT_LEN                                    16
#define CELL_VOL_085_MIN                                        0
#define CELL_VOL_085_MAX                                        60000
#define CELL_VOL_086_START_BIT                                  1936      /**< 单体电池086电压. */
#define CELL_VOL_086_BIT_LEN                                    16
#define CELL_VOL_086_MIN                                        0
#define CELL_VOL_086_MAX                                        60000
#define CELL_VOL_087_START_BIT                                  1952      /**< 单体电池087电压. */
#define CELL_VOL_087_BIT_LEN                                    16
#define CELL_VOL_087_MIN                                        0
#define CELL_VOL_087_MAX                                        60000
#define CELL_VOL_088_START_BIT                                  1968      /**< 单体电池088电压. */
#define CELL_VOL_088_BIT_LEN                                    16
#define CELL_VOL_088_MIN                                        0
#define CELL_VOL_088_MAX                                        60000
#define CELL_VOL_089_START_BIT                                  1984      /**< 单体电池089电压. */
#define CELL_VOL_089_BIT_LEN                                    16
#define CELL_VOL_089_MIN                                        0
#define CELL_VOL_089_MAX                                        60000
#define CELL_VOL_090_START_BIT                                  2000      /**< 单体电池090电压. */
#define CELL_VOL_090_BIT_LEN                                    16
#define CELL_VOL_090_MIN                                        0
#define CELL_VOL_090_MAX                                        60000
#define CELL_VOL_091_START_BIT                                  2016      /**< 单体电池091电压. */
#define CELL_VOL_091_BIT_LEN                                    16
#define CELL_VOL_091_MIN                                        0
#define CELL_VOL_091_MAX                                        60000
#define CELL_VOL_092_START_BIT                                  2032      /**< 单体电池092电压. */
#define CELL_VOL_092_BIT_LEN                                    16
#define CELL_VOL_092_MIN                                        0
#define CELL_VOL_092_MAX                                        60000
#define CELL_VOL_093_START_BIT                                  2048      /**< 单体电池093电压. */
#define CELL_VOL_093_BIT_LEN                                    16
#define CELL_VOL_093_MIN                                        0
#define CELL_VOL_093_MAX                                        60000
#define CELL_VOL_094_START_BIT                                  2064      /**< 单体电池094电压. */
#define CELL_VOL_094_BIT_LEN                                    16
#define CELL_VOL_094_MIN                                        0
#define CELL_VOL_094_MAX                                        60000
#define CELL_VOL_095_START_BIT                                  2080      /**< 单体电池095电压. */
#define CELL_VOL_095_BIT_LEN                                    16
#define CELL_VOL_095_MIN                                        0
#define CELL_VOL_095_MAX                                        60000
#define CELL_VOL_096_START_BIT                                  2096      /**< 单体电池096电压. */
#define CELL_VOL_096_BIT_LEN                                    16
#define CELL_VOL_096_MIN                                        0
#define CELL_VOL_096_MAX                                        60000
#define CELL_VOL_097_START_BIT                                  2112      /**< 单体电池097电压. */
#define CELL_VOL_097_BIT_LEN                                    16
#define CELL_VOL_097_MIN                                        0
#define CELL_VOL_097_MAX                                        60000
#define CELL_VOL_098_START_BIT                                  2128      /**< 单体电池098电压. */
#define CELL_VOL_098_BIT_LEN                                    16
#define CELL_VOL_098_MIN                                        0
#define CELL_VOL_098_MAX                                        60000
#define CELL_VOL_099_START_BIT                                  2144      /**< 单体电池099电压. */
#define CELL_VOL_099_BIT_LEN                                    16
#define CELL_VOL_099_MIN                                        0
#define CELL_VOL_099_MAX                                        60000
#define CELL_VOL_100_START_BIT                                  2160      /**< 单体电池100电压. */
#define CELL_VOL_100_BIT_LEN                                    16
#define CELL_VOL_100_MIN                                        0
#define CELL_VOL_100_MAX                                        60000
#define CELL_VOL_101_START_BIT                                  2176      /**< 单体电池101电压. */
#define CELL_VOL_101_BIT_LEN                                    16
#define CELL_VOL_101_MIN                                        0
#define CELL_VOL_101_MAX                                        60000
#define CELL_VOL_102_START_BIT                                  2192      /**< 单体电池102电压. */
#define CELL_VOL_102_BIT_LEN                                    16
#define CELL_VOL_102_MIN                                        0
#define CELL_VOL_102_MAX                                        60000
#define CELL_VOL_103_START_BIT                                  2208      /**< 单体电池103电压. */
#define CELL_VOL_103_BIT_LEN                                    16
#define CELL_VOL_103_MIN                                        0
#define CELL_VOL_103_MAX                                        60000
#define CELL_VOL_104_START_BIT                                  2224      /**< 单体电池104电压. */
#define CELL_VOL_104_BIT_LEN                                    16
#define CELL_VOL_104_MIN                                        0
#define CELL_VOL_104_MAX                                        60000
#define CELL_VOL_105_START_BIT                                  2240      /**< 单体电池105电压. */
#define CELL_VOL_105_BIT_LEN                                    16
#define CELL_VOL_105_MIN                                        0
#define CELL_VOL_105_MAX                                        60000
#define CELL_VOL_106_START_BIT                                  2256      /**< 单体电池106电压. */
#define CELL_VOL_106_BIT_LEN                                    16
#define CELL_VOL_106_MIN                                        0
#define CELL_VOL_106_MAX                                        60000
#define CELL_VOL_107_START_BIT                                  2272      /**< 单体电池107电压. */
#define CELL_VOL_107_BIT_LEN                                    16
#define CELL_VOL_107_MIN                                        0
#define CELL_VOL_107_MAX                                        60000
#define CELL_VOL_108_START_BIT                                  2288      /**< 单体电池108电压. */
#define CELL_VOL_108_BIT_LEN                                    16
#define CELL_VOL_108_MIN                                        0
#define CELL_VOL_108_MAX                                        60000
#define CELL_VOL_109_START_BIT                                  2304      /**< 单体电池109电压. */
#define CELL_VOL_109_BIT_LEN                                    16
#define CELL_VOL_109_MIN                                        0
#define CELL_VOL_109_MAX                                        60000
#define CELL_VOL_110_START_BIT                                  2320      /**< 单体电池110电压. */
#define CELL_VOL_110_BIT_LEN                                    16
#define CELL_VOL_110_MIN                                        0
#define CELL_VOL_110_MAX                                        60000
#define CELL_VOL_111_START_BIT                                  2336      /**< 单体电池111电压. */
#define CELL_VOL_111_BIT_LEN                                    16
#define CELL_VOL_111_MIN                                        0
#define CELL_VOL_111_MAX                                        60000
#define CELL_VOL_112_START_BIT                                  2352      /**< 单体电池112电压. */
#define CELL_VOL_112_BIT_LEN                                    16
#define CELL_VOL_112_MIN                                        0
#define CELL_VOL_112_MAX                                        60000
#define CELL_VOL_113_START_BIT                                  2368      /**< 单体电池113电压. */
#define CELL_VOL_113_BIT_LEN                                    16
#define CELL_VOL_113_MIN                                        0
#define CELL_VOL_113_MAX                                        60000
#define CELL_VOL_114_START_BIT                                  2384      /**< 单体电池114电压. */
#define CELL_VOL_114_BIT_LEN                                    16
#define CELL_VOL_114_MIN                                        0
#define CELL_VOL_114_MAX                                        60000
#define CELL_VOL_115_START_BIT                                  2400      /**< 单体电池115电压. */
#define CELL_VOL_115_BIT_LEN                                    16
#define CELL_VOL_115_MIN                                        0
#define CELL_VOL_115_MAX                                        60000
#define CELL_VOL_116_START_BIT                                  2416      /**< 单体电池116电压. */
#define CELL_VOL_116_BIT_LEN                                    16
#define CELL_VOL_116_MIN                                        0
#define CELL_VOL_116_MAX                                        60000
#define CELL_VOL_117_START_BIT                                  2432      /**< 单体电池117电压. */
#define CELL_VOL_117_BIT_LEN                                    16
#define CELL_VOL_117_MIN                                        0
#define CELL_VOL_117_MAX                                        60000
#define CELL_VOL_118_START_BIT                                  2448      /**< 单体电池118电压. */
#define CELL_VOL_118_BIT_LEN                                    16
#define CELL_VOL_118_MIN                                        0
#define CELL_VOL_118_MAX                                        60000
#define CELL_VOL_119_START_BIT                                  2464      /**< 单体电池119电压. */
#define CELL_VOL_119_BIT_LEN                                    16
#define CELL_VOL_119_MIN                                        0
#define CELL_VOL_119_MAX                                        60000
#define CELL_VOL_120_START_BIT                                  2480      /**< 单体电池120电压. */
#define CELL_VOL_120_BIT_LEN                                    16
#define CELL_VOL_120_MIN                                        0
#define CELL_VOL_120_MAX                                        60000
#define PROBE_TEMP_NUM_START_BIT                                2496      /**< 温度探针个数. */
#define PROBE_TEMP_NUM_BIT_LEN                                  16
#define PROBE_TEMP_NUM_MIN                                      0
#define PROBE_TEMP_NUM_MAX                                      40
#define PROBE_TEMP_001_START_BIT                                2512      /**< 探针001温度值. */
#define PROBE_TEMP_001_BIT_LEN                                  8
#define PROBE_TEMP_001_MIN                                      0
#define PROBE_TEMP_001_MAX                                      250
#define PROBE_TEMP_002_START_BIT                                2520      /**< 探针002温度值. */
#define PROBE_TEMP_002_BIT_LEN                                  8
#define PROBE_TEMP_002_MIN                                      0
#define PROBE_TEMP_002_MAX                                      250
#define PROBE_TEMP_003_START_BIT                                2528      /**< 探针003温度值. */
#define PROBE_TEMP_003_BIT_LEN                                  8
#define PROBE_TEMP_003_MIN                                      0
#define PROBE_TEMP_003_MAX                                      250
#define PROBE_TEMP_004_START_BIT                                2536      /**< 探针004温度值. */
#define PROBE_TEMP_004_BIT_LEN                                  8
#define PROBE_TEMP_004_MIN                                      0
#define PROBE_TEMP_004_MAX                                      250
#define PROBE_TEMP_005_START_BIT                                2544      /**< 探针005温度值. */
#define PROBE_TEMP_005_BIT_LEN                                  8
#define PROBE_TEMP_005_MIN                                      0
#define PROBE_TEMP_005_MAX                                      250
#define PROBE_TEMP_006_START_BIT                                2552      /**< 探针006温度值. */
#define PROBE_TEMP_006_BIT_LEN                                  8
#define PROBE_TEMP_006_MIN                                      0
#define PROBE_TEMP_006_MAX                                      250
#define PROBE_TEMP_007_START_BIT                                2560      /**< 探针007温度值. */
#define PROBE_TEMP_007_BIT_LEN                                  8
#define PROBE_TEMP_007_MIN                                      0
#define PROBE_TEMP_007_MAX                                      250
#define PROBE_TEMP_008_START_BIT                                2568      /**< 探针008温度值. */
#define PROBE_TEMP_008_BIT_LEN                                  8
#define PROBE_TEMP_008_MIN                                      0
#define PROBE_TEMP_008_MAX                                      250
#define PROBE_TEMP_009_START_BIT                                2576      /**< 探针009温度值. */
#define PROBE_TEMP_009_BIT_LEN                                  8
#define PROBE_TEMP_009_MIN                                      0
#define PROBE_TEMP_009_MAX                                      250
#define PROBE_TEMP_010_START_BIT                                2584      /**< 探针010温度值. */
#define PROBE_TEMP_010_BIT_LEN                                  8
#define PROBE_TEMP_010_MIN                                      0
#define PROBE_TEMP_010_MAX                                      250
#define PROBE_TEMP_011_START_BIT                                2592      /**< 探针011温度值. */
#define PROBE_TEMP_011_BIT_LEN                                  8
#define PROBE_TEMP_011_MIN                                      0
#define PROBE_TEMP_011_MAX                                      250
#define PROBE_TEMP_012_START_BIT                                2600      /**< 探针012温度值. */
#define PROBE_TEMP_012_BIT_LEN                                  8
#define PROBE_TEMP_012_MIN                                      0
#define PROBE_TEMP_012_MAX                                      250
#define PROBE_TEMP_013_START_BIT                                2608      /**< 探针013温度值. */
#define PROBE_TEMP_013_BIT_LEN                                  8
#define PROBE_TEMP_013_MIN                                      0
#define PROBE_TEMP_013_MAX                                      250
#define PROBE_TEMP_014_START_BIT                                2616      /**< 探针014温度值. */
#define PROBE_TEMP_014_BIT_LEN                                  8
#define PROBE_TEMP_014_MIN                                      0
#define PROBE_TEMP_014_MAX                                      250
#define PROBE_TEMP_015_START_BIT                                2624      /**< 探针015温度值. */
#define PROBE_TEMP_015_BIT_LEN                                  8
#define PROBE_TEMP_015_MIN                                      0
#define PROBE_TEMP_015_MAX                                      250
#define PROBE_TEMP_016_START_BIT                                2632      /**< 探针016温度值. */
#define PROBE_TEMP_016_BIT_LEN                                  8
#define PROBE_TEMP_016_MIN                                      0
#define PROBE_TEMP_016_MAX                                      250
#define PROBE_TEMP_017_START_BIT                                2640      /**< 探针017温度值. */
#define PROBE_TEMP_017_BIT_LEN                                  8
#define PROBE_TEMP_017_MIN                                      0
#define PROBE_TEMP_017_MAX                                      250
#define PROBE_TEMP_018_START_BIT                                2648      /**< 探针018温度值. */
#define PROBE_TEMP_018_BIT_LEN                                  8
#define PROBE_TEMP_018_MIN                                      0
#define PROBE_TEMP_018_MAX                                      250
#define PROBE_TEMP_019_START_BIT                                2656      /**< 探针019温度值. */
#define PROBE_TEMP_019_BIT_LEN                                  8
#define PROBE_TEMP_019_MIN                                      0
#define PROBE_TEMP_019_MAX                                      250
#define PROBE_TEMP_020_START_BIT                                2664      /**< 探针020温度值. */
#define PROBE_TEMP_020_BIT_LEN                                  8
#define PROBE_TEMP_020_MIN                                      0
#define PROBE_TEMP_020_MAX                                      250
#define PROBE_TEMP_021_START_BIT                                2672      /**< 探针021温度值. */
#define PROBE_TEMP_021_BIT_LEN                                  8
#define PROBE_TEMP_021_MIN                                      0
#define PROBE_TEMP_021_MAX                                      250
#define PROBE_TEMP_022_START_BIT                                2680      /**< 探针022温度值. */
#define PROBE_TEMP_022_BIT_LEN                                  8
#define PROBE_TEMP_022_MIN                                      0
#define PROBE_TEMP_022_MAX                                      250
#define PROBE_TEMP_023_START_BIT                                2688      /**< 探针023温度值. */
#define PROBE_TEMP_023_BIT_LEN                                  8
#define PROBE_TEMP_023_MIN                                      0
#define PROBE_TEMP_023_MAX                                      250
#define PROBE_TEMP_024_START_BIT                                2696      /**< 探针024温度值. */
#define PROBE_TEMP_024_BIT_LEN                                  8
#define PROBE_TEMP_024_MIN                                      0
#define PROBE_TEMP_024_MAX                                      250
#define PROBE_TEMP_025_START_BIT                                2704      /**< 探针025温度值. */
#define PROBE_TEMP_025_BIT_LEN                                  8
#define PROBE_TEMP_025_MIN                                      0
#define PROBE_TEMP_025_MAX                                      250
#define PROBE_TEMP_026_START_BIT                                2712      /**< 探针026温度值. */
#define PROBE_TEMP_026_BIT_LEN                                  8
#define PROBE_TEMP_026_MIN                                      0
#define PROBE_TEMP_026_MAX                                      250
#define PROBE_TEMP_027_START_BIT                                2720      /**< 探针027温度值. */
#define PROBE_TEMP_027_BIT_LEN                                  8
#define PROBE_TEMP_027_MIN                                      0
#define PROBE_TEMP_027_MAX                                      250
#define PROBE_TEMP_028_START_BIT                                2728      /**< 探针028温度值. */
#define PROBE_TEMP_028_BIT_LEN                                  8
#define PROBE_TEMP_028_MIN                                      0
#define PROBE_TEMP_028_MAX                                      250
#define PROBE_TEMP_029_START_BIT                                2736      /**< 探针029温度值. */
#define PROBE_TEMP_029_BIT_LEN                                  8
#define PROBE_TEMP_029_MIN                                      0
#define PROBE_TEMP_029_MAX                                      250
#define PROBE_TEMP_030_START_BIT                                2744      /**< 探针030温度值. */
#define PROBE_TEMP_030_BIT_LEN                                  8
#define PROBE_TEMP_030_MIN                                      0
#define PROBE_TEMP_030_MAX                                      250
#define PROBE_TEMP_031_START_BIT                                2752      /**< 探针031温度值. */
#define PROBE_TEMP_031_BIT_LEN                                  8
#define PROBE_TEMP_031_MIN                                      0
#define PROBE_TEMP_031_MAX                                      250
#define PROBE_TEMP_032_START_BIT                                2760      /**< 探针032温度值. */
#define PROBE_TEMP_032_BIT_LEN                                  8
#define PROBE_TEMP_032_MIN                                      0
#define PROBE_TEMP_032_MAX                                      250
#define PROBE_TEMP_033_START_BIT                                2768      /**< 探针033温度值. */
#define PROBE_TEMP_033_BIT_LEN                                  8
#define PROBE_TEMP_033_MIN                                      0
#define PROBE_TEMP_033_MAX                                      250
#define PROBE_TEMP_034_START_BIT                                2776      /**< 探针034温度值. */
#define PROBE_TEMP_034_BIT_LEN                                  8
#define PROBE_TEMP_034_MIN                                      0
#define PROBE_TEMP_034_MAX                                      250
#define PROBE_TEMP_035_START_BIT                                2784      /**< 探针035温度值. */
#define PROBE_TEMP_035_BIT_LEN                                  8
#define PROBE_TEMP_035_MIN                                      0
#define PROBE_TEMP_035_MAX                                      250
#define PROBE_TEMP_036_START_BIT                                2792      /**< 探针036温度值. */
#define PROBE_TEMP_036_BIT_LEN                                  8
#define PROBE_TEMP_036_MIN                                      0
#define PROBE_TEMP_036_MAX                                      250
#define PROBE_TEMP_037_START_BIT                                2800      /**< 探针037温度值. */
#define PROBE_TEMP_037_BIT_LEN                                  8
#define PROBE_TEMP_037_MIN                                      0
#define PROBE_TEMP_037_MAX                                      250
#define PROBE_TEMP_038_START_BIT                                2808      /**< 探针038温度值. */
#define PROBE_TEMP_038_BIT_LEN                                  8
#define PROBE_TEMP_038_MIN                                      0
#define PROBE_TEMP_038_MAX                                      250
#define PROBE_TEMP_039_START_BIT                                2816      /**< 探针039温度值. */
#define PROBE_TEMP_039_BIT_LEN                                  8
#define PROBE_TEMP_039_MIN                                      0
#define PROBE_TEMP_039_MAX                                      250
#define PROBE_TEMP_040_START_BIT                                2824      /**< 探针040温度值. */
#define PROBE_TEMP_040_BIT_LEN                                  8
#define PROBE_TEMP_040_MIN                                      0
#define PROBE_TEMP_040_MAX                                      250
#define VCU_620_START_BIT                                       2832      /**< VCU_620h故障. */
#define VCU_620_BIT_LEN                                         64
#define VCU_620_MIN                                             0
#define VCU_620_MAX                                             0xFFFFFFFFFFFFFFFF
#define VCU_623_START_BIT                                       2896      /**< VCU_623h故障. */
#define VCU_623_BIT_LEN                                         64
#define VCU_623_MIN                                             0
#define VCU_623_MAX                                             0xFFFFFFFFFFFFFFFF
#define RECHARGE_START_BIT                                      2960      /**< 续航里程. */
#define RECHARGE_BIT_LEN                                        16
#define RECHARGE_MIN                                            0
#define RECHARGE_MAX                                            65531
#define CRASH_STATUS_START_BIT                                  2976      /**< 车辆碰撞状态. */
#define CRASH_STATUS_BIT_LEN                                    8
#define CRASH_STATUS_MIN                                        0x00
#define CRASH_STATUS_MAX                                        0x10
#define AVG_ELE_CONSUME_START_BIT                               2984      /**< 平均电耗. */
#define AVG_ELE_CONSUME_BIT_LEN                                 8
#define AVG_ELE_CONSUME_MIN                                     0x00
#define AVG_ELE_CONSUME_MAX                                     0xFA
#define SUN_ROOF_GLASS_STATUS_START_BIT                         2992      /**< 天窗玻璃状态. */
#define SUN_ROOF_GLASS_STATUS_BIT_LEN                           3
#define SUN_ROOF_GLASS_STATUS_MIN                               0x01
#define SUN_ROOF_GLASS_STATUS_MAX                               0x02
#define SUN_ROOF_CURTAIN_STATUS_START_BIT                       2995      /**< 天窗窗帘状态. */
#define SUN_ROOF_CURTAIN_STATUS_BIT_LEN                         3
#define SUN_ROOF_CURTAIN_STATUS_MIN                             0x01
#define SUN_ROOF_CURTAIN_STATUS_MAX                             0x02
#define SUN_ROOF_GLASS_POS_START_BIT                            2998      /**< 天窗玻璃位置. */
#define SUN_ROOF_GLASS_POS_BIT_LEN                              2
#define SUN_ROOF_GLASS_POS_MIN                                  0x00
#define SUN_ROOF_GLASS_POS_MAX                                  0x03
#define LF_WIN_STATUS_START_BIT                                 3000      /**< 左前窗状态. */
#define LF_WIN_STATUS_BIT_LEN                                   3
#define LF_WIN_STATUS_MIN                                       0x00
#define LF_WIN_STATUS_MAX                                       0x07
#define LR_WIN_STATUS_START_BIT                                 3003      /**< 左后窗状态. */
#define LR_WIN_STATUS_BIT_LEN                                   3
#define LR_WIN_STATUS_MIN                                       0x00
#define LR_WIN_STATUS_MAX                                       0x07
#define RF_WIN_STATUS_START_BIT                                 3006      /**< 右前窗状态. */
#define RF_WIN_STATUS_BIT_LEN                                   3
#define RF_WIN_STATUS_MIN                                       0x00
#define RF_WIN_STATUS_MAX                                       0x07
#define RR_WIN_STATUS_START_BIT                                 3009      /**< 右后窗状态. */
#define RR_WIN_STATUS_BIT_LEN                                   3
#define RR_WIN_STATUS_MIN                                       0x00
#define RR_WIN_STATUS_MAX                                       0x07
#define LF_WIN_OPENED_VALUE_START_BIT                           3012      /**< 左前窗开度值. */
#define LF_WIN_OPENED_VALUE_BIT_LEN                             7
#define LF_WIN_OPENED_VALUE_MIN                                 0
#define LF_WIN_OPENED_VALUE_MAX                                 100
#define LR_WIN_OPENED_VALUE_START_BIT                           3019      /**< 左后窗开度值. */
#define LR_WIN_OPENED_VALUE_BIT_LEN                             7
#define LR_WIN_OPENED_VALUE_MIN                                 0
#define LR_WIN_OPENED_VALUE_MAX                                 100
#define RF_WIN_OPENED_VALUE_START_BIT                           3026      /**< 右前窗开度值. */
#define RF_WIN_OPENED_VALUE_BIT_LEN                             7
#define RF_WIN_OPENED_VALUE_MIN                                 0
#define RF_WIN_OPENED_VALUE_MAX                                 100
#define RR_WIN_OPENED_VALUE_START_BIT                           3033      /**< 右前窗开度值. */
#define RR_WIN_OPENED_VALUE_BIT_LEN                             7
#define RR_WIN_OPENED_VALUE_MIN                                 0
#define RR_WIN_OPENED_VALUE_MAX                                 100
#define OUT_TEM_START_BIT                                       3040      /**< 车外温度，偏移量-400. */
#define OUT_TEM_BIT_LEN                                         16
#define OUT_TEM_MIN                                             0
#define OUT_TEM_MAX                                             2500
#define IN_TEM_1_START_BIT                                      3056      /**< 车内温度1，偏移量-400. */
#define IN_TEM_1_BIT_LEN                                        16
#define IN_TEM_1_MIN                                            0
#define IN_TEM_1_MAX                                            1270
#define IN_TEM_2_START_BIT                                      3072      /**< 车内温度2，偏移量-400. */
#define IN_TEM_2_BIT_LEN                                        16
#define IN_TEM_2_MIN                                            0
#define IN_TEM_2_MAX                                            1270
#define AIR_VOLUME_1_START_BIT                                  3088      /**< 实际风量1. */
#define AIR_VOLUME_1_BIT_LEN                                    4
#define AIR_VOLUME_1_MIN                                        0x00
#define AIR_VOLUME_1_MAX                                        0x0A
#define AIR_VOLUME_2_START_BIT                                  3092      /**< 实际风量2. */
#define AIR_VOLUME_2_BIT_LEN                                    4
#define AIR_VOLUME_2_MIN                                        0x00
#define AIR_VOLUME_2_MAX                                        0x0A
#define BLW_STATUS_START_BIT                                    3096      /**< 鼓风机状态. */
#define BLW_STATUS_BIT_LEN                                      3
#define BLW_STATUS_MIN                                          0x00
#define BLW_STATUS_MAX                                          0x01
#define KEY_AC_START_BIT                                        3099      /**< AC开关. */
#define KEY_AC_BIT_LEN                                          2
#define KEY_AC_MIN                                              0x01
#define KEY_AC_MAX                                              0x02
#define REAR_DE_FROSE_START_BIT                                 3101      /**< 后除霜. */
#define REAR_DE_FROSE_BIT_LEN                                   2
#define REAR_DE_FROSE_MIN                                       0x01
#define REAR_DE_FROSE_MAX                                       0x02
#define FRONT_DE_FROSE_START_BIT                                3103      /**< 前除霜. */
#define FRONT_DE_FROSE_BIT_LEN                                  2
#define FRONT_DE_FROSE_MIN                                      0x01
#define FRONT_DE_FROSE_MAX                                      0x02
#define KEY_CYCLE_START_BIT                                     3105      /**< 循环模式. */
#define KEY_CYCLE_BIT_LEN                                       2
#define KEY_CYCLE_MIN                                           0x01
#define KEY_CYCLE_MAX                                           0x02
#define KEY_MODE_START_BIT                                      3107      /**< 吹风模式. */
#define KEY_MODE_BIT_LEN                                        3
#define KEY_MODE_MIN                                            0x01
#define KEY_MODE_MAX                                            0x03
#define LOW_BEAM_START_BIT                                      3110      /**< 近光灯状态. */
#define LOW_BEAM_BIT_LEN                                        2
#define LOW_BEAM_MIN                                            0x01
#define LOW_BEAM_MAX                                            0x02
#define HIGH_BEAM_START_BIT                                     3112      /**< 远光灯状态. */
#define HIGH_BEAM_BIT_LEN                                       2
#define HIGH_BEAM_MIN                                           0x01
#define HIGH_BEAM_MAX                                           0x02
#define LT_LAMP_START_BIT                                       3114      /**< 左转灯状态. */
#define LT_LAMP_BIT_LEN                                         2
#define LT_LAMP_MIN                                             0x01
#define LT_LAMP_MAX                                             0x02
#define RT_LAMP_START_BIT                                       3116      /**< 右转灯状态. */
#define RT_LAMP_BIT_LEN                                         2
#define RT_LAMP_MIN                                             0x01
#define RT_LAMP_MAX                                             0x02
#define POS_LAMP_START_BIT                                      3118      /**< 位置灯状态. */
#define POS_LAMP_BIT_LEN                                        2
#define POS_LAMP_MIN                                            0x01
#define POS_LAMP_MAX                                            0x02
#define FRONT_FOG_LAMP_START_BIT                                3120      /**< 前雾灯状态. */
#define FRONT_FOG_LAMP_BIT_LEN                                  2
#define FRONT_FOG_LAMP_MIN                                      0x01
#define FRONT_FOG_LAMP_MAX                                      0x02
#define REAR_FOG_LAMP_START_BIT                                 3122      /**< 后雾灯状态. */
#define REAR_FOG_LAMP_BIT_LEN                                   2
#define REAR_FOG_LAMP_MIN                                       0x01
#define REAR_FOG_LAMP_MAX                                       0x02
#define LF_DOOR_STATUS_START_BIT                                3124      /**< 左前门状态. */
#define LF_DOOR_STATUS_BIT_LEN                                  1
#define LF_DOOR_STATUS_MIN                                      0x00
#define LF_DOOR_STATUS_MAX                                      0x01
#define LR_DOOR_STATUS_START_BIT                                3125      /**< 左后门状态. */
#define LR_DOOR_STATUS_BIT_LEN                                  1
#define LR_DOOR_STATUS_MIN                                      0x00
#define LR_DOOR_STATUS_MAX                                      0x01
#define RF_DOOR_STATUS_START_BIT                                3126      /**< 右前门状态. */
#define RF_DOOR_STATUS_BIT_LEN                                  1
#define RF_DOOR_STATUS_MIN                                      0x00
#define RF_DOOR_STATUS_MAX                                      0x01
#define RR_DOOR_STATUS_START_BIT                                3127      /**< 右后门状态. */
#define RR_DOOR_STATUS_BIT_LEN                                  1
#define RR_DOOR_STATUS_MIN                                      0x00
#define RR_DOOR_STATUS_MAX                                      0x01
#define BACK_DOOR_STATUS_START_BIT                              3128      /**< 背门状态. */
#define BACK_DOOR_STATUS_BIT_LEN                                1
#define BACK_DOOR_STATUS_MIN                                    0x00
#define BACK_DOOR_STATUS_MAX                                    0x01
#define LF_DOOR_LOCK_STATUS_START_BIT                           3129      /**< 左前门锁状态. */
#define LF_DOOR_LOCK_STATUS_BIT_LEN                             1
#define LF_DOOR_LOCK_STATUS_MIN                                 0x00
#define LF_DOOR_LOCK_STATUS_MAX                                 0x01
#define LR_DOOR_LOCK_STATUS_START_BIT                           3130      /**< 左后门锁状态. */
#define LR_DOOR_LOCK_STATUS_BIT_LEN                             1
#define LR_DOOR_LOCK_STATUS_MIN                                 0x00
#define LR_DOOR_LOCK_STATUS_MAX                                 0x01
#define RF_DOOR_LOCK_STATUS_START_BIT                           3131      /**< 右前门锁状态. */
#define RF_DOOR_LOCK_STATUS_BIT_LEN                             1
#define RF_DOOR_LOCK_STATUS_MIN                                 0x00
#define RF_DOOR_LOCK_STATUS_MAX                                 0x01
#define RR_DOOR_LOCK_STATUS_START_BIT                           3132      /**< 右后门锁状态. */
#define RR_DOOR_LOCK_STATUS_BIT_LEN                             1
#define RR_DOOR_LOCK_STATUS_MIN                                 0x00
#define RR_DOOR_LOCK_STATUS_MAX                                 0x01
#define BACK_DOOR_LOCK_STATUS_START_BIT                         3133      /**< 背门锁状态. */
#define BACK_DOOR_LOCK_STATUS_BIT_LEN                           1
#define BACK_DOOR_LOCK_STATUS_MIN                               0x00
#define BACK_DOOR_LOCK_STATUS_MAX                               0x01
#define BATTERY_VOL_START_BIT                                   3134      /**< 电瓶电压，精度：0.16；单位：V. */
#define BATTERY_VOL_BIT_LEN                                     7
#define BATTERY_VOL_MIN                                         0x00
#define BATTERY_VOL_MAX                                         0x71
#define BODY_GUARD_STATUS_START_BIT                             3141      /**< 车身防盗状态. */
#define BODY_GUARD_STATUS_BIT_LEN                               2
#define BODY_GUARD_STATUS_MIN                                   0x00
#define BODY_GUARD_STATUS_MAX                                   0x03
#define PRIVATE_INFO_LOCK_START_BIT                             3143      /**< 中控屏保护. */
#define PRIVATE_INFO_LOCK_BIT_LEN                               2
#define PRIVATE_INFO_LOCK_MIN                                   0x01
#define PRIVATE_INFO_LOCK_MAX                                   0x02
#define BRAKE_PEDAL_STATUS_VALID_START_BIT                      3145      /**< 制动踏板状态有效值. */
#define BRAKE_PEDAL_STATUS_VALID_BIT_LEN                        1
#define BRAKE_PEDAL_STATUS_VALID_MIN                            0
#define BRAKE_PEDAL_STATUS_VALID_MAX                            1
#define BRAKE_PEDAL_STATUS_START_BIT                            3146      /**< 制动踏板状态. */
#define BRAKE_PEDAL_STATUS_BIT_LEN                              1
#define BRAKE_PEDAL_STATUS_MIN                                  0
#define BRAKE_PEDAL_STATUS_MAX                                  1
#define BRAKE_PEDAL_POS_VALID_START_BIT                         3147      /**< 制动踏板位置有效性. */
#define BRAKE_PEDAL_POS_VALID_BIT_LEN                           1
#define BRAKE_PEDAL_POS_VALID_MIN                               0
#define BRAKE_PEDAL_POS_VALID_MAX                               1
#define ACCELERATROR_PEDAL_POS_VALID_START_BIT                  3148      /**< 加速踏板位置有效性. */
#define ACCELERATROR_PEDAL_POS_VALID_BIT_LEN                    1
#define ACCELERATROR_PEDAL_POS_VALID_MIN                        0
#define ACCELERATROR_PEDAL_POS_VALID_MAX                        1
#define PADDING_START_BIT                                       3149      /**< 字节对齐. */
#define PADDING_BIT_LEN                                         3
#define PADDING_MIN                                             0x00
#define PADDING_MAX                                             0x7F
#define GB32960_DATA_LEN                                        394
#define GB32960_INIT_VALUES 0x02,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x0C,0x00,0x00,0x00,0x00,0x01,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0x00,0x00,0xFF,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x15,0x3E,0xAA,0xA0,0x7C,0x01,0x00
/* Data on UART as follow */ 
/* AA 55 8C 01 00 80 02 03 01 00 00 00 00 00 00 00 00 00 00 00 02 0C 00 00 00 00 01 03 00 00 00 00 00 00 00 00 00 00 FF FF FF FF FF FF 00 00 FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 00 00 FF 00 00 FF 00 FF 00 00 00 00 00 00 00 00 FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 00 00 FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 FC 00 00 00 00 00 00 00 00 00 00 00 FF 15 3E AA A0 7C 01 00 */
/** @} */ /* GB32960. */

#endif /* __GEN_UPSTREAM_H__ */
