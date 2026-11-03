#include <stdio.h>
// #include <stm32f103xb.h> to try next time


// GPIOA registers
#define GPIOA_CRL          (*(volatile uint32_t *)0x40010800)
#define GPIOB_CRH          (*(volatile uint32_t *)0x40010C00)
#define GPIOB_BSRR         (*(volatile uint32_t *)0x40010C10)
#define GPIOB_BSRR_BS12    (1 << 12)
#define GPIOB_BSRR_BR12    (1 << 28)
#define GPIOB_BSRR_BS13    (1 << 13)
#define GPIOB_BSRR_BR13    (1 << 29)

// Clock control registers
#define RCC_APB1ENR   (*(volatile uint32_t *)0x4002101C)
#define RCC_APB2ENR   (*(volatile uint32_t *)0x40021018)

// TIM2 registers
#define TIM2_CR1      (*(volatile uint32_t *)0x40000000)
#define TIM2_PSC      (*(volatile uint32_t *)0x40000028)
#define TIM2_ARR      (*(volatile uint32_t *)0x4000002C)
#define TIM2_EGR      (*(volatile uint32_t *)0x40000014)

// PWM channel
#define TIM2_CCR1     (*(volatile uint32_t *)0x40000034)
#define TIM2_CCMR1    (*(volatile uint32_t *)0x40000018)
#define TIM2_CCER     (*(volatile uint32_t *)0x40000020)

/*
// Motor control logic
GPIOB_BSRR = GPIO_BSRR_BS12 | GPIO_BSRR_BR13;   // forward
GPIOB_BSRR = GPIO_BSRR_BR12 | GPIO_BSRR_BS13;   // reverse
GPIOB_BSRR = GPIO_BSRR_BR12 | GPIO_BSRR_BR13;   // both low: motor stops (coast)
*/

int main (void) {

    // Enable clock for TIM2
    RCC_APB1ENR |= (1 << 0); // Enable clock for TIM2
    // Enable clock for GPIOA and GPIOB
    RCC_APB2ENR |= (1 << 2) | (1 << 3); // Enable clock for GPIOA and GPIOB

    // Configure PA0 as output
    (GPIOA_CRL + 0x10) = (1 << 0) | (1 << 16); 
    GPIOA_CRL &= ~(0xF << 0); // Clear mode and configuration bits for PA0
    GPIOA_CRL |= (0x9 << (0 * 4));  // Set mode to Alternate function output Push-pull MODE=01, CNF=10 → 1001.

    // Configure PB12 as output
    GPIOB_CRH &= ~(0xF << 16); // Clear mode and configuration
    GPIOB_CRH |= (0x1 << 16);  // Set mode to Alternate function output Push-pull MODE=01, CNF=00 → 0001

    // configure PB13 as output
    GPIOB_CRH &= ~(0xF << 20); // Clear mode and configuration
    GPIOB_CRH |= (0x1 << 20);  // Set mode to Alternate function output Push-pull MODE=01, CNF=00 → 0001

    // Configure TIM2
    TIM2_CR1 = (1 << 7); // Enable auto-reload preload
    TIM2_ARR = 1000 - 1; // Set auto-reload value for 1 kHz frequency
    TIM2_PSC = 7; // 

    while (1) {
        
    }

    return 0;
}