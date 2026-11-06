#include <stdint.h>

extern volatile uint32_t daq_flag;
void delay_ms(uint32_t ms);
void tim2_init(void);
void delay_us(uint32_t us);
void tim3_init(void);