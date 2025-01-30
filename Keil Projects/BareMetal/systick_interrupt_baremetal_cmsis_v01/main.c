#include "stm32f4xx.h"  // Include the CMSIS header for STM32F4 series

// Define SysTick reload value for 1-second delay (assuming a 16 MHz SysTick clock)
#define SYSTICK_DELAY 16000000U  // Adjust this value depending on the system clock

// Function prototypes
void setup_mcu(void);
void SysTick_Handler(void);  // SysTick interrupt handler

// Global variables for LED state
volatile int led_state = 0;

int main(void) {
    // MCU setup
    setup_mcu();
    
    // Manually configure SysTick for 1-second delay
    SysTick->LOAD = SYSTICK_DELAY - 1;  // Set reload value (16 MHz => 1-second delay)
    SysTick->VAL = 0;  // Clear the current value register
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk | // Enable high speed mode
										 SysTick_CTRL_TICKINT_Msk | 	// Enable Systick Interrupt
										 SysTick_CTRL_ENABLE_Msk;  		// Enable SysTick, with interrupt and processor clock as source
    
    // Main loop - the LED will be toggled by the SysTick interrupt
    while (1) {
        // Do other tasks here if needed
        // The LED toggling is done in the SysTick_Handler
    }
    
    return 0;
}

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

void SysTick_Handler(void) {
    // Toggle the LED state
    if (led_state) {
        GPIOA->ODR |= (0x01U << 5);  // Turn on the LED (PA5)
    } else {
        GPIOA->ODR &= ~(0x01U << 5);  // Turn off the LED (PA5)
    }
    
    // Toggle the state
    led_state = !led_state;
}
