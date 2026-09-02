#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define RCC_APB1ENR   (*(volatile uint32_t *)0x4002101C) // enable clock
#define RCC_APB2ENR   (*(volatile uint32_t *)0x40021018) // enable GPIOA clock
#define GPIOA_CRL     (*(volatile uint32_t *)0x40010800)

#define TIM2_CR1      (*(volatile uint32_t *)0x40000000)
#define TIM3_CR1      (*(volatile uint32_t *)0x40000400)

#define TIM2_PSC      (*(volatile uint32_t *)0x40000028)
#define TIM3_PSC      (*(volatile uint32_t *)0x40000428)

#define TIM2_ARR      (*(volatile uint32_t *)0x4000002C)
#define TIM3_ARR      (*(volatile uint32_t *)0x4000042C)

#define TIM2_CCMR1    (*(volatile uint32_t *)0x40000018)
#define TIM3_CCMR1    (*(volatile uint32_t *)0x40000418)

#define TIM2_CCER     (*(volatile uint32_t *)0x40000020)
#define TIM3_CCER     (*(volatile uint32_t *)0x40000420)

#define TIM2_CCR2     (*(volatile uint32_t *)0x40000038)
#define TIM3_CCR2     (*(volatile uint32_t *)0x40000438)


int main(void) {
    // Enable the clock for GPIO port A
    RCC_APB2ENR |= (1 << 2);
    // Enable the clock for TIM2EN and TIM3EN bits 0 and 1
    RCC_APB1ENR |= (1 << 0) | (1 << 1); // Enable TIM2 and TIM3 clocks

    // setting GPIO A pin 1 as output
    GPIOA_CRL &= ~(0xF << 4);// Clear PA1 configuration bits
    GPIOA_CRL |= (0xA << 4); // Set PA1 as alternate function output push-pull, max speed 2 MHz

    // setting GPIO A pin 7 as output
    GPIOA_CRL &= ~(0xF << 28); // Clear PA7 configuration bits
    GPIOA_CRL |= (0xA << 28); // Set PA7 as alternate function output push-pull, max speed 2 MHz

    // Configure TIM2 for a 1k Hz frequency
    TIM2_PSC = 0x47; // Set prescaler to 71 (PSC = 71)
    TIM2_ARR = 0x3E7; // Set auto-reload value to 999

    // configure TIM3 for a 1k Hz frequency
    TIM3_PSC = 71; // Set prescaler to 71 
    TIM3_ARR = 19999; // Set auto-reload value to 19999

    // Configure TIM2 channel 2 for PWM mode 1
    TIM2_CCMR1 &= ~(0xF << 11); // Clear CCM 
    TIM2_CCMR1 |= (0xD << 11) ; // Set PWM mode 1

    // Configure TIM3 channel 2 for PWM mode 1
    TIM3_CCMR1 &= ~(0xF << 11); // Clear CCM
    TIM3_CCMR1 |= (0xD << 11); // Set PWM mode 1

    // Enable TIM2 channel 2 output
    TIM2_CCER |= (1 << 4); // Enable output for channel 2

    // Enable TIM3 channel 2 output
    TIM3_CCER |= (1 << 4); // Enable output for channel 2

    // Enable TIM2 counter
    TIM2_CR1 |= (1 << 0); // Enable TIM2

    // Enable TIM3 counter
    TIM3_CR1 |= (1 << 0); // Enable TIM3

    // Your application code here

    while (1) {
        // Main loop
        for (volatile int i = 0; i < 999; i++) {
            TIM2_CCR2 = i; // Update the duty cycle
            for (volatile int j = 0; j < 1000; j++); // Delay loop
        }
        for (volatile int i = 1000; i < 2000; i++) {
            TIM3_CCR2 = i; // Update the duty cycle
            for (volatile int j = 0; j < 10000; j++); // Delay loop
        }
        
        for (volatile int i = 999; i >= 0; i--) {
            TIM2_CCR2 = i; // Update the duty cycle
            for (volatile int j = 0; j < 1000; j++); // Delay loop
        }
        for (volatile int i = 2000; i > 1000; i--) {
            TIM3_CCR2 = i; // Update the duty cycle
            for (volatile int j = 0; j < 1000; j++); // Delay loop
        }
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