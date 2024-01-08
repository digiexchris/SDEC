extern "C" {
    #include <stm32f1xx.h>
}
#include <stdio.h>
#include <core_cm3.h>
#include "encoder.hpp"
#include "debug.hpp"

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
*/

int main(void) {

    DebugLogger& debugLog = DebugLog::instance();
    encoder.Init();

    etl::string<100> debugOutput;

    while (1) {
        
        debugLog.Debug(debugLog.Format("Position: %lu, Angle: %lu/%lu, RPM: %lu\r\n", encoder.GetFullIndexCounts(),encoder.GetAngularPositionInCounts(), encoder.GetRpm()).c_str());
        for (int i = 0; i < 1000000; i++); // Delay
    }
}
