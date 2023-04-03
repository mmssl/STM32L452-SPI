#define STM32L452xx 1
#include "stm32l4xx.h"
#include "Delay_ms.h"

/*Private functions decleration*/
void SPI1_Config (void);
void SPI2_Config (void);
void GPIO_Config (void); 
void SPI1_Transmit (volatile uint8_t *data, volatile int size);
/*Private variables*/
volatile uint8_t data[8] = {0xff,0xfa,0x31,0x1,0xd8,0x3a,0xb2,0xc1};
volatile uint8_t Rxdata[8];
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
  SPI1->CR1 &=  ~(1<<9); // Hardware NSS is active
  SPI1->CR1 |=  (1<<2); // master mode
  
  // CR2 configuration
  SPI1->CR2 |= (7<<8); // 8bit data
  SPI1->CR2 |= (1<<2); // SSOE bit is set in master mode
}

void SPI2_Config (void)
{
  RCC->APB1ENR1 |= (1<<14);

  //CR1 configuration
  SPI2->CR1 |=  (1<<0) | (1<<1);
  //SPI2->CR1 |=  (4<<3);
  SPI2->CR1 &= ~(1<<14);
  SPI2->CR1 |=  (1<<15);
  SPI2->CR1 |=  (1<<10);
  SPI2->CR1 &= ~(1<<7);
  SPI2->CR1 &= ~(1<<9);
  SPI2->CR1 &= ~(1<<2);

  //CR2 configuration
  SPI2->CR2 |=  (7<<8);
  SPI2->CR2 &= ~(1<<2);
}

void GPIO_Config (void) 
{
  RCC->AHB2ENR   |=  (1<<0)  | (1<<1); // Enable the GPIOA & GPIOB
  //spi1 gpio configuration
  GPIOA->MODER   &= ~(0xffffffff);
  GPIOA->MODER   |=  (2<<10) | (2<<12) | (2<<14) | (2<<8);   // alternate function mode for PA 4-5-6-7
  GPIOA->OSPEEDR |=  (3<<10) | (3<<12) | (3<<14) | (3<<8);  // high speed for all pins
  GPIOA->AFR[0]  |=  (5<<16) | (5<<20) | (5<<24) | (5<<28);// alternate functions
  //spi2 gpio configuration
  GPIOB->MODER   &= ~(0xffffffff);
  GPIOB->MODER   |=  (2<<24) | (2<<26) | (2<<28) | (2<<30);  // alternate functions mode for PB 12-13-14-15
  GPIOB->OSPEEDR |=  (3<<24) | (3<<26) | (3<<28) | (3<<30); // high speed
  GPIOB->AFR[1]  |=  (5<<16) | (5<<20) | (5<<24) | (5<<28);// alternate functions 

}

void SPI1_Transmit (volatile uint8_t *data, volatile int size)
{
  volatile int i=0;
  while (i<size)
  {
    while(!((SPI1->SR)&(1<<1))) {} // wait to TXE bit is set for the load data to DR
    *((volatile uint8_t* ) &(SPI1->DR)) = data[i]; //load data to DR 
    i++;
  }

  while(!((SPI1->SR)&(1<<1))) {} // wait to TXE bit is set
  while(!((SPI1->SR)&(1<<7))) {} // wait to BSY bit is set

  // Dummy reading to clear the overrun flag.
  //temp = SPI1->DR;
  //temp = SPI1->SR;
}

void SPI2_Receive (volatile uint8_t *data, volatile int size)
{
	while (size)
  {
    //while(!((SPI2->SR)&(1<<7))) {} // wait to BSY bit is reset
    SPI2->DR = 0; // send dummy data to start clk pin
    while(!((SPI2->SR)&(1<<0))) {} // wait for rxne bit to set
	  *data++ = *((volatile uint8_t* ) &(SPI2->DR));
	  size--;
    while(!((SPI2->SR)&(1<<7))) {} // wait to BSY bit is reset
  }	
}


int main (void)
{
  SystemInit();
  GPIO_Config();
  SPI1_Config();
  SPI2_Config();
  

  SPI1->CR1 |= (1<<6);
  SPI2->CR1 |= (1<<6);
  SPI1_Transmit(data, 8);
  SPI2_Receive(Rxdata, 8);
 //SPI1->CR1 &= ~(1<<6);
 //SPI2->CR1 &= ~(1<<6);

  while (1)
  {}

return 0;
}