
#include "Sim80X.h"
#include "Sim80XConfig.h"

Sim80x_t      Sim80x;
osThreadId 		Sim80xTaskHandle;
osThreadId 		Sim80xBuffTaskHandle;
void 	        StartSim80xTask(void const * argument);
void 	        StartSim80xBuffTask(void const * argument);
//######################################################################################################################
//######################################################################################################################
//######################################################################################################################
void	Sim80x_SendString(char *str)
{
  HAL_UART_Receive_IT(&_SIM80X_USART,&Sim80x.UsartRxTemp,1);	
  #if (_SIM80X_DMA_TRANSMIT==1)
	while(_SIM80X_USART.hdmatx->State != HAL_DMA_STATE_READY)
		osDelay(10);
	HAL_UART_Transmit_DMA(&_SIM80X_USART,(uint8_t*)str,strlen(str));
	while(_SIM80X_USART.hdmatx->State != HAL_DMA_STATE_READY)
		osDelay(10);
  #else
 	HAL_UART_Transmit(&_SIM80X_USART,(uint8_t*)str,strlen(str),100);
  osDelay(10);
  #endif
}
//######################################################################################################################
void  Sim80x_SendRaw(uint8_t *Data,uint16_t len)
{
  HAL_UART_Receive_IT(&_SIM80X_USART,&Sim80x.UsartRxTemp,1);	
  #if (_SIM80X_DMA_TRANSMIT==1)
	while(_SIM80X_USART.hdmatx->State != HAL_DMA_STATE_READY)
		osDelay(10);
	HAL_UART_Transmit_DMA(&_SIM80X_USART,Data,len);
	while(_SIM80X_USART.hdmatx->State != HAL_DMA_STATE_READY)
		osDelay(10);
  #else
	HAL_UART_Transmit(&_SIM80X_USART,Data,len,100);
  osDelay(10);
  #endif

}
//######################################################################################################################
void	Sim80x_RxCallBack(void)
{
  if((Sim80x.Status.DataTransferMode==0)&&(Sim80x.UsartRxTemp!=0))
  {
    Sim80x.UsartRxLastTime = HAL_GetTick();
    Sim80x.UsartRxBuffer[Sim80x.UsartRxIndex] = Sim80x.UsartRxTemp;
    if(Sim80x.UsartRxIndex < (_SIM80X_BUFFER_SIZE-1))
      Sim80x.UsartRxIndex++;
  }
  else if(Sim80x.Status.DataTransferMode==1)
  {
    Sim80x.UsartRxLastTime = HAL_GetTick();
    Sim80x.UsartRxBuffer[Sim80x.UsartRxIndex] = Sim80x.UsartRxTemp;
    if(Sim80x.UsartRxIndex < (_SIM80X_BUFFER_SIZE-1))
      Sim80x.UsartRxIndex++;
  }
	HAL_UART_Receive_IT(&_SIM80X_USART,&Sim80x.UsartRxTemp,1);	
}
//######################################################################################################################
uint8_t     Sim80x_SendAtCommand(char *AtCommand,int32_t  MaxWaiting_ms,uint8_t HowMuchAnswers,...)
{
  while(Sim80x.Status.Busy == 1)
  {
    osDelay(100);
  }
  Sim80x.Status.Busy = 1;
  Sim80x.AtCommand.FindAnswer = 0;
  Sim80x.AtCommand.ReceiveAnswerExeTime=0;
  Sim80x.AtCommand.SendCommandStartTime = HAL_GetTick();
  Sim80x.AtCommand.ReceiveAnswerMaxWaiting = MaxWaiting_ms;
  memset(Sim80x.AtCommand.ReceiveAnswer,0,sizeof(Sim80x.AtCommand.ReceiveAnswer));
  va_list tag;
	va_start (tag,HowMuchAnswers);
	char *arg[HowMuchAnswers];
	for(uint8_t i=0; i<HowMuchAnswers ; i++)
	{
    arg[i] = va_arg (tag, char *);	
    strncpy(Sim80x.AtCommand.ReceiveAnswer[i],arg[i],sizeof(Sim80x.AtCommand.ReceiveAnswer[0]));
  }
  va_end (tag);		  
  strncpy(Sim80x.AtCommand.SendCommand,AtCommand,sizeof(Sim80x.AtCommand.SendCommand));            
  Sim80x_SendString(Sim80x.AtCommand.SendCommand); 
  while( MaxWaiting_ms > 0)
  {
    osDelay(10);
    if(Sim80x.AtCommand.FindAnswer > 0)
      return Sim80x.AtCommand.FindAnswer;    
    MaxWaiting_ms-=10;
  }
  memset(Sim80x.AtCommand.ReceiveAnswer,0,sizeof(Sim80x.AtCommand.ReceiveAnswer));
  Sim80x.Status.Busy=0;
  return Sim80x.AtCommand.FindAnswer;
}

//######################################################################################################################
//######################################################################################################################
//######################################################################################################################
void  Sim80x_InitValue(void)
{
  Sim80x_SendAtCommand("ATE1\r\n",200,1,"ATE1\r\r\nOK\r\n");
  Sim80x_SendAtCommand("AT+COLP=1\r\n",200,1,"AT+COLP=1\r\r\nOK\r\n");
  Sim80x_SendAtCommand("AT+CLIP=1\r\n",200,1,"AT+CLIP=1\r\r\nOK\r\n");
  Sim80x_SendAtCommand("AT+FSHEX=0\r\n",200,1,"AT+FSHEX=0\r\r\nOK\r\n");
  Gsm_MsgSetMemoryLocation(GsmMsgMemory_OnModule);
  Gsm_MsgSetFormat(GsmMsgFormat_Text);
  Gsm_MsgSetTextModeParameter(17,167,0,0);
  Gsm_MsgGetCharacterFormat();
  Gsm_MsgGetFormat();
  if(Sim80x.Gsm.MsgFormat != GsmMsgFormat_Text)
    Gsm_MsgSetFormat(GsmMsgFormat_Text);
  Gsm_MsgGetServiceNumber();
  Gsm_MsgGetTextModeParameter();
  Sim80x_GetIMEI(NULL);
  Sim80x_GetLoadVol();
  Sim80x_GetRingVol();
  Sim80x_GetMicGain();
  Sim80x_GetToneVol();
  #if (_SIM80X_USE_BLUETOOTH==1)
  Bluetooth_SetAutoPair(true);
  #endif
  Sim80x_SendAtCommand("AT+CREG?\r\n",200,1,"\r\n+CREG:");  
  Sim80x_UserInit();
}
//######################################################################################################################
void   Sim80x_SaveParameters(void)
{
  Sim80x_SendAtCommand("AT&W\r\n",1000,1,"AT&W\r\r\nOK\r\n");
  #if (_SIM80X_DEBUG==1)
  printf("\r\Sim80x_SaveParameters() ---> OK\r\n");
  #endif
}
//######################################################################################################################
void  Sim80x_SetPower(bool TurnOn)
{ 
  if(TurnOn==true)
  {  
    if(Sim80x_SendAtCommand("AT\r\n",200,1,"AT\r\r\nOK\r\n") == 1)
    {
      osDelay(100);
      #if (_SIM80X_DEBUG==1)
      printf("\r\nSim80x_SetPower(ON) ---> OK\r\n");
      #endif
      Sim80x.Status.Power=1;
      Sim80x_InitValue();
    }
    else
    {     
      #if (_SIM80X_USE_POWER_KEY==1)  
      HAL_GPIO_WritePin(_SIM80X_POWER_KEY_GPIO,_SIM80X_POWER_KEY_PIN,GPIO_PIN_RESET);
      osDelay(1200);
      HAL_GPIO_WritePin(_SIM80X_POWER_KEY_GPIO,_SIM80X_POWER_KEY_PIN,GPIO_PIN_SET);
      #endif
      osDelay(3000);  
      if(Sim80x_SendAtCommand("AT\r\n",200,1,"AT\r\r\nOK\r\n") == 1)
      {
        osDelay(10000);
        #if (_SIM80X_DEBUG==1)
        printf("\r\nSim80x_SetPower(ON) ---> OK\r\n");
        #endif
        Sim80x.Status.Power=1;
        Sim80x_InitValue();   
      }
      else
        Sim80x.Status.Power=0;
    }
  }
  else
  {
    if(Sim80x_SendAtCommand("AT\r\n",200,1,"AT\r\r\nOK\r\n") == 1)
    {
      #if (_SIM80X_DEBUG==1)
      printf("\r\nSim80x_SetPower(OFF) ---> OK\r\n");
      #endif
      Sim80x.Status.Power=0;
      #if (_SIM80X_USE_POWER_KEY==1) 
      HAL_GPIO_WritePin(_SIM80X_POWER_KEY_GPIO,_SIM80X_POWER_KEY_PIN,GPIO_PIN_RESET);
      osDelay(1200);
      HAL_GPIO_WritePin(_SIM80X_POWER_KEY_GPIO,_SIM80X_POWER_KEY_PIN,GPIO_PIN_SET);
      #endif
      #if (_SIM80X_USE_POWER_KEY==0)
      Sim80x_SendAtCommand("AT+CPOWD=1\r\n",2000,1,"\r\nOK\r\n"); 
      #endif      
    }
  }  
}
//######################################################################################################################
void Sim80x_SetFactoryDefault(void)
{
  Sim80x_SendAtCommand("AT&F0\r\n",1000,1,"AT&F0\r\r\nOK\r\n");
  #if (_SIM80X_DEBUG==1)
  printf("\r\nSim80x_SetFactoryDefault() ---> OK\r\n");
  #endif
}
//######################################################################################################################
void  Sim80x_GetIMEI(char *IMEI)
{
  Sim80x_SendAtCommand("AT+GSN\r\n",1000,1,"\r\nOK\r\n");
  #if (_SIM80X_DEBUG==1)
  printf("\r\nSim80x_GetIMEI(%s) <--- OK\r\n",Sim80x.IMEI);
  #endif
}
//######################################################################################################################
uint8_t  Sim80x_GetRingVol(void)
{
  uint8_t answer;
  answer=Sim80x_SendAtCommand("AT+CRSL?\r\n",1000,2,"\r\nOK\r\n","\r\n+CME ERROR:");
  if(answer==1)
  {
    #if (_SIM80X_DEBUG==1)
    printf("\r\nSim80x_GetRingVol(%d) <--- OK\r\n",Sim80x.RingVol);
    #endif    
    return Sim80x.RingVol;
  }
  else
  {
    #if (_SIM80X_DEBUG==1)
    printf("\r\nSim80x_GetRingVol() <--- ERROR\r\n");
    #endif    
    return 0;
  }
}
//######################################################################################################################
bool  Sim80x_SetRingVol(uint8_t Vol_0_to_100)
{
  uint8_t answer;
  char str[16];
  snprintf(str,sizeof(str),"AT+CRSL=%d\r\n",Vol_0_to_100);
  answer=Sim80x_SendAtCommand(str,1000,2,"\r\nOK\r\n","\r\n+CME ERROR:");
  if(answer==1)
  {
    Sim80x.RingVol=Vol_0_to_100;
    #if (_SIM80X_DEBUG==1)
    printf("\r\nSim80x_SetRingVol(%d) ---> OK\r\n",Sim80x.RingVol);
    #endif    
    return true;
  }
  else
  {
    #if (_SIM80X_DEBUG==1)
    printf("\r\nSim80x_SetRingVol(%d) ---> ERROR\r\n",Sim80x.RingVol);
    #endif    
    return false;
  }
}
//######################################################################################################################
uint8_t  Sim80x_GetLoadVol(void)
{
  uint8_t answer;
  answer=Sim80x_SendAtCommand("AT+CLVL?\r\n",1000,2,"\r\nOK\r\n","\r\n+CME ERROR:");
  if(answer==1)
  {
    #if (_SIM80X_DEBUG==1)
    printf("\r\nSim80x_GetLoadVol(%d) <--- OK\r\n",Sim80x.LoadVol);
    #endif    
    return Sim80x.LoadVol;
  }
  else
  {
    #if (_SIM80X_DEBUG==1)
    printf("\r\nSim80x_GetLoadVol() <--- ERROR\r\n");
    #endif      
    return 0;  
  }
}
//######################################################################################################################
bool  Sim80x_SetLoadVol(uint8_t Vol_0_to_100)
{
  uint8_t answer;
  char str[16];
  snprintf(str,sizeof(str),"AT+CLVL=%d\r\n",Vol_0_to_100);
  answer=Sim80x_SendAtCommand(str,1000,2,"\r\nOK\r\n","\r\n+CME ERROR:");
  if(answer==1)
  {
    Sim80x.LoadVol=Vol_0_to_100;
    #if (_SIM80X_DEBUG==1)
    printf("\r\nSim80x_SetLoadVol(%d) ---> OK\r\n",Sim80x.LoadVol);
    #endif    
    return true;
  }
  else
  {
    #if (_SIM80X_DEBUG==1)
    printf("\r\nSim80x_SetLoadVol(%d) ---> ERROR\r\n",Sim80x.LoadVol);
    #endif 
    return false;
  }
}
//######################################################################################################################
Sim80xWave_t   Sim80x_WaveGetState(void)
{
  Sim80x_SendAtCommand("AT+CREC?\r\n",1000,1,"\r\n+CREC:");
  #if (_SIM80X_DEBUG==1)
  printf("\r\nSim80x_WaveGetState(%d) ---> OK\r\n",Sim80x.WaveState);
  #endif   
  return Sim80x.WaveState;
}
//######################################################################################################################
bool  Sim80x_WaveRecord(uint8_t ID,uint8_t TimeLimitInSecond)
{
  uint8_t answer;
  char str[32];
  snprintf(str,sizeof(str),"AT+CREC=1,\"C:\\User\\%d.amr\",0,%d\r\n",ID,TimeLimitInSecond);
  answer = Sim80x_SendAtCommand(str,1000,1,"\r\nOK\r\n");
  if(answer == 1)
  {
    Sim80x.WaveState = Sim80xWave_Recording;
    #if (_SIM80X_DEBUG==1)
    printf("\r\nSim80x_WaveRecord(Recording) ---> OK\r\n");
    #endif     
    return true;
  }
  else
  {
    Sim80x.WaveState = Sim80xWave_Idle;
    #if (_SIM80X_DEBUG==1)
    printf("\r\nSim80x_WaveRecord(Recording) ---> ERROR\r\n");
    #endif     
    return false;
  }  
}
//######################################################################################################################
bool  Sim80x_WavePlay(uint8_t ID)
{
  uint8_t answer;
  char str[32];
  snprintf(str,sizeof(str),"AT+CREC=4,\"C:\\User\\%d.amr\",0,%d\r\n",ID,Sim80x.LoadVol);
  answer = Sim80x_SendAtCommand(str,1000,1,"\r\nOK\r\n");
  if(answer == 1)
  {
    #if (_SIM80X_DEBUG==1)
    printf("\r\nSim80x_WavePlay() ---> OK\r\n");
    #endif     
    Sim80x.WaveState = Sim80xWave_Playing;
    return true;
  }
  else
  {
    #if (_SIM80X_DEBUG==1)
    printf("\r\nSim80x_WavePlay() ---> ERROR\r\n");
    #endif     
    Sim80x.WaveState = Sim80xWave_Idle;
    return false;
  }   
}
//######################################################################################################################
bool  Sim80x_WaveStop(void)
{
  uint8_t answer;
  answer = Sim80x_SendAtCommand("AT+CREC=5\r\n",1000,1,"\r\nOK\r\n");
  if(answer == 1)
  {
    #if (_SIM80X_DEBUG==1)
    printf("\r\nSim80x_WaveStop() ---> OK\r\n");
    #endif     
    Sim80x.WaveState = Sim80xWave_Idle;
    return true;
  }
  else
  {
    #if (_SIM80X_DEBUG==1)
    printf("\r\nSim80x_WaveStop() ---> ERROR\r\n");
    #endif     
    return false;
  }   
}
//######################################################################################################################
bool  Sim80x_WaveDelete(uint8_t ID)
{
  uint8_t answer;
  char str[32];
  snprintf(str,sizeof(str),"AT+CREC=3,\"C:\\User\\%d.amr\"\r\n",ID);
  answer = Sim80x_SendAtCommand(str,1000,1,"\r\nOK\r\n");
  if(answer == 1)
  {
    #if (_SIM80X_DEBUG==1)
    printf("\r\nSim80x_WaveDelete(%d.amr) ---> OK\r\n",ID);
    #endif     
    return true;
  }
  else
  {
    #if (_SIM80X_DEBUG==1)
    printf("\r\nSim80x_WaveDelete(%d.amr) ---> ERROR\r\n",ID_1_to_20);
    #endif     
    return false;
  }
}
//######################################################################################################################
bool  Sim80x_SetMicGain(uint8_t Channel_0_to_4,uint8_t Gain_0_to_15)
{
   uint8_t answer;
  char str[32];
  snprintf(str,sizeof(str),"AT+CMIC=%d,%d\r\n",Channel_0_to_4,Gain_0_to_15);
  answer = Sim80x_SendAtCommand(str,1000,1,"\r\nOK\r\n");
  if(answer == 1)
  {
    #if (_SIM80X_DEBUG==1)
    printf("\r\Sim80x_SetMicGain(%d,%d) ---> OK\r\n",Channel_0_to_4,Gain_0_to_15);
    #endif  
    Sim80x_GetMicGain();    
    return true;
  }
  else
  {
    #if (_SIM80X_DEBUG==1)
    printf("\r\Sim80x_SetMicGain(%d,%d) ---> ERROR\r\n",Channel_0_to_4,Gain_0_to_15);
    #endif     
    return false;
  }  
}
//######################################################################################################################
bool  Sim80x_GetMicGain(void)
{
  uint8_t answer;
  answer=Sim80x_SendAtCommand("AT+CMIC?\r\n",1000,2,"\r\nOK\r\n","\r\n+CME ERROR:");
  if(answer==1)
  {
    #if (_SIM80X_DEBUG==1)
    printf("\r\nSim80x_GetMicGain(%d,%d,%d,%d) <--- OK\r\n",Sim80x.MicGainMain,Sim80x.MicGainAux,Sim80x.MicGainMainHandsfree,Sim80x.MicGainAuxHandsfree);   
    #endif    
    return Sim80x.LoadVol;
  }
  else
  {
    #if (_SIM80X_DEBUG==1)
    printf("\r\Sim80x_GetMicGain() <--- ERROR\r\n");
    #endif      
    return 0;  
  }  
}
//######################################################################################################################
bool  Sim80x_TonePlay(Sim80xTone_t Sim80xTone,uint32_t  Time_ms)
{
  uint8_t answer;
  char str[32];
  snprintf(str,sizeof(str),"AT+STTONE=1,%d,%d\r\n",Sim80xTone,Time_ms);
  answer = Sim80x_SendAtCommand(str,1000,2,"\r\nOK\r\n","\r\n+CME ERROR\r\n");
  if(answer == 1)
  {
    #if (_SIM80X_DEBUG==1)
    printf("\r\Sim80x_TonePlay(%d,%d) ---> OK\r\n",Sim80xTone,Time_ms);
    #endif     
    return true;
  }
  else
  {
    #if (_SIM80X_DEBUG==1)
    printf("\r\Sim80x_TonePlay() ---> ERROR\r\n");
    #endif     
    return false;
  }     
}
//######################################################################################################################
bool  Sim80x_ToneStop(void)
{
  uint8_t answer;
  answer=Sim80x_SendAtCommand("AT+STTONE=0\r\n",1000,2,"\r\nOK\r\n","\r\n+CME ERROR:");
  if(answer==1)
  {
    #if (_SIM80X_DEBUG==1)
    printf("\r\Sim80x_ToneStop() <--- OK\r\n"); 
    #endif    
    return true;
  }
  else
  {
    #if (_SIM80X_DEBUG==1)
    printf("\r\Sim80x_ToneStop() <--- ERROR\r\n");
    #endif      
    return false;  
  }  
}
//######################################################################################################################
uint8_t Sim80x_GetToneVol(void)
{
  uint8_t answer;
  answer=Sim80x_SendAtCommand("AT+SNDLEVEL?\r\n",1000,2,"\r\nOK\r\n","\r\n+CME ERROR:");
  if(answer==1)
  {
    #if (_SIM80X_DEBUG==1)
    printf("\r\Sim80x_GetToneVol(%d) <--- OK\r\n",Sim80x.ToneVol);   
    #endif    
    return Sim80x.ToneVol;
  }
  else
  {
    #if (_SIM80X_DEBUG==1)
    printf("\r\Sim80x_GetToneVol() <--- ERROR\r\n");
    #endif      
    return 0;  
  }  
  
}
//######################################################################################################################
bool  Sim80x_SetToneVol(uint8_t Vol_0_to_100)
{
  uint8_t answer;
  char str[32];
  snprintf(str,sizeof(str),"AT+SNDLEVEL=0,%d\r\n",Vol_0_to_100);
  answer = Sim80x_SendAtCommand(str,1000,2,"\r\nOK\r\n","\r\n+CME ERROR\r\n");
  if(answer == 1)
  {
    #if (_SIM80X_DEBUG==1)
    printf("\r\Sim80x_SetToneVol(%d) ---> OK\r\n",Vol_0_to_100);
    #endif 
    Sim80x_GetToneVol();    
    return true;
  }
  else
  {
    #if (_SIM80X_DEBUG==1)
    printf("\r\Sim80x_SetToneVol(%d) ---> ERROR\r\n",Vol_0_to_100);
    #endif     
    return false;
  }     
  
}
//######################################################################################################################
bool  Sim80x_SetRingTone(uint8_t Tone_0_to_19,bool Save)
{
  uint8_t answer;
  char str[32];
  snprintf(str,sizeof(str),"AT+CALS=%d\r\n",Tone_0_to_19);
  answer = Sim80x_SendAtCommand(str,1000,2,"\r\nOK\r\n","\r\n+CME ERROR\r\n");
  if(answer == 1)
  {
    #if (_SIM80X_DEBUG==1)
    printf("\r\Sim80x_SetRingTone(%d) ---> OK\r\n",Tone_0_to_19);
    #endif    
    if(Save==true)
      Sim80x_SaveParameters();
    return true;
  }
  else
  {
    #if (_SIM80X_DEBUG==1)
    printf("\r\Sim80x_SetRingTone(%d) ---> ERROR\r\n",Tone_0_to_19);
    #endif     
    return false;
  }       
} 
//######################################################################################################################
//######################################################################################################################
//######################################################################################################################
void	Sim80x_Init(osPriority Priority)
{
  #if (_SIM80X_USE_POWER_KEY==1)  
  HAL_GPIO_WritePin(_SIM80X_POWER_KEY_GPIO,_SIM80X_POWER_KEY_PIN,GPIO_PIN_SET);
  #else
  osDelay(1000);
  #endif
	memset(&Sim80x,0,sizeof(Sim80x));
	memset(Sim80x.UsartRxBuffer,0,_SIM80X_BUFFER_SIZE);
	HAL_UART_Receive_IT(&_SIM80X_USART,&Sim80x.UsartRxTemp,1);
  osThreadDef(Sim80xTask, StartSim80xTask, Priority, 0, 256);
  Sim80xTaskHandle = osThreadCreate(osThread(Sim80xTask), NULL);
  osThreadDef(Sim80xBuffTask, StartSim80xBuffTask, Priority, 0, 256);
  Sim80xBuffTaskHandle = osThreadCreate(osThread(Sim80xBuffTask), NULL);
  for(uint8_t i=0 ;i<50 ;i++)  
  {
    if(Sim80x_SendAtCommand("AT\r\n",200,1,"AT\r\r\nOK\r\n") == 1)
      break;
    osDelay(200);
  }  
  Sim80x_SetPower(true); 
}
//######################################################################################################################
void  Sim80x_BufferProcess(void)
{
  char      *strStart,*str1,*str2,*str3,tmp_str[16];
  int32_t   tmp_int32_t;
  
  strStart = (char*)&Sim80x.UsartRxBuffer[0];  
  //##################################################
  //+++       Buffer Process
  //##################################################
  str1 = strstr(strStart,"\r\n+CREG:");
  if(str1!=NULL)
  {
    str1 = strchr(str1,',');
    str1++;
    if(atoi(str1)==1)
      Sim80x.Status.RegisterdToNetwork=1;
    else
      Sim80x.Status.RegisterdToNetwork=0;
  }
  //##################################################
  str1 = strstr(strStart,"\r\nCall Ready\r\n");
  if(str1!=NULL)
    Sim80x.Status.CallReady=1;  
  //##################################################
  str1 = strstr(strStart,"\r\nSMS Ready\r\n");
  if(str1!=NULL)
    Sim80x.Status.SmsReady=1;  
  //##################################################
  str1 = strstr(strStart,"\r\n+COLP:");
  if(str1!=NULL)
  {
    Sim80x.Gsm.GsmVoiceStatus = GsmVoiceStatus_MyCallAnswerd;
  }  
  //##################################################
  str1 = strstr(strStart,"\r\n+CLIP:");
  if(str1!=NULL)
  {
    str1 = strchr(str1,'"');
    str1++;
    str2 = strchr(str1,'"');
    strncpy(Sim80x.Gsm.CallerNumber,str1,str2-str1);
    Sim80x.Gsm.HaveNewCall=1;  
  }  
  //##################################################
  str1 = strstr(strStart,"\r\n+CSQ:");
  if(str1!=NULL)
  {
    str1 = strchr(str1,':');
    str1++;
    Sim80x.Status.Signal = atoi(str1);      
  }
  //##################################################
  str1 = strstr(strStart,"\r\n+CBC:");
  if(str1!=NULL)
  {
    str1 = strchr(str1,':');
    str1++;
    tmp_int32_t = atoi(str1);
    if(tmp_int32_t==0)
    {
      Sim80x.Status.BatteryCharging=0;
      Sim80x.Status.BatteryFull=0;
    }
    if(tmp_int32_t==1)
    {
      Sim80x.Status.BatteryCharging=1;
      Sim80x.Status.BatteryFull=0;
    }
    if(tmp_int32_t==2)
    {
      Sim80x.Status.BatteryCharging=0;
      Sim80x.Status.BatteryFull=1;
    }
    str1 = strchr(str1,',');
    str1++;
    Sim80x.Status.BatteryPercent = atoi(str1);
    str1 = strchr(str1,',');
    str1++;
    Sim80x.Status.BatteryVoltage = atof(str1)/1000;      
  }
  //##################################################
  str1 = strstr(strStart,"\r\nBUSY\r\n");
  if(str1!=NULL)
  {
    Sim80x.Gsm.GsmVoiceStatus=GsmVoiceStatus_ReturnBusy;
  }
  //##################################################
  str1 = strstr(strStart,"\r\nNO DIALTONE\r\n");
  if(str1!=NULL)
  {
    Sim80x.Gsm.GsmVoiceStatus=GsmVoiceStatus_ReturnNoDialTone;
  }
  //##################################################
  str1 = strstr(strStart,"\r\nNO CARRIER\r\n");
  if(str1!=NULL)
  {
    Sim80x.Gsm.GsmVoiceStatus=GsmVoiceStatus_ReturnNoCarrier;
  }
  //##################################################
  str1 = strstr(strStart,"\r\nNO ANSWER\r\n");
  if(str1!=NULL)
  {
    Sim80x.Gsm.GsmVoiceStatus=GsmVoiceStatus_ReturnNoAnswer;
  }
  //##################################################  
  str1 = strstr(strStart,"\r\n+CMGS:");
  if(str1!=NULL)
  {
    Sim80x.Gsm.MsgSent=1;
  }  
  //##################################################  
  str1 = strstr(strStart,"\r\n+CPMS:");
  if(str1!=NULL)
  {
    str1 = strchr(str1,':');
    str1++;
    str1++;
    if(*str1 == '"')
    {
      str1 = strchr(str1,',');
      str1++;
    }
    Sim80x.Gsm.MsgUsed = atoi(str1);
    str1 = strchr(str1,',');
    str1++;      
    Sim80x.Gsm.MsgCapacity = atoi(str1);
  }  
  //##################################################  
  str1 = strstr(strStart,"\r\n+CMGR:");
  if(str1!=NULL)
  {
    if(Sim80x.Gsm.MsgFormat == GsmMsgFormat_Text)
    {
      memset(Sim80x.Gsm.Msg,0,sizeof(Sim80x.Gsm.Msg));
      memset(Sim80x.Gsm.MsgDate,0,sizeof(Sim80x.Gsm.MsgDate));
      memset(Sim80x.Gsm.MsgNumber,0,sizeof(Sim80x.Gsm.MsgNumber));
      memset(Sim80x.Gsm.MsgTime,0,sizeof(Sim80x.Gsm.MsgTime));
      tmp_int32_t = sscanf(str1,"\r\n+CMGR: %*[^,],\"%[^\"]\",%*[^,],\"%[^,],%[^+-]%*d\"\r\n%[^\r]s\r\nOK\r\n",Sim80x.Gsm.MsgNumber,Sim80x.Gsm.MsgDate,Sim80x.Gsm.MsgTime,Sim80x.Gsm.Msg);      
      if(tmp_int32_t == 4)
        Sim80x.Gsm.MsgReadIsOK=1;
      else
        Sim80x.Gsm.MsgReadIsOK=0;
    }else if(Sim80x.Gsm.MsgFormat == GsmMsgFormat_PDU)
    {

      
    }    
  }
  //################################################## 
  str1 = strstr(strStart,"\r\n+CRSL:");
  if(str1!=NULL)
  {
    str1 = strchr(str1,':');
    str1++;
    Sim80x.RingVol = atoi(str1);    
  }
  //################################################## 
  str1 = strstr(strStart,"\r\n+CLVL:");
  if(str1!=NULL)
  {
    str1 = strchr(str1,':');
    str1++;
    Sim80x.LoadVol = atoi(str1);    
  }
  //################################################## 
  str1 = strstr(strStart,"\r\n+CMTI:");
  if(str1!=NULL)
  {
    str1 = strchr(str1,',');
    str1++;
    for(uint8_t i=0 ;i<sizeof(Sim80x.Gsm.HaveNewMsg) ; i++)
    {
      if(Sim80x.Gsm.HaveNewMsg[i]==0)
      {
        Sim80x.Gsm.HaveNewMsg[i] = atoi(str1);    
        break;
      }
    }
  }
  //##################################################  
  str1 = strstr(strStart,"\r\n+CSCA:");
  if(str1!=NULL)
  {
    memset(Sim80x.Gsm.MsgServiceNumber,0,sizeof(Sim80x.Gsm.MsgServiceNumber));
    str1 = strchr(str1,'"');
    str1++;
    str2 = strchr(str1,'"');
    strncpy(Sim80x.Gsm.MsgServiceNumber,str1,str2-str1);
  }
  //##################################################  
  str1 = strstr(strStart,"\r\n+CSMP:");
  if(str1!=NULL)
  {
    tmp_int32_t = sscanf(str1,"\r\n+CSMP: %hhd,%hhd,%hhd,%hhd\r\nOK\r\n",&Sim80x.Gsm.MsgTextModeParameterFo,&Sim80x.Gsm.MsgTextModeParameterVp,&Sim80x.Gsm.MsgTextModeParameterPid,&Sim80x.Gsm.MsgTextModeParameterDcs);
  }
  //##################################################  
  str1 = strstr(strStart,"\r\n+CUSD:");
  if(str1!=NULL)
  {
    sscanf(str1,"\r\n+CUSD: 0, \"%[^\r]s",Sim80x.Gsm.Msg);    
    tmp_int32_t = strlen(Sim80x.Gsm.Msg);
    if(tmp_int32_t > 5)
    {
      Sim80x.Gsm.Msg[tmp_int32_t-5] = 0;
    }
  }
  //##################################################  
  str1 = strstr(strStart,"\nAT+GSN\r");
  if(str1!=NULL)
  {
    sscanf(str1,"\nAT+GSN\r\r\n%[^\r]",Sim80x.IMEI);    
  }
  //##################################################  
  str1 = strstr(strStart,"\r\n+CREC: ");
  if(str1!=NULL)
  {
    str1 = strchr(str1,':');
    str1++;
    Sim80x.WaveState = (Sim80xWave_t)atoi(str1);
  }
  //##################################################  
  str1 = strstr(strStart,"\r\n+CMIC: ");
  if(str1!=NULL)
  {
    while(strchr(str1,'(')!=NULL)
    {
      str1 = strchr(str1,'(');
      str1++;
      tmp_int32_t = atoi(str1);
      switch(tmp_int32_t)
      {
        case 0:
          str1 = strchr(str1,',');
          str1++;
          Sim80x.MicGainMain = atoi(str1);
        break;        
        case 1:
          str1 = strchr(str1,',');
          str1++;
          Sim80x.MicGainAux = atoi(str1);
        break;        
        case 2:
          str1 = strchr(str1,',');
          str1++;
          Sim80x.MicGainMainHandsfree = atoi(str1);
        break;        
        case 3:
          str1 = strchr(str1,',');
          str1++;
          Sim80x.MicGainAuxHandsfree = atoi(str1);
        break;        
      }
    }    
  }  
  //##################################################  
  str1 = strstr(strStart,"\r\n+SNDLEVEL:");
  if(str1!=NULL)
  {
    while(strchr(str1,'(')!=NULL)
    {
      str1 = strchr(str1,'(');
      str1++;
      tmp_int32_t = atoi(str1);
      switch(tmp_int32_t)
      {
        case 0:
          str1 = strchr(str1,',');
          str1++;
          Sim80x.ToneVol = atoi(str1);
        break;        
        case 1:
          str1 = strchr(str1,',');
          str1++;
          // ...        
        break;        
      }
    }
  }
  //##################################################  
  
  //##################################################  
  
  //##################################################  
  
  //##################################################  
  
  //##################################################  
  //##################################################   
  //################################################## 
  #if( _SIM80X_USE_BLUETOOTH==1)  
  //################################################## 
  str1 = strstr(strStart,"\r\n+BTHOST:");
  if(str1!=NULL)
  {
    sscanf(str1,"\r\n+BTHOST: %[^,],%[^\r]",Sim80x.Bluetooth.HostName,Sim80x.Bluetooth.HostAddress);    
  }  
  //##################################################  
  str1 = strstr(strStart,"\r\n+BTSTATUS:");
  if(str1!=NULL)
  {
    str1 = strchr(str1,':');
    str1++;
    Sim80x.Bluetooth.Status = (BluetoothStatus_t)atoi(str1);
    
    str3 = strstr(str1,"\r\nOK\r\n");
    CheckAnotherConnectedProfile:
    str2 = strstr(str1,"\r\nC:");
    if((str2 != NULL) && (str2 <str3) && (str2 > str1))
    {
      tmp_int32_t = sscanf(str2,"\r\nC: %hhd,%[^,],%[^,],%[^\r]",&Sim80x.Bluetooth.ConnectedID,Sim80x.Bluetooth.ConnectedName,Sim80x.Bluetooth.ConnectedAddress,tmp_str);       
      if(strcmp(tmp_str,"\"HFP\"")==0)
        tmp_int32_t = BluetoothProfile_HSP_HFP;
      else if(strcmp(tmp_str,"\"HSP\"")==0)
        tmp_int32_t = BluetoothProfile_HSP_HFP;
      else if(strcmp(tmp_str,"\"A2DP\"")==0)
        tmp_int32_t = BluetoothProfile_A2DP;
      else if(strcmp(tmp_str,"\"GAP\"")==0)
        tmp_int32_t = BluetoothProfile_GAP;
      else if(strcmp(tmp_str,"\"GOEP\"")==0)
        tmp_int32_t = BluetoothProfile_GOEP;
      else if(strcmp(tmp_str,"\"OPP\"")==0)
        tmp_int32_t = BluetoothProfile_OPP;
      else if(strcmp(tmp_str,"\"SDAP\"")==0)
        tmp_int32_t = BluetoothProfile_SDAP;
      else if(strcmp(tmp_str,"\"SPP\"")==0)
        tmp_int32_t = BluetoothProfile_SSP;
      else tmp_int32_t = BluetoothProfile_NotSet;
    
      for(uint8_t i=0 ;i<sizeof(Sim80x.Bluetooth.ConnectedProfile) ; i++)
      {
        if(Sim80x.Bluetooth.ConnectedProfile[i]==(BluetoothProfile_t)tmp_int32_t)
          break;
        if(Sim80x.Bluetooth.ConnectedProfile[i]==BluetoothProfile_NotSet)
        {
          Sim80x.Bluetooth.ConnectedProfile[i]=(BluetoothProfile_t)tmp_int32_t;
          break;
        }
      }
      str1+=5;
      goto CheckAnotherConnectedProfile;
    }    
  }  
  //##################################################  
  str1 = strstr(strStart,"\r\n+BTPAIRING:");
  if(str1!=NULL)
  {
    Sim80x.Bluetooth.ConnectedID=0;
    memset(Sim80x.Bluetooth.ConnectedAddress,0,sizeof(Sim80x.Bluetooth.ConnectedAddress));
    memset(Sim80x.Bluetooth.ConnectedName,0,sizeof(Sim80x.Bluetooth.ConnectedName));
    tmp_int32_t = sscanf(str1,"\r\n+BTPAIRING: \"%[^\"]\",%[^,],%[^\r]",Sim80x.Bluetooth.ConnectedName,Sim80x.Bluetooth.ConnectedAddress,Sim80x.Bluetooth.PairingPassword);
    if(tmp_int32_t == 3)
      Sim80x.Bluetooth.ConnectedID = 255;      
  }    
  //##################################################  
  str1 = strstr(strStart,"\r\n+BTPAIR:");
  if(str1!=NULL)
  {
    
  }
  //##################################################  
  str1 = strstr(strStart,"\r\n+BTCONNECT:");
  if(str1!=NULL)
  {
    Sim80x.Bluetooth.ConnectedID=0;
    memset(Sim80x.Bluetooth.ConnectedAddress,0,sizeof(Sim80x.Bluetooth.ConnectedAddress));
    memset(Sim80x.Bluetooth.ConnectedName,0,sizeof(Sim80x.Bluetooth.ConnectedName));
    tmp_int32_t = sscanf(str1,"\r\n+BTCONNECT: %hhd,\"%[^\"]\",%[^,],%[^\r]",&Sim80x.Bluetooth.ConnectedID,Sim80x.Bluetooth.ConnectedName,Sim80x.Bluetooth.ConnectedAddress,tmp_str);
    if(strcmp(tmp_str,"\"HFP\"")==0)
      tmp_int32_t = BluetoothProfile_HSP_HFP;
    else if(strcmp(tmp_str,"\"HSP\"")==0)
      tmp_int32_t = BluetoothProfile_HSP_HFP;
    else if(strcmp(tmp_str,"\"A2DP\"")==0)
      tmp_int32_t = BluetoothProfile_A2DP;
    else if(strcmp(tmp_str,"\"GAP\"")==0)
      tmp_int32_t = BluetoothProfile_GAP;
    else if(strcmp(tmp_str,"\"GOEP\"")==0)
      tmp_int32_t = BluetoothProfile_GOEP;
    else if(strcmp(tmp_str,"\"OPP\"")==0)
      tmp_int32_t = BluetoothProfile_OPP;
    else if(strcmp(tmp_str,"\"SDAP\"")==0)
      tmp_int32_t = BluetoothProfile_SDAP;
    else if(strcmp(tmp_str,"\"SPP\"")==0)
      tmp_int32_t = BluetoothProfile_SSP;
    else tmp_int32_t = BluetoothProfile_NotSet;
    
    for(uint8_t i=0 ;i<sizeof(Sim80x.Bluetooth.ConnectedProfile) ; i++)
    {
      if(Sim80x.Bluetooth.ConnectedProfile[i]==(BluetoothProfile_t)tmp_int32_t)
        break;
      if(Sim80x.Bluetooth.ConnectedProfile[i]==BluetoothProfile_NotSet)
      {
        Sim80x.Bluetooth.ConnectedProfile[i]=(BluetoothProfile_t)tmp_int32_t;
        break;
      }
    }
  }
  //##################################################  
  str1 = strstr(strStart,"\r\n+BTCONNECTING:");
  if(str1!=NULL)
  {
    memset(tmp_str,0,sizeof(tmp_str));
    str1 = strchr(str1,',');
    str1++;
    str2 = strchr(str1+1,'"');
    str2++;
    strncpy(tmp_str,str1,str2-str1);
    if(strcmp(tmp_str,"\"HFP\"")==0)
      Sim80x.Bluetooth.ConnectingRequestProfile = BluetoothProfile_HSP_HFP;
    else if(strcmp(tmp_str,"\"HSP\"")==0)
      Sim80x.Bluetooth.ConnectingRequestProfile = BluetoothProfile_HSP_HFP;
    else if(strcmp(tmp_str,"\"A2DP\"")==0)
      Sim80x.Bluetooth.ConnectingRequestProfile = BluetoothProfile_A2DP;
    else if(strcmp(tmp_str,"\"GAP\"")==0)
      Sim80x.Bluetooth.ConnectingRequestProfile = BluetoothProfile_GAP;
    else if(strcmp(tmp_str,"\"GOEP\"")==0)
      Sim80x.Bluetooth.ConnectingRequestProfile = BluetoothProfile_GOEP;
    else if(strcmp(tmp_str,"\"OPP\"")==0)
      Sim80x.Bluetooth.ConnectingRequestProfile = BluetoothProfile_OPP;
    else if(strcmp(tmp_str,"\"SDAP\"")==0)
      Sim80x.Bluetooth.ConnectingRequestProfile = BluetoothProfile_SDAP;
    else if(strcmp(tmp_str,"\"SPP\"")==0)
      Sim80x.Bluetooth.ConnectingRequestProfile = BluetoothProfile_SSP;
    else Sim80x.Bluetooth.ConnectingRequestProfile = BluetoothProfile_NotSet;
  }
  //##################################################  
  str1 = strstr(strStart,"\r\n+BTDISCONN:");
  if(str1!=NULL)
  {
    Sim80x.Bluetooth.ConnectedID=0;    
    memset(Sim80x.Bluetooth.ConnectedProfile,0,sizeof(Sim80x.Bluetooth.ConnectedProfile));
    memset(Sim80x.Bluetooth.ConnectedAddress,0,sizeof(Sim80x.Bluetooth.ConnectedAddress));
    memset(Sim80x.Bluetooth.ConnectedName,0,sizeof(Sim80x.Bluetooth.ConnectedName));    
    Sim80x.Bluetooth.NeedGetStatus=1;
  }
  //##################################################  
  str1 = strstr(strStart,"\r\n+BTVIS:");
  if(str1!=NULL)
  {
    str1 = strchr(str1,':');
    str1++;
    Sim80x.Bluetooth.Visibility=atoi(str1);
  }
  //##################################################  
  str1 = strstr(strStart,"\r\n+BTSPPDATA:");
  if(str1!=NULL)
  {
    str1 = strchr(str1,',');
    str1++;
    tmp_int32_t = atoi(str1);
    str1 = strchr(str1,',');
    str1++;
    strncpy(Sim80x.Bluetooth.SPPBuffer,str1,tmp_int32_t);    
    Sim80x.Bluetooth.SPPLen = tmp_int32_t;
  }  
  //##################################################  
  
  //##################################################  
  
  //##################################################  
  
  //##################################################  
  
  
  //##################################################  
  #endif
  //##################################################  
  //##################################################  
  //##################################################
  #if (_SIM80X_USE_GPRS==1)
  //##################################################  
  str1 = strstr(strStart,"\r\n+CGDCONT:");
  if(str1!=NULL)
  {
    
  }  
  //##################################################  
  str1 = strstr(strStart,"\r\n+CGQMIN:");
  if(str1!=NULL)
  {
    
  }    
  //##################################################  
  str1 = strstr(strStart,"\r\n+CGQREQ:");
  if(str1!=NULL)
  {
    
  }    
  //##################################################  
  str1 = strstr(strStart,"\r\n+CGACT:");
  if(str1!=NULL)
  {
    
  }   
  //##################################################  
  str1 = strstr(strStart,"\r\n+CGPADDR:");
  if(str1!=NULL)
  {
    
  } 
  //##################################################  
  str1 = strstr(strStart,"\r\n+CGCLASS:");
  if(str1!=NULL)
  {
    
  } 
  //##################################################  
  str1 = strstr(strStart,"\r\n+CGEREP:");
  if(str1!=NULL)
  {
    
  } 
  //##################################################  
  str1 = strstr(strStart,"\r\n+CGREG:");
  if(str1!=NULL)
  {
    
  } 
  //##################################################  
  str1 = strstr(strStart,"\r\n+CSTT:");
  if(str1!=NULL)
  {
    sscanf(str1,"\r\n+CSTT: \"%[^\"]\",\"%[^\"]\",\"%[^\"]\"\r\n",Sim80x.GPRS.APN,Sim80x.GPRS.APN_UserName,Sim80x.GPRS.APN_Password);    
  }
  //##################################################  
  str1 = strstr(strStart,"AT+CIFSR\r\r\n");
  if(str1!=NULL)
  {
    sscanf(str1,"AT+CIFSR\r\r\n%[^\r]",Sim80x.GPRS.LocalIP);
  } 
  //##################################################  
  str1 = strstr(strStart,"\r\n+CIPMUX:");
  if(str1!=NULL)
  {
    str1 =strchr(str1,':');
    str1++;
    if(atoi(str1)==0)
      Sim80x.GPRS.MultiConnection=0;
    else
      Sim80x.GPRS.MultiConnection=1;
  } 
  //##################################################  
  str1 = strstr(strStart,"\r\nCONNECT OK\r\n");
  if(str1!=NULL)
  {
    if(Sim80x.GPRS.MultiConnection==0)
    {
      Sim80x.GPRS.Connection[0] = GPRSConnection_ConnectOK;      
    }
    else
    {

    }   
  } 
  //##################################################  
  str1 = strstr(strStart,"\r\nCONNECT FAIL\r\n");
  if(str1!=NULL)
  {
    if(Sim80x.GPRS.MultiConnection==0)
    {
      Sim80x.GPRS.Connection[0] = GPRSConnection_ConnectFail;      
    }
    else
    {

    }   
  } 
  //##################################################  
  str1 = strstr(strStart,"\r\nALREADY CONNECT\r\n");
  if(str1!=NULL)
  {
    if(Sim80x.GPRS.MultiConnection==0)
    {
      Sim80x.GPRS.Connection[0] = GPRSConnection_AlreadyConnect;      
    }
    else
    {

    }   
  } 
  //##################################################  
  str1 = strstr(strStart,"\r\nSEND OK\r\n");
  if(str1!=NULL)
  {
    if(Sim80x.GPRS.MultiConnection==0)
    {
      Sim80x.GPRS.SendStatus[0] = GPRSSendData_SendOK;      
    }
    else
    {

    }   
  } 
  //##################################################  
  str1 = strstr(strStart,"\r\n+HTTPACTION:");
  if(str1!=NULL)
  {
    str1 = strchr(str1,':');
    str1++;
    Sim80x.GPRS.HttpAction.Method=(GPRSHttpMethod_t)atoi(str1);
    str1 = strchr(str1,',');
    str1++;
    Sim80x.GPRS.HttpAction.ResultCode = atoi(str1);
    str1 = strchr(str1,',');
    str1++;
    Sim80x.GPRS.HttpAction.DataLen = atoi(str1);    
  } 
  //##################################################  
  str1 = strstr(strStart,"\r\n+HTTPREAD:");
  if(str1!=NULL)
  {
    str1 = strchr(str1,':');
    str1++;
    Sim80x.GPRS.HttpAction.TransferDataLen = atoi(str1);
    str1 = strchr(str1,'\n');
    str1++;
    strncpy(Sim80x.GPRS.HttpAction.Data,str1,Sim80x.GPRS.HttpAction.TransferDataLen);
    Sim80x.GPRS.HttpAction.CopyToBuffer=1;
  }
  //##################################################    
  #endif  
  //##################################################  
  //##################################################  
  //##################################################  
  for( uint8_t parameter=0; parameter<11; parameter++)
  {
    if((parameter==10) || (Sim80x.AtCommand.ReceiveAnswer[parameter][0]==0))
    {
      Sim80x.AtCommand.FindAnswer=0;
      break;
    }
    str1 = strstr(strStart,Sim80x.AtCommand.ReceiveAnswer[parameter]);
    if(str1!=NULL)
    {
      Sim80x.AtCommand.FindAnswer = parameter+1;
      Sim80x.AtCommand.ReceiveAnswerExeTime = HAL_GetTick()-Sim80x.AtCommand.SendCommandStartTime;
      break;
    }    
  }
  //##################################################
  //---       Buffer Process
  //##################################################
  #if (_SIM80X_DEBUG==2)
  printf("%s",strStart);
  #endif
  Sim80x.UsartRxIndex=0;
  memset(Sim80x.UsartRxBuffer,0,_SIM80X_BUFFER_SIZE);    
  Sim80x.Status.Busy=0;
}

//######################################################################################################################
//######################################################################################################################
//######################################################################################################################
void StartSim80xBuffTask(void const * argument)
{ 
  while(1)
  {
    if( ((Sim80x.UsartRxIndex>4) && (HAL_GetTick()-Sim80x.UsartRxLastTime > 50)))
    {
      Sim80x.BufferStartTime = HAL_GetTick();      
      Sim80x_BufferProcess();      
      Sim80x.BufferExeTime = HAL_GetTick()-Sim80x.BufferStartTime;
    }
    osDelay(10);
  }    
}
//######################################################################################################################
void StartSim80xTask(void const * argument)
{ 
  uint32_t TimeForSlowRun=0;
  #if( _SIM80X_USE_GPRS==1)
  uint32_t TimeForSlowRunGPRS=0;
  #endif
  uint8_t UnreadMsgCounter=1;
  while(1)
  {    
    //###########################################
    #if( _SIM80X_USE_BLUETOOTH==1)
    //###########################################
    if(Sim80x.Bluetooth.SPPLen >0 )
    {      
      Bluetooth_UserNewSppData(Sim80x.Bluetooth.SPPBuffer,Sim80x.Bluetooth.SPPLen);
      Sim80x.Bluetooth.SPPLen=0;
    }
    //###########################################
    if(Sim80x.Bluetooth.NeedGetStatus==1)
    {
      Sim80x.Bluetooth.NeedGetStatus=0;
      Bluetooth_GetStatus();
    }    
    //###########################################
    if(Sim80x.Bluetooth.ConnectingRequestProfile != BluetoothProfile_NotSet)
    {
      Bluetooth_UserConnectingSpp();
      Sim80x.Bluetooth.ConnectingRequestProfile = BluetoothProfile_NotSet;          
    }
    //###########################################
    if(Sim80x.Bluetooth.ConnectedID==255)
    {
      Sim80x.Bluetooth.ConnectedID=0;
      Bluetooth_UserNewPairingRequest(Sim80x.Bluetooth.ConnectedName,Sim80x.Bluetooth.ConnectedAddress,Sim80x.Bluetooth.PairingPassword);      
    }
    //###########################################
    #endif
    //###########################################
    //###########################################
    #if( _SIM80X_USE_GPRS==1)
    //###########################################
    if(HAL_GetTick()-TimeForSlowRunGPRS > 5000)
    {
      
      
      TimeForSlowRunGPRS=HAL_GetTick();
    }
    
    
    

    //###########################################
    #endif
    //###########################################
    for(uint8_t i=0 ;i<sizeof(Sim80x.Gsm.HaveNewMsg) ; i++)
    {
      if(Sim80x.Gsm.HaveNewMsg[i] > 0)
      {
        //Gsm_MsgGetMemoryStatus();        
        if(Gsm_MsgRead(Sim80x.Gsm.HaveNewMsg[i])==true)
        {
          Gsm_UserNewMsg(Sim80x.Gsm.MsgNumber,Sim80x.Gsm.MsgDate,Sim80x.Gsm.MsgTime,Sim80x.Gsm.Msg);
          Gsm_MsgDelete(Sim80x.Gsm.HaveNewMsg[i]);
        }
        Gsm_MsgGetMemoryStatus();  
        Sim80x.Gsm.HaveNewMsg[i]=0;
      }        
    }    
    //###########################################
    if(Sim80x.Gsm.MsgUsed > 0)
    {   
      if(Gsm_MsgRead(UnreadMsgCounter)==true)
      {
        Gsm_UserNewMsg(Sim80x.Gsm.MsgNumber,Sim80x.Gsm.MsgDate,Sim80x.Gsm.MsgTime,Sim80x.Gsm.Msg);
        Gsm_MsgDelete(UnreadMsgCounter);
        Gsm_MsgGetMemoryStatus();
      }
      UnreadMsgCounter++;
      if(UnreadMsgCounter==150)
        UnreadMsgCounter=0;      
    }
    //###########################################
    if(Sim80x.Gsm.HaveNewCall == 1)
    {
      Sim80x.Gsm.GsmVoiceStatus = GsmVoiceStatus_Ringing;
      Sim80x.Gsm.HaveNewCall = 0;
      Gsm_UserNewCall(Sim80x.Gsm.CallerNumber);     
    }    
    //###########################################
    if(HAL_GetTick() - TimeForSlowRun > 20000)
    {
      Sim80x_SendAtCommand("AT+CSQ\r\n",200,1,"\r\n+CSQ:");  
      Sim80x_SendAtCommand("AT+CBC\r\n",200,1,"\r\n+CBC:"); 
      Sim80x_SendAtCommand("AT+CREG?\r\n",200,1,"\r\n+CREG:");  
      Gsm_MsgGetMemoryStatus();      
      TimeForSlowRun=HAL_GetTick();
    }
    //###########################################
    Gsm_User(HAL_GetTick());
    //###########################################
    osDelay(100);
    
  }    
}



