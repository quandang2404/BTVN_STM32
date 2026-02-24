#include "stm32f103xb.h"

void GPIO_Init(void)
{
    /* 1. Enable clock GPIOA & GPIOC */
    RCC->APB2ENR |= (1 << 2);   // GPIOA
    RCC->APB2ENR |= (1 << 4);   // GPIOC

    /* 2. PC13 - Output Push Pull, 2MHz */
    GPIOC->CRH &= ~(0xF << 20);     // clear CNF13 & MODE13
    GPIOC->CRH |=  (0x2 << 20);     // MODE13 = 10 (Output 2MHz), CNF13 = 00

    /* 3. PA0 - Input Pull-up */
    GPIOA->CRL &= ~(0xF << 0);      // clear CNF0 & MODE0
    GPIOA->CRL |=  (0x8 << 0);      // CNF0 = 10 (Input PU/PD)
    GPIOA->ODR |=  (1 << 0);        // Pull-up
}

int main(void)
{
    GPIO_Init();

    while (1)
    {
        if ((GPIOA->IDR & (1 << 0)) == 0)   // Button pressed
        {
            GPIOC->ODR &= ~(1 << 13);      // LED ON
        }
        else
        {
            GPIOC->ODR |= (1 << 13);       // LED OFF
        }
    }
}

