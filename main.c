#include "stm8s.h" // From https://github.com/alx741/stm8s-sdcc-lib/stm8s.h
#include <stdio.h>
#include <stdbool.h>

void uart_init();
void pulse(bool direction);

int position = 0;

int main()
{
    char somechar;

    CLK_CKDIVR.HSIDIV = HSIDIV_0;
    CLK_CKDIVR.CPUDIV = CPUDIV_0;

    PORTA.DDR1 = DDR_INPUT_MODE;
    PORTA.CR21 = CR2_INPUT_INTERRUPT_ENABLED;
    PORTA.CR11 = 0x00;

    PORTC.DDR3 = DDR_OUTPUT_MODE;
    PORTC.CR13 = CR1_OUTPUT_PUSH_PULL;
    PORTC.CR23 = CR2_OUTPUT_2MHZ;

    PORTC.DDR4 = DDR_OUTPUT_MODE;
    PORTC.CR14 = CR1_OUTPUT_PUSH_PULL;
    PORTC.CR24 = CR2_OUTPUT_2MHZ;

    uart_init();

    EXTI_CR1.PAIS = EXTI_RISING_EDGE;
    enable_interrupts();

    while (1)
    {
        somechar = getchar();
        if (somechar == 'j')
        {
            pulse(true);
        }
        else if (somechar == 'l')
        {
            pulse(false);
        }
    }
}

void delay(void)
{
    for (int i = 0; i < 30000; i++) {}
}

void pulse(bool direction)
{
    if (direction)
    {
            PORTC.ODR3 = true;
            delay();
            PORTC.ODR3 = false;
    }
    else
    {
            PORTC.ODR4 = true;
            delay();
            PORTC.ODR4 = false;
    }
}

void porta_isr3(void) __interrupt(IRQ_EXTI0_PORTA)
{
    if (PORTA.IDR2)
    {
        position++;
    }
    else
    {
        position--;
    }
    printf("%d", position);
}

void uart_init()
{
    UART_BRR2 = 0x03;
    UART_BRR1 = 0x68;

    UART_CR1.UARTD = 0; // UART enabled
    UART_CR1.M     = 0; // 8 bit word
    UART_CR1.PCEN  = 0; // No parity
    UART_CR1.PIEN  = 0; // No parity interrupt

    UART_CR2.TEN = 1; // Transmitter enabled
    UART_CR2.REN = 1; // Receiver enabled
}

int putchar(int c)
{
    UART_DR = c;
    while (! UART_SR.TC) {}
    return c;
}

int getchar(void)
{
    while (! UART_SR.RXNE) {}
    return UART_DR;
}
