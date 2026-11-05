#include <stdio.h>
#include <stdint.h>
// #include <stm32f103xb.h> to try next time

static void delay(volatile uint32_t n) {
    while (n--) { __asm volatile ("nop"); }
}

// GPIOA registers
#define GPIOA_CRL          (*(volatile uint32_t *)0x40010800)
#define GPIOB_CRH          (*(volatile uint32_t *)0x40010C04)
#define GPIOB_BSRR         (*(volatile uint32_t *)0x40010C10)
#define GPIOB_BSRR_BS12    (1u << 12)
#define GPIOB_BSRR_BR12    (1u << 28)
#define GPIOB_BSRR_BS13    (1u << 13)
#define GPIOB_BSRR_BR13    (1u << 29)

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
    GPIOA_CRL &= ~(0xF << 0); // Clear mode and configuration bits for PA0
    GPIOA_CRL |= (0x9 << (0 * 4));  // Set mode  MODE=01, CNF=10 → 1001.

    // Configure PB12 as output
    GPIOB_CRH &= ~(0xF << 16); // Clear mode and configuration
    GPIOB_CRH |= (0x1 << 16);  // Set mode to MODE=01, CNF=00 → 0001

    // configure PB13 as output
    GPIOB_CRH &= ~(0xF << 20); // Clear mode and configuration
    GPIOB_CRH |= (0x1 << 20);  // Set mode to Alternate function output Push-pull MODE=01, CNF=00 → 0001

    // Configure TIM2
    TIM2_CR1 = (1 << 7); // Enable auto-reload preload
    TIM2_ARR = 1000 - 1; // Set auto-reload value for 1 kHz frequency
    TIM2_PSC = 7; // 
    // ---- PWM channel 1 (TIM2_CH1 on PA0) ----
    TIM2_CCMR1 &= ~((3u << 0) | (7u << 4));   // CC1S = 00 (output), clear OC1M
    TIM2_CCMR1 |=  (6u << 4) | (1u << 3);     // OC1M = 110 (PWM mode 1), OC1PE = 1
    TIM2_CCR1   = 0;                          // start at 0% duty
    TIM2_CCER  &= ~(1u << 1);                 // CC1P = 0 (active high)
    TIM2_CCER  |=  (1u << 0);                 // CC1E = 1 (connect output to PA0)

    // ---- load registers and start ----
    TIM2_EGR |= (1u << 0);                    // UG: update event, load PSC/ARR/CCR1
    TIM2_CR1 |= (1u << 0);                    // CEN: start the counter

    while (1) {
        // ---- Forward: ramp up, hold, ramp down ----
        GPIOB_BSRR = GPIOB_BSRR_BS12 | GPIOB_BSRR_BR13;
        for (uint32_t d = 0; d <= 999; d += 1) { TIM2_CCR1 = d; delay(20000); }
        delay(500000);
        for (int32_t d = 999; d >= 0; d -= 1)  { TIM2_CCR1 = d; delay(20000); }
        TIM2_CCR1 = 0;
        
        // ---- Stop briefly before changing direction ----
        GPIOB_BSRR = GPIOB_BSRR_BR12 | GPIOB_BSRR_BR13;
        delay(500000);
        
        // ---- Reverse: ramp up, hold, ramp down ----
        GPIOB_BSRR = GPIOB_BSRR_BR12 | GPIOB_BSRR_BS13;
        for (uint32_t d = 0; d <= 999; d += 1) { TIM2_CCR1 = d; delay(20000); }
        delay(500000);
        for (int32_t d = 999; d >= 0; d -= 1)  { TIM2_CCR1 = d; delay(20000); }
        TIM2_CCR1 = 0;
        
        // ---- Stop briefly before the next cycle ----
        GPIOB_BSRR = GPIOB_BSRR_BR12 | GPIOB_BSRR_BR13;
        delay(500000);
    }

    return 0;
}

int main(void);
__attribute__((noreturn)) void _reset(void) {
    main();
    for(;;)(void) 0; // Infinite loop to prevent returning from reset
}

//stack 
extern void _estack(void); // defined from link.ld

//16 and 91 specific interrupt service routine handlers
__attribute__((section(".vectors"))) void (* const tab [16 + 19])(void) = {
    _estack, _reset 
};