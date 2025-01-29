#include <stdint.h>  // Include standard integer types

void delay(long delay);
void setup_mcu();

// PA5 pin is connected to the onboard Green LED (LD2) on STM32 Nucleo board

/*** RCC Register Addresses ***/
// RCC_CR (Clock Control Register) - Address: 0x40023800
unsigned int *RCC_CR = (unsigned int *)0x40023800;

// RCC_AHB1ENR (AHB1 Peripheral Clock Enable Register) - Address: 0x40023830
unsigned int *RCC_AHB1ENR = (unsigned int *)0x40023830;

/*** GPIOA Register Addresses ***/
// GPIOA_MODER (Mode Register) - Address: 0x40020000
unsigned int *GPIOA_MODER = (unsigned int *)0x40020000;

// GPIOA_OTYPER (Output Type Register) - Address: 0x40020004
unsigned int *GPIOA_TYPER = (unsigned int *)0x40020004;

// GPIOA_OSPEEDR (Output Speed Register) - Address: 0x40020008
unsigned int *GPIOA_SPEEDR = (unsigned int *)0x40020008;

// GPIOA_ODR (Output Data Register) - Address: 0x40020014
unsigned int *GPIOA_ODR = (unsigned int *)0x40020014;

int main()
{
    // Setup the MCU (Clock, GPIO)
    setup_mcu();

    // Infinite loop for LED blinking
    while (1)
    {
        *GPIOA_ODR |= (0x01U << 5); // Set PA5 HIGH (Turn ON LED)
        delay(500000); delay(500000); delay(500000); // Crude delay

        *GPIOA_ODR &= ~(0x01U << 5); // Set PA5 LOW (Turn OFF LED)
        delay(20000); delay(20000); // Another crude delay
    }

    return 0;  // This will never be reached
}

void setup_mcu()
{
    // Enable HSI (High-Speed Internal) 16MHz clock
    *RCC_CR |= (0x01U << 0);  // Set HSION bit (Bit 0) in RCC_CR

    // Enable GPIOA peripheral clock
    *RCC_AHB1ENR |= (0x01U << 0);  // Set Bit 0 in RCC_AHB1ENR (Enable GPIOA)

    // Set PA5 as General Purpose Output Mode
    *GPIOA_MODER |= (0x01U << 10); // Set bits 10-11 (01: Output Mode)

    // Set GPIOA Output Type as Push-Pull (Default)
    *GPIOA_TYPER = 0x0;

    // Set GPIOA Output Speed as Low Speed (Default)
    *GPIOA_SPEEDR = 0x0;
}

// Crude delay function using a blocking loop
void delay(long delay)
{
    while (--delay); // Simple decrementing loop
}
