#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/exti.h>

#include "config.hpp"
#include "encoder.h"
#include "encoder.hpp"
#include "Helpers.hpp"

#ifndef ENCODER_Z_PULLDOWN
    #ifndef ENCODER_Z_PULLUP
        #define ENCODER_Z_FLOATING
    #endif
#endif

#ifndef ENCODER_AB_PULLDOWN
    #ifndef ENCODER_AB_PULLUP
        #define ENCODER_AB_FLOATING
    #endif
#endif

EncoderABZ encoder;

EncoderABZ::EncoderABZ() {
    zCount = UINT32_MAX/2; //right in the middle, so we don't have to test the overflow just yet
    isInitialized = false;
    elapsedTime = 0;
    lastZCount = UINT32_MAX/2;
    rpm = 0;
}

void EncoderABZ::Init() {
    ABTimer_Encoder_Init_16bit();
    Z_Init_32bit();
    //SysTick_Init();
    isInitialized = true;
}

bool EncoderABZ::IsInitialized() {
    return isInitialized;
}

uint16_t EncoderABZ::GetRpm(void) {
    return rpm;
}

uint16_t EncoderABZ::GetAngularPositionInCounts() {
    return timer_get_counter(TIM2);
}

uint16_t EncoderABZ::GetTotalAngularCounts() {
    return ENCODER_COUNTS;
}

uint32_t EncoderABZ::GetFullIndexCounts() {
    return (zCount);
}

void EncoderABZ::UpdateIndex() {
    // Check the direction of TIM2
    int direction = Helpers::TimerGetDirection(TIM2);

    if (direction) {
        // TIM2 is counting down
        timer_set_counter(TIM2, ENCODER_COUNTS); // Reset TIM2 count to maximum (assuming ENCODER_COUNTS is the max count)
        zCount--;
    } else {
        // TIM2 is counting up
        timer_set_counter(TIM2, 0); // Reset TIM2 count to 0
        zCount++;
    }
}

//RPM calculator
void EncoderABZ::UpdateRPM(void) {
    elapsedTime++; // Increment elapsed time every millisecond

    static uint32_t lastTime = 0;
    if (elapsedTime - lastTime >= RPM_UPDATE_INTERVAL) { // Calculate RPM every RPM_UPDATE_INTERVAL milliseconds
        uint32_t currentCount = GetFullIndexCounts();
        int32_t countDiff = currentCount - lastZCount;
        rpm = (countDiff * 60) / ENCODER_COUNTS; // Calculate RPM
        lastZCount = currentCount;
        lastTime = elapsedTime;
    }
}


//Init Functions

void EncoderABZ::ABTimer_Encoder_Init_16bit() {
    // Enable clock for TIM2 and GPIOA
    rcc_periph_clock_enable(RCC_TIM2);
    rcc_periph_clock_enable(RCC_GPIOA);

    #ifdef ENCODER_AB_PULLUP
    gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO0 | GPIO);
    gpio_set(GPIOb, GPIO6);  // Set to pull-up
    #endif

    #ifdef ENCODER_AB_PULLDOWN
    gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO0 | GPIO);
    gpio_clear(GPIOB, GPIO6);  // Set to pull-down
    #endif

    #ifdef ENCODER_AB_FLOATING
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO0 | GPIO1);
    #endif

    // TIM2 configuration for encoder mode
    timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
    timer_ic_set_input(TIM2, TIM_IC1, TIM_IC_IN_TI1);
    timer_ic_set_input(TIM2, TIM_IC2, TIM_IC_IN_TI2);

    switch (ENCODER_MODE) {
        case EncoderMultMode::X1:
            timer_slave_set_mode(TIM2, TIM_SMCR_SMS_EM1);
            break;
        case EncoderMultMode::X2:
            timer_slave_set_mode(TIM2, TIM_SMCR_SMS_EM3);
            break;
        case EncoderMultMode::X4:
            timer_slave_set_mode(TIM2, TIM_SMCR_SMS_EM3);
            timer_ic_set_polarity(TIM2, TIM_IC1, TIM_IC_RISING);
            timer_ic_set_polarity(TIM2, TIM_IC2, TIM_IC_RISING);
            break;
    }

    timer_enable_counter(TIM2);
}

// 32 bit counter for Z signal
void EncoderABZ::Z_Init_32bit(void) {

    rcc_periph_clock_enable(RCC_GPIOB); // Assuming GPIOB is used for the Z signal
    rcc_periph_clock_enable(RCC_AFIO);  // Enable Alternate Function clock in case it happens to be used for the pin
    #ifdef ENCODER_Z_PULLUP
    gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO11);
    gpio_set(GPIOB, GPIO11);  // Set to pull-up
    #endif

    #ifdef ENCODER_Z_PULLDOWN
    gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO11);
    gpio_clear(GPIOB, GPIO11);  // Set to pull-down
    #endif

    #ifdef ENCODER_Z_FLOATING
    gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO11);
    #endif

    nvic_enable_irq(NVIC_EXTI15_10_IRQ); // Enable NVIC interrupt for EXTI line 15 to 10
    exti_select_source(EXTI11, GPIOB);   // Select PB11 as the source for EXTI11
    exti_set_trigger(EXTI11, EXTI_TRIGGER_FALLING); // Trigger on falling edge (or rising, or both)
    exti_enable_request(EXTI11);         // Enable EXTI11
}

    void Encoder_EXTI9_5_IRQHandler_CXX() {
        if (encoder.IsInitialized()) {
            encoder.UpdateIndex();
        }
    }

    void Calculate_RPM_Handler_CXX() {
        if (encoder.IsInitialized()) {
            encoder.UpdateRPM();
        }
    }