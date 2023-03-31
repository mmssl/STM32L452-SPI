#define STM32L452xx 1
#include "stm32l4xx.h"
#include "Delay_ms.h"

/*Private functions decleration*/
void SPI1_Config (void);
void GPIO_Config (void); 
void SPI_Transmit (volatile uint8_t *data, volatile int size);
/*Private variables*/
volatile uint8_t data[3] = {0xf,0xa,0x31};
volatile uint8_t temp;

void SPI1_Config (void)
{
  RCC->APB2ENR |= (1<<12); // Enable the spi clock

  // CR1 configuration
  SPI1->CR1 |=  (1<<0) | (1<<1); // CPOL=1 CPHA=1
  SPI1->CR1 |=  (4<<3); // baud rate 2.5
  SPI1->CR1 |=  (1<<14) | (1<<15); // output enabled and bidirectional
  SPI1->CR1 &= ~(1<<10); // reset rxonly bit full-duplex mode
  SPI1->CR1 &= ~(1<<7); // MSB first
  SPI1->CR1 |=  (1<<9) | (1<<8); // Hardware NSS is active
  SPI1->CR1 |=  (1<<2); // master mode
  
  // CR2 configuration
  SPI1->CR2 |= (7<<8); // 8bit data
  SPI1->CR2 |= (1<<2); // SSOE bit is set in master mode
}

void GPIO_Config (void) 
{
  RCC->AHB2ENR   |=  (1<<0)  | (1<<1); // Enable the GPIOA
  GPIOA->MODER   &= ~(0xffffffff);
  GPIOA->MODER   |=  (2<<10) | (2<<12) | (2<<14) | (2<<8); // alternate function mode for PA5,6,7 and output mode for PA4
  GPIOA->OSPEEDR |=  (3<<10) | (3<<12) | (3<<14) | (3<<8); // high speed for all pins
  GPIOA->AFR[0]  |=  (5<<16) | (5<<20) | (5<<24) | (5<<28);
}

void SPI_Transmit (volatile uint8_t *data, volatile int size)
{
  volatile int i=0;
  while (i<=size)
  {
    while(!((SPI1->SR)&(1<<1))) {} // wait to TXE bit is set for the load data to DR
    SPI1->DR = data[i]; //load data to DR 
    i++;
  }

  while(!((SPI1->SR)&(1<<1))) {} // wait to TXE bit is set
  while(!((SPI1->SR)&(1<<7))) {} // wait to BSY bit is set

  // Dummy reading to clear the overrun flag.
  temp = SPI1->DR;
  temp = SPI1->SR;
}

int main (void)
{
  SystemInit();
  SPI1_Config();
  GPIO_Config();

  SPI1->CR1 |= (1<<6);
  //GPIOA->BSRR |= (1<<4); 
  SPI_Transmit(data, 3);
  //GPIOA->BSRR |= (1<<20); 
  SPI1->CR1 &= ~(1<<6);

  while (1)
  {


  }
return 0;
}