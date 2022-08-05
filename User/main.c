/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "drv8303_soft_spi.h"
#include "uart.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t uwTimingDelay;
RCC_ClocksTypeDef RCC_Clocks;

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
    GPIO_InitTypeDef GPIO_InitStructure;
    //此处系统时钟已经初始化
    RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);/* SysTick end of count event each 1ms */
    Delay(50);/* Insert 50 ms delay */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); /* Enable the GPIOA peripheral */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);/* Enable the GPIOACperipheral */

    softspi_init();   //  初始化PC10 11 12 PD2为 drv8303spi通信用的引脚
    onboard_uart1_init(); // * 初始化UART1，板载daplink上的cdc串口
    dap_uart_printf("****************************\nSTM32F401RC_FOC_Driver is running! \n");
    dap_uart_printf("SYSCLK_Frequency = %lu \n", RCC_Clocks.SYSCLK_Frequency);
    dap_uart_printf("HCLK_Frequency = %lu \n", RCC_Clocks.HCLK_Frequency);
    dap_uart_printf("PCLK1_Frequency = %lu \n", RCC_Clocks.PCLK1_Frequency);
    dap_uart_printf("PCLK2_Frequency = %lu \n", RCC_Clocks.PCLK2_Frequency);

    volatile uint16_t ret = drv8303IDread();
    dap_uart_printf("ID = 0x%02X \n", (uint8_t) (ret & 0xff));
    Delay(1000);
    ret = drv8303_reg_test();
    if (ret)
        dap_uart_printf("寄存器错误\n");
    else
        dap_uart_printf("drv8303工作正常\n");

    /* Infinite loop */

    while (1) {

    }
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void Delay(__IO uint32_t nTime) {
    uwTimingDelay = nTime;

    while (uwTimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void) {
    if (uwTimingDelay != 0x00) {
        uwTimingDelay--;
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


