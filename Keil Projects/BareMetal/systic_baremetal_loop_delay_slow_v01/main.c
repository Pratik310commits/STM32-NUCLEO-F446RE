#include "stm32f446xx.h"

void SysTick_Init_AHB_Div8(uint32_t ticks);
void delay_ms(uint32_t ms);
void setup_mcu();

int main(void) {
    setup_mcu();  // Enable GPIO clock and configure LED pin

    // Initialize SysTick to use AHB/8 clock
    SysTick_Init_AHB_Div8(2000U);  // Assuming 16MHz system clock, AHB/8 = 2MHz, this sets 1ms tick

    while (1) {
        GPIOA->ODR ^= (1U << 5);  // Toggle LED
        delay_ms(5000);  // 500ms delay
    }
}

// SysTick Initialization (AHB/8 Clock)
void SysTick_Init_AHB_Div8(uint32_t ticks) {
    SysTick->LOAD = ticks - 1;  // Set reload register
    SysTick->VAL = 0;           // Clear current value register
    SysTick->CTRL = (0U << 2) | // Use AHB/8 (bit 2 = 0)
                    SysTick_CTRL_ENABLE_Msk;  // Enable SysTick

}

// Delay function using SysTick (AHB/8)
void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms; i++) {
        while (!(SysTick->CTRL & (1U << 16)));  // Wait for COUNTFLAG to be set
    }
}

// GPIO Setup (PA5 for LED)
void setup_mcu(void) {
	
		// Enable HSI (High-Speed Internal) clock (although this is default behavior)
    RCC->CR |= RCC_CR_HSION; // Set HSION bit (Bit 0) in the Clock Control Register (CR)
	
    // Enable clock for GPIOA
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;  // Enable GPIOA clock
    
    // Set PA5 as output (for onboard LED)
    GPIOA->MODER |= GPIO_MODER_MODE5_0;  // Set PA5 as output
    
    // Set PA5 as push-pull output (default)
    GPIOA->OTYPER &= ~GPIO_OTYPER_OT5;
    
    // Set PA5 output speed to low (although this is default behavior)
	// GPIO_OSPEEDR_OSPEEDR5
    GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED5;
    
    // Set PA5 to no pull-up, no pull-down (although this is default behavior)
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR5;
}
