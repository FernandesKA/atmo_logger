/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    OLED.h
  * @brief   SSD1306-based 128x64 OLED display driver for I2C.
  ******************************************************************************
  */
/* USER CODE END Header */
#ifndef OLED_H
#define OLED_H

#include "stm32h5xx_hal.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

bool oled_init(I2C_HandleTypeDef *hi2c);
void oled_clear(void);
void oled_draw_text(uint8_t col, uint8_t page, const char *text);
bool oled_update(void);

#ifdef __cplusplus
}
#endif

#endif /* OLED_H */
