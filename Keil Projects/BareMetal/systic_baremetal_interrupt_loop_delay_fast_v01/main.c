#include "stm32f446xx.h"

volatile uint32_t systick_counter = 0;  // Global counter incremented in ISR

void SysTick_Init_AHB(uint32_t ticks);
void delay_ms(uint32_t ms);
void setup_mcu();

int main(void) {
    setup_mcu();  // Enable GPIO clock and configure LED pin

    // Initialize SysTick to use full system clock
    SysTick_Init_AHB(16000U);  // Assuming 16MHz system clock, this sets 1ms tick

    while (1) {
        GPIOA->ODR ^= (1U << 5);  // Toggle LED
        delay_ms(300);  // 300ms delay
    }
}

// SysTick Initialization (AHB Clock - Full Speed)
void SysTick_Init_AHB(uint32_t ticks) {
    SysTick->LOAD = ticks - 1;  // Set reload register
    SysTick->VAL = 0;           // Clear current value register
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |  // Use processor clock (AHB)
										SysTick_CTRL_TICKINT_Msk | 		// Enable systick interrupt
                    SysTick_CTRL_ENABLE_Msk;  		// Enable SysTick
	
		//// NVIC_SetPriority(SysTick_IRQn, 1);  // Set SysTick interrupt priority (low)
	
}

// Delay function using SysTick counter
/**
Handling Overflow Safely
Using Unsigned Integer Wrapping (Modulo Arithmetic)
**/
void delay_ms(uint32_t ms) {
    uint32_t start_time = systick_counter;  // Capture the start time
    while ((systick_counter - start_time) < ms);  // Wait safely using modulo arithmetic
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

// SysTick Interrupt Handler (ISR)
void SysTick_Handler(void) {
    systick_counter++;  // Increment every 1ms
}