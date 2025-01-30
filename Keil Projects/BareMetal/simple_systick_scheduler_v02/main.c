#include "stm32f446xx.h"

#define TASK_FLAG_1 (1U << 0)  // Task 1: LED on PA5
#define TASK_FLAG_2 (1U << 1)  // Task 2: External LED on PB3
#define MAX_TASKS 2

volatile uint32_t systick_counter = 0;  // Global counter incremented in ISR
volatile uint32_t last_task1_run = 0;
volatile uint32_t last_task2_run = 0;
volatile uint8_t task_flags = 0;  // Flags to track scheduled tasks

typedef enum {
    TASK_STATE_READY,
    TASK_STATE_RUNNING,
    TASK_STATE_WAITING
} TaskState;

typedef struct {
    uint8_t task_id;
    void (*task_function)(void);  // Function pointer to the task
    TaskState state;
    uint32_t next_run_time;  // When to execute next (ms)
    uint32_t interval;  // Execution interval
} TaskControlBlock;

void SysTick_Init_AHB(uint32_t ticks);
void delay_ms(uint32_t ms);
void setup_mcu();
void execute_tasks(void);
void task1(void);
void task2(void);

TaskControlBlock tasks[MAX_TASKS] = {
    {0, task1, TASK_STATE_READY, 0, 2000},  // Task 1: Runs every 500ms
    {1, task2, TASK_STATE_READY, 0, 1000}  // Task 2: Runs every 1000ms
};

int main(void) {
    setup_mcu();  // Enable GPIO clock and configure LED pin

    // Initialize SysTick to use full system clock
    SysTick_Init_AHB(16000U);  // Assuming 16MHz system clock, this sets 1ms tick

    while (1) {
        // Everything is handled inside SysTick_Handler & scheduler()
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
    // Enable HSI (High-Speed Internal) clock (default behavior)
    RCC->CR |= RCC_CR_HSION; // Set HSION bit (Bit 0) in the Clock Control Register (CR)
    
    // Enable clock for GPIOA
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;  // Enable GPIOA clock
    // RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;  // (No need for this line since we're using PA6, not PB3)

    // Set PA5 as output (for onboard LED)
    GPIOA->MODER |= GPIO_MODER_MODE5_0;  // Set PA5 as output

    // Set PA6 as output (External LED)
    GPIOA->MODER |= GPIO_MODER_MODE6_0;  // Set PA6 as output

    // Set PA5 as push-pull output (default)
    GPIOA->OTYPER &= ~GPIO_OTYPER_OT5;
    
    // Set PA6 as push-pull output (default)
    GPIOA->OTYPER &= ~GPIO_OTYPER_OT6;  // Set PA6 to push-pull output
    
    // Set PA5 output speed to low (default)
    GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED5;
    
    // Set PA6 output speed to low (default)
    GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED6;  // Set PA6 speed to low
    
    // Set PA5 to no pull-up, no pull-down (default)
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR5;
    
    // Set PA6 to no pull-up, no pull-down (default)
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR6;  // No pull-up, no pull-down for PA6
}


void scheduler(void) {
    uint32_t current_time = systick_counter;  // Get system time

    for (int i = 0; i < MAX_TASKS; i++) {
        if (tasks[i].state == TASK_STATE_READY && current_time >= tasks[i].next_run_time) {
            tasks[i].state = TASK_STATE_RUNNING;  // Set state to running
            tasks[i].task_function();  // Execute the task
            tasks[i].next_run_time = current_time + tasks[i].interval;  // Schedule next run
            tasks[i].state = TASK_STATE_WAITING;  // Task is now waiting
        }

        // If the waiting period is over, mark the task as READY again
        if (tasks[i].state == TASK_STATE_WAITING && current_time >= tasks[i].next_run_time) {
            tasks[i].state = TASK_STATE_READY;
        }
    }
}


void task1(void) {
    GPIOA->ODR ^= GPIO_ODR_OD5;  // Toggle PA5
}

void task2(void) {
    GPIOA->ODR ^= GPIO_ODR_OD6;  // Toggle PA6
}


// SysTick Interrupt Handler (ISR)

void SysTick_Handler(void) {
    systick_counter++;  // Increment system time
    scheduler();  // Call scheduler every 1ms
}

/*
void SysTick_Handler_v2(void) {
    systick_counter++;  // Increment every 1ms

    // Check if it's time for Task 1 (LD2 Blink at 500ms)
    if (systick_counter % 500 == 0) {
        task_flags |= TASK_FLAG_1;
    }
    
    // Check if it's time for Task 2 (External LED Blink at 1000ms)
    if (systick_counter % 1000 == 0) {
        task_flags |= TASK_FLAG_2;
    }
}
*/