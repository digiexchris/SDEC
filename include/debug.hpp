#pragma once

#include <etl/string.h>
#include <etl/singleton.h>
#define DEBUG(msg) DebugLog::instance().Debug(msg, __FILE__, __LINE__, __FUNCTION__)
class DebugLogger {
public:
    // Singleton access through etl::singleton<DebugLogger>::instance()
    DebugLogger() {
        Init(); // Call Init inside the constructor
    }
    void Init(void);
    void Debug(const char* str, const char* file, uint16_t line, const char* func);
    void Debug(etl::string<100> str,  const char* file, uint16_t line, const char* func);
    etl::string<100> Format(const char* str, ...);

private:
    
    void SendString(const char* str);
    void USART2_Init(void);
};

using DebugLog = etl::singleton<DebugLogger>;