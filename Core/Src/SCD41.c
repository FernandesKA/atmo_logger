/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    SCD41.c
  * @brief   Sensirion SCD41 CO2 sensor driver.
  ******************************************************************************
  */
/* USER CODE END Header */
#include "SCD41.h"

static uint8_t scd41_crc8(const uint8_t *data, uint16_t len)
{
  uint8_t crc = 0xFF;
  for (uint16_t i = 0; i < len; i++)
  {
    crc ^= data[i];
    for (uint8_t bit = 0; bit < 8; bit++)
    {
      if (crc & 0x80)
      {
        crc = (crc << 1) ^ 0x31;
      }
      else
      {
        crc <<= 1;
      }
    }
  }
  return crc;
}

static bool scd41_check_crc(const uint8_t *data, uint16_t len, uint8_t checksum)
{
  return scd41_crc8(data, len) == checksum;
}

bool scd41_start_periodic(I2C_HandleTypeDef *hi2c)
{
  uint8_t cmd[2] = { 0x21, 0xB1 };
  
  /* Try to ping device first */
  uint8_t dummy;
  HAL_I2C_Master_Receive(hi2c, SCD41_I2C_ADDRESS, &dummy, 1, 100);
  
  HAL_StatusTypeDef ret = HAL_I2C_Master_Transmit(hi2c, SCD41_I2C_ADDRESS, cmd, sizeof(cmd), 1000);
  HAL_Delay(100);  /* Give sensor time to process */
  return ret == HAL_OK;
}

bool scd41_read_measurement(I2C_HandleTypeDef *hi2c, uint16_t *co2, float *temperature, float *humidity)
{
  uint8_t cmd[2] = { 0xEC, 0x05 };
  uint8_t rx[9] = {0};

  /* Read measurement using memory read (repeated start) */
  if (HAL_I2C_Mem_Read(hi2c, SCD41_I2C_ADDRESS, 0xEC05, I2C_MEMADD_SIZE_16BIT, rx, sizeof(rx), 1000) != HAL_OK)
  {
    return false;
  }

  /* Verify CRC of received data */
  if (!scd41_check_crc(rx + 0, 2, rx[2]) ||
      !scd41_check_crc(rx + 3, 2, rx[5]) ||
      !scd41_check_crc(rx + 6, 2, rx[8]))
  {
    return false;
  }

  *co2 = (uint16_t)rx[0] << 8 | rx[1];
  uint16_t raw_temp = (uint16_t)rx[3] << 8 | rx[4];
  uint16_t raw_hum = (uint16_t)rx[6] << 8 | rx[7];

  *temperature = (float)raw_temp / 200.0f - 45.0f;
  *humidity = (float)raw_hum / 1000.0f;

  return true;
}

bool scd41_stop_periodic(I2C_HandleTypeDef *hi2c)
{
  uint8_t cmd[2] = { 0x3F, 0x86 };
  HAL_StatusTypeDef ret = HAL_I2C_Master_Transmit(hi2c, SCD41_I2C_ADDRESS, cmd, sizeof(cmd), 1000);
  HAL_Delay(100);
  return ret == HAL_OK;
}
