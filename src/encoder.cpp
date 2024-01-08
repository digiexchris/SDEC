

#include <stm32f1xx.h>

#include "config.hpp"
#include "encoder.h"
#include "encoder.hpp"

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
    return TIM2->CNT;
}

uint16_t EncoderABZ::GetTotalAngularCounts() {
    return ENCODER_COUNTS;
}

uint32_t EncoderABZ::GetFullIndexCounts() {
    uint32_t counterValue;
    uint16_t lowerCounter;

    do {
        counterValue = upperCounter;
        lowerCounter = TIM3->CNT;
    } while (counterValue != upperCounter); // Handle ISR update during read

    return (counterValue << 16) | lowerCounter;
}

void EncoderABZ::UpdateIndex() {
        //uint32_t capturedValue = TIM4->CCR1; // Read captured value (not used in this example)

        // Check the direction of TIM2
        int direction = TIM2->CR1 & TIM_CR1_DIR;

        if (direction) {
            // TIM2 is counting down
            TIM2->CNT = ENCODER_COUNTS; // Reset TIM2 count to 0
            TIM4->CNT--; // Decrement TIM4 count
        } else {
            // TIM2 is counting up
            TIM2->CNT = 0; // Reset TIM2 count to COUNTS
            TIM4->CNT++; // Increment TIM4 count
        }
}

//Handle the upper 16 bits of the counter
void EncoderABZ::UpdateEncoderUpperCount(void) {
    if (TIM3->SR & TIM_SR_UIF) { // Check update event flag
        TIM3->SR &= ~TIM_SR_UIF; // Clear update event flag

        // Check the direction of TIM2
        int direction = TIM2->CR1 & TIM_CR1_DIR;
        
        if (direction) {
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
    SystemCoreClockUpdate(); // Update the SystemCoreClock variable
    SysTick_Config(SystemCoreClock / 1000); // Configure SysTick to generate an interrupt every 1 ms
    NVIC_SetPriority(SysTick_IRQn, RPM_PRIORITY); // Set SysTick interrupt priority (higher priority)
}

void EncoderABZ::ABTimer_Encoder_Init_16bit() {
    // Enable clock for TIM2 and GPIOA
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // Configure PA0 and PA1 for TIM2 CH1 and CH2 (Encoder inputs)
    GPIOA->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_MODE0 | GPIO_CRL_CNF1 | GPIO_CRL_MODE1);
    GPIOA->CRL |= GPIO_CRL_CNF0_1 | GPIO_CRL_CNF1_1; // Alternate function input

    // TIM2 configuration for encoder mode
    TIM2->CCMR1 = TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0; // CC1 and CC2 configured as inputs

switch (ENCODER_MODE) {
    case EncoderMultMode::X1:
        TIM2->SMCR = TIM_SMCR_SMS_1; // Encoder mode 1
        break;
    case EncoderMultMode::X2:
        TIM2->SMCR = TIM_SMCR_SMS_1 | TIM_SMCR_SMS_0; // Encoder mode 3
        break;
    case EncoderMultMode::X4:
        TIM2->SMCR = TIM_SMCR_SMS_1 | TIM_SMCR_SMS_0; // Encoder mode 3
        TIM2->CCER |= TIM_CCER_CC1P | TIM_CCER_CC2P; // Capture on both edges
        break;
    default:
    //TODO handle this error
        break;
    }

    TIM2->CR1 |= TIM_CR1_CEN; // Enable TIM2
}

//TIM3 and TIM4 are being combined for a 32 bit timer
void EncoderABZ::Z_Init_32bit(void) {
    // Enable clock for TIM3
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    // Configure TIM3 as a basic up-counter
    TIM3->CR1 |= TIM_CR1_CEN; // Enable TIM3
    TIM3->DIER |= TIM_DIER_UIE; // Enable update event interrupt (overflow)

    // Initialize TIM3->CNT to the lower 16 bits of the midpoint
    TIM3->CNT = 0x0000; // Lower 16 bits of 2147483648

    // Enable clock for TIM4 and GPIOB (assuming PB6 for Z signal)
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

    // Configure PB6 as input (Z signal input)
    GPIOB->CRL &= ~(GPIO_CRL_CNF6 | GPIO_CRL_MODE6);
    GPIOB->CRL |= GPIO_CRL_CNF6_1; // Alternate function input

    // Configure TIM4 in input capture mode
    TIM4->CCMR1 |= TIM_CCMR1_CC1S_0; // CC1 channel is configured as input, IC1 is mapped on TI1
    TIM4->CCER |= TIM_CCER_CC1E; // Enable capture from IC1
    TIM4->DIER |= TIM_DIER_CC1IE; // Enable the capture/compare 1 interrupt

    // Initialize upperCounter to the upper 16 bits of the midpoint
    upperCounter = 0x8000; // Upper 16 bits of 214748364

    TIM4->CR1 |= TIM_CR1_CEN; // Enable TIM4

    // Do interrupty things
    NVIC_SetPriority(TIM3_IRQn, ENCODER_PRIORITY);
    NVIC_SetPriority(TIM4_IRQn, ENCODER_PRIORITY);
    NVIC_EnableIRQ(TIM3_IRQn);
    NVIC_EnableIRQ(TIM4_IRQn);
}

    void TIM4_IRQHandler_CXX() {
        if (TIM4->SR & TIM_SR_CC1IF) {
             // Capture event occurred
             TIM4->SR &= ~TIM_SR_CC1IF; // Clear the capture/compare interrupt flag
            if(encoder.IsInitialized()){
                encoder.UpdateIndex();
            } else {

            }
            
        }   
    }

    void TIM3_IRQHandler_CXX() {
        if (TIM3->SR & TIM_SR_UIF) { // Check update event flag
            TIM3->SR &= ~TIM_SR_UIF; // Clear update event flag
            if(encoder.IsInitialized()){
                encoder.UpdateEncoderUpperCount();
            }
        }
    }

    void SysTick_Handler_CXX() {
        if(encoder.IsInitialized()){
            encoder.UpdateRPM();
        }
    }