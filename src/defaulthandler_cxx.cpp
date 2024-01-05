// #include "debug.hpp"

// // Function to get the currently active interrupt number
// uint32_t getActiveInterruptNumber() {
//     return (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk);
// }

// extern "C" {
// // Default Handler modified for debug purposes
// void Default_Handler_CXX(void) {
//     uint32_t activeInterrupt = getActiveInterruptNumber();

//     DebugLogger& debugLog = DebugLog::instance();
//     // Code to output the active interrupt number
//     // For example, send it over USART or store it in a variable
//     debugLog.Debug(debugLog.Format("Unexpected interrupt:  %d", activeInterrupt).c_str());

//     // Infinite loop to halt execution
//     while (1) {
//     }
// }
// }