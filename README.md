# Sim800 series library
<br />
I hope use it and enjoy.
<br />
I use Stm32f407vg and Keil Compiler and Stm32CubeMX wizard.
 <br />
Please Do This ...
<br />
<br />
1) Enable FreeRTOS  
<br />
2) Config your usart and enable RX interrupt and TX DMA on CubeMX (115200 bps )
<br />
3) One control Pin needed. (PowerKey>>>>output,open drain,default to SET) 
<br />
PowerKey connect to Sim800 Power Key.
<br />
4) Select "General peripheral Initalizion as a pair of '.c/.h' file per peripheral" on project settings.
<br />
5) Config your Sim80xConfig.h file.
<br />
6) Add Sim80x_RxCallBack() on usart interrupt routin. 
<br />
7) call  Sim80x_Init(osPriorityLow) on your app.
<br />
8) Config your app on GsmUser.c and BlutoothUser.c.

