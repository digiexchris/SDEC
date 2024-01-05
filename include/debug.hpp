#pragma once
extern "C" {
    #include <stm32f1xx.h>
}

#include <etl/string.h>
#include <etl/singleton.h>

class DebugLogger {
public:
// Singleton access through etl::singleton
    
    void Init(void);
    void Debug(const char* str, const char* func = __FUNCTION__ , uint16_t line = __LINE__);
    void Debug(etl::string<100> str,  const char* func = __FUNCTION__ , uint16_t line = __LINE__);
    etl::string<100> Format(const char* str, ...);

private:
    // Private constructor
    DebugLogger() {
        Init(); // Call Init inside the constructor
    }

    // Other private methods
    void SendChar(const char ch);
    void SendString(const char* str);
    void USART2_Init(void);
};

using DebugLog = etl::singleton<DebugLogger>;