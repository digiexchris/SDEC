extern "C" {
    //#include <stm32f1xx.h>
}
#include <stdio.h>
#include <libopencm3/stm32/rcc.h>
#include "encoder.hpp"
#include "encoder.h"
#include "debug.hpp"
#include <etl/string.h>
#include <libopencm3/cm3/nvic.h>
#include "Helpers.hpp"

extern EncoderABZ encoder;
static uint32_t systick_counter = 0;
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
    rcc_clock_setup_in_hse_8mhz_out_72mhz();

    // Wait until PLL is used as the system clock source
    while (rcc_system_clock_source() != RCC_CFGR_SWS_SYSCLKSEL_PLLCLK);
}

void sys_tick_handler(void) {
    systick_counter++;

    Calculate_RPM_Handler_CXX();
}

void DebugOutput() {
    DebugLogger& debugLog = DebugLog::instance();
        etl::string<100> debugOutput;
        DEBUG(debugLog.Format("Position: %lu, Angle: %hu/%hu, RPM: %hu\r\n", 
            encoder.GetFullIndexCounts(), 
            encoder.GetAngularPositionInCounts(), 
            encoder.GetTotalAngularCounts(),
            encoder.GetRpm()
        ).c_str());
}

int main(void) {
    SystemClockInit();
    Helpers::SystickSetup(rcc_ahb_frequency / 1000);
    DebugLogger& debugLog = DebugLog::instance();
    debugLog.Init();
    encoder.Init();

    //shouldn't get ht due to RTOS
    while (1) {

        DebugOutput();
        Helpers::Wait(&systick_counter, 1000);
    }
}
