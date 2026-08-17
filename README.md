# w25q64-stm32-driver
Bare-metal SPI flash driver for STM32F407

# Bare-Metal STM32F407 & W25Q64 SPI Flash Driver

## Overview
This project provides a complete, register-level (No HAL) C driver for interfacing an STM32F407 microcontroller with a Winbond W25Q64 64Mbit SPI Flash memory chip. 

It demonstrates hardware-level configuration of the SPI and USART peripherals, raw memory manipulation, and includes a custom assembly-level HardFault handler to detect and debug memory crashes (like buffer overflows).

## Features
* **100% Bare-Metal:** Direct register manipulation without the ST HAL or Standard Peripheral Libraries.
* **Full SPI Control:** Custom SPI transfer logic with Software Slave Management (SSM).
* **W25Q64 Command Set:** 
  * Read JEDEC ID (`0x9F`) and Device ID (`0x90`, `0xAB`)
  * Sector Erase (`0x20`) and Full Chip Erase (`0xC7`)
  * Page Program / Write (`0x02`)
  * Read Data (`0x03`)
* **Custom UART Debugging:** Built-in hex-to-ASCII conversion to print raw memory bytes to a computer terminal via USART3.al.

## Hardware Requirements
* STM32F407 Development Board
* Winbond W25Q64 SPI Flash Module (or similar 25Q series)
* USB-to-TTL Serial Converter (for UART terminal output)

## Pin Connections

### W25Q64 SPI Connections (SPI1)
| STM32F407 Pin | W25Q64 Pin | Description |
| :--- | :--- | :--- |
| **PB2** | CS / CE | Chip Select (Configured as General Purpose Output) |
| **PB3** | CLK / SCK | SPI Clock (Alternate Function 5) |
| **PB4** | DO / MISO | Data Out / Master In Slave Out (Alternate Function 5) |
| **PB5** | DI / MOSI | Data In / Master Out Slave In (Alternate Function 5) |
| **3.3V** | VCC | Power |
| **3.3V** | /WP | Write Protect (Must be HIGH to allow writing/erasing) |
| **3.3V** | /HOLD | Hold (Must be HIGH for normal operation) |
| **GND** | GND | Common Ground |

### UART Debug Connections (USART3)
| STM32F407 Pin | USB-to-TTL Pin | Description |
| :--- | :--- | :--- |
| **PB10** | RX | USART3 Transmit |
| **PB11** | TX | USART3 Receive (Optional) |
| **GND** | GND | Common Ground |

## Usage & Terminal Output

1. Connect your USB-to-TTL converter to your PC and open a terminal program (e.g., PuTTY, TeraTerm) configured to **9600 Baud** (or your specific `USART3->BRR` calculation).
2. Flash the code to the STM32F407.
3. The microcontroller will output the data read from the Flash chip in Hexadecimal format.

### Example: Reading the JEDEC ID
```c
// Output should be: EF 40 17
spi_transfer(0x9F);
uint8_t mfg_id = spi_transfer(0x00);
uint8_t mem_type = spi_transfer(0x00);
uint8_t capacity = spi_transfer(0x00);
