extern "C" {
    #include <stm32f1xx.h>
}
#include <stdio.h>
#include <core_cm3.h>
#include "encoder.hpp"
#include "debug.hpp"
#include "cmsis_os2.h"
#include <etl/string.h>

extern EncoderABZ encoder;

/**
 * Pinout:
 * PA0 - TIM2 CH1 (Encoder input)
 * PA1 - TIM2 CH2 (Encoder input)
 * 
 * PB6 - TIM4 CH1 (Z signal input)
 * 
 * PA2 - USART2 TX //debugging output, goes to a serial UART RX
 * PA3 - USART2 RX //Currently unused
 * 
 * This file:
 * 1. Initializes USART2 for debugging output
 * 2. Initializes TIM2 as an encoder counter
 * 3. Initializes TIM3 as a 32-bit counter
 * 4. Initializes TIM4 as an input capture timer for the Z signal
*/\

void SystemClockInit(void) {
    // Enable HSE (High-Speed External) oscillator
    RCC->CR |= RCC_CR_HSEON;

    // Wait until HSE is ready
    while (!(RCC->CR & RCC_CR_HSERDY)) {}

    // Set the PLL multiplication factor to 9 (8 MHz * 9 = 72 MHz)
    RCC->CFGR |= RCC_CFGR_PLLMULL9;

    // Set PLL source to HSE
    RCC->CFGR |= RCC_CFGR_PLLSRC;

    // Enable PLL
    RCC->CR |= RCC_CR_PLLON;

    // Wait until PLL is ready
    while (!(RCC->CR & RCC_CR_PLLRDY)) {}

    // Set AHB prescaler to 1
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

    // Set APB1 prescaler to 2 (36 MHz max for APB1)
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;

    // Set APB2 prescaler to 1
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;

    // Select PLL as the system clock source
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    // Wait until PLL is used as the system clock source
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {}

    // Update the SystemCoreClock variable
    SystemCoreClockUpdate();
}

void DebugOutputTask(void *pvParameters) {
    DebugLogger& debugLog = DebugLog::instance();
    while (1) {
        etl::string<100> debugOutput;
        debugLog.Debug(debugLog.Format("Position: %lu, Angle: %hu/%hu, RPM: %hu\r\n", 
            encoder.GetFullIndexCounts(), 
            encoder.GetAngularPositionInCounts(), 
            encoder.GetTotalAngularCounts(),
            encoder.GetRpm()
        ).c_str());

        osDelay(1000); // Delay for 1000 ms
    }
}


int main(void) {
    SystemClockInit();
    DebugLogger& debugLog = DebugLog::instance();
    debugLog.Init();
    encoder.Init();

    osKernelInitialize(); // Initialize CMSIS-RTOS

    // Create the task
    osThreadAttr_t debugOutputTask_attributes = {};
    debugOutputTask_attributes.name = "Debug-UART-Output";
    debugOutputTask_attributes.stack_size = 128 * 4;  // Adjust stack size as needed
    debugOutputTask_attributes.priority = (osPriority_t) osPriorityNormal;

    osThreadNew(DebugOutputTask, NULL, &debugOutputTask_attributes);

    osKernelStart(); // Start thread execution


    //shouldn't get ht due to RTOS
    while (1) {
        
        

        
        for (int i = 0; i < 1000000; i++); // Delay
    }
}
