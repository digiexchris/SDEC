#include "inc/EntryPoint.hpp"
#include "main.h"
#include <etl/string.h>
#include <etl/format_spec.h>
#include <etl/string_stream.h>
#include <FreeRTOS.h>
#include <task.h>
#include <cmsis_os2.h>

extern "C" {
    extern UART_HandleTypeDef huart2;
}

void EntryPoint_Start()
{
	while (1)
	{
		/* USER CODE END WHILE */
		//encoderValue = TIM2->CNT;
		etl::string<50> text;
		etl::string_stream stream(text);

		stream << "\r              \rCount:" << etl::setprecision(3) << 3.1415;
		
		HAL_UART_Transmit(&huart2, (uint8_t *)stream.str().c_str(), stream.str().length(), HAL_MAX_DELAY);

		osDelay(1000*portTICK_PERIOD_MS);
		//HAL_Delay(1000000);
		/* USER CODE BEGIN 3 */
	}
}