// From https://github.com/alx741/stm8s-sdcc-lib
#include <stm8s.h>
#include <i2c.h>
#include <external/mpu6050.h>
#include "/usr/share/sdcc/include/stdio.h"
#include <stdbool.h>
#include <string.h>

void uart_init();
void pulse(bool direction);
void delay(void);

int position = 0;

void main()
{
    uint8_t somechar;
    ACCEL_t accel;
    ACCEL_RAW_t accel_raw;
    GYRO_t gyro;

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

    EXTI_CR1.PAIS = EXTI_RISING_EDGE;


    uart_init();
    i2c_init_100khz();
    enable_interrupts();

    delay(); // Wait MPU6050 to initialize

    mpu6050_wake_up();

    while (1)
    {
        delay();

        /* printf("TEMP = %f\r", mpu6050_read_temp()); */

        mpu6050_read_accel(&accel);
        printf("X = %f\t\tY = %f\t\tZ = %f\r", accel.X, accel.Y, accel.Z);

        /* mpu6050_read_gyro(&gyro); */
        /* printf("X = %f\tY = %f\tZ = %f\r", gyro.X, gyro.Y, gyro.Z); */

        /* somechar = getchar(); */
        /* if (somechar == 'j') */
        /* { */
        /*     pulse(true); */
        /* } */
        /* else if (somechar == 'l') */
        /* { */
        /*     pulse(false); */
        /* } */
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

void porta_isr(void) __interrupt(IRQ_EXTI0_PORTA)
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
    UART_BRR2 = 0x03;   // 9600 baud
    UART_BRR1 = 0x68;
    UART_CR1.UARTD = 0; // UART enabled
    UART_CR1.M     = 0; // 8 bit word
    UART_CR1.PCEN  = 0; // No parity
    UART_CR1.PIEN  = 0; // No parity interrupt
    UART_CR2.TEN = 1;   // Transmitter enabled
    UART_CR2.REN = 1;   // Receiver enabled
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
