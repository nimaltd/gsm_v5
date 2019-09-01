# Sim800 series library
#### I hope use it and enjoy.
#### I use Stm32f407vg and Keil Compiler and Stm32CubeMX wizard.
 <br />
Please Do This ...
<br />
<br />
1) Enable FreeRTOS.  
<br />
2) Config your usart and enable RX interrupt and TX DMA on CubeMX.
<br />
3) If you want Turn On By Microcontroller, One control Pin needed. (PowerKey>>>>output,open drain,default to SET) 
<br />
PowerKey connect to Sim800 Power Key.(if Needed,See Sim80xConfig.h)
<br />
4) Select "General peripheral Initalizion as a pair of '.c/.h' file per peripheral" on project settings.
<br />
5) Config your Sim80xConfig.h file.
<br />
6) Add Sim80x_RxCallBack() on usart interrupt routin. 
<br />
7) call  Sim80x_Init(osPriorityLow) on your app.
<br />
8) Config your app on Sim80xUser.c,GsmUser.c,BlutoothUser.c,GprsUser.c.

```
#include "Sim80x.h"
.
.
.
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  Sim80x_RxCallBack();
}
.
.
.
/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{
  Sim80x_Init(osPriorityLow);
  osDelay(10000);
  Gsm_MsgSendText("+98911xxxxx,"test msg\r\n");
  for(;;)
  {
    osDelay(10);
  }


```
