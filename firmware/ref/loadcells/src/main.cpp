#include "stm32h723xx.h"
#include "stm32_timer.h"
#include "stm32_usart.h"

#define LED1_PIN  (1U << 0)
#define ONBOARD_BTTN_PIN (1U << 13)

#define MAX_24BIT_SIGNED 0x7FFFFF
#define CALIBRATION_CONST_SMALL_LOAD_CELL -398161
#define CALIBRATION_CONST_LARGE_LOAD_CELL 2000

//----------------------------- Declarations -----------------------------------
void blinkLED(void);

//------------------------------ Definitions -----------------------------------

class HX711 {

    private:

        // fixme include functionality to return false if timeout exceeded
        bool is_ready(int timeout=0) {

            if(this->port->IDR & (1U << this->data_pin)) {
                print_text("ADC module ON");
            }
            // Wait for data ready
            while(this->port->IDR & (1U << this->data_pin)) {}
            print_text("ADC module bitstream ready");

            return true;
        }

        void calibrate(uint32_t force) {

            int sensitivity_conversion = 20/(5*(this->sensitivity));
            //
            // KGs to LBs
            float max_capacity_lbs = this->capacity*2.20462;
            float calibrated_force = (float((force + this->calibration)*sensitivity_conversion)/float(MAX_24BIT_SIGNED))*max_capacity_lbs;

            this->mass = calibrated_force;

        }

    public:

        GPIO_TypeDef* port;
        int data_pin;
        int clock_pin;
        float capacity;
        int calibration;
        int sensitivity;
        float mass;

        HX711(GPIO_TypeDef* PORT, int dt, int clk, float max_capacity, int calibration_const, int sensitivity_const) {
            port = PORT;
            data_pin = dt;
            clock_pin = clk;
            capacity = max_capacity;
            calibration = calibration_const;
            sensitivity = sensitivity_const;

            // Set input mode for data pin
            port->MODER &= ~(3U << (this->data_pin * 2));
            port->MODER &= ~(3U << (this->clock_pin * 2));
            //
            // Set output mode for clock pin
            port->MODER |=  (1U << (this->clock_pin * 2));
            //
            // Drive SCK on ADC low before waiting for ready state
            port->ODR &= ~(1U << this->clock_pin);
        }

        void read() {

            // Initialize force/weight variable
            int32_t force = 0;

            // Checks if bitstream is ready
            this->is_ready();

            // Create pulse & read bit from input
            // One pulse will shift new bit from 24bit data into DT on ADC
            // Pulse must be hi for >0.2us & <50us, lo for >0.2us
            for(int i=0; i<24; i++) {
                
                // Drive high
                this->port->ODR |= (1U << this->clock_pin);
                delay_us(20);
                //
                // Drive low
                this->port->ODR &= ~(1U << this->clock_pin);
                delay_us(20);

                // Shift 'force' variable left & turn LSB high if data pin high
                // Basically shifts nexts MSB of bitstream into LSB of 'force'
                force = force << 1;
                if(this->port->IDR & (1U << this->data_pin)) {
                    force++;
                }

            }

            // ADC module datasheet says to do this, not sure why
            if (force & 0x800000) {
                force |= 0xFF000000;
            }

            // Send one more pulse to set gain 128 in next measurement cycle
            // 128 gain presumes +/- 20mV analogue input
            //
            // Drive high
            this->port->ODR |= (1U << this->clock_pin);
            delay_us(20);
            //
            // Drive low
            this->port->ODR &= ~(1U << this->clock_pin);
            delay_us(20);

            calibrate(force);

        }

};

int main(void) {

    // ----------------------- Runs once ever only -----------------------------
    // volatile bool interrupt;
    // bool run = true;

    // GPIO enable
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOCEN;

    usart_init(9600);
    tim2_init();
    tim3_init();
    delay_us(10000000);
    
    // Example: small load cell object init, read value, print value, delete object
    HX711* small_load_cell = new HX711(GPIOB,               // Port
                                        8,                  // Data pin
                                        9,                  // Clock pin
                                        20,                 // Load cell capacity
                                        CALIBRATION_CONST_SMALL_LOAD_CELL, 
                                        1                   // Sensitivity
                                    );


    //-------------- Infinite while loop unless interrupt triggered var --------

    /*
    1. Setup up timer interrupt
    2. Upon each trigger, set flag to read sensor in next iteration of main
    3. Upon sensor read, reset flag
    */
    while(1) {

        if(daq_flag) {
            daq_flag = 0;
            small_load_cell->read();
            print_text("\nMass: \n");
            print_float(small_load_cell->mass);
            blinkLED();
        }

    }
    delete(small_load_cell);
}


// Silly little starter function to test workspace
void blinkLED(void) {

    // Bunch of init values for PB0
    GPIOB->MODER &= ~(3U << (0 * 2));
    //
    // Set output mode: MODER = 0b01 = 1U
    GPIOB->MODER |=  (1U << (0 * 2));
    //
    // Type
    GPIOB->OTYPER &= ~(1U << 0);
    //
    // Pull-up resistor
    GPIOB->PUPDR  &= ~(3U << (0 * 2));
    //
    // Speed
    GPIOB->OSPEEDR &= ~(3U << (0 * 2));

    // Init for PC13
    //
    // Set input mode: MODER = 0b00 = ~0b11 = ~3U
    GPIOC->MODER &= ~(3U << (13 * 2));

    // Toggle LED every 1s
    GPIOB->ODR ^= LED1_PIN;
    delay_ms(500);
    GPIOB->ODR ^= LED1_PIN;
   
}