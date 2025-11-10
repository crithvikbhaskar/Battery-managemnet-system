/**
 ******************************************************************************
 * @file           : bq76920.c
 * @brief          : BQ76920 AFE Driver Implementation
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

#include "bq76920.h"
#include <math.h>

/* Set to 1 to enable CRC-8 check (required for BQ7692003) */
#define BQ76920_CRC_ENABLED        1

/* Global BMS Handle */
BQ76920_HandleTypeDef hBms;

/* Private I2C Handle Pointer */
static I2C_HandleTypeDef *bms_i2c_port = NULL;

/**
 * @brief CRC-8 calculation function for BQ76920
 * Polynomial: x^8 + x^2 + x^1 + 1 (0x07)
 */
static uint8_t BQ76920_CRC8(const uint8_t *data, uint8_t len) {
    uint8_t crc = 0x00;
    for (uint8_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x07;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

/**
 * @brief Write a single byte to a register (handles CRC if enabled)
 */
static HAL_StatusTypeDef BQ76920_WriteRegister(uint8_t reg, uint8_t val) {
    uint8_t tx_buf[3];
    uint8_t tx_len = 0;

    tx_buf[tx_len++] = reg;
    tx_buf[tx_len++] = val;

#if BQ76920_CRC_ENABLED
    /* CRC is calculated over [SlaveAddr_W, Register, Value] */
    uint8_t crc_input[3];
    crc_input[0] = BQ76920_I2C_ADDR_8BIT; /* R/W bit is 0 for write */
    crc_input[1] = reg;
    crc_input[2] = val;
    tx_buf[tx_len++] = BQ76920_CRC8(crc_input, 3);
#endif

    return HAL_I2C_Master_Transmit(bms_i2c_port, BQ76920_I2C_ADDR_8BIT, tx_buf, tx_len, 100);
}

/**
 * @brief Read a single register (handles CRC verification if enabled)
 */
static HAL_StatusTypeDef BQ76920_ReadRegister(uint8_t reg, uint8_t *val) {
    HAL_StatusTypeDef status;

#if BQ76920_CRC_ENABLED
    uint8_t rx_buf[2]; /* [Data, CRC] */
    
    /* Set register address */
    status = HAL_I2C_Master_Transmit(bms_i2c_port, BQ76920_I2C_ADDR_8BIT, &reg, 1, 100);
    if (status != HAL_OK) return status;

    /* Read Data and CRC */
    status = HAL_I2C_Master_Receive(bms_i2c_port, BQ76920_I2C_ADDR_8BIT | 1, rx_buf, 2, 100);
    if (status != HAL_OK) return status;

    /* Verify CRC over [SlaveAddr_R, Data] */
    uint8_t crc_input[2];
    crc_input[0] = BQ76920_I2C_ADDR_8BIT | 1; /* Read bit set */
    crc_input[1] = rx_buf[0];
    
    /* Note: In standard single read, BQ76920 actually calculates CRC over:
       [SlaveAddr_W, Reg, SlaveAddr_R, Data]. Let's verify:
       The TI specification dictates that for a single byte read, the CRC is calculated
       across all bytes in the read sequence starting from the write.
       So: [SlaveAddr_W, Reg, SlaveAddr_R, Data] */
    uint8_t crc_input_full[4];
    crc_input_full[0] = BQ76920_I2C_ADDR_8BIT;
    crc_input_full[1] = reg;
    crc_input_full[2] = BQ76920_I2C_ADDR_8BIT | 1;
    crc_input_full[3] = rx_buf[0];

    uint8_t calculated_crc = BQ76920_CRC8(crc_input_full, 4);
    if (calculated_crc != rx_buf[1]) {
        return HAL_ERROR; /* CRC fault */
    }

    *val = rx_buf[0];
#else
    status = HAL_I2C_Mem_Read(bms_i2c_port, BQ76920_I2C_ADDR_8BIT, reg, I2C_MEMADD_SIZE_8BIT, val, 1, 100);
#endif

    return status;
}

/**
 * @brief Read a block of registers sequentially (handles CRC verification if enabled)
 */
static HAL_StatusTypeDef BQ76920_ReadBlock(uint8_t start_reg, uint8_t *buffer, uint8_t len) {
    HAL_StatusTypeDef status;

#if BQ76920_CRC_ENABLED
    /* In CRC mode, the device sends [Data1, CRC1, Data2, CRC2, ... ] */
    uint16_t rx_len = len * 2;
    uint8_t rx_temp[40]; /* Max block read size is typically 10-15 bytes */
    
    if (rx_len > sizeof(rx_temp)) return HAL_ERROR;

    /* Set starting register address */
    status = HAL_I2C_Master_Transmit(bms_i2c_port, BQ76920_I2C_ADDR_8BIT, &start_reg, 1, 100);
    if (status != HAL_OK) return status;

    /* Receive interlaced Data and CRC */
    status = HAL_I2C_Master_Receive(bms_i2c_port, BQ76920_I2C_ADDR_8BIT | 1, rx_temp, rx_len, 200);
    if (status != HAL_OK) return status;

    /* Verify CRC for each byte */
    for (uint8_t i = 0; i < len; i++) {
        uint8_t data = rx_temp[i * 2];
        uint8_t crc_received = rx_temp[i * 2 + 1];
        uint8_t crc_calculated = 0;

        if (i == 0) {
            /* First CRC is calculated over [SlaveAddr_W, StartRegister, SlaveAddr_R, Data1] */
            uint8_t crc_input[4];
            crc_input[0] = BQ76920_I2C_ADDR_8BIT;
            crc_input[1] = start_reg;
            crc_input[2] = BQ76920_I2C_ADDR_8BIT | 1;
            crc_input[3] = data;
            crc_calculated = BQ76920_CRC8(crc_input, 4);
        } else {
            /* Subsequent CRCs are calculated only over the Data byte itself */
            crc_calculated = BQ76920_CRC8(&data, 1);
        }

        if (crc_calculated != crc_received) {
            return HAL_ERROR; /* CRC mismatch in block read */
        }
        buffer[i] = data;
    }
#else
    status = HAL_I2C_Mem_Read(bms_i2c_port, BQ76920_I2C_ADDR_8BIT, start_reg, I2C_MEMADD_SIZE_8BIT, buffer, len, 200);
#endif

    return status;
}

/**
 * @brief Initialize the BQ76920 AFE
 */
HAL_StatusTypeDef BQ76920_Init(I2C_HandleTypeDef *hi2c) {
    bms_i2c_port = hi2c;
    HAL_StatusTypeDef status;
    uint8_t reg_val;

    /* 1. Read calibration coefficients from EEPROM registers */
    uint8_t gain1, gain2, offset;
    status = BQ76920_ReadRegister(BQ_REG_ADCGAIN1, &gain1);
    if (status != HAL_OK) return status;
    
    status = BQ76920_ReadRegister(BQ_REG_ADCOFFSET, &offset);
    if (status != HAL_OK) return status;

    status = BQ76920_ReadRegister(BQ_REG_ADCGAIN2, &gain2);
    if (status != HAL_OK) return status;

    /* Reconstruct gain and offset */
    /* GAIN is 5 bits: Upper 2 bits in ADCGAIN1<3:2> (wait, register value 0x50 has bits 3:2 as GAIN<4:3>), 
       and lower 3 bits in ADCGAIN2<7:5> (register 0x59 has bits 7:5 as GAIN<2:0>). */
    hBms.adc_gain_uV = 365 + (((gain1 & 0x0C) << 1) | ((gain2 & 0xE0) >> 5));
    hBms.adc_offset_mV = (int8_t)offset;

    /* 2. Configure System Control 1 (Enable ADC, set temp sensor source to external thermistor) */
    /* Set ADC_EN bit to 1, TEMP_SEL bit to 0 (external thermistor) */
    reg_val = BQ_CTRL1_ADC_EN;
    status = BQ76920_WriteRegister(BQ_REG_SYS_CTRL1, reg_val);
    if (status != HAL_OK) return status;

    /* 3. Configure System Control 2 (Enable Coulomb Counter, disable FETs initially for safety) */
    reg_val = BQ_CTRL2_CC_EN;
    status = BQ76920_WriteRegister(BQ_REG_SYS_CTRL2, reg_val);
    if (status != HAL_OK) return status;
    hBms.chg_fet_state = false;
    hBms.dsg_fet_state = false;

    /* 4. Configure Protection Settings (SCD, OCD, OV, UV) */
    /* Set RSNS = 1 (1x range), SCD Delay = 200us, SCD Threshold = 89mV (approx 89A with 1mOhm) */
    status = BQ76920_WriteRegister(BQ_REG_PROTECT1, BQ_PROT1_RSNS | 0x03); 
    if (status != HAL_OK) return status;

    /* Set OCD Delay = 320ms, OCD Threshold = 22mV (22A with 1mOhm) */
    status = BQ76920_WriteRegister(BQ_REG_PROTECT2, 0x0B); 
    if (status != HAL_OK) return status;

    /* Set UV Delay = 1s, OV Delay = 1s */
    status = BQ76920_WriteRegister(BQ_REG_PROTECT3, 0x00); 
    if (status != HAL_OK) return status;

    /* Set OV Threshold (raw ADC mapping) */
    /* Formula: OV_Trip = (OV_Threshold_mV - ADC_Offset_mV) * 1000 / ADC_Gain_uV */
    /* Target OV = 4220 mV */
    uint16_t ov_val = (uint16_t)((4220 - hBms.adc_offset_mV) * 1000 / hBms.adc_gain_uV);
    /* OV_TRIP register stores the upper 8 bits of the 14-bit ADC trip point (shifted by 2) */
    status = BQ76920_WriteRegister(BQ_REG_OV_TRIP, (uint8_t)((ov_val & 0x3FC0) >> 6));
    if (status != HAL_OK) return status;

    /* Set UV Threshold */
    /* Target UV = 2800 mV */
    uint16_t uv_val = (uint16_t)((2800 - hBms.adc_offset_mV) * 1000 / hBms.adc_gain_uV);
    status = BQ76920_WriteRegister(BQ_REG_UV_TRIP, (uint8_t)((uv_val & 0x3FC0) >> 6));
    if (status != HAL_OK) return status;

    /* 5. Clear any power-on faults in SYS_STAT */
    status = BQ76920_ClearFaults(0xFF);
    if (status != HAL_OK) return status;

    /* Enable protection FETs (CHG and DSG) */
    status = BQ76920_SetFETs(true, true);
    if (status != HAL_OK) return status;

    return HAL_OK;
}

/**
 * @brief Read voltages (Cell 1-4 and Pack voltage)
 */
HAL_StatusTypeDef BQ76920_ReadVoltages(void) {
    HAL_StatusTypeDef status;
    uint8_t volt_buf[10];

    /* Read all 5 cell voltage register pairs sequentially (10 bytes total) */
    status = BQ76920_ReadBlock(BQ_REG_VC1_HI, volt_buf, 10);
    if (status != HAL_OK) return status;

    /* Convert raw 14-bit right-aligned readings using Gain & Offset */
    for (int i = 0; i < 4; i++) {
        uint16_t raw_val = ((uint16_t)(volt_buf[i * 2] & 0x3F) << 8) | volt_buf[i * 2 + 1];
        hBms.cell_voltages_mv[i] = (uint16_t)(((uint32_t)raw_val * hBms.adc_gain_uV) / 1000 + hBms.adc_offset_mV);
    }

    /* Cell 5 is shorted to Cell 4 in 4-Cell configuration (we don't write to hBms.cell_voltages_mv[4]) */

    /* Read Battery Pack Voltage */
    uint8_t bat_buf[2];
    status = BQ76920_ReadBlock(BQ_REG_BAT_HI, bat_buf, 2);
    if (status != HAL_OK) return status;

    uint16_t raw_bat = ((uint16_t)bat_buf[0] << 8) | bat_buf[1];
    /* Pack voltage calculation: 4 * gain * raw_bat + 5 * offset */
    hBms.pack_voltage_mv = (uint32_t)((4.0f * raw_bat * hBms.adc_gain_uV) / 1000.0f + 5.0f * hBms.adc_offset_mV);

    return HAL_OK;
}

/**
 * @brief Read current from Coulomb Counter
 */
HAL_StatusTypeDef BQ76920_ReadCurrent(void) {
    HAL_StatusTypeDef status;
    uint8_t cc_buf[2];

    status = BQ76920_ReadBlock(BQ_REG_CC_HI, cc_buf, 2);
    if (status != HAL_OK) return status;

    int16_t raw_cc = (int16_t)(((uint16_t)cc_buf[0] << 8) | cc_buf[1]);
    
    /* Coulomb Counter LSB is 8.44 uV. */
    /* Current (mA) = (raw_cc * 8.44 uV) / Rsense (mOhms) */
    /* For Rsense in micro-ohms: Current (mA) = (raw_cc * 8440) / Rsense_uOhm */
    hBms.current_ma = (int32_t)(((float)raw_cc * 8440.0f) / BQ_SHUNTR_MICRO_OHM);

    return HAL_OK;
}

/**
 * @brief Read temperature from TS1 thermistor channel
 */
HAL_StatusTypeDef BQ76920_ReadTemperature(void) {
    HAL_StatusTypeDef status;
    uint8_t ts_buf[2];

    status = BQ76920_ReadBlock(BQ_REG_TS1_HI, ts_buf, 2);
    if (status != HAL_OK) return status;

    uint16_t raw_ts = ((uint16_t)(ts_buf[0] & 0x3F) << 8) | ts_buf[1];

    /* Convert to voltage (LSB is nominally 382 uV) */
    float V_ts = (float)raw_ts * 0.000382f;

    /* Calculate NTC Thermistor resistance (10k internal pullup to 3.3V) */
    if (V_ts >= 3.3f) return HAL_ERROR; /* Avoid division by zero */
    float R_ts = (10000.0f * V_ts) / (3.3f - V_ts);

    /* Apply Beta Equation to find temperature in Celsius */
    /* 1/T = 1/T0 + (1/Beta) * ln(R/R0) */
    float inv_T = (1.0f / 298.15f) + (1.0f / (float)BQ_THERM_BETA) * logf(R_ts / (float)BQ_THERM_R_NOM);
    float temp_k = 1.0f / inv_T;
    
    hBms.temperature_c = temp_k - 273.15f;

    return HAL_OK;
}

/**
 * @brief Enable/Disable passive cell balancing
 * cell_mask: bitmask where bits 0 to 3 correspond to Cell 1 to 4
 */
HAL_StatusTypeDef BQ76920_SetBalancing(uint8_t cell_mask) {
    /* For BQ76920: CELLBAL1 maps bits 0-4 to Cells 1-5. 
       We only balance cells 1-4, so mask must not exceed 0x0F */
    uint8_t bal_reg_val = cell_mask & 0x0F;
    
    HAL_StatusTypeDef status = BQ76920_WriteRegister(BQ_REG_CELLBAL1, bal_reg_val);
    if (status == HAL_OK) {
        hBms.balancing_mask = bal_reg_val;
    }
    return status;
}

/**
 * @brief Turn protection FETs ON or OFF
 */
HAL_StatusTypeDef BQ76920_SetFETs(bool chg_on, bool dsg_on) {
    uint8_t ctrl2;
    HAL_StatusTypeDef status;

    status = BQ76920_ReadRegister(BQ_REG_SYS_CTRL2, &ctrl2);
    if (status != HAL_OK) return status;

    /* Modify FET control bits */
    if (chg_on) {
        ctrl2 |= BQ_CTRL2_CHG_ON;
    } else {
        ctrl2 &= ~BQ_CTRL2_CHG_ON;
    }

    if (dsg_on) {
        ctrl2 |= BQ_CTRL2_DSG_ON;
    } else {
        ctrl2 &= ~BQ_CTRL2_DSG_ON;
    }

    status = BQ76920_WriteRegister(BQ_REG_SYS_CTRL2, ctrl2);
    if (status == HAL_OK) {
        hBms.chg_fet_state = chg_on;
        hBms.dsg_fet_state = dsg_on;
    }
    return status;
}

/**
 * @brief Clear selected faults in the SYS_STAT register
 * BQ76920 clears faults by writing a '1' to the corresponding bit
 */
HAL_StatusTypeDef BQ76920_ClearFaults(uint8_t fault_mask) {
    return BQ76920_WriteRegister(BQ_REG_SYS_STAT, fault_mask);
}

/**
 * @brief Return whether any fault is currently active in the SYS_STAT register
 */
bool BQ76920_IsFaultActive(void) {
    uint8_t stat;
    if (BQ76920_ReadRegister(BQ_REG_SYS_STAT, &stat) == HAL_OK) {
        hBms.fault_status = stat;
        return (stat & 0x3F) != 0; /* Bits 0-5 are faults */
    }
    return true; /* Assume fault on communication failure */
}

/**
 * @brief ALERT Interrupt Handler (asynchronous alert processing)
 * Called when the ALERT pin transitions from LOW to HIGH.
 */
void BQ76920_HandleAlert(void) {
    uint8_t stat;
    
    /* 1. Read the system status to decode the fault */
    if (BQ76920_ReadRegister(BQ_REG_SYS_STAT, &stat) != HAL_OK) {
        /* I2C Error, emergency shut down FETs to protect pack */
        BQ76920_SetFETs(false, false);
        return;
    }
    
    hBms.fault_status = stat;

    /* 2. Check for serious protection faults (SCD, OCD, OV, UV) */
    if (stat & (BQ_STAT_SCD | BQ_STAT_OCD | BQ_STAT_UV | BQ_STAT_OV | BQ_STAT_DEVICE_XREADY)) {
        /* Immediate local protection: AFE hardware will open the FET gates automatically 
           for SCD/OCD and OV/UV depending on configuration, but we must update MCU states 
           and enforce full safety lock out. */
        hBms.chg_fet_state = false;
        hBms.dsg_fet_state = false;
        
        /* Disable FET gates explicitly from host side */
        BQ76920_SetFETs(false, false);
    }
}
