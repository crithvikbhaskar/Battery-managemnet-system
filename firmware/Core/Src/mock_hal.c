/**
 ******************************************************************************
 * @file           : mock_hal.c
 * @brief          : Mock STM32 HAL implementations for test verification
 ******************************************************************************
 */

#include "stm32g0xx_hal.h"
#include <time.h>

/* Mock global tick */
static uint32_t mock_tick = 0;

HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout) {
    (void)hi2c;
    (void)DevAddress;
    (void)pData;
    (void)Size;
    (void)Timeout;
    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Master_Receive(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout) {
    (void)hi2c;
    (void)DevAddress;
    (void)Timeout;
    
    /* Mock receiving values from BQ76920. 
       If it is cell reading, we fill it with mock values. 
       We mock 10 bytes for cell reading (VC1-VC5 HI/LO). 
       We mock the bytes with typical cell voltage value (e.g. 3.7V). */
    if (Size == 20) { /* 10 bytes of data + 10 bytes of CRC */
        for (uint16_t i = 0; i < Size; i++) {
            pData[i] = 0; // standard default
        }
    }
    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout) {
    (void)hi2c;
    (void)DevAddress;
    (void)MemAddress;
    (void)MemAddSize;
    (void)Timeout;
    
    for (uint16_t i = 0; i < Size; i++) {
        pData[i] = 0;
    }
    return HAL_OK;
}

GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
    (void)GPIOx;
    (void)GPIO_Pin;
    return GPIO_PIN_RESET;
}

void HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState) {
    (void)GPIOx;
    (void)GPIO_Pin;
    (void)PinState;
}

void HAL_Delay(uint32_t Delay) {
    mock_tick += Delay;
}

uint32_t HAL_GetTick(void) {
    /* Return the simulated system tick */
    return mock_tick;
}
