#include <stdint.h>

void SystemInit(void) {
}

#define RCC_APB2ENR   (*(volatile uint32_t*)0x40021018)
#define GPIOA_CRL     (*(volatile uint32_t*)0x40010800)
#define GPIOA_ODR     (*(volatile uint32_t*)0x4001080C)
#define SPI1_CR1      (*(volatile uint32_t*)0x40013000)
#define SPI1_SR       (*(volatile uint32_t*)0x40013008)
#define SPI1_DR       (*(volatile uint32_t*)0x4001300C)

#define CS_PIN   4
#define DC_PIN   1
#define RES_PIN  0

#define RCC_APB2ENR_SPI1EN    (1 << 12)
#define RCC_APB2ENR_IOPAEN    (1 << 2)

#define SPI_CR1_CPHA          (1 << 0)
#define SPI_CR1_CPOL          (1 << 1)
#define SPI_CR1_MSTR          (1 << 2)
#define SPI_CR1_BR_2          (1 << 5)
#define SPI_CR1_SSI           (1 << 8)
#define SPI_CR1_SSM           (1 << 9)
#define SPI_CR1_SPE           (1 << 6)

#define SPI_SR_TXE            (1 << 1)
#define SPI_SR_RXNE           (1 << 0)

void delay(uint32_t count) {
    for(volatile uint32_t i = 0; i < count; i++);
}

void SPI1_Init(void) {
    RCC_APB2ENR |= RCC_APB2ENR_SPI1EN | RCC_APB2ENR_IOPAEN;
    
    GPIOA_CRL |= (1 << 0) | (1 << 4) | (1 << 16);
    GPIOA_CRL |= (0xB << 20) | (0xB << 28);
    
    SPI1_CR1 |= SPI_CR1_CPHA | SPI_CR1_CPOL | SPI_CR1_MSTR | 
                SPI_CR1_BR_2 | SPI_CR1_SSI | SPI_CR1_SSM | SPI_CR1_SPE;
    
    GPIOA_ODR |= (1 << CS_PIN) | (1 << RES_PIN);
}

void SPI1_Write(uint8_t data) {
    while(!(SPI1_SR & SPI_SR_TXE));
    SPI1_DR = data;
}

uint8_t SPI1_Read(void) {
    SPI1_DR = 0xFF;
    while(!(SPI1_SR & SPI_SR_RXNE));
    return SPI1_DR;
}

void display_cmd(uint8_t cmd) {
    GPIOA_ODR &= ~(1 << CS_PIN);
    GPIOA_ODR &= ~(1 << DC_PIN);
    SPI1_Write(cmd);
    GPIOA_ODR |= (1 << CS_PIN);
}

void display_data(uint8_t data) {
    GPIOA_ODR &= ~(1 << CS_PIN);
    GPIOA_ODR |= (1 << DC_PIN);
    SPI1_Write(data);
    GPIOA_ODR |= (1 << CS_PIN);
}

void display_init(void) {
    GPIOA_ODR &= ~(1 << RES_PIN);
    delay(10000);
    GPIOA_ODR |= (1 << RES_PIN);
    delay(10000);
    
    display_cmd(0xAE);
    display_cmd(0x20); display_cmd(0x00);
    display_cmd(0x21); display_cmd(0x00); display_cmd(0x7F);
    display_cmd(0x22); display_cmd(0x00); display_cmd(0x07);
    display_cmd(0x8D); display_cmd(0x14);
    display_cmd(0xAF);
}

void display_chessboard(void) {
    for(uint8_t page = 0; page < 8; page++) {
        display_cmd(0xB0 + page);
        display_cmd(0x00);
        display_cmd(0x10);
        
        for(uint8_t col = 0; col < 128; col++) {
            if(((col / 8) + page) % 2 == 0) {
                display_data(0xFF);
            } else {
                display_data(0x00);
            }
        }
    }
}

int main(void) {
    SPI1_Init();
    display_init();
    display_chessboard();
    
    while(1) {
    }
}