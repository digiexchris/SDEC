
#include "encoder.h"
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>

void exti15_10_isr(void) {
    if (exti_get_flag_status(EXTI11)) {
        exti_reset_request(EXTI11); // Clear interrupt flag
        Encoder_EXTI9_5_IRQHandler_CXX();
    }
}

