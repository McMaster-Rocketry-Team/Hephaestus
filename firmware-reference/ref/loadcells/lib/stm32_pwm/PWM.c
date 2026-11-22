#include <PWM_H>

#include "stm32h723xx.h"

uint32_t timer_clock(void){
    SystemCoreClockUpdate();
    uint32_t hclk = SystemCoreClock; // starts up system core clock (very unobvious)
    
    uint32_t d2ppre1 = (RCC->D2CFGR >> 4) & 0x07; // controls how much the clock is being divided by based on num of bits ex: 111, 110, etc 
    // specifically the D2PPRE1 register


    // Convert register code (4-7) into a divisor (1, 2, 4, etc)
    uint32_t divisor = 1;
    if (d2ppre1 >= 4) divisor = 1 << (d2ppre1 - 3);
    // A very fancy way of determining how much to divide hclk to make the clock speed compatable

    uint32_t pclk1 = hclk / divisor;
    // The new divided clock speed without consdering the time multiplier.


    // Calculate the time multiplier
    // Note, this only applies if the divisor isint equal to 1, and is there in order to make the PCLK timing and timer frequency the same (kinda weird rule)
    uint32_t tim_clk;
    if (divisor == 1) {
        tim_clk = pclk1;
    } else {
        tim_clk = pclk1 * 2;
    }

    return tim_clk;
    // final clock val

}

void servo_init(void){


    RCC->AHB4ENR |= (1 << 0); // Enable GPIOA
    RCC->APB1LENR |= (1 << 0); // Enable TIM2

    (void)RCC->APB1LENR; //Make sure clock is enabled (this might do literally the same as the one above, might take out later)
    
    // Assuming this is pin PA1, so PIN_NUM = 1 and AF_TIM2 = 1
    GPIOA->MODER &= ~(3 << (PIN_NUM * 2)); // erase whatevers in MODER0
    GPIOA->MODER |= (2 << (PIN_NUM * 2)); // Set MODER0 as Alternative Function mode
    GPIOA->AFR[0] &= ~(0xF << (PIN_NUM * 4)); // erase whatevers in AF[0]
    GPIOA->AFR[0] |= (AF_TIM2 << (PIN_NUM * 4)); // Set AF[0] to AF1

    TIM2->PSC = (timer_clock()/1000000UL)-1; // Prescaler Value, which helps set the counter clock frequency, (formula f(CK_PSC) / PSC[15:0]+1)

    TIM2->ARR = 20000 - 1; // Auto Reload Value - Counts the timer before it resets back to 0.

    TIM2->EGR |= (1 << 0); // Sets to Update Generation, so essentially it resets the counter and updates each register

    // Force CCRM1 to be on output mode (CC2S == 0)
    TIM2->CCMR1 &= ~(3 << 8); // Erases whatevers in CC2S and sets it to 0, making the CCRM1 as output

    // PWM Mode 1
    TIM2->CCMR1 &= ~(7 << 12); // Sets to 0 whatevers in IC2F
    TIM2->CCMR1 |= (6 << 12); // So this is probably the most complicated one..
    // In essence, what this does is that its basically a hardware noise canceller
    // Depending on the bits you input (in this case 6), You set the fSampling and N values
    // What fSampling does is that it takes your internal timer clock and divides it by a number (in this case 4)
    // and looks only at values at this specific interval
    // In the case of N, the device only considers whatever you input a valid signal change only when it detects that pin change at least N consecutive samples in a row.


    // What this register does is basically help manage speed/frequency by only taking the positive edge every _ turns
    // For this case in particular, its set at taking the positive edge every 2 turns.
    // Once it captures a valid pos edge, grabs the value and saves it in the capture register. 
    TIM2->CCMR1 |= (1 << 11);

   
    // Enable Capture/Compare channel 2 output.
    TIM2->CCER |= (1 << 4);
  
    // Initial pulse, also you put the servo arm to its original position, (I set SERVO_MIN at 1000 so that it starts at 0 degrees)
    TIM2->CCR2 = SERVO_MIN;

    // Start timer
    // (1 << 7) helps create a buffer whenever you write a new value to ARR, (1 << 0) starts the timer.
    TIM2->CR1 |= (1 << 7) | (1 << 0);
    
}

void servo_set_angle(uint16_t angle){
    if (angle > 180) angle = 180;
    // Interpolates the angle to set to, based on SERVO_MIN and SERVO_MAX
    uint32_t pulse = SERVO_MIN + (angle * (SERVO_MAX - SERVO_MIN)) / 180; 
    TIM2 -> CCR2 = pulse;
}



int main(void){

    servo_init();

    while(1){
   // Should be running..
    }
}