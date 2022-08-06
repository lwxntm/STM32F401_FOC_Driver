/* Includes ------------------------------------------------------------------*/
#include <memory.h>
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
RCC_ClocksTypeDef RCC_Clocks;
int64_t tStart;
int32_t tEnd;
extern uint16_t ad_value[2];

/* Private function prototypes -----------------------------------------------*/
static void Delay(__IO uint32_t nTime);

/* Private functions ---------------------------------------------------------*/

float convert_res_temp_NCP18XH103F03RB(uint32_t res_value) {
    if (res_value > 27219)
        return 0.0f;
    if (res_value < 27219 && res_value > 22021)
        return (27219 - res_value) / (27219.0f - 22021.0f) * 5 + 0;
    if (res_value < 22021 && res_value > 17926)
        return (22021 - res_value) / (22021.0f - 17926.0f) * 5 + 5;
    if (res_value < 17926 && res_value > 14674)
        return (17926 - res_value) / (17926.0f - 14674.0f) * 5 + 10;
    if (res_value < 14674 && res_value > 12081)
        return (14674 - res_value) / (14674.0f - 12081.0f) * 5 + 15;
    if (res_value < 12081 && res_value > 10000)
        return (12081 - res_value) / (12081.0f - 10000.0f) * 5 + 20;
    if (res_value < 10000 && res_value > 8315)
        return (10000 - res_value) / (10000.0f - 8315.0f) * 5 + 25;
    if (res_value < 8315 && res_value > 6948)
        return (8315 - res_value) / (8315.0f - 6948.0f) * 5 + 30;
    return 100.0f;
}


int main(void) {
    NVIC_SetPriorityGrouping(NVIC_PriorityGroup_4);
    //此处系统时钟已经初始化
    RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
    SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);/* SysTick end of count event each 1ms */
    init_cycle_counter(true);
    delay_ms(50);/* Insert 50 ms delay */
    /**************************************************************************************/
    onboard_uart1_init(); // * 初始化UART1，板载daplink上的cdc串口
    dap_uart_debug_printf("****************************\nSTM32F401RC_FOC_Driver is running! \n");
    dap_uart_debug_printf("SYSCLK_Frequency = %lu MHz \n", RCC_Clocks.SYSCLK_Frequency / 1000000);
    dap_uart_debug_printf("HCLK_Frequency = %lu MHz \n", RCC_Clocks.HCLK_Frequency / 1000000);
    dap_uart_debug_printf("PCLK1_Frequency = %lu MHz \n", RCC_Clocks.PCLK1_Frequency / 1000000);
    dap_uart_debug_printf("PCLK2_Frequency = %lu MHz \n", RCC_Clocks.PCLK2_Frequency / 1000000);

    softspi_init();   //  初始化PC10 11 12 PD2为 drv8303spi通信用的引脚
    drv8303_reg_test();
    onboard_adc_init();
    /* Infinite loop */
    //ad_getValue();

    while (1) {
        do {
            tStart = get_system_ticks();
            float vbus_value_v = ad_value[0] * 0.0069911251073576f;
            float temp_board_c = convert_res_temp_NCP18XH103F03RB(
                    (3.3f * 4700 / (float) (ad_value[1] * 3.3 / 4095) - 4700));
            just_float_transmit(vbus_value_v, temp_board_c);
            tEnd = ((int32_t) (get_system_ticks() - tStart));
            // dap_uart_debug_printf("总线电压: %.2f V,主板温度: %.2f °C \n ", vbus_value_v, temp_board_c);
            // dap_uart_debug_printf("------------- cost ticks: %d, time: %d us\n", tEnd, tEnd / 120);
        } while (0);
        delay_us(100);
    }
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


