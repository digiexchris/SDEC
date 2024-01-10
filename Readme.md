# Stm32 Dedicated Encoder Counter (SDEC)
Many Electronic Lead Screw projects struggle with timing reading the encoder input and generating stepper steps with the correct timing without ever missing either an encoder pulse, or a stepper pulse.

## Current functionality
- Configurable only at compile time and not dynamic:
    - Encoder mode (X1, X2, X4)
    - PPR
    - Debugging output over USART2

## How to use
Precompiled binaries are in TODO and can be flashed using TODO with an SWD, JTAG, or UART dongle.

## STM32F103C8T6 Pinout Chart
| Peripheral     | Function      | Pin  | Connects To |
|----------------|---------------|------|-------------|
| Debug (USART)  | TX            | PA2  | Serial RX |
| Unused         | RX            | PA3  | NC |
| TIM2           | CH1 (Encoder) | PA0  | ENC_A |
| TIM2           | CH2 (Encoder) | PA1  | ENC_B |
| TIM4           | CH1 (Z signal input) | PB6  | ENC_Z |
| GND            | GND            | GND | GND|

NOTE: ENC_A and ENC_B may be reversed if you want to change the direction reported.

## This project aims to 
- offload the encoder portion of that equasion onto dedicated quadrature decoding hardware, reducing cpu load for the main system
- always keep an internal count, regardless of what is happening to the main system
- leverage hardware that is designed for encoder counting
- provide optional additional data, such as RPM
- recover automatically from slight positioning errors by leveraging the Z signal
- We can optionally cause a GPIO output to go high when the index interrupt or any of the positions change, allowing the main system to be informed of a count.
- Be reliable at at least 15,000 RPM in 4x mode for a 600PPR encoder

# Advantages

- If something in the main system causes the position read to be late, no position data is lost. The current position including any late positions will be sent the next time the MCU can read it.
- The STM32F1 can be placed very close to the encoder, keeping the impact of environmental noise to a minimum
- We can communicate to the main MCU with a noise resistant or error correcting protocol that works better at longer cable lengths than the capacitance in long cables for the high speed encoder would allow
- Provides a count of how many full revolutions the encoder has done, plus the current angular position within the current revolution, reducing the size of number the main system would have to store.

# How it works

The STM32F1 has timers that can be put into a quadrature encoder mode. TIM2 is used for the ENC_A and ENC_B channels of a standard quadrature encoder. TIM3 is in input capture mode for an additional index signal, such as povided by an ENC_Z if the encoder is equipped, or in my case, a simple hall sensor and schmidt trigger and a magnet. The index is expected once per revolution.

One spindle rotation can easily be counted within a single uint32, but we would have to handle rollover within about 40 minutes of running if using a 40 tooth encoder wheel. To solve this, SDEC uses uint16 for one full rotation, and combines TIM3 and TIM4 to make a single 32 bit counter for full rotations.

This allows us to make a few assumptions about what's happening:

- If our TIM2 position is near the maximum for 1 revolution, and we get an index pulse, we can assume that we lost some steps. Reset the timer to the next known position.
- If our TIM2 position wraps back to zero before we recieve an index pulse, we gained some steps somehow. Reset the timer to 0 again.
- RPM can be calculated off of the index pulse, reducing the cpu load handling a timing loop for RPM calculations
- It's possible to switch from X4 mode to X2 or X1 mode dynamically based on RPM if we're tracking RPM separately with a Z index signal
- We could possibly algorithmically determine the number of pulses per rotation at first startup by watching TIM2's count through few activations of the index signal, 
- We can calculate RPM and direction and send that to the MCU with the position data

# Calculations used for maximum encoder pulse rates

Using a 600 PPR encoder, there are 4 total edges for 1 complete pulse. So in 4x encoder mode, there are 2400 counts per revolution. Considering an STM32F103C8T6's maximum PCLK1 frequency of 38MHz, we should be able to reliably detect in the neighborhood of 20,000 rpm with a 600 PPR encoder.

If you routinely operate near or above that, or are seeing less performance due to the real world never matching theroetical limits, you can try 2x, 1x, or turning down the filter to a lower value. See config.h for details.

Maximum Timer Frequency Considering Nyquist Limit:
The Nyquist limit for a 36 MHz timer clock is half of that, so 18 MHz.

Calculations Considering Nyquist Limit:

No Filter:
Maximum countable frequency in 4x mode = 18 MHz / 4 = 4.5 MHz (since each pulse is counted four times in 4x mode).
Each revolution gives 600 PPR * 4 counts = 2400 counts.
Max RPM = (4.5 MHz / 2400 counts) * 60 = (4.5 * 10^6 / 2400) * 60 ≈ 112,500 RPM.

With N2 Filter:
With N2 filter, the effective frequency is further halved due to the 2-cycle validation delay.
Max countable frequency with N2 filter = 4.5 MHz / 2 = 2.25 MHz.
Max RPM with N2 = (2.25 MHz / 2400 counts) * 60 ≈ 56,250 RPM.

With N4 Filter:
With N4 filter, the effective frequency is quartered.
Max countable frequency with N4 filter = 4.5 MHz / 4 = 1.125 MHz.
Max RPM with N4 = (1.125 MHz / 2400 counts) * 60 ≈ 28,125 RPM.
