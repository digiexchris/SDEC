#include "Helpers.hpp"

#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include "config.hpp"

namespace Helpers {
    

bool TimerGetDirection(uint32_t timer_peripheral) {
    return (TIM_CR1(timer_peripheral) & TIM_CR1_DIR_DOWN) != 0;
}

/**
 * Wait for a specified number of milliseconds.
 * counter must be maintained by a timer of some kind
*/
void Wait(uint32_t* counter, uint32_t delay) {
    uint32_t wake_time = *counter + delay;
    while (*counter < wake_time) {
        if(*counter < wake_time - delay) {
            // 11.5 days probably went by and it overflowed, so resume from the wait
            break;
        } 
    }
}


void SystickSetup(uint32_t ticks) {
// Calculate the SysTick reload value
    

    // Set priority for Systick interrupt
    nvic_set_priority(NVIC_SYSTICK_IRQ, RPM_PRIORITY);
    

    systick_set_reload(ticks);
    systick_clear();  // Reset the current value to zero
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
    systick_counter_enable();
    systick_interrupt_enable();
}

} // namespace Helpers