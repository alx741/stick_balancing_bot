#include "stm8s.h" // From https://github.com/alx741/stm8s-sdcc-lib/stm8s.h
#include <stdio.h>
#include <stdbool.h>

void uart_init();
void i2c_init();
/* void pulse(bool direction); */
void delay(void);

#define I2C_READ            0x01
#define I2C_WRITE           0x00

/* #define HMC5883_ADDR        (0x1E << 1) */
#define HMC5883_ADDR        0b11010000

#define HMC5883_CR_A        0x00
#define HMC5883_CR_B        0x01
#define HMC5883_MODE        0x02
#define HMC5883_DATA_OUT    0x03

/* #define HMC5883_ID_REG_A    0x0A */
#define HMC5883_ID_REG_A    0x00

#define _MEM_(mem_addr)         (*(volatile uint8_t *)(mem_addr))
#define _SFR_(mem_addr)         (*(volatile uint8_t *)(0x5000 + (mem_addr)))
#define _SFR16_(mem_addr)       (*(volatile uint16_t *)(0x5000 + (mem_addr)))

/* PORT C */
#define PC_ODR                  _SFR_(0x0A)
#define PC_IDR                  _SFR_(0x0B)
#define PC_DDR                  _SFR_(0x0C)
#define PC_CR1                  _SFR_(0x0D)
#define PC_CR2                  _SFR_(0x0E)

/* I2C */
#define _I2C_CR1                 _SFR_(0x210)
#define _I2C_CR1_PE              0
#define _I2C_CR2                 _SFR_(0x211)
#define _I2C_CR2_ACK             2
#define _I2C_CR2_STOP            1
#define _I2C_CR2_START           0
#define _I2C_FREQR               _SFR_(0x212)
#define _I2C_FREQR_FREQ2         2
#define _I2C_FREQR_FREQ1         1
#define _I2C_FREQR_FREQ0         0
#define _I2C_OARL                _SFR_(0x213)
#define _I2C_OARH                _SFR_(0x214)
#define _I2C_OARH_ADDMODE        7
#define _I2C_OARH_ADDCONF        6
#define _I2C_DR                  _SFR_(0x216)
#define _I2C_SR1                 _SFR_(0x217)
#define _I2C_SR1_TXE             7
#define _I2C_SR1_RXNE            6
#define _I2C_SR1_BTF             2
#define _I2C_SR1_ADDR            1
#define _I2C_SR1_SB              0
#define _I2C_SR2                 _SFR_(0x218)
#define _I2C_SR3                 _SFR_(0x219)
#define _I2C_SR3_BUSY            1
#define _I2C_SR3_MSL             0
#define _I2C_ITR                 _SFR_(0x21A)
#define _I2C_CCRL                _SFR_(0x21B)
#define _I2C_CCRH                _SFR_(0x21C)
#define _I2C_TRISER              _SFR_(0x21D)
#define _I2C_PECR                _SFR_(0x21E)


void i2c_init() {
    _I2C_FREQR = (1 << _I2C_FREQR_FREQ1);
    _I2C_CCRL = 0x0A; // 100kHz
    _I2C_OARH = (1 << _I2C_OARH_ADDMODE); // 7-bit addressing
    _I2C_CR1 = (1 << _I2C_CR1_PE);
}

void i2c_start() {
    _I2C_CR2 |= (1 << _I2C_CR2_START);
    while (!(_I2C_SR1 & (1 << _I2C_SR1_SB)));
}

void i2c_stop() {
    _I2C_CR2 |= (1 << _I2C_CR2_STOP);
    while (_I2C_SR3 & (1 << _I2C_SR3_MSL));
}

void i2c_write(uint8_t data) {
    _I2C_DR = data;
    while (!(_I2C_SR1 & (1 << _I2C_SR1_TXE)));
}

void i2c_write_addr(uint8_t addr) {
    _I2C_DR = addr;
    while (!(_I2C_SR1 & (1 << _I2C_SR1_ADDR)));
    (void) _I2C_SR3; // check BUS_BUSY
    _I2C_CR2 |= (1 << _I2C_CR2_ACK);
}

uint8_t i2c_read() {
    _I2C_CR2 &= ~(1 << _I2C_CR2_ACK);
    i2c_stop();
    while (!(_I2C_SR1 & (1 << _I2C_SR1_RXNE)));
    return _I2C_DR;
}

void i2c_read_arr(uint8_t *buf, int len) {
    while (len-- > 1) {
        _I2C_CR2 |= (1 << _I2C_CR2_ACK);
        while (!(_I2C_SR1 & (1 << _I2C_SR1_RXNE)));
        *(buf++) = _I2C_DR;
    }
    *buf = i2c_read();
}


void hmc5883_get_id(uint8_t *id) {
    uint8_t data;
    i2c_start();
    i2c_write_addr(HMC5883_ADDR + I2C_WRITE);
    i2c_write(HMC5883_ID_REG_A);
    i2c_stop();

    i2c_start();
    i2c_write_addr(HMC5883_ADDR + I2C_READ);
    /* i2c_read_arr(id, 3); */
    data = i2c_read();
    PC_ODR = 0xFF;
    /* printf("Device ID:"); */
    printf("Device ID: %d\n", data);
}

void main() {
    uint8_t id[3];

    PC_DDR = 0xFF;
    PC_CR1 = 0xFF;
    PC_CR2 = 0xFF;

    uart_init();
    i2c_init();
    hmc5883_get_id(id);
    /* printf("Device ID: %c%c%c\n", id[0], id[1], id[2]); */

    while (1) {
        /* measure(); */
        /* delay_ms(250); */
    }
}


void delay(void)
{
    for (int i = 0; i < 30000; i++) {}
}

void uart_init()
{
    uint16_t div = (2000000 + 9600 / 2) / 9600;
    UART_BRR2 = ((div >> 8) & 0xF0) + (div & 0x0F);
    UART_BRR1 = div >> 4;

    /* UART_BRR2 = 0x02;   // 9600 baud */
    /* UART_BRR1 = 0x41; */
    UART_CR1.UARTD = 0; // UART enabled
    UART_CR1.M     = 0; // 8 bit word
    UART_CR1.PCEN  = 0; // No parity
    UART_CR1.PIEN  = 0; // No parity interrupt
    UART_CR2.TEN = 1;   // Transmitter enabled
    UART_CR2.REN = 1;   // Receiver enabled

    /* madness.. */
    /* UART1_BRR2 = ((div >> 8) & 0xF0) + (div & 0x0F); */
    /* UART1_BRR1 = div >> 4; */
    /* /1* enable transmitter and receiver *1/ */
    /* UART1_CR2 = (1 << UART1_CR2_TEN) | (1 << UART1_CR2_REN); */
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
