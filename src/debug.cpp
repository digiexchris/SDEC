#include "debug.hpp"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include "helpers.hpp"
#include "etl_profile.h"
#include <stdio.h>
#include <stdarg.h>
#include <etl/string.h>


void DebugLogger::Init() {
    USART2_Init();
    SendString("DebugLogger initialized\r\n");
}

void DebugLogger::USART2_Init(void) {
    rcc_periph_clock_enable(RCC_USART2);
    rcc_periph_clock_enable(RCC_GPIOA);

    // Configure PA2 (USART2 TX) and PA3 (USART2 RX)
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART2_TX);
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
                  GPIO_CNF_INPUT_FLOAT, GPIO_USART2_RX);

    // USART2 configuration
    usart_set_baudrate(USART2, 9600); // Assuming 72MHz PCLK1 for USART2
    usart_set_databits(USART2, 8);
    usart_set_stopbits(USART2, USART_STOPBITS_1);
    usart_set_mode(USART2, USART_MODE_TX_RX);
    usart_set_parity(USART2, USART_PARITY_NONE);
    usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);

    // Enable USART2
    usart_enable(USART2);
}

void DebugLogger::Debug(const char* str, const char* func, uint16_t line) {
    SendString(Format( "%s:%d: %s\r\n", func, line, str).c_str());
}

void DebugLogger::Debug(etl::string<100> str, const char* func, uint16_t line) {
    Debug(str.c_str(), func, line);
}

void DebugLogger::SendString(const char* str) {
    while (*str) {
        usart_send_blocking(USART2, *str++); // Send current char and move to next
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