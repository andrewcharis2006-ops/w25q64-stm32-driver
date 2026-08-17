#include "stm32f407xx.h"


void bus_init(){
    RCC->AHB1ENR |=(1<<1);
    volatile uint32_t dummy = RCC->AHB1ENR; // read back � forces clock enable to settle
    RCC->APB2ENR |=(1<<12);
    dummy = RCC->APB2ENR; // read back
    RCC->APB1ENR |=(1<<18);
    dummy = RCC->APB1ENR;
    (void)dummy;
}


void spi_init(){
GPIOB->MODER &=~(63<<6);  
GPIOB->MODER |=(42<<6); // pins to alternate functin 
	
	
GPIOB->MODER &=~(3<<4);// CS pin
GPIOB->MODER |=(1<<4);	
GPIOB->ODR |=(1<<2);
GPIOB->OSPEEDR |= (63<<6); // high speed for pins 3,4,5
GPIOB->AFR[0] |=(5<<20);// mosi PB5
GPIOB->AFR[0] |=(5<<16);// miso PB4
GPIOB->AFR[0] |=(5<<12);// sck  PB3
SPI1->CR1 |= (1<<9);        // SSM: Software slave management enabled
SPI1->CR1 |= (1<<8);
SPI1->CR1 &=~(1<<0); //Clock phase =0
SPI1->CR1 &=~(1<<1); // Clock polarity =0
SPI1->CR1 |= (1<<2);   // Master mode
SPI1->CR1 |= (1<<6);  //SPI enable
     }

void uart_init(){

GPIOB->MODER &=~(15<<20);
GPIOB->MODER |=(10<<20);  // GPIOA pins to alternate mode
	

GPIOB->AFR[1] |=(7<<8);
GPIOB->AFR[1] |=(7<<12); // GPIOA  pins to uart 3 bus
GPIOB->ODR |=(1<<2);	
	
USART3->BRR |=(8<<4)|(11<<0); // baud rate 115200
USART3->CR1|=(1<<2);
USART3->CR1|=(1<<3);
USART3->CR1|=(1<<13);	

}

/*
	serial periphereal transfer logic
	used in every communication with the w25Q64

*/
static uint8_t spi_transfer(uint8_t data){
    while(!(SPI1->SR & (1<<1)));
    SPI1->DR = data;
    while(!(SPI1->SR & (1<<0)));
		return SPI1->DR;
}




void write_char(char character){

	while(!(USART3->SR & (1<<7)));
	USART3->DR = (character & 0xFF);
	
}
 void write_HEX(uint8_t byte){
 
		char array_lookup[]= "0123456789ABCDEF";
	 uint8_t high_nibble = (byte >> 4)& 0x0F;
	 write_char(array_lookup[high_nibble]);
	 uint8_t lower_nibble = byte& 0x0F;
	 write_char(array_lookup[lower_nibble]);
 
 
 }
 
 /*
	*	send_address -> divides the address into 3 bytes and sent to W25Q64
	*address -> addresss received from use 
  */
 static void send_address(uint32_t address){
		
	 uint8_t high_byte = (address >>16) & 0x000000FF;
	 uint8_t middle_byte = (address >>8) & 0x000000FF;
	 uint8_t low_byte = address & 0x000000FF;
	 spi_transfer(high_byte);
	 spi_transfer(middle_byte);
	 spi_transfer(low_byte);
	 
  }   
 
	
 void CS_low(){
GPIOB->ODR &=~(1<<2);
	
}
 void CS_high(){

GPIOB->ODR |=(1<<2);
}


/*
 * page_data -> user's string is transfered to W25Q64
 * *data -> using pointers to reduce variables
 */
static void page_data(char *data){


while((*data) !='\0') {
	
	spi_transfer(*data);
	data++;
}


}



void print_read_buffer(uint8_t *read_buffer, uint32_t length ){
	
	
	for(uint32_t i=0; i<length ; i++){
	write_char(read_buffer[i]);
	}

}

/* 
 * page_write -> writes data to flash
 * address -> target memory address
 * data -> string to write
 */

void page_write(uint32_t address , char *data){

CS_low();
	spi_transfer(0x06);
CS_high();
	

	
		CS_low();
		spi_transfer(0x02);
		send_address(address);
		page_data(data);
		CS_high();
	
	CS_low();

	uint8_t status ;
	do{
	spi_transfer(0x05);
		status = spi_transfer(0x00);
	
	}while(status & (1<<0));
CS_high();
	

}

/*
 * page_read -> reads data from flash
 * address -> target memory address 
 * read_buffer -> buffer to store , read later 
 * length -> number of bytes to read 
 */

void page_read(uint32_t address,uint8_t *read_buffer, uint32_t length){

CS_low();
	spi_transfer(0x03);
	send_address(address);
	for(uint32_t i=0; i<length ; i++){
	
	read_buffer[i]=spi_transfer(0x00);
		
	}
	CS_high();
	
	print_read_buffer(read_buffer,length);
}
