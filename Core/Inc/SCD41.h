/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    SCD41.h
  * @brief   Sensirion SCD41 CO2 sensor driver.
  ******************************************************************************
  */
/* USER CODE END Header */
#ifndef SCD41_H
#define SCD41_H

#include "stm32h5xx_hal.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SCD41_I2C_ADDRESS ((uint16_t)0x62 << 1)

bool scd41_start_periodic(I2C_HandleTypeDef *hi2c);
bool scd41_read_measurement(I2C_HandleTypeDef *hi2c, uint16_t *co2, float *temperature, float *humidity);
bool scd41_stop_periodic(I2C_HandleTypeDef *hi2c);

#ifdef __cplusplus
}
#endif

#endif /* SCD41_H */
