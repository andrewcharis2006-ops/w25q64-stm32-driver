#include "stm32f407xx.h"


void bus_init();
void spi_init();
void uart_init();

void write_char(char character);
 void write_HEX(uint8_t byte);


void page_write(uint32_t address , char *data);
void page_read(uint32_t address,uint8_t *read_buffer, uint32_t length);