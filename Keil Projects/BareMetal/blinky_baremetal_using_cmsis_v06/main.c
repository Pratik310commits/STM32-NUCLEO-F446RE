#include "stm32f446xx.h"  // Include the CMSIS header file for STM32F446xx series

// Function Prototypes
void delay(long delay);
void setup_mcu();

int main()
{
    // Initialize the microcontroller settings (Clock, GPIO, etc.)
    setup_mcu();

    // Infinite loop for LED blinking
    while (1)
    {
        GPIOA->ODR |= (0x01U << 5); // Set PA5 HIGH (Turn ON LED)
        delay(500000);  // Delay
        delay(500000);  // Additional delay

        GPIOA->ODR &= ~(0x01U << 5); // Set PA5 LOW (Turn OFF LED)
        delay(500000);  // Delay before next toggle
    }

    return 0;  // This will never be reached
}

// Function to configure the microcontroller settings
void setup_mcu()
{
    // Enable HSI (High-Speed Internal) clock
    RCC->CR |= (0x01U << 0); // Set HSION bit (Bit 0) in the Clock Control Register (CR)
    
    // Enable GPIOA peripheral clock
    RCC->AHB1ENR |= (0x01U << 0); // Set Bit 0 in AHB1ENR (AHB1 peripheral clock enable register)

    // Set PA5 as General Purpose Output Mode
    GPIOA->MODER |= (0x01U << 10); // Set bits 10-11 (01: General purpose output mode)

    // Set GPIOA Output Type as Push-Pull (Default)
    GPIOA->OTYPER = 0x0;

    // Set GPIOA Output Speed as Low Speed (Default)
    GPIOA->OSPEEDR = 0x0;
}

// Function to create a simple delay loop
void delay(long delay)
{
    while (delay--); // Simple decrementing loop for crude delay
}
