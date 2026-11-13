#ifndef PWM_H
#define PWM_H

#include <stdint.h>

// VALUE DEFINITIONS 

#define SERVO_PWM_FREQUENCY_HZ 50
#define SERVO_MIN 1000
#define SERVO_MAX 2000
#define PIN_NUM 1
#define AF_TIM2 1

void servo_init(void);

void servo_set_angle(uint16_t angle);

uint32_t timer_clock(void);

#endif

// NOTE: THIS IS FOR SETTING CUSTOM HIGH_TIMES and LOW_TIMES

// WRITE TIM2->PSC for setting the tick rate
// WRITE TIM2->ARR for setting total period to whatever
// TIM2->CCR2 for setting high pulse duration
// Based on the PWM Frequency you have selected (custom to what the user says)

// PWM_FREQUENCY = f TIM_CLK / (PSC + 1) * (ARR + 1)

// also note that f TIM_CLK is clock frequency entering the timer
