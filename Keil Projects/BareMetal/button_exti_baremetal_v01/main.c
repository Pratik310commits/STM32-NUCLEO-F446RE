#include "stm32f4xx.h"  // Include the CMSIS header for STM32F4 series

// Function prototypes
void setup_mcu(void);
void EXTI15_10_IRQHandler(void);  // Interrupt handler for the button press

// Global variable for LED state
volatile int led_state = 0;

int main(void) {
    // MCU setup
    setup_mcu();
    
    while (1) {}
    
    return 0;
}

void setup_mcu(void) {
    // Enable clock for GPIOA, GPIOC, and SYSCFG using CMSIS macros
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;  // Enable GPIOA clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;  // Enable GPIOC clock
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;  // Enable SYSCFG clock
    
    // Set PA5 as output (LED pin)
    GPIOA->MODER |= GPIO_MODER_MODE5_0;  // Set PA5 as output
    
    // Set PC13 as input (Button pin)
    GPIOC->MODER &= ~GPIO_MODER_MODE13;  // Set PC13 as input (clear bits)
    
    // Configure the external interrupt for PC13 (button press)
    // SYSCFG->EXTICR[3] |= (0x01U << 5);  // Select GPIOC for EXTI13
	
		SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC;  // Select GPIOC for EXTI13
    EXTI->IMR |= EXTI_IMR_MR13;  // Enable interrupt for EXTI line 13
    EXTI->FTSR |= EXTI_FTSR_TR13;  // Trigger on falling edge (button press)
    
    // Enable global interrupts (NVIC)
    NVIC_EnableIRQ(EXTI15_10_IRQn);  // Enable EXTI line 15-10 interrupt (PC13)
}

// Interrupt Service Routine for EXTI line 13 (button press)
void EXTI15_10_IRQHandler(void) {
    if (EXTI->PR & EXTI_PR_PR13) {  // Check if EXTI line 13 was triggered
        EXTI->PR |= EXTI_PR_PR13;  // Clear the pending interrupt
        
        // Toggle the LED state
        GPIOA->ODR ^= (0x01U << 5);
    }
}

