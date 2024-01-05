#include "debug.hpp"
extern "C" {
    #include <stm32f1xx.h>
}
#include "etl_profile.h"
#include <stdio.h>
#include <core_cm3.h>
#include <stdarg.h>
#include <etl/string.h>

void DebugLogger::Init() {
    USART2_Init();
}

void DebugLogger::USART2_Init(void) {
    // Enable clocks for USART2 and GPIOA
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // Configure PA2 (USART2 TX) and PA3 (USART2 RX)
    GPIOA->CRL &= ~(GPIO_CRL_CNF2 | GPIO_CRL_MODE2 | GPIO_CRL_CNF3 | GPIO_CRL_MODE3);
    GPIOA->CRL |= GPIO_CRL_CNF2_1 | GPIO_CRL_MODE2_0; // USART2 TX
    GPIOA->CRL |= GPIO_CRL_CNF3_0 | GPIO_CRL_MODE3_0; // USART2 RX

    // USART2 configuration
    USART2->BRR = 72000000 / 9600;  // Assuming 72MHz PCLK1 for USART2
    USART2->CR1 |= USART_CR1_TE | USART_CR1_RE; // Enable TX and RX
    USART2->CR1 |= USART_CR1_UE; // Enable USART2
}

void DebugLogger::Debug(const char* str, const char* func, uint16_t line) {
    SendString(Format( "%s:%d: %s\r\n", func, line, str).c_str());
}

void DebugLogger::Debug(etl::string<100> str, const char* func, uint16_t line) {
    Debug(str.c_str(), func, line);
}

void DebugLogger::SendChar(const char ch) {
    while (!(USART2->SR & USART_SR_TXE)); // Wait until TX is empty
    USART2->DR = ch;
}

void DebugLogger::SendString(const char* str) {
    while (*str) {
        SendChar(*str++);
    }
}

etl::string<100> DebugLogger::Format(const char* str, ...) {
    char buf[100];
    va_list args;
    va_start(args, str);
    vsnprintf(buf, sizeof(buf), str, args); // Safer alternative
    va_end(args);
    return etl::string<100>(buf);
}