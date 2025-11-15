/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body for BMS Controller
 ******************************************************************************
 */

#include "main.h"
#include "bq76920.h"

/* Peripheral Handles */
I2C_HandleTypeDef hi2c1;

/* Local variables */
uint32_t last_measurement_tick = 0;
uint32_t last_balancing_tick = 0;

/* Private Function Prototypes */
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void AFE_Boot(void);
static void Run_CellBalancing_Algorithm(void);

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    /* Initialize GPIOs and I2C */
    MX_GPIO_Init();
    
    /* Boot the BQ76920 AFE via TS1 boot pulse */
    AFE_Boot();
    
    /* Initialize I2C peripheral */
    MX_I2C1_Init();

    /* Initialize BQ76920 AFE and configuration registers */
    if (BQ76920_Init(&hi2c1) != HAL_OK) {
        Error_Handler();
    }

    last_measurement_tick = HAL_GetTick();
    last_balancing_tick = HAL_GetTick();

    /* Infinite loop */
    while (1)
    {
        uint32_t current_tick = HAL_GetTick();

        /* Task 1: Read cell voltages, current, and temperature every 250ms */
        if (current_tick - last_measurement_tick >= 250) {
            last_measurement_tick = current_tick;

            /* Check if any faults are present in AFE */
            if (BQ76920_IsFaultActive()) {
                /* System is in fault! Alert handler was called by EXTI,
                   but we double-check here to ensure safety. */
                BQ76920_SetFETs(false, false);
            } else {
                /* Read telemetry */
                BQ76920_ReadVoltages();
                BQ76920_ReadCurrent();
                BQ76920_ReadTemperature();
            }
        }

        /* Task 2: Evaluate and execute cell balancing every 5 seconds */
        if (current_tick - last_balancing_tick >= 5000) {
            last_balancing_tick = current_tick;
            
            /* Run cell balancing only if FETs are active and no faults are present */
            if (!BQ76920_IsFaultActive() && hBms.chg_fet_state) {
                Run_CellBalancing_Algorithm();
            } else {
                /* Disable balancing during fault or discharge/idle with open FETs */
                BQ76920_SetBalancing(0);
            }
        }

        /* Standard STM32 low-power sleep while waiting for timer interrupt */
        HAL_Delay(10);
    }
}

/**
 * @brief Wakes up BQ76920 from SHIP mode by sending a >1ms pulse to TS1
 */
static void AFE_Boot(void) {
    /* Set Boot control pin high to inject current into TS1 pin */
    HAL_GPIO_WritePin(AFE_BOOT_PORT, AFE_BOOT_PIN, GPIO_PIN_SET);
    HAL_Delay(3); /* Hold for 3ms (AFE needs >1ms pulse) */
    HAL_GPIO_WritePin(AFE_BOOT_PORT, AFE_BOOT_PIN, GPIO_PIN_RESET);
    HAL_Delay(10); /* Wait for internal LDO (REGOUT) to stabilize */
}

/**
 * @brief Standard cell balancing algorithm (passive bypass)
 * Balances the highest voltage cell if it is above 4.0V and delta V > 20mV
 */
static void Run_CellBalancing_Algorithm(void) {
    uint16_t max_v = hBms.cell_voltages_mv[0];
    uint16_t min_v = hBms.cell_voltages_mv[0];
    uint8_t max_idx = 0;

    /* Find minimum, maximum, and index of the highest cell */
    for (int i = 1; i < 4; i++) {
        if (hBms.cell_voltages_mv[i] > max_v) {
            max_v = hBms.cell_voltages_mv[i];
            max_idx = i;
        }
        if (hBms.cell_voltages_mv[i] < min_v) {
            min_v = hBms.cell_voltages_mv[i];
        }
    }

    uint16_t delta_v = max_v - min_v;

    /* Passive balancing criteria:
       1. Max cell is above 4000mV (balancing threshold)
       2. Imbalance delta V exceeds 20mV
       3. We only balance one cell at a time to prevent thermal load on AFE */
    if (max_v > 4000 && delta_v > 20) {
        /* Enable balance switch for the highest cell (bit mask 1 << index) */
        BQ76920_SetBalancing(1 << max_idx);
    } else {
        /* Turn off all cell balancing */
        BQ76920_SetBalancing(0);
    }
}

/**
 * @brief GPIO Initialization Function
 */
static void MX_GPIO_Init(void) {
    /* Pins are configured on STM32G0:
       - PB1 (AFE_BOOT_PIN) as Output (Push-Pull, Pull-down)
       - PB0 (AFE_ALERT_PIN) as Input (EXTI interrupt on rising edge, Pull-down)
    */
    HAL_GPIO_WritePin(AFE_BOOT_PORT, AFE_BOOT_PIN, GPIO_PIN_RESET);
}

/**
 * @brief I2C1 Initialization Function
 */
static void MX_I2C1_Init(void) {
    /* Initialize I2C1 peripheral at 100kHz standard mode */
    hi2c1.Instance = 1;
}

/**
 * @brief EXTI line detection callback (ALERT pin interrupt)
 */
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == AFE_ALERT_PIN) {
        /* Execute safety interrupt routine */
        BQ76920_HandleAlert();
    }
}

/**
 * @brief Error handler function
 */
void Error_Handler(void) {
    /* Disable protection FETs immediately on error */
    BQ76920_SetFETs(false, false);
    while (1) {
        /* Blink an LED or enter lock-out state */
    }
}
