## gsm library 
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
* Add gsm and atc library to your project.
* Configure `gsmConfig.h` and `atcConfig.h` files.
* Add 'gsm_rxCallback()' to selected usart interrupt.
* Call `gsm_init()`.
* Call `gsm_loop()` in infinit loop.

* None RTOS example:
```
#include "gsm.h"

int main()
{
  gsm_init();
  gsm_waitForRegister(30);
  while (1)
  {
    gsm_loop();
  }  
}
```



