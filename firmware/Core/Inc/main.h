/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 */

#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Include standard HAL (we will provide a mock header so it compiles standalone) */
#include "stm32g0xx_hal.h"

/* Exported functions prototypes */
void Error_Handler(void);

/* Pin Mappings for the BMS Circuit */
#define AFE_ALERT_PIN           GPIO_PIN_0
#define AFE_ALERT_PORT          GPIOB
#define AFE_BOOT_PIN            GPIO_PIN_1
#define AFE_BOOT_PORT           GPIOB

#define I2C_SCL_PIN             GPIO_PIN_11
#define I2C_SCL_PORT            GPIOA
#define I2C_SDA_PIN             GPIO_PIN_12
#define I2C_SDA_PORT            GPIOA

#ifdef __cplusplus
}
#endif

#endif /* MAIN_H */
