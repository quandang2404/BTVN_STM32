#include "stm32f103xb.h"

/* ===== Delay ms bằng TIM2 ===== */
void delay_ms(uint32_t ms)
{
    TIM2->CNT = 0;
    while (TIM2->CNT < ms);
}

/* ===== ADC PA0 ===== */
void ADC_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_ADC1EN;

    GPIOA->CRL &= ~(0xF << 0);     // PA0 analog

    ADC1->SMPR2 |= ADC_SMPR2_SMP0;
    ADC1->SQR3 = 0;

    ADC1->CR2 |= ADC_CR2_ADON;
    for (volatile int i = 0; i < 1000; i++);

    ADC1->CR2 |= ADC_CR2_CAL;
    while (ADC1->CR2 & ADC_CR2_CAL);
}

uint16_t ADC_Read(void)
{
    ADC1->CR2 |= ADC_CR2_ADON;
    while (!(ADC1->SR & ADC_SR_EOC));
    return ADC1->DR;
}

/* ===== PWM TIM3 PA6 ===== */
void PWM_Init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    GPIOA->CRL &= ~(0xF << 24);
    GPIOA->CRL |=  (0xB << 24);    // PA6 AF PP

    TIM3->PSC = 71;      // 1 MHz
    TIM3->ARR = 999;     // PWM 1 kHz
    TIM3->CCR1 = 0;

    TIM3->CCMR1 |= (6 << 4);   // PWM mode 1
    TIM3->CCER  |= 1;
    TIM3->CR1   |= 1;
}

/* ===== TIM2 cho delay ===== */
void TIM2_Init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2->PSC = 7199;    // 10 kHz
    TIM2->ARR = 0xFFFF;
    TIM2->CR1 |= 1;
}

/* ===== MAIN ===== */
int main(void)
{
    TIM2_Init();
    PWM_Init();
    ADC_Init();

    uint16_t adc;
    uint32_t pwm;

    while (1)
    {
        adc = ADC_Read();                // 0–4095
        pwm = (adc * 1000) / 4095;       // map duty

        TIM3->CCR1 = pwm;
        delay_ms(10);
    }
}
