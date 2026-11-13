#ifndef stm32_timer
#define stm32_timer

#include "stm32h723xx.h"
#include "stm32_usart.h"

//-------------------------- Timer 2 configed for delays -----------------------

void tim2_init(void) {

    // Default system clock is 64Mhz from rcc_tim2_ker_ck source
    // We want to make a timer that is 1Mhz freq
    RCC->APB1LENR |= RCC_APB1LENR_TIM2EN;  // Enable timer TIM2 peripheral
    TIM2->ARR = 0xFFFFFFFF;                // Auto reload register, when CNT resets to 0
    TIM2->CNT = 0;                         // Start value of timer
    TIM2->PSC = 63;                        // Divider - 1, 64Mhz/(63+1)=1Mhz
    TIM2->EGR = TIM_EGR_UG;                // Manually trigger an event to reset timer
    TIM2->CR1 |= TIM_CR1_CEN;              // Enable TIM2 from ctrl register

}

void delay_us(uint32_t us) {

    uint32_t start = TIM2->CNT;
    while ((TIM2->CNT - start) < us) {
    }

}

void delay_ms(uint32_t ms) {

    delay_us(ms * 1000);

}

//------------------- Timer 3 configed as software interrupt -------------------

volatile uint32_t daq_flag = 0;

void tim3_init(void) {

    // Default system clock is 64Mhz from rcc_tim2_ker_ck source
    // We want to make a timer that is 1Mhz freq
    RCC->APB1LENR |= RCC_APB1LENR_TIM3EN;  // Enable timer TIM2 peripheral
    TIM3->ARR = 0xF4240;                   // Auto reload register, when an interrupt is triggered (1 sec)
    TIM3->CNT = 0;                         // Start value of timer
    TIM3->PSC = 63;                        // Divider - 1, 64Mhz/(63+1)=1Mhz
    TIM3->EGR = TIM_EGR_UG;                // Manually trigger an event to reset timer
    TIM3->DIER = TIM_DIER_UIE;

    NVIC_SetPriority(TIM3_IRQn, 2);
    NVIC_EnableIRQ(TIM3_IRQn);

    TIM3->CR1 |= TIM_CR1_CEN;              // Enable TIM2 from ctrl register

}

extern "C" void TIM3_IRQHandler() {

    TIM3->SR &= ~TIM_SR_UIF;               // Clear interrupt
    daq_flag = 1;                          // Set flag

}

#endif