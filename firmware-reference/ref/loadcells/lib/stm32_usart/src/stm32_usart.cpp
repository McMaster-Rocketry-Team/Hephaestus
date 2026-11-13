#ifndef usart
#define usart


#include "stm32h723xx.h"
#include <bits/stdc++.h>
using namespace std;

void usart_init(uint32_t baud_rate) {

    // Enable GPIO Port D
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIODEN;

    // PD8 config (UART TX)
    // AFR[1] is pins 8:15
    // Each pin -> 4 bits in AFR, pin 8: shift by 0
    GPIOD->MODER &= ~(3U << (8 * 2)); // Clear bits associated w/ pin 8
    GPIOD->MODER |= (2U << (8 * 2));  // Set alternate mode: MODER = 0b10 = 2U
    GPIOD->AFR[1] &= ~(0xFU << 0);    // Clear alternate func for pin 8
    GPIOD->AFR[1] |= (7U << 0);       // Set alternate func as USART for pin 8, USART = alt func #7

    // PD9 config (UART RX)
    // Each pin -> 4 bits in AFR, pin 9: shift by 4
    GPIOD->MODER &= ~(3U << (9 * 2)); // Clear bits associated w/ pin 9
    GPIOD->MODER |= (2U << (9 * 2));  // Set alternate mode: MODER = 0b10 = 2U
    GPIOD->AFR[1] &= ~(0xFU << 4);    // Clear alternate func for pin 8
    GPIOD->AFR[1] |= (7U << 4);       // Set alternate func as USART for pin 8, USART = alt func #7

    // Clock nonsense for USART3
    // Default clock: rcc_pclk1, refer to Domain 2 Kernel clck config register (RCC_D2CCIP2R) if you don't want to use default clock
    // RX clock stuff isn't done for now since it's not needed
    RCC->APB1LENR |= RCC_APB1LENR_USART3EN; // Enable clock for USART3, default clock: rcc_pclk1=64Mhz
    USART3->BRR = 64000000/baud_rate;       // 64Mhz clck / 9600 baud
    USART3->CR1 |= USART_CR1_UE;            // Enable USART3
    USART3->CR1 |= USART_CR1_TE;            // Enable TX for USART3
}

void print_text(const char *message) {
    int i = 0;
    while(message[i] != '\0') {
        while ((USART3->ISR & USART_ISR_TXE_TXFNF) == 0) {}  // Wait for TX ready
        USART3->TDR = message[i];                            // Split str into chars
        i++;
    }
    while ((USART3->ISR & USART_ISR_TXE_TXFNF) == 0) {}
    USART3->TDR = '\n';
}

void print_uint(uint32_t data) {

    char message_buffer[30];
    sprintf(message_buffer, "%lu", (unsigned long)data);      // Cast unsigned int to str

    int i = 0;
    while(message_buffer[i] != '\0') {
        while ((USART3->ISR & USART_ISR_TXE_TXFNF) == 0) {}  // Wait for TX ready
        USART3->TDR = message_buffer[i];                     // Split str into chars
        i++;
    }
    while ((USART3->ISR & USART_ISR_TXE_TXFNF) == 0) {}
    USART3->TDR = '\n';
}

void print_int(int32_t data) {

    char message_buffer[30];
    sprintf(message_buffer, "%d", data);                     // Cast signed int to str

    int i = 0;
    while(message_buffer[i] != '\0') {
        while ((USART3->ISR & USART_ISR_TXE_TXFNF) == 0) {}  // Wait for TX ready
        USART3->TDR = message_buffer[i];                     // Split str into chars
        i++;
    }
    while ((USART3->ISR & USART_ISR_TXE_TXFNF) == 0) {}
    // USART3->TDR = '\n';
}

// void print_float(float data) {

//     char message_buffer[30];
//     sprintf(message_buffer, "%.4f", data);                     // Cast signed int to str

//     int i = 0;
//     while(message_buffer[i] != '\0') {
//         while ((USART3->ISR & USART_ISR_TXE_TXFNF) == 0) {}  // Wait for TX ready
//         USART3->TDR = message_buffer[i];                     // Split str into chars
//         i++;
//     }
//     while ((USART3->ISR & USART_ISR_TXE_TXFNF) == 0) {}
//     USART3->TDR = '\n';
// }
void print_float(float data) {
    if (data < 0) {
        print_text("-");
        data = -data;
    }

    uint32_t integer_part = (uint32_t)data;
    uint32_t decimal_part = (uint32_t)((data - integer_part) * 10000.0f);

    print_uint(integer_part);
    print_text(".");

    if (decimal_part < 1000) print_text("0");
    if (decimal_part < 100)  print_text("0");
    if (decimal_part < 10)   print_text("0");

    print_uint(decimal_part);
    print_text("\n");
}

#endif