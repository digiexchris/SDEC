#include "encoder.hpp"
#include "main.hpp"

EncoderABZ& encoder;

void TIM4_IRQHandler(void) {
    TIM4_IRQHandler_CXX(encoder);
}

void TIM3_IRQHandler(void) {
    TIM3_IRQHandler_CXX(encoder);
}

void SysTick_Handler(void) {
    SysTick_Handler_CXX(encoder);
}