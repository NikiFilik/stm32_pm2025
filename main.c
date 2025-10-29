#include <stdint.h>

void SystemInit(void) {

}

#define RCC_APB2ENR   (*((volatile uint32_t*)0x40021018))
#define GPIOC_CRH     (*((volatile uint32_t*)0x40011004))
#define GPIOC_ODR     (*((volatile uint32_t*)0x4001100C))
#define GPIOC_IDR     (*((volatile uint32_t*)0x40011008))

const uint32_t delays[] = {
    256000000,  // 1/64 Гц (step -6)
    128000000,  // 1/32 Гц (step -5)
    64000000,   // 1/16 Гц (step -4)
    32000000,   // 1/8 Гц  (step -3)
    16000000,   // 1/4 Гц  (step -2)
    8000000,    // 1/2 Гц  (step -1)
    4000000,    // 1 Гц    (step 0)
    2000000,    // 2 Гц    (step 1)
    1000000,    // 4 Гц    (step 2)
    500000,     // 8 Гц    (step 3)
    250000,     // 16 Гц   (step 4)
    125000,     // 32 Гц   (step 5)
    62500       // 64 Гц   (step 6)
};

volatile int8_t current_step = 6;

void simple_delay(uint32_t cycles) {
    for(volatile uint32_t i = 0; i < cycles; i++);
}

void debounce_delay(void) {
    for(volatile uint32_t i = 0; i < 50000; i++);
}

void check_buttons(void) {
    static uint8_t last_button_A = 1;
    static uint8_t last_button_C = 1;
    
    uint8_t current_button_A = (GPIOC_IDR & (1 << 14)) ? 1 : 0;
    uint8_t current_button_C = (GPIOC_IDR & (1 << 15)) ? 1 : 0;
    
    if (current_button_A == 0 && last_button_A == 1) {
        debounce_delay();
        if ((GPIOC_IDR & (1 << 14)) == 0) {
            if (current_step < 12) { 
                current_step++;
            }
            while ((GPIOC_IDR & (1 << 14)) == 0); 
            debounce_delay();
        }
    }
    
    if (current_button_C == 0 && last_button_C == 1) {
        debounce_delay();
        if ((GPIOC_IDR & (1 << 15)) == 0) {
            if (current_step > 0) { 
                current_step--;
            }
            while ((GPIOC_IDR & (1 << 15)) == 0); 
            debounce_delay();
        }
    }
    
    last_button_A = current_button_A;
    last_button_C = current_button_C;
}

int main(void) {
    RCC_APB2ENR |= (1 << 4);
    
    GPIOC_CRH &= ~(0xFF << 20);
    GPIOC_CRH |= (1 << 20);
    
    GPIOC_CRH &= ~(0xFF << 24);
    GPIOC_CRH |= (0x8 << 24);
    GPIOC_ODR |= (1 << 14);
    
    GPIOC_CRH &= ~(0xFF << 28);
    GPIOC_CRH |= (0x8 << 28);
    GPIOC_ODR |= (1 << 15);
    
    while(1) {
        GPIOC_ODR ^= (1 << 13);
        
        simple_delay(delays[current_step]);
        
        check_buttons();
    }
}