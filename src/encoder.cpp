#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>

#include "config.hpp"
#include "encoder.h"
#include "encoder.hpp"
#include "Helpers.hpp"

EncoderABZ encoder;

EncoderABZ::EncoderABZ() {
    upperCounter = 0;
    isInitialized = false;
    elapsedTime = 0;
    lastEncoderCount = 0;
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
    uint32_t counterValue;
    uint16_t lowerCounter;

    do {
        counterValue = upperCounter;
        lowerCounter = timer_get_counter(TIM3);
    } while (counterValue != upperCounter); // Handle ISR update during read

    return (counterValue << 16) | lowerCounter;
}

void EncoderABZ::UpdateIndex() {
    // Uncomment the following line if you need to read the captured value from TIM4
    // uint32_t capturedValue = timer_get_counter(TIM4); // Read captured value

    // Check the direction of TIM2
    int direction = Helpers::TimerGetDirection(TIM2);

    if (direction) {
        // TIM2 is counting down
        timer_set_counter(TIM2, ENCODER_COUNTS); // Reset TIM2 count to maximum (assuming ENCODER_COUNTS is the max count)
        timer_set_counter(TIM4, timer_get_counter(TIM4) - 1); // Decrement TIM4 count
    } else {
        // TIM2 is counting up
        timer_set_counter(TIM2, 0); // Reset TIM2 count to 0
        timer_set_counter(TIM4, timer_get_counter(TIM4) + 1); // Increment TIM4 count
    }
}

//Handle the upper 16 bits of the counter
void EncoderABZ::UpdateEncoderUpperCount(void) {
    if (timer_get_flag(TIM3, TIM_SR_UIF)) { // Check update event flag
    timer_clear_flag(TIM3, TIM_SR_UIF); // Clear update event flag

    // Check the direction of TIM2
    int direction = Helpers::TimerGetDirection(TIM2);

    if (direction == TIM_CR1_DIR_DOWN) {
        // TIM2 is counting down
        if (upperCounter > 0) {
            upperCounter--;
        } else {
            upperCounter = 0xFFFF; // Rollover to maximum value for 16-bit counter
        }
    } else {
        // TIM2 is counting up
        upperCounter++;
    }
}

}

//RPM calculator
void EncoderABZ::UpdateRPM(void) {
    elapsedTime++; // Increment elapsed time every millisecond

    static uint32_t lastTime = 0;
    if (elapsedTime - lastTime >= RPM_UPDATE_INTERVAL) { // Calculate RPM every RPM_UPDATE_INTERVAL milliseconds
        uint32_t currentCount = GetFullIndexCounts();
        int32_t countDiff = currentCount - lastEncoderCount;
        rpm = (countDiff * 60) / ENCODER_COUNTS; // Calculate RPM
        lastEncoderCount = currentCount;
        lastTime = elapsedTime;
    }
}


//Init Functions


void EncoderABZ::SysTick_Init() {
    
}

void EncoderABZ::ABTimer_Encoder_Init_16bit() {
    // Enable clock for TIM2 and GPIOA
    rcc_periph_clock_enable(RCC_TIM2);
    rcc_periph_clock_enable(RCC_GPIOA);

    // Configure PA0 and PA1 for TIM2 CH1 and CH2 (Encoder inputs)
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO0 | GPIO1);

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

//TIM3 and TIM4 are being combined for a 32 bit timer
void EncoderABZ::Z_Init_32bit(void) {
    // Enable clock for TIM3 and TIM4, and GPIOB (assuming PB6 for Z signal)
    rcc_periph_clock_enable(RCC_TIM3);
    rcc_periph_clock_enable(RCC_TIM4);
    rcc_periph_clock_enable(RCC_GPIOB);

    // Configure TIM3 as a basic up-counter
    //timer_reset(TIM3);
    timer_set_mode(TIM3, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
    timer_enable_irq(TIM3, TIM_DIER_UIE); // Enable update event interrupt (overflow)
    timer_enable_counter(TIM3);

    // Configure PB6 as input (Z signal input)
    gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO6);

    // Configure TIM4 in input capture mode
    //timer_reset(TIM4);
    timer_set_mode(TIM4, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
    timer_ic_set_input(TIM4, TIM_IC1, TIM_IC_IN_TI1);
    timer_ic_enable(TIM4, TIM_IC1);
    timer_enable_irq(TIM4, TIM_DIER_CC1IE); // Enable the capture/compare 1 interrupt
    timer_enable_counter(TIM4);

    // NVIC configuration
    nvic_set_priority(NVIC_TIM3_IRQ , ENCODER_PRIORITY);
    nvic_enable_irq(NVIC_TIM3_IRQ );

    nvic_set_priority(NVIC_TIM4_IRQ, ENCODER_PRIORITY);
    nvic_enable_irq(NVIC_TIM4_IRQ);
}

    void TIM4_IRQHandler_CXX() {
        if (timer_get_flag(TIM4, TIM_SR_CC1IF)) {
            // Capture event occurred
            timer_clear_flag(TIM4, TIM_SR_CC1IF); // Clear the capture/compare interrupt flag
            if (encoder.IsInitialized()) {
                encoder.UpdateIndex();
            }
        }
    }

    void TIM3_IRQHandler_CXX() {
        if (timer_get_flag(TIM3, TIM_SR_UIF)) {
            // Update event occurred
            timer_clear_flag(TIM3, TIM_SR_UIF); // Clear the update interrupt flag
            if (encoder.IsInitialized()) {
                encoder.UpdateEncoderUpperCount();
            }
        }
    }

    void Calculate_RPM_Handler_CXX() {
        if (encoder.IsInitialized()) {
            encoder.UpdateRPM();
        }
    }