/**
 ******************************************************************************
 * @file           : bq76920.h
 * @brief          : BQ76920 AFE Driver Header File
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026. All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#ifndef BQ76920_H
#define BQ76920_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>
#include <stdbool.h>

/* BQ76920 I2C 7-bit Address (Standard factory programmed default is 0x08) */
#define BQ76920_I2C_ADDR_7BIT      0x08
#define BQ76920_I2C_ADDR_8BIT      (BQ76920_I2C_ADDR_7BIT << 1)

/* BQ76920 Registers */
#define BQ_REG_SYS_STAT            0x00
#define BQ_REG_CELLBAL1            0x01
#define BQ_REG_SYS_CTRL1           0x04
#define BQ_REG_SYS_CTRL2           0x05
#define BQ_REG_PROTECT1            0x06
#define BQ_REG_PROTECT2            0x07
#define BQ_REG_PROTECT3            0x08
#define BQ_REG_OV_TRIP             0x09
#define BQ_REG_UV_TRIP             0x0A
#define BQ_REG_CC_CFG              0x0B

/* Cell Voltage Registers */
#define BQ_REG_VC1_HI              0x0C
#define BQ_REG_VC1_LO              0x0D
#define BQ_REG_VC2_HI              0x0E
#define BQ_REG_VC2_LO              0x0F
#define BQ_REG_VC3_HI              0x10
#define BQ_REG_VC3_LO              0x11
#define BQ_REG_VC4_HI              0x12
#define BQ_REG_VC4_LO              0x13
#define BQ_REG_VC5_HI              0x14
#define BQ_REG_VC5_LO              0x15

/* Battery Stack Voltage Registers */
#define BQ_REG_BAT_HI              0x2A
#define BQ_REG_BAT_LO              0x2B

/* Temperature Registers */
#define BQ_REG_TS1_HI              0x2C
#define BQ_REG_TS1_LO              0x2D

/* Coulomb Counter Registers */
#define BQ_REG_CC_HI               0x32
#define BQ_REG_CC_LO               0x33

/* Calibration Registers (EEPROM) */
#define BQ_REG_ADCGAIN1            0x50
#define BQ_REG_ADCOFFSET           0x51
#define BQ_REG_ADCGAIN2            0x59

/* SYS_STAT Bit Definitions */
#define BQ_STAT_OCD                (1 << 0)  /* Overcurrent in discharge */
#define BQ_STAT_SCD                (1 << 1)  /* Short circuit in discharge */
#define BQ_STAT_OV                 (1 << 2)  /* Overvoltage */
#define BQ_STAT_UV                 (1 << 3)  /* Undervoltage */
#define BQ_STAT_OVRD_ALERT         (1 << 4)  /* Override alert */
#define BQ_STAT_DEVICE_XREADY      (1 << 5)  /* Internal fault indicator */

/* SYS_CTRL1 Bit Definitions */
#define BQ_CTRL1_SHUTDOWN          (1 << 0)
#define BQ_CTRL1_TEMP_SEL          (1 << 3)  /* 0 = External thermistor, 1 = Die temp */
#define BQ_CTRL1_ADC_EN            (1 << 4)  /* Enable cell measurement ADC */
#define BQ_CTRL1_LOAD_PRESENT      (1 << 7)

/* SYS_CTRL2 Bit Definitions */
#define BQ_CTRL2_CHG_ON            (1 << 0)  /* Enable charge FET */
#define BQ_CTRL2_DSG_ON            (1 << 1)  /* Enable discharge FET */
#define BQ_CTRL2_CC_ONESHOT        (1 << 5)
#define BQ_CTRL2_CC_EN             (1 << 6)  /* Enable continuous Coulomb Counter */
#define BQ_CTRL2_DELAY_DIS         (1 << 7)

/* PROTECT1 Bit Definitions */
#define BQ_PROT1_SCD_THRESH_MASK   0x07
#define BQ_PROT1_SCD_DELAY_MASK    0x18
#define BQ_PROT1_RSNS              (1 << 7)  /* 0 = 2x sense range, 1 = 1x sense range */

/* PROTECT2 Bit Definitions */
#define BQ_PROT2_OCD_THRESH_MASK   0x0F
#define BQ_PROT2_OCD_DELAY_MASK    0x70

/* PROTECT3 Bit Definitions */
#define BQ_PROT3_UV_DELAY_MASK     0x30
#define BQ_PROT3_OV_DELAY_MASK     0xC0

/* Shunt Sense Resistor Value in micro-Ohms (1 mOhm standard) */
#define BQ_SHUNTR_MICRO_OHM        1000

/* Thermistor B-Coefficient and Series Resistor */
#define BQ_THERM_BETA              3435
#define BQ_THERM_R_NOM             10000

/**
 * @brief BMS Diagnostic and Measurements Structure
 */
typedef struct {
    uint16_t cell_voltages_mv[4]; /* 4 active cells in series */
    uint32_t pack_voltage_mv;     /* Stack voltage in mV */
    int32_t  current_ma;          /* Pack current in mA (+ charge, - discharge) */
    float    temperature_c;       /* External thermistor temperature in Celsius */
    uint8_t  fault_status;        /* SYS_STAT fault register status */
    uint8_t  balancing_mask;      /* Active balancing cells bitmask (bits 0-3) */
    
    /* Reconstructed ADC Gain and Offset calibration values */
    uint8_t  adc_gain_uV;         /* Gain in uV/LSB */
    int8_t   adc_offset_mV;       /* Offset in mV */
    
    bool     chg_fet_state;       /* Charge FET gate state */
    bool     dsg_fet_state;       /* Discharge FET gate state */
} BQ76920_HandleTypeDef;

/* External Handle Reference */
extern BQ76920_HandleTypeDef hBms;

/* API Functions */
HAL_StatusTypeDef BQ76920_Init(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef BQ76920_ReadVoltages(void);
HAL_StatusTypeDef BQ76920_ReadCurrent(void);
HAL_StatusTypeDef BQ76920_ReadTemperature(void);
HAL_StatusTypeDef BQ76920_SetBalancing(uint8_t cell_mask);
HAL_StatusTypeDef BQ76920_SetFETs(bool chg_on, bool dsg_on);
HAL_StatusTypeDef BQ76920_ClearFaults(uint8_t fault_mask);
void BQ76920_HandleAlert(void);
bool BQ76920_IsFaultActive(void);

#ifdef __cplusplus
}
#endif

#endif /* BQ76920_H */
