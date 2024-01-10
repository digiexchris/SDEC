#pragma once
#include <stdint.h>
namespace Helpers {

    
    /**
     * Get the current count direction of a timer.
     * @param timer_peripheral The timer peripheral (e.g., TIM2, TIM3, etc.).
     * @return true if the timer is counting down, false if counting up.
     */
    bool TimerGetDirection(uint32_t timer_peripheral);

    void Wait(uint32_t* counter, uint32_t delay);

    void SystickSetup(uint32_t ticks);

    // uint32_t GetPCLK1Frequency() {
    //     SystemCoreClockUpdate();
    //     uint32_t pclk1_div_factor = 1;
    //     uint32_t hclk_freq = SystemCoreClock; // Assuming SystemCoreClock is updated with the current HCLK frequency

    //     // Read the APB1 prescaler value
    //     uint32_t apb1_prescaler = (RCC->CFGR & RCC_CFGR_PPRE1) >> 8;

    //     // Decode the prescaler value to get the division factor
    //     if (apb1_prescaler & 0x4) { // If the MSB of PPRE1 is set
    //         pclk1_div_factor = 2 << (apb1_prescaler & 0x3); // Shift by the two LSBs
    //     }

    //     // Calculate the PCLK1 frequency
    //     return hclk_freq / pclk1_div_factor;
    // }

} // namespace Helpers