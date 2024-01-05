#pragma once

#if __cplusplus
extern "C" {
#endif
#include "common.hpp"

#define ENCODER_MODE EncoderMultMode::X4 // encoder mode and resulting resolution X1, X2 or X4
#define ENCODER_COUNTS 192 // Number of counts per revolution of the encoder in the above mode
#define RPM_UPDATE_INTERVAL 50 // Interval in ms between RPM updates. Not done every count to reduce cpu load

// If Z requires a pullup or pulldown, uncomment only ONE of these. If it is to be left floating, comment both out (default).
// the reference hardware uses inexpensive optical encoder boards that have built in pullups and so the input is left floating.
// #define ENCODER_Z_PULLDOWN 1
// #define ENCODER_Z_PULLUP 1
// same for the AB channels. This is configured separately to allow encoders that don't have a Z to emulate it with an additional sensor.
// #define ENCODER_AB_PULLDOWN 1
// #define ENCODER_AB_PULLUP 1


//system stuff, don't change unless you know what you're doing
#define ENCODER_PRIORITY 1 // Interrupt priority for encoder interrupts
#define RPM_PRIORITY 2 // Interrupt priority for RPM update interrupt

#if __cplusplus
}
#endif