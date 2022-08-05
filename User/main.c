/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
RCC_ClocksTypeDef RCC_Clocks;
uint16_t ad_value;

/* Private function prototypes -----------------------------------------------*/
static void Delay(__IO uint32_t nTime);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void) {
    NVIC_SetPriorityGrouping(NVIC_PriorityGroup_4);
    //此处系统时钟已经初始化
    RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
    SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);/* SysTick end of count event each 1ms */
    SystemCoreClockUpdate();
    init_cycle_counter(true);
    delay_ms(50);/* Insert 50 ms delay */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); /* Enable the GPIOA peripheral */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);/* Enable the GPIOACperipheral */
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

    while (1) {
        ad_value = ad_getValue();
        dap_uart_debug_printf("ad value is %d, ", ad_value);
        dap_uart_debug_printf("VBus is %.2f V\n", ad_value*8.675f*3.3f/4096);
        delay_ms(1000);
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


