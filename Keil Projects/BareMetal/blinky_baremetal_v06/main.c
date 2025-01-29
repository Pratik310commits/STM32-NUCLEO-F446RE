#include <stdint.h>

#define RCC_BASE        0x40023800U  // RCC Base Address
#define GPIOA_BASE      0x40020000U  // GPIOA Base Address
#define SYSTICK_BASE    0xE000E010U  // SysTick Base Address


///// CMSIS-like structs for registers

// Define structured access for GPIO registers
typedef struct {
    volatile uint32_t MODER;   // Mode Register
    volatile uint32_t OTYPER;  // Output Type Register
    volatile uint32_t OSPEEDR; // Output Speed Register
    volatile uint32_t PUPDR;   // Pull-Up/Pull-Down Register
    volatile uint32_t IDR;     // Input Data Register
    volatile uint32_t ODR;     // Output Data Register
    volatile uint32_t BSRR;    // Bit Set/Reset Register
    volatile uint32_t LCKR;    // Configuration Lock Register
    volatile uint32_t AFR[2];  // Alternate Function Registers
} GPIO_TypeDef;

// Define structured access for RCC registers
typedef struct {
    volatile uint32_t CR;
    volatile uint32_t PLL_CFGR;
    volatile uint32_t CFGR;
    volatile uint32_t CIR;
    volatile uint32_t AHB1_RSTR;
    volatile uint32_t AHB2_RSTR;
    volatile uint32_t AHB3_RSTR;
    volatile uint32_t RESERVED0;
    volatile uint32_t APB1_RSTR;
    volatile uint32_t APB2_RSTR;
    volatile uint32_t RESERVED1[2];
    volatile uint32_t AHB1_ENR;   //// This is the one we are after
    volatile uint32_t AHB2_ENR;
    volatile uint32_t AHB3_ENR;
} RCC_TypeDef;

// Define structured access for SysTick registers
typedef struct {
    volatile uint32_t CTRL;
    volatile uint32_t LOAD;
    volatile uint32_t VAL;
    volatile uint32_t CALIB;
} SysTick_TypeDef;


// Define peripheral instances using base addresses
#define GPIOA    ((GPIO_TypeDef *) GPIOA_BASE)
#define RCC      ((RCC_TypeDef *) RCC_BASE)
#define SysTick  ((SysTick_TypeDef *) SYSTICK_BASE)

#define LED_PIN  5  // PA5 is connected to onboard LED

// Function prototypes
void setup_mcu(void);
void delay_blocking(long delay);
void delay_ms(uint32_t ms);

int main() {
    setup_mcu(); // Initialize the MCU

    while (1) {
        GPIOA->ODR ^= (1U << LED_PIN);  // Toggle LED
        // delay_blocking(50000);// delay_blocking(50000);
			delay_ms(500);  // Delay 500ms using SysTick
    }

    return 0; // Never reached
}

// Setup clock and GPIO for LED
void setup_mcu(void) {
    RCC->CR |= (1U << 0);  // Enable HSI (High-Speed Internal Clock)

    RCC->AHB1_ENR |= (1U << 0);  // Enable GPIOA clock

    GPIOA->MODER &= ~(3U << (LED_PIN * 2));  // Clear mode bits
    GPIOA->MODER |= (1U << (LED_PIN * 2));   // Set PA5 as output

    GPIOA->OTYPER &= ~(1U << LED_PIN);  // Push-pull mode
    GPIOA->OSPEEDR &= ~(3U << (LED_PIN * 2));  // Low speed

    // Configure SysTick Timer (1ms tick)
    SysTick->LOAD = (16000U - 1);  // Assuming 16MHz clock
    SysTick->VAL = 0;  // Reset current value
    SysTick->CTRL = (1U << 2) | (1U << 0); // Processor clock & enable

}

// Crude Blocking delay
void delay_blocking(long delay)
{
    while (--delay); // Simple decrementing loop
}

// Blocking delay using SysTick Timer
void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms; i++) {
        while (!(SysTick->CTRL & (1U << 16)));  // Wait for COUNTFLAG
    }
}