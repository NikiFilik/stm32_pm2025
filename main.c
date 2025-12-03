#include <stdint.h>
#include <stm32f10x.h>

#ifndef TIM2_IRQn
#define TIM2_IRQn 28
#endif

void SystemInit(void) {
    // Пустая функция, но обязательная для линковки
}

#define LED_PORT GPIOC
#define LED_PIN 13U

#define BUTTON_PORT GPIOA
#define BUTTON_INC_PIN 0U
#define BUTTON_DEC_PIN 1U

static void gpio_init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    /* PC13 as push-pull output, 2 MHz */
    LED_PORT->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13);
    LED_PORT->CRH |= GPIO_CRH_MODE13_0;
    LED_PORT->BSRR = GPIO_BSRR_BS13; /* LED off */

    /* PA0/PA1 input with pull-up */
    BUTTON_PORT->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0 |
                           GPIO_CRL_MODE1 | GPIO_CRL_CNF1);
    BUTTON_PORT->CRL |= (GPIO_CRL_CNF0_1 | GPIO_CRL_CNF1_1);
    BUTTON_PORT->ODR |= (1U << BUTTON_INC_PIN) | (1U << BUTTON_DEC_PIN);
}

static void timer_update_prescaler(uint16_t prescaler) {
    TIM2->PSC = prescaler;
    TIM2->EGR = TIM_EGR_UG; /* Apply new prescaler immediately */
}

static void timer_init(uint16_t prescaler, uint16_t reload) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    timer_update_prescaler(prescaler);
    TIM2->ARR = reload;

    TIM2->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM2_IRQn);

    TIM2->CR1 |= TIM_CR1_CEN;
}

void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~TIM_SR_UIF;
        LED_PORT->ODR ^= (1U << LED_PIN);
    }
}

int __attribute__((noreturn)) main(void) {
    gpio_init();

    const uint16_t arr_reload = 9999U;      /* 10 000 ticks */
    uint16_t prescaler = 7200U - 1U;        /* ~1 Hz at 72 MHz */

    timer_init(prescaler, arr_reload);

    uint8_t prev_inc = 1U;
    uint8_t prev_dec = 1U;

    while (1) {
        uint32_t idr = BUTTON_PORT->IDR;
        uint8_t inc_pressed = (idr & (1U << BUTTON_INC_PIN)) ? 1U : 0U;
        uint8_t dec_pressed = (idr & (1U << BUTTON_DEC_PIN)) ? 1U : 0U;

        if (!inc_pressed && prev_inc) {
            uint16_t next = (prescaler < 0x8000U) ? (uint16_t)(prescaler << 1U) : 0xFFFFU;
            if (next != prescaler) {
                prescaler = next;
                timer_update_prescaler(prescaler);
            }
        }

        if (!dec_pressed && prev_dec) {
            uint16_t next = (prescaler > 1U) ? (prescaler >> 1U) : 1U;
            if (next != prescaler) {
                prescaler = next;
                timer_update_prescaler(prescaler);
            }
        }

        prev_inc = inc_pressed;
        prev_dec = dec_pressed;
    }
}