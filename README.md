# Atmo Logger

A bare-metal STM32H5 atmospheric monitoring device that measures CO2 concentration, temperature, and humidity using the Sensirion SCD41 sensor and displays readings on an SSD1306 OLED screen.

## Features

- **CO2 Measurement**: Real-time CO2 concentration (ppm) using SCD41 sensor
- **Temperature & Humidity**: Ambient temperature (°C) and relative humidity (%) 
- **OLED Display**: 128x64 pixel SSD1306 display showing current readings
- **Periodic Updates**: Measurements updated every 5 seconds in SCD41 periodic mode
- **LED Indicator**: GPIO blink on successful measurement

## Hardware Requirements

- **Microcontroller**: STM32H503CBT6
- **CO2 Sensor**: Sensirion SCD41 (I2C address 0x62)
- **Display**: Adafruit 128x64 OLED (SSD1306 controller, I2C address 0x3C)
- **Power Supply**: 3.3V for all components
- **I2C Pull-ups**: 4.7kΩ resistors on SDA/SCL lines to 3.3V

### Pin Connections

| Component | Pin | STM32 Pin | Notes |
|-----------|-----|-----------|-------|
| SCD41 SDA | SDA | PB10 | I2C1 |
| SCD41 SCL | SCL | PB6 | I2C1 |
| OLED SDA  | SDA | PB10 | I2C1 |
| OLED SCL  | SCL | PB6 | I2C1 |
| LED       | PC13 | Output, active low |

## Usage

1. Power on the device
2. The OLED will show startup messages:
   - "SCD41 Logger"
   - "I2C1: OLED"
   - "I2C1: SCD41"
3. After 5 seconds initialization, measurements will appear:
   - CO2: XXXX ppm
   - T: XX.X°C
   - H: XX.X%
4. The LED (PC13) blinks on each successful measurement
5. Measurements update approximately every 5 seconds

## Project Structure

```
atmo_logger/
├── Core/
│   ├── Inc/
│   │   ├── main.h
│   │   ├── SCD41.h          # SCD41 sensor driver header
│   │   └── OLED.h           # OLED display driver header
│   └── Src/
│       ├── main.c           # Main application
│       ├── SCD41.c          # SCD41 sensor driver implementation
│       └── OLED.c           # OLED display driver implementation
├── Drivers/
│   ├── CMSIS/               # ARM CMSIS headers
│   └── STM32H5xx_HAL_Driver/ # STM32 HAL drivers
├── Debug/                   # Build output directory
│   ├── makefile             # Build configuration
│   └── atmo_logger.elf      # Compiled binary
├── STM32H503CBTX_FLASH.ld   # Linker script
└── Startup/
    └── startup_stm32h503cbtx.s  # Startup code
```

## Sensor Details

### SCD41 CO2 Sensor
- **Interface**: I2C (address 0x62)
- **Measurement Range**: 400-5000 ppm CO2
- **Accuracy**: ±50 ppm CO2
- **Response Time**: < 30 seconds
- **Periodic Mode**: Measurements every 5 seconds

### SSD1306 OLED Display
- **Resolution**: 128x64 pixels
- **Controller**: SSD1306
- **Interface**: I2C (address 0x3C)

## Configuration

### I2C Settings
- **Speed**: ~25 kHz
- **Mode**: Standard mode
- **Addressing**: 7-bit
