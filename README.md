## gsm module library for STM32 LL
*	Author:     Nima Askari
*	WebSite:    https://www.github.com/NimaLTD
*	Instagram:  https://www.instagram.com/github.NimaLTD
*	LinkedIn:   https://www.linkedin.com/in/NimaLTD
*	Youtube:    https://www.youtube.com/channel/UCUhY7qY1klJm1d2kulr9ckw 
--------------------------------------------------------------------------------
* This library using my 'atc' library, please download 'http://github.com/nimaltd/atc'.
--------------------------------------------------------------------------------
* [x] NONE RTOS Supported.
* [x] RTOS V1 Supported.
* [x] RTOS V2 Supported.
--------------------------------------------------------------------------------
* [x] SIM800C tested.
* [ ] SIM800 tested.
* [ ] SIM800H tested.
--------------------------------------------------------------------------------   
* Enable USART (LL Library) and RX interrupt.
* Enable a gpio as output and open drain to connect gsm power button.
* Add gsm and atc library to your project.
* Configure `gsmConfig.h` and `atcConfig.h` files.
* Add 'gsm_rxCallback()' to selected usart interrupt.
* Call `gsm_init()`.
* Call `gsm_loop()` in infinit loop.
* If using FREERTOS, please create a task for gsm with at least 512 word heap size. 
--------------------------------------------------------------------------------
## None RTOS example:
* file atcConfig.h
``` 
#define	_ATC_DEBUG            0       //  use printf debug
#define	_ATC_RTOS             0       //  0: no rtos    1: cmsis_os v1    2: cmsis_os v2
#define	_ATC_RXSIZE           1024    //  at-command rx buffer size
#define	_ATC_SEARCH_CMD_MAX   5       //  maximum of answer in at-command
#define	_ATC_SEARCH_MAX       10      //  maximum	of always search in buffer
#define	_ATC_RXTIMEOUT_MS     50      //  rx timeout to get new packet
```
* file main.c   
```
#include "gsm.h"

int main()
{
  gsm_init();
  gsm_power(true);
  gsm_waitForRegister(30);
  gsm_msg_send("+98xxxxxxx", "TEST MSG 1");
  while (1)
  {
    gsm_loop();
  }  
}
```
--------------------------------------------------------------------------------
## RTOS example:
* file atcConfig.h
```
#define	_ATC_DEBUG            0       //  use printf debug
#define	_ATC_RTOS             1       //  0: no rtos    1: cmsis_os v1    2: cmsis_os v2
#define	_ATC_RXSIZE           1024    //  at-command rx buffer size
#define	_ATC_SEARCH_CMD_MAX   5       //  maximum of answer in at-command
#define	_ATC_SEARCH_MAX       10      //  maximum	of always search in buffer
#define	_ATC_RXTIMEOUT_MS     50      //  rx timeout to get new packet
```
* file main.c   
```
#include "gsm.h"

int main()
{
  ...  
}

void task_gsm(void const * argument)
{
  gsm_init();
  gsm_power(true);
  while (1)
  {
    gsm_loop();
  }
}

void task_other(void const * argument)
{
  gsm_waitForRegister(30);
  gsm_msg_send("+98xxxxxxx", "TEST MSG 1");
  while (1)
  {    
    osDelay(10000);
  }
}


```



