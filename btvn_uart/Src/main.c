#include "stm32f103xb.h"
#include <string.h>
void UART1_Init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN;
    // PA9 TX
    GPIOA->CRH &= ~(0xF << 4);
    GPIOA->CRH |=  (0xB << 4);
    // PA10 RX
    GPIOA->CRH &= ~(0xF << 8);
    GPIOA->CRH |=  (0x4 << 8);
    USART1->BRR = (52 << 4) | 1; // 9600 baud
    USART1->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
}
void UART1_SendChar(char c) {
    while (!(USART1->SR & USART_SR_TXE));
    USART1->DR = c;
}
void UART1_SendString(const char *s) {
    while (*s) UART1_SendChar(*s++);
}
int main(void) {
    char buffer[8] = {0};
    int index = 0;

    UART1_Init();
    UART1_SendString("\r\nNhap MSSV (7 so): ");
    while (1) {
        while (!(USART1->SR & USART_SR_RXNE));
        char c = USART1->DR;
        UART1_SendChar(c);
        if (c >= '0' && c <= '9') {
            buffer[index++] = c;
        }
        if (c == '\r' || c == '\n')
            continue;
        if (index == 7) {
            buffer[7] = '\0';
            UART1_SendString("\r\n");
            if (strcmp(buffer, "2312819") == 0)
                UART1_SendString("Ho ten: DANG MINH QUAN\r\n");
            else
                UART1_SendString("Sai MSSV!\r\n");

            UART1_SendString("Nhap MSSV (7 so): ");
            memset(buffer, 0, sizeof(buffer));
            index = 0;
        }
    }
}
