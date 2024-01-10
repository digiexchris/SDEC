
#include "encoder.h"

void tim4_isr(void) {
    TIM4_IRQHandler_CXX();
}

void tim3_isr(void) {
    TIM3_IRQHandler_CXX();
}

