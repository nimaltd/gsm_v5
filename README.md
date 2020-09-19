# Sim800 series library. Please use http://github.com/nimaltd/gsm
[![ko-fi](https://www.ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/O5O4221XY)
* http://www.github.com/NimaLTD   
* https://www.instagram.com/github.nimaltd/   
* https://www.youtube.com/channel/UCUhY7qY1klJm1d2kulr9ckw   

I use Stm32f407vg and Keil Compiler and Stm32CubeMX wizard.
* Enable FreeRTOS.  
* Config your usart and enable RX interrupt and TX DMA on CubeMX.
* If you want Turn On By Microcontroller, One control Pin needed. (PowerKey>>>>output,open drain,default to SET)  
* PowerKey connect to Sim800 Power Key.(if Needed,See Sim80xConfig.h)
* Select "General peripheral Initalizion as a pair of '.c/.h' file per peripheral" on project settings.
* Config your Sim80xConfig.h file.  
* Add Sim80x_RxCallBack() on usart interrupt routin. 
* call  Sim80x_Init(osPriorityLow) on your app.   
* Config your app on Sim80xUser.c,GsmUser.c,BlutoothUser.c,GprsUser.c.   

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
