#include "stm8s.h" // From https://github.com/alx741/stm8s-sdcc-lib/stm8s.h
#include <stdbool.h>


void delay(void)
{
    for (int delay_cnt = 0; delay_cnt < 30000; delay_cnt++) {}
}

int main()
{
    CLK_CKDIVR.HSIDIV = HSIDIV_0;
    CLK_CKDIVR.CPUDIV = CPUDIV_0;

    PORTA.DDR3 = DDR_INPUT_MODE;
    PORTA.CR1 = 0xFF;
    PORTA.CR23 = CR2_INPUT_INTERRUPT_ENABLED;

    PORTC.DDR3 = DDR_OUTPUT_MODE;
    PORTC.CR13 = CR1_OUTPUT_PUSH_PULL;
    PORTC.CR23 = CR2_OUTPUT_2MHZ;

    EXTI_CR1.PAIS = EXTI_RISING_FALLING_EDGE;
    enable_interrupts();

    while (1) { }
}

void porta_isr3(void) __interrupt(IRQ_EXTI0_PORTA)
{
    PORTC.ODR3 ^= true;
}
