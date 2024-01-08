#pragma once

#if __cplusplus
extern "C" {
#endif
#include "common.hpp"

#define ENCODER_MODE EncoderMultMode::X4 // encoder mode and resulting resolution X1, X2 or X4
#define ENCODER_COUNTS 192 // Number of counts per revolution of the encoder in the above mode
#define RPM_UPDATE_INTERVAL 1000 // Interval in ms between RPM updates


//system stuff, don't change unless you know what you're doing
#define ENCODER_PRIORITY 1 // Interrupt priority for encoder interrupts
#define RPM_PRIORITY 2 // Interrupt priority for RPM update interrupt

#if __cplusplus
}
#endif