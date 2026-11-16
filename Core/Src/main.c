
#include "stm32f446xx.h"

void GPIO_Init(void);
void EXTI_Init(void);

int main(void)
{
    GPIO_Init();
    EXTI_Init();

    while (1)
    {
        // CPU waits for interrupts
        __WFI();
    }
}

void GPIO_Init(void)
{
    // Enable GPIOA and GPIOC clocks
    RCC->AHB1ENR |= (1U << 0) | (1U << 2);

    // PA5 output
    GPIOA->MODER &= ~(3U << 10);
    GPIOA->MODER |=  (1U << 10);

    // PC13 input
    GPIOC->MODER &= ~(3U << 26);
}

void EXTI_Init(void)
{
    // Enable SYSCFG clock
    RCC->APB2ENR |= (1U << 14);

    // Map EXTI13 to GPIOC
    SYSCFG->EXTICR[3] &= ~(0xFU << 4);
    SYSCFG->EXTICR[3] |=  (0x2U << 4);

    // Enable rising and falling edge triggers
    EXTI->RTSR |= (1U << 13);
    EXTI->FTSR |= (1U << 13);

    // Unmask EXTI13
    EXTI->IMR |= (1U << 13);

    // Enable interrupt in NVIC
    NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void EXTI15_10_IRQHandler(void)
{
    if (EXTI->PR & (1U << 13))
    {
        // Button pressed (active LOW)
        if ((GPIOC->IDR & (1U << 13)) == 0)
        {
            GPIOA->BSRR = (1U << 5);        // LED ON
        }
        else
        {
            GPIOA->BSRR = (1U << (5 + 16)); // LED OFF
        }

        // Clear pending interrupt
        EXTI->PR = (1U << 13);
    }
}
