/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    OLED.c
  * @brief   Minimal SSD1306 128x64 OLED display driver.
  ******************************************************************************
  */
/* USER CODE END Header */
#include "OLED.h"
#include <string.h>

#define OLED_I2C_ADDRESS   ((uint16_t)0x3C << 1)
#define OLED_WIDTH         128
#define OLED_HEIGHT        64
#define OLED_PAGES         (OLED_HEIGHT / 8)
#define OLED_BUFFER_SIZE   (OLED_WIDTH * OLED_PAGES)

static uint8_t oled_buffer[OLED_BUFFER_SIZE];
static I2C_HandleTypeDef *oled_hi2c;

static bool oled_i2c_write(const uint8_t *data, uint16_t size)
{
  return HAL_I2C_Master_Transmit(oled_hi2c, OLED_I2C_ADDRESS, (uint8_t *)data, size, 1000) == HAL_OK;
}

static bool oled_command(const uint8_t *cmd, uint16_t len)
{
  uint8_t packet[32];

  if (len + 1 > sizeof(packet))
  {
    return false;
  }

  packet[0] = 0x00;
  memcpy(&packet[1], cmd, len);
  return oled_i2c_write(packet, len + 1);
}

static bool oled_data(const uint8_t *data, uint16_t len)
{
  uint8_t packet[129];

  if (len + 1 > sizeof(packet))
  {
    return false;
  }

  packet[0] = 0x40;
  memcpy(&packet[1], data, len);
  return oled_i2c_write(packet, len + 1);
}

static const uint8_t oled_font_uppercase[26][5] = {
  {0x7C,0x12,0x11,0x12,0x7C}, /* A */
  {0x7F,0x49,0x49,0x49,0x36}, /* B */
  {0x3E,0x41,0x41,0x41,0x22}, /* C */
  {0x7F,0x41,0x41,0x22,0x1C}, /* D */
  {0x7F,0x49,0x49,0x49,0x41}, /* E */
  {0x7F,0x09,0x09,0x09,0x01}, /* F */
  {0x3E,0x41,0x49,0x49,0x7A}, /* G */
  {0x7F,0x08,0x08,0x08,0x7F}, /* H */
  {0x00,0x41,0x7F,0x41,0x00}, /* I */
  {0x20,0x40,0x41,0x3F,0x01}, /* J */
  {0x7F,0x08,0x14,0x22,0x41}, /* K */
  {0x7F,0x40,0x40,0x40,0x40}, /* L */
  {0x7F,0x02,0x04,0x02,0x7F}, /* M */
  {0x7F,0x04,0x08,0x10,0x7F}, /* N */
  {0x3E,0x41,0x41,0x41,0x3E}, /* O */
  {0x7F,0x09,0x09,0x09,0x06}, /* P */
  {0x3E,0x41,0x51,0x21,0x5E}, /* Q */
  {0x7F,0x09,0x19,0x29,0x46}, /* R */
  {0x46,0x49,0x49,0x49,0x31}, /* S */
  {0x01,0x01,0x7F,0x01,0x01}, /* T */
  {0x3F,0x40,0x40,0x40,0x3F}, /* U */
  {0x1F,0x20,0x40,0x20,0x1F}, /* V */
  {0x7F,0x20,0x18,0x20,0x7F}, /* W */
  {0x63,0x14,0x08,0x14,0x63}, /* X */
  {0x07,0x08,0x70,0x08,0x07}, /* Y */
  {0x61,0x51,0x49,0x45,0x43}, /* Z */
};

static const uint8_t oled_font_digits[10][5] = {
  {0x3E,0x51,0x49,0x45,0x3E}, /* 0 */
  {0x00,0x42,0x7F,0x40,0x00}, /* 1 */
  {0x42,0x61,0x51,0x49,0x46}, /* 2 */
  {0x21,0x41,0x45,0x4B,0x31}, /* 3 */
  {0x18,0x14,0x12,0x7F,0x10}, /* 4 */
  {0x27,0x45,0x45,0x45,0x39}, /* 5 */
  {0x3C,0x4A,0x49,0x49,0x30}, /* 6 */
  {0x01,0x71,0x09,0x05,0x03}, /* 7 */
  {0x36,0x49,0x49,0x49,0x36}, /* 8 */
  {0x06,0x49,0x49,0x29,0x1E}, /* 9 */
};

static const uint8_t oled_font_symbols[][5] = {
  {0x00,0x00,0x00,0x00,0x00}, /* space */
  {0x00,0x36,0x36,0x00,0x00}, /* : */
  {0x00,0x08,0x3E,0x08,0x00}, /* ! */
  {0x00,0x64,0x18,0x64,0x00}, /* % */
  {0x00,0x40,0x60,0x00,0x00}, /* . */
};

static const uint8_t *oled_font_for_char(char c)
{
  if (c >= 'A' && c <= 'Z')
  {
    return oled_font_uppercase[c - 'A'];
  }
  if (c >= '0' && c <= '9')
  {
    return oled_font_digits[c - '0'];
  }
  switch (c)
  {
    case ' ': return oled_font_symbols[0];
    case ':': return oled_font_symbols[1];
    case '!': return oled_font_symbols[2];
    case '%': return oled_font_symbols[3];
    case '.': return oled_font_symbols[4];
    default: return oled_font_symbols[0];
  }
}

bool oled_init(I2C_HandleTypeDef *hi2c)
{
  static const uint8_t init_cmds[] = {
    0xAE, /* Display OFF */
    0xD5, 0x80, /* Clock divide ratio */
    0xA8, 0x3F, /* Multiplex ratio */
    0xD3, 0x00, /* Display offset */
    0x40, /* Set display start line */
    0x8D, 0x14, /* Charge pump on */
    0x20, 0x00, /* Memory addressing mode: horizontal */
    0xA1, /* Segment remap: rotated 180° horizontal */
    0xC8, /* COM output scan direction: rotated 180° vertical */
    0xDA, 0x12, /* COM pins hardware config */
    0x81, 0x7F, /* Contrast */
    0xD9, 0xF1, /* Pre-charge */
    0xDB, 0x40, /* VCOM detect */
    0xA4, /* Entire display on resume */
    0xA6, /* Normal display */
    0xAF, /* Display ON */
  };

  oled_hi2c = hi2c;
  oled_clear();
  return oled_command(init_cmds, sizeof(init_cmds));
}

void oled_clear(void)
{
  memset(oled_buffer, 0, sizeof(oled_buffer));
}

void oled_draw_text(uint8_t col, uint8_t page, const char *text)
{
  if (page >= OLED_PAGES || col >= OLED_WIDTH)
  {
    return;
  }

  uint16_t index = page * OLED_WIDTH + col;
  while (*text && index + 5 < OLED_BUFFER_SIZE && col + 6 <= OLED_WIDTH)
  {
    const uint8_t *glyph = oled_font_for_char(*text);
    for (uint8_t x = 0; x < 5; x++)
    {
      oled_buffer[index++] = glyph[x];
    }
    oled_buffer[index++] = 0x00;
    col += 6;
    text++;
  }
}

bool oled_update(void)
{
  for (uint8_t page = 0; page < OLED_PAGES; page++)
  {
    uint8_t cmd[4] = { 0xB0 | page, 0x00, 0x10, 0x40 };
    if (!oled_command(cmd, 3))
    {
      return false;
    }

    uint16_t page_offset = page * OLED_WIDTH;
    if (!oled_data(&oled_buffer[page_offset], OLED_WIDTH))
    {
      return false;
    }
  }

  return true;
}
