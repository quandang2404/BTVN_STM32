#include "stm32f103xb.h"

/* ==== UART ==== */
#define GPIOAEN   (1U << 2)
#define UART1EN   (1U << 14)
#define ADC1EN    (1U << 9)

#define SR_TXE    (1U << 7)

void uart1_init(void)
{
    RCC->APB2ENR |= GPIOAEN | UART1EN;

    // PA9 TX, PA10 RX
    GPIOA->CRH &= ~(0xFF << 4);
    GPIOA->CRH |=  (0xB << 4);   // PA9 AF PP
    GPIOA->CRH |=  (0x4 << 8);   // PA10 input floating

    USART1->BRR = 0x0341;        // 9600 @ 8 MHz
    USART1->CR1 |= (1U << 13) | (1U << 3);
}

void uart1_write(char c)
{
    while (!(USART1->SR & SR_TXE));
    USART1->DR = c;
}

void uart1_print(char *s)
{
    while (*s)
        uart1_write(*s++);
}

/* ==== ADC ==== */
void adc_init(void)
{
    RCC->APB2ENR |= GPIOAEN | ADC1EN;

    // PA0 analog
    GPIOA->CRL &= ~(0xF << 0);

    ADC1->SMPR2 |= (7 << 0);     // sample time max

    ADC1->CR2 |= (1U << 0);      // ADON
    for (volatile int i = 0; i < 1000; i++);

    ADC1->CR2 |= (1U << 2);      // CAL
    while (ADC1->CR2 & (1U << 2));
}

uint16_t adc_read(void)
{
    ADC1->SQR3 = 0;              // channel 0
    ADC1->CR2 |= (1U << 0);      // start
    while (!(ADC1->SR & (1U << 1)));
    return ADC1->DR;
}

/* ==== MAIN ==== */
int main(void)
{
    char buf[32];
    uint16_t adc;

    uart1_init();
    adc_init();

    while (1)
    {
        adc = adc_read();

        // in số ADC (0–4095)
        int n = adc;
        int i = 0;
        char tmp[6];

        do {
            tmp[i++] = (n % 10) + '0';
            n /= 10;
        } while (n);

        uart1_print("ADC = ");
        while (i--)
            uart1_write(tmp[i]);
        uart1_print("\r\n");

        for (volatile int d = 0; d < 2000000; d++);
    }
}
