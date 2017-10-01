
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
void	Sim80x_SerialSendString(char *str)
{
	while(_SIM80X_USART.gState != HAL_UART_STATE_READY)
		osDelay(10);
	HAL_UART_Transmit_DMA(&_SIM80X_USART,(uint8_t*)str,strlen(str));
	while(_SIM80X_USART.gState != HAL_UART_STATE_READY)
		osDelay(10);
}
//######################################################################################################################
void	Sim80x_RxCallBack(void)
{
  if(Sim80x.UsartRxTemp!=0)
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
  Sim80x_SerialSendString(Sim80x.AtCommand.SendCommand); 
  while( MaxWaiting_ms > 0)
  {
    osDelay(10);
    if(Sim80x.Status.Busy==0)
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
void  Sim80x_SetPower(bool TurnOn)
{ 
  if(TurnOn==true)
  {    
    if(Sim80x_SendAtCommand("AT\r\n",200,1,"AT\r\r\nOK\r\n") == 1)
    {
      Sim80x.Status.Power=1;
      Sim80x_SendAtCommand("ATE1\r\n",200,1,"ATE1\r\r\nOK\r\n");
      Sim80x_SendAtCommand("AT+COLP=1\r\n",200,1,"AT+COLP=1\r\r\nOK\r\n");
      Sim80x_SendAtCommand("AT+CLIP=1\r\n",200,1,"AT+CLIP=1\r\r\nOK\r\n");
      Gsm_MsgSetMemoryLocation(GsmMsgMemory_OnModule);
      Gsm_MsgSetFormat(GsmMsgFormat_Text);
      Gsm_MsgGetCharacterFormat();
      Gsm_MsgGetFormat();
    }
    else
    {      
      HAL_GPIO_WritePin(_SIM80X_POWER_KEY_GPIO,_SIM80X_POWER_KEY_PIN,GPIO_PIN_RESET);
      osDelay(1200);
      HAL_GPIO_WritePin(_SIM80X_POWER_KEY_GPIO,_SIM80X_POWER_KEY_PIN,GPIO_PIN_SET);
      osDelay(3000);  
      if(Sim80x_SendAtCommand("AT\r\n",200,1,"AT\r\r\nOK\r\n") == 1)
      {
        Sim80x.Status.Power=1;
        Sim80x_SendAtCommand("ATE1\r\n",200,1,"ATE1\r\r\nOK\r\n");
        Sim80x_SendAtCommand("AT+COLP=1\r\n",200,1,"AT+COLP=1\r\r\nOK\r\n");
        Sim80x_SendAtCommand("AT+CLIP=1\r\n",200,1,"AT+CLIP=1\r\r\nOK\r\n");
        Gsm_MsgSetMemoryLocation(GsmMsgMemory_OnModule);
        Gsm_MsgSetFormat(GsmMsgFormat_Text);
        Gsm_MsgGetCharacterFormat();
        Gsm_MsgGetFormat();
        
      }
      else
        Sim80x.Status.Power=0;
    }
  }
  else
  {
    if(Sim80x_SendAtCommand("AT\r\n",200,1,"AT\r\r\nOK\r\n") == 1)
    {
      Sim80x.Status.Power=0;
      HAL_GPIO_WritePin(_SIM80X_POWER_KEY_GPIO,_SIM80X_POWER_KEY_PIN,GPIO_PIN_RESET);
      osDelay(1200);
      HAL_GPIO_WritePin(_SIM80X_POWER_KEY_GPIO,_SIM80X_POWER_KEY_PIN,GPIO_PIN_SET);
    }
  }  
}
//######################################################################################################################
void	Sim80x_Init(osPriority Priority)
{
  HAL_GPIO_WritePin(_SIM80X_POWER_KEY_GPIO,_SIM80X_POWER_KEY_PIN,GPIO_PIN_SET);
	Sim80x.UsartRxLastTime=0;
	Sim80x.UsartRxIndex=0;
	memset(Sim80x.UsartRxBuffer,0,_SIM80X_BUFFER_SIZE);
	HAL_UART_Receive_IT(&_SIM80X_USART,&Sim80x.UsartRxTemp,1);
  osThreadDef(Sim80xTask, StartSim80xTask, Priority, 0, 256);
  Sim80xTaskHandle = osThreadCreate(osThread(Sim80xTask), NULL);
  osThreadDef(Sim80xBuffTask, StartSim80xBuffTask, Priority, 0, 256);
  Sim80xBuffTaskHandle = osThreadCreate(osThread(Sim80xBuffTask), NULL);
  
  Sim80x_SetPower(true); 
}
  
//######################################################################################################################
void  Sim80x_BufferProcess(void)
{
  char      *strStart,*str1,*str2,*str3,*str4;
  char      tmp_str[32];
  int32_t   tmp_int32_t;
  float     tmp_float;
  
  strStart = (char*)&Sim80x.UsartRxBuffer[0];  
  //##################################################
  //+++       Buffer Process
  //##################################################
 
  //##################################################
  str1 = strstr(strStart,"\r\nCall Ready\r\n");
  if(str1!=NULL)
    Sim80x.Status.CallReady=1;  
  //##################################################
  str1 = strstr(strStart,"\r\nSMS Ready\r\n");
  if(str1!=NULL)
    Sim80x.Status.SmsReady=1;  
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
    Sim80x.Gsm.GsmVoiceCallReturn=GsmVoiceCallReturn_Busy;   
  }
  //##################################################
  str1 = strstr(strStart,"\r\nNO DIALTONE\r\n");
  if(str1!=NULL)
  {
    Sim80x.Gsm.GsmVoiceCallReturn=GsmVoiceCallReturn_NoDialTone;
  }
  //##################################################
  str1 = strstr(strStart,"\r\nNO CARRIER\r\n");
  if(str1!=NULL)
  {
    Sim80x.Gsm.GsmVoiceCallReturn=GsmVoiceCallReturn_NoCarrier;
  }
  //##################################################
  str1 = strstr(strStart,"\r\nNO ANSWER\r\n");
  if(str1!=NULL)
  {
    Sim80x.Gsm.GsmVoiceCallReturn=GsmVoiceCallReturn_NoAnswer;
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
  str1 = strstr(strStart,"\r\n+CMTI:");
  if(str1!=NULL)
  {
    str1 = strchr(str1,',');
    str1++;
    Sim80x.Gsm.HaveNewMsg = atoi(str1);    
  }
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
      Sim80x_BufferProcess();      
    }
    osDelay(10);
  }    
}
//######################################################################################################################
void StartSim80xTask(void const * argument)
{ 
  uint32_t TimeForSlowRun=0;
  uint8_t UnreadMsgCounter=1;
  while(1)
  {
    //###########################################
    if(Sim80x.Gsm.MsgUsed > 0)
    {
      if(Gsm_MsgRead(UnreadMsgCounter)==true)
        Gsm_UserHaveNewMsg(Sim80x.Gsm.MsgNumber,Sim80x.Gsm.MsgDate,Sim80x.Gsm.MsgTime,Sim80x.Gsm.Msg);
      Gsm_MsgDelete(UnreadMsgCounter);
      Gsm_MsgGetMemoryStatus();
      UnreadMsgCounter++;
      if(UnreadMsgCounter==150)
        UnreadMsgCounter=1;
    }
    //###########################################
    if(Sim80x.Gsm.HaveNewMsg > 0)
    {
      Gsm_MsgGetMemoryStatus();
      if(Gsm_MsgRead(Sim80x.Gsm.HaveNewMsg)==true)
        Gsm_UserHaveNewMsg(Sim80x.Gsm.MsgNumber,Sim80x.Gsm.MsgDate,Sim80x.Gsm.MsgTime,Sim80x.Gsm.Msg);
      Gsm_MsgDelete(Sim80x.Gsm.HaveNewMsg);
      Gsm_MsgGetMemoryStatus();  
      Sim80x.Gsm.HaveNewMsg=0;
    }
    //###########################################
    if(Sim80x.Gsm.HaveNewCall == 1)
    {
      Sim80x.Gsm.HaveNewCall = 0;
      Gsm_UserHaveNewCall(Sim80x.Gsm.CallerNumber);     
    }    
    //###########################################
    if(HAL_GetTick() - TimeForSlowRun > 20000)
    {
      Sim80x_SendAtCommand("AT+CSQ\r\n",200,1,"\r\n+CSQ:");  
      Sim80x_SendAtCommand("AT+CBC\r\n",200,1,"\r\n+CBC:");  
      Gsm_MsgGetMemoryStatus();      
      TimeForSlowRun=HAL_GetTick();
    }
    //###########################################
    Gsm_User(HAL_GetTick());
    //###########################################
    osDelay(100);
    
  }    
}



