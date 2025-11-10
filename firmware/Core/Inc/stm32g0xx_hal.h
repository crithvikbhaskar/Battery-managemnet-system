/**
 ******************************************************************************
 * @file           : stm32g0xx_hal.h
 * @brief          : Mock STM32 HAL header for testing and standalone compilation
 ******************************************************************************
 */

#ifndef STM32G0XX_HAL_H
#define STM32G0XX_HAL_H

#include <stdint.h>
#include <stddef.h>

#define GPIO_PIN_0                 ((uint16_t)0x0001)
#define GPIO_PIN_1                 ((uint16_t)0x0002)
#define GPIO_PIN_11                ((uint16_t)0x0800)
#define GPIO_PIN_12                ((uint16_t)0x1000)

typedef enum {
    HAL_OK       = 0x00U,
    HAL_ERROR    = 0x01U,
    HAL_BUSY     = 0x02U,
    HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;

#define I2C_MEMADD_SIZE_8BIT       0x00000001U

typedef struct {
    uint32_t Instance;
    /* Other mock fields */
} I2C_HandleTypeDef;

typedef struct {
    uint32_t OD;
    /* Other mock fields */
} GPIO_TypeDef;

#define GPIOA                      ((GPIO_TypeDef *)0x40020000)
#define GPIOB                      ((GPIO_TypeDef *)0x40020400)

typedef enum {
    GPIO_PIN_RESET = 0,
    GPIO_PIN_SET
} GPIO_PinState;

/* Mock Function Signatures */
HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Master_Receive(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);

GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
void HAL_Delay(uint32_t Delay);
uint32_t HAL_GetTick(void);

#endif /* STM32G0XX_HAL_H */
