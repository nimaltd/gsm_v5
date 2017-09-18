
//			www.GitHub.com/NimaLTD
//			GsmLib Version2				
//			2.005


#include "GsmLib.h"


#define			ANS_OK					"\nOK\r\n"
#define			ANS_ERROR				"\nERROR\r\n"
#define			ANS_AT_OK				"AT\r\r\nOK\r\n"
#define			ANS_CALL_READY	"\r\nCall Ready\r\n"
#define			ANS_SMS_READY		"\r\nSMS Ready\r\n"
#define			ANS_CLIP				"\r\n+CLIP:"
#define			ANS_ATA					"\r\nATA\r\r\nOK\r\n"
#define			ANS_ATH					"\r\nATH\r\r\nOK\r\n"
#define			ANS_HVOIC				"\r\nHVOIC\r\r\nOK\r\n"
#define			ANS_BUSY				"\r\nBUSY\r\n"
#define			ANS_NODIALTONE	"\r\nNO DIALTONE\r\n"
#define			ANS_NOCARRIER		"\r\nNO CARRIER\r\n"
#define			ANS_NOANSWER		"\r\nNO ANSWER\r\n"
#define			ANS_NODIALTONE	"\r\nNO DIALTONE\r\n"
#define			ANS_ANSWERCALL	"\r\n+COLP:"
#define			ANS_SIGNAL			"\r\n+CSQ:"
#define			ANS_BATTERY			"\r\n+CBC:"
#define			ANS_USSD				"\r\n+CUSD:"
#define			ANS_CSCS				"\r\n+CSCS:"
#define			ANS_CSTA				"\r\n+CSTA:"
#define			ANS_CMGD				"\nAT+CMGD="
#define			ANS_CMTI				"\r\n+CMTI:"			// new message
#define			ANS_CMGF				"\r\n+CMGF:"
#define			ANS_CMGR				"\r\n+CMGR:"
#define			ANS_CPMS				"\r\n+CPMS:"			// get message free space
#define			ANS_CSCA				"\r\n+CSCA:"			// get message Service number
#define			ANS_SENDREADY		"\r\r\n> "
#define			ANS_CMGL				"\r\n+CMGL:"
#define			ANS_CSMP				"\r\n+CSMP:"			// get message text oarameter
#define			ANS_CREC				"\r\n+CREC:"






Gsm_t	Gsm;

osThreadId 		GsmTaskHandle;
osThreadId 		GsmBufferTaskHandle;



void 	StartGsmTask(void const * argument);
void 	StartGsmBufferTask(void const * argument);


//########################################################################################################################
//########################################################################################################################
//########################################################################################################################

//########################################################################################################################
void	Gsm_SerialSend(const char *SendStr)
{
	while((_GSM_USART.gState != HAL_UART_STATE_READY) || (Gsm.SerialIndex > (_GSM_BUFFER_SIZE-5)))
		osDelay(10);
	
	HAL_UART_Transmit_DMA(&_GSM_USART,(uint8_t*)SendStr,strlen(SendStr));

	while(_GSM_USART.gState != HAL_UART_STATE_READY)
		osDelay(10);

}
//########################################################################################################################
//########################################################################################################################
//########################################################################################################################
void	Gsm_RxCallBack(void)
{
	if(Gsm.SerialIndex < (_GSM_BUFFER_SIZE-1))
	{
		if(Gsm.SerialTmp!=0)
		{
			Gsm.SerialRxBuff[Gsm.SerialIndex]=Gsm.SerialTmp;
			Gsm.SerialIndex++;
		}
	}
	Gsm.SerialLastTimeReceived = HAL_GetTick();
	HAL_UART_Receive_IT(&_GSM_USART,&Gsm.SerialTmp,1);
}

//########################################################################################################################
//########################################################################################################################
//########################################################################################################################

void StartGsmTask(void const * argument)
{
	#if (_GSM_DEBUG==1)
	printf("GSM : GsmTask Started\r\n");
	#endif	
	uint8_t	SlowTask=0;
	
	while(1)
	{		
		//++++++++
		if(Gsm.Answer.Calling==true)
		{			
			Gsm_UserGetCall(Gsm.LastCallerNumber);
			Gsm_CallDisconnect();
			Gsm.Answer.Calling=false;
		}
		//++++++++
		for(uint8_t i=0; i<sizeof(Gsm.LastMsgNew) ; i++)
		{
			if(Gsm.LastMsgNew[i] != 0)
			{
				Gsm_MsgRead(Gsm.LastMsgNew[i]);
				Gsm_UserGetMsg(Gsm.LastMsgNumber,Gsm.LastMsgDate,Gsm.LastMsgTime,Gsm.LastMsg);
				Gsm_MsgDelete(Gsm.LastMsgNew[i]);
				Gsm.LastMsgNew[i]=0;
				Gsm_MsgGetFreeSpace();
			}	
			
		}
		//++++++++
		if(Gsm.Answer.MsgUsed > 0)
		{
			Gsm_MsgGetUnreadIndex();
			
			
		}		
		//++++++++
		
		Gsm_UserRunEverySecond();		
		SlowTask++;
		if(SlowTask==10)
		{
			SlowTask=0;
			Gsm_GetSignalQuality();
			Gsm_GetBatteryStatus();
			Gsm_MsgGetFreeSpace();
		}
		osDelay(1000);
	}
}
//########################################################################################################################
void StartGsmBufferTask(void const * argument)
{
	while(1)
	{
		// +++ New Data
		if((HAL_GetTick()-Gsm.SerialLastTimeReceived>50) && (Gsm.SerialIndex>4))
		{
			char *str1,*str2,*str3;
			char str[32];
			//############################
			str1 = Gsm.SerialRxBuff;
			str1 = strstr(str1,ANS_OK);
			if(str1!=NULL)
			{
				Gsm.Answer.OK=1;				
			}
			//############################
			str1 = Gsm.SerialRxBuff;
			str1 = strstr(str1,ANS_ERROR);
			if(str1!=NULL)
			{
				Gsm.Answer.ERROR=1;				
			}
			//############################
			str1 = Gsm.SerialRxBuff;
			str1 = strstr(str1,ANS_AT_OK);
			if(str1!=NULL)
			{
				Gsm.Answer.Power=1;				
			}
			//############################
			str1 = Gsm.SerialRxBuff;
			str1 = strstr(str1,ANS_CALL_READY);
			if(str1!=NULL)
			{
				Gsm.Answer.CallReady=1;
			}			
			//############################
			str1 = Gsm.SerialRxBuff;
			str1 = strstr(str1,ANS_CLIP);
			if(str1!=NULL)
			{					
				str2 = strchr(str1,'"');
				if(str2 != NULL)
				{
					str2++;
					str3 = strchr(str2,'"');
					if(str3!=NULL)
					{
						strncpy(Gsm.LastCallerNumber,str2,str3-str2);						
						Gsm.Answer.Calling=1;
					}					
				}								
			}			
			//############################
			str1 = Gsm.SerialRxBuff;
			str1 = strstr(str1,ANS_SMS_READY);
			if(str1!=NULL)
			{
				Gsm.Answer.SmsReady=1;				
			}
			//############################
			str1 = Gsm.SerialRxBuff;
			str1 = strstr(str1,ANS_ATA);
			if(str1!=NULL)
			{
				Gsm.Answer.CallAnswerIncoming=1;				
			}
			//############################
			str1 = Gsm.SerialRxBuff;
			str1 = strstr(str1,ANS_ATH);
			if(str1!=NULL)
			{
				Gsm.Answer.CallDisconnect=1;				
			}			
			//############################
			str1 = Gsm.SerialRxBuff;
			str1 = strstr(str1,ANS_BUSY);
			if(str1!=NULL)
			{
				Gsm.Answer.CallBusy=1;				
			}			
			//############################
			str1 = Gsm.SerialRxBuff;
			str1 = strstr(str1,ANS_NOANSWER);
			if(str1!=NULL)
			{
				Gsm.Answer.CallNoAnswer=1;				
			}						
			//############################
			str1 = Gsm.SerialRxBuff;
			str1 = strstr(str1,ANS_NODIALTONE);
			if(str1!=NULL)
			{
				Gsm.Answer.CallNoDialTone=1;				
			}						
			//############################
			str1 = Gsm.SerialRxBuff;
			str1 = strstr(str1,ANS_NOCARRIER);
			if(str1!=NULL)
			{
				Gsm.Answer.CallNoCarrier=1;				
			}						
			//############################
			str1 = Gsm.SerialRxBuff;
			str1 = strstr(str1,ANS_ANSWERCALL);
			if(str1!=NULL)
			{
				Gsm.Answer.CallAnswerOutgoing=1;				
			}						
			//############################			
			str1 = Gsm.SerialRxBuff;
			str1 = strstr(str1,ANS_SIGNAL);
			if(str1!=NULL)
			{
				str1 = strchr(str1,':');
				if(str1!=NULL)
				{
					str1++;
					Gsm.Answer.Signal=atoi(str1);
					if(Gsm.Answer.Signal==99)
						Gsm.Answer.Signal=0;
					else
						Gsm.Answer.Signal =(uint8_t)(Gsm.Answer.Signal * 3.3f);
					if(Gsm.Answer.Signal>100)
						Gsm.Answer.Signal=100;
				}
				else
					Gsm.Answer.Signal=0;
			}	
			//############################
			str1 = Gsm.SerialRxBuff;
			str1 = strstr(str1,ANS_BATTERY);
			if(str1!=NULL)
			{
				str1 = strchr(str1,':');
				str1++;
				if(atoi(str1) == 0)
				{
					Gsm.Answer.BatteryCharging=0;
					Gsm.Answer.BatteryFull=0;
				}
				if(atoi(str1) == 1)
				{
					Gsm.Answer.BatteryCharging=1;
					Gsm.Answer.BatteryFull=0;
				}
				if(atoi(str1) == 2)
				{
					Gsm.Answer.BatteryCharging=0;
					Gsm.Answer.BatteryFull=1;
				}
				str1 = strchr(str1,',');
				str1++;
				Gsm.Answer.BatteryPercent=atoi(str1);
			}									
			//############################
			str1 = Gsm.SerialRxBuff;
			str1 = strstr(str1,ANS_USSD);
			if(str1!=NULL)
			{
				str1 = strchr(str1,':');
				str1++;
				memset(Gsm.LastMsg,0,sizeof(Gsm.LastMsg));
				if(atoi(str1) == 0)
				{
					str1 = strchr(str1,'"');
					str1++;
					str2 = strstr(str1,"\", ");
					if(str2!=NULL)
					{
						strncpy(Gsm.LastMsg,str1,str2-str1);
					}						
				}
				Gsm.Answer.UssdAnswer=1;	
			}
			//############################
			str1 = Gsm.SerialRxBuff;
			str1 = strstr(str1,ANS_CSCS);
			if(str1!=NULL)
			{
				str1 = strchr(str1,':');
				str1++;
				if(strncmp(str1," \"GSM\"",6) == 0)
					Gsm.Config.TECharacterSet = GsmTECharacterSet_GSM;
				else if(strncmp(str1," \"UCS2\"",7) == 0)
					Gsm.Config.TECharacterSet = GsmTECharacterSet_UCS2;
				else if(strncmp(str1," \"IRA\"",6) == 0)
					Gsm.Config.TECharacterSet = GsmTECharacterSet_IRA;
				else if(strncmp(str1," \"IRA\"",6) == 0)
					Gsm.Config.TECharacterSet = GsmTECharacterSet_IRA;
				else if(strncmp(str1," \"HEX\"",6) == 0)
					Gsm.Config.TECharacterSet = GsmTECharacterSet_HEX;
				else if(strncmp(str1," \"PCCP\"",7) == 0)
					Gsm.Config.TECharacterSet = GsmTECharacterSet_PCCP;
				else if(strncmp(str1," \"PCDN\"",7) == 0)
					Gsm.Config.TECharacterSet = GsmTECharacterSet_PCDN;
				else if(strncmp(str1," \"8859-1\"",9) == 0)
					Gsm.Config.TECharacterSet = GsmTECharacterSet_8859_1;				
			}			
			//############################
			str1 = Gsm.SerialRxBuff;
			str1 = strstr(str1,ANS_CSTA);
			if(str1!=NULL)
			{
				str1 = strchr(str1,':');
				str1++;
				Gsm.Config.TypeOfAddress = (GsmTypeOfAddress_t)atoi(str1);				
			}						
			//############################
			str1 = Gsm.SerialRxBuff;
			str1 = strstr(str1,ANS_CMGD);
			if(str1!=NULL)
			{
				sprintf(str,"\nAT+CMGD=%d\r\r\nOK\r\n",Gsm.LastMsgDelete);
				if(strstr(str1,str)!=NULL)
					Gsm.Answer.MsgDeleteStatus=1;
				else
					Gsm.Answer.MsgDeleteStatus=0;			
			}			
			//############################
			str1 = Gsm.SerialRxBuff;
			str1 = strstr(str1,ANS_CMGF);
			if(str1!=NULL)
			{
				str1=strchr(str1,':');
				str1++;
				Gsm.Config.MsgFormat = (GsmMsgFormat_t) atoi(str1);
			}			
			//############################
			str1 = Gsm.SerialRxBuff;
			str1 = strstr(str1,ANS_CMGR);
			if(str1!=NULL)
			{
				memset(Gsm.LastMsg,0,sizeof(Gsm.LastMsg));
				memset(Gsm.LastMsgDate,0,sizeof(Gsm.LastMsgDate));
				memset(Gsm.LastMsgNumber,0,sizeof(Gsm.LastMsgNumber));
				memset(Gsm.LastMsgTime,0,sizeof(Gsm.LastMsgTime));
				str1=strchr(str1,':');
				str1++;
				str1 = strchr(str1,',');
				str1+=2;	
				str2 = strchr(str1,'"');
				strncpy(Gsm.LastMsgNumber,str1,str2-str1);
				str1 = strchr(str2,',');
				str1++;
				str1 = strchr(str1,',');
				str1+=2;
				str2 = strchr(str1,',');
				strncpy(Gsm.LastMsgDate,str1,str2-str1);
				str2++;
				strncpy(Gsm.LastMsgTime,str2,8);
				str1 = strstr(str2,"\r\n");
				str1+=2;
				str2 = strstr(str1,"\r\n\r\nOK\r\n");
				if(str2 != NULL)
				{
					if((str2-str1) <= sizeof(Gsm.LastMsg)) 
						strncpy(Gsm.LastMsg,str1,str2-str1);				
					else
						sprintf(Gsm.LastMsg,"OVER LOAD");
				}
				else
					sprintf(Gsm.LastMsg,"OVER LOAD");
				Gsm.Answer.MsgReadMsg=1;				
			}			
			//############################
			str1 = Gsm.SerialRxBuff;
			str1 = strstr(str1,ANS_CPMS);
			if(str1!=NULL)
			{				
				str1 = strchr(str1,',');
				str1++;
				Gsm.Answer.MsgUsed = atoi(str1);
				str1 = strchr(str1,',');
				str1++;
				Gsm.Answer.MsgCapacity = atoi(str1);
			}			
			//############################
			str1 = Gsm.SerialRxBuff;
			str1 = strstr(str1,ANS_CSCA);
			if(str1!=NULL)
			{				
				str1 = strchr(str1,'"');
				str1++;
				str2 = strchr(str1,'"');
				strncpy(Gsm.Config.MsgSeriviceCenter,str1,str2-str1);
			}			
			//############################
			str1 = Gsm.SerialRxBuff;
			str1 = strstr(str1,ANS_SENDREADY);
			if(str1!=NULL)
			{
				Gsm.Answer.MsgReadyToSend=1;				
			}			
			//############################
			str1 = Gsm.SerialRxBuff;
			str1 = strstr(str1,ANS_CMTI);
			if(str1!=NULL)
			{				
				for(uint8_t i=0; i<sizeof(Gsm.LastMsgNew) ; i++)
				{
					if( Gsm.LastMsgNew[i] == 0)
					{
						str1 = strchr(str1,',');
						str1++;
						Gsm.LastMsgNew[i] = atoi(str1);
						break;
					}					
				}				
			}
			//############################
			str1 = Gsm.SerialRxBuff;
			str1 = strstr(str1,ANS_CMGL);
			if(str1!=NULL)
			{				
				for(uint8_t i=0; i<sizeof(Gsm.LastMsgNew) ; i++)
				{
					if( Gsm.LastMsgNew[i] == 0)
					{
						str1 = strchr(str1,':');
						str1++;
						Gsm.LastMsgNew[i] = atoi(str1);
						break;
					}					
				}					
			}
			//############################
			str1 = Gsm.SerialRxBuff;
			str1 = strstr(str1,ANS_CSMP);
			if(str1!=NULL)
			{
				str1 = strchr(str1,':');
				str1++;
				Gsm.Config.MsgFo = atoi(str1);
				str1 = strchr(str1,',');
				str1++;
				Gsm.Config.MsgVp = atoi(str1);
				str1 = strchr(str1,',');
				str1++;
				Gsm.Config.MsgPid = atoi(str1);
				str1 = strchr(str1,',');
				str1++;
				Gsm.Config.MsgDcs = atoi(str1);
			}	
			//############################
			str1 = Gsm.SerialRxBuff;
			str1 = strstr(str1,ANS_CREC);
			if(str1!=NULL)
			{
				str1 = strchr(str1,':');
				str1++;
				Gsm.Answer.Record = (GsmRecord_t)atoi(str1);
			}			
			//############################
			
			//############################
			//############################
			//############################
			
			#if (_GSM_DEBUG==2)
			printf(Gsm.SerialRxBuff);
			#endif

			memset(Gsm.SerialRxBuff,0,_GSM_BUFFER_SIZE);
			Gsm.SerialIndex=0;			
		}
	
			

		// --- New Data
		osDelay(10);
	}
	
}
//########################################################################################################################
//########################################################################################################################
//########################################################################################################################
void			Gsm_Init(osPriority Priority)
{

	#if (_GSM_DEBUG==1)
	printf("GSM : Init\r\n");
	#endif

	HAL_GPIO_WritePin(GSM_POWER_KEY_GPIO_Port, GSM_POWER_KEY_Pin, GPIO_PIN_SET);
	Gsm.SerialIndex=0;
	memset(Gsm.SerialRxBuff,0,_GSM_BUFFER_SIZE);
	HAL_UART_Receive_IT(&_GSM_USART,&Gsm.SerialTmp,1);

	
	
  osThreadDef(GsmTask, StartGsmTask, Priority, 0, 256);
  GsmTaskHandle = osThreadCreate(osThread(GsmTask), NULL);

	osThreadDef(GsmBufferTask, StartGsmBufferTask, Priority, 0, 256);
  GsmBufferTaskHandle = osThreadCreate(osThread(GsmBufferTask), NULL);

	TurnOnAgain:
	Gsm_SerialSend("AT\r\n");
	osDelay(100);
	if(Gsm.Answer.Power==0)
	{		
		HAL_GPIO_WritePin(GSM_POWER_KEY_GPIO_Port, GSM_POWER_KEY_Pin, GPIO_PIN_RESET);
		osDelay(1500);
		HAL_GPIO_WritePin(GSM_POWER_KEY_GPIO_Port, GSM_POWER_KEY_Pin, GPIO_PIN_SET);
		osDelay(5000);		
		Gsm_SerialSend("AT\r\n");
		osDelay(100);
	}
	if(Gsm.Answer.Power == 0)
	{
		goto TurnOnAgain; 		
	}		
	
	Gsm_SerialSend("ATE1\r\n");
	osDelay(100);
	Gsm_SerialSend("COLP=1\r\n");
	Gsm_SetMonitorSpeakerLoudness(9);
	
	Gsm_MsgSetFormat(GsmMsgFormat_Text);
	Gsm_GetTECharacterSet();
	Gsm_GetTypeOfAddress();
	Gsm_MsgSetStoreOnSim(false);
	Gsm_MsgGetServiceCenterAddress();
	Gsm_MsgGetFreeSpace();
	if(Gsm.Config.MsgFormat == GsmMsgFormat_Text)
		Gsm_MsgSetTextModeParameter(17,167,0,0);	
}
//########################################################################################################################
//########################################################################################################################
//########################################################################################################################
bool			Gsm_CallAnswer(void)
{
	Gsm.Answer.CallAnswerIncoming=0;
	Gsm_SerialSend("ATA\r\n");
	osDelay(1000);
	if(Gsm.Answer.CallAnswerIncoming==1)
	{
		Gsm.Answer.CallAnswerIncoming=0;
		return true;
	}
	else
		return false;	
}
//########################################################################################################################
bool			Gsm_CallDisconnect(void)
{
	Gsm.Answer.CallDisconnect=0;
	Gsm_SerialSend("AT+HVOIC\r\n");
	osDelay(1000);
	if(Gsm.Answer.CallDisconnect==1)
	{
		Gsm.Answer.CallDisconnect=0;
		Gsm.Answer.CallDialling=0;
		return true;
	}
	else
		return false;	
}
//########################################################################################################################
bool	Gsm_CallDial(char	*DialNumber,uint8_t	WaitForAnswerInSecond)
{
	if(Gsm.Answer.CallDialling==1)
		return false;
	Gsm.Answer.CallDialling=1;
	char str[24];
	uint16_t	tryIndex=WaitForAnswerInSecond*10;
	Gsm.Answer.CallAnswerOutgoing=0;
	Gsm.Answer.CallBusy=0;
	Gsm.Answer.CallNoAnswer=0;
	Gsm.Answer.CallNoCarrier=0;
	Gsm.Answer.CallNoDialTone=0;
	
	snprintf(str,sizeof(str),"ATD%s;\r\n",DialNumber);
	Gsm_SerialSend(str);
	while(tryIndex>0)
	{
		osDelay(100);
		tryIndex--;
		if((Gsm.Answer.CallBusy==1) || (Gsm.Answer.CallNoAnswer==1) || (Gsm.Answer.CallNoCarrier==1) || (Gsm.Answer.CallNoDialTone==1))
		{
			Gsm.Answer.CallDialling=0;
			return false;
		}
		if(Gsm.Answer.CallAnswerOutgoing==1)
		{
			Gsm.Answer.CallDialling=0;
			return true;
		}
	}
	Gsm_CallDisconnect();
	Gsm.Answer.CallDialling=0;
	return false;
}
//########################################################################################################################
//########################################################################################################################
//########################################################################################################################
bool			Gsm_Ussd(char *Request,char *Answer)
{
	char str[32];
	Gsm.Answer.UssdAnswer=0;
	uint16_t UssdTimeout=600; // 60 seconds
	snprintf(str,sizeof(str),"AT+CUSD=1,\"%s\"\r\n",Request);
	Gsm_SerialSend(str);	
	
	while(UssdTimeout)
	{
		UssdTimeout--;	
		osDelay(100);
		if(Gsm.Answer.UssdAnswer==1)
		{
			Gsm.Answer.UssdAnswer=0;
			if(Answer != NULL)
				strcpy(Answer,Gsm.LastMsg);
			return true;
		}		
	}
	return false;
}
//########################################################################################################################
//########################################################################################################################
//########################################################################################################################
void			Gsm_SetFactoryReset(void)
{
	Gsm_SerialSend("AT&F0\r\n");
	osDelay(1000);	
}
//########################################################################################################################
void			Gsm_SetMonitorSpeakerLoudness(uint8_t	Vol_0_9)
{
	char str[8];
	if(Vol_0_9>9)
		Vol_0_9=9;
	sprintf(str,"ATL%d\r\n",Vol_0_9);
	Gsm_SerialSend(str);	
}
//########################################################################################################################
void			Gsm_SetRingLevel(uint8_t	Vol_0_100)
{
	char str[16];
	if(Vol_0_100>100)
		Vol_0_100=100;
	sprintf(str,"AT+CRSL=%d\r\n",Vol_0_100);
	Gsm_SerialSend(str);	
}
//########################################################################################################################
void			Gsm_SetLoudSpeakerVolumeLevel(uint8_t	Vol_0_100)
{
	char str[16];
	if(Vol_0_100>100)
		Vol_0_100=100;
	sprintf(str,"AT+CLVL=%d\r\n",Vol_0_100);
	Gsm_SerialSend(str);	
}
//########################################################################################################################
void			Gsm_SetMute(bool	Enable)
{
	if(Enable==false)
		Gsm_SerialSend("AT+CMUT=0\r\n");	
	else
		Gsm_SerialSend("AT+CMUT=1\r\n");	
}
//########################################################################################################################
GsmTECharacterSet_t			Gsm_GetTECharacterSet(void)
{
	Gsm_SerialSend("AT+CSCS?\r\n");
	osDelay(1000);
	return Gsm.Config.TECharacterSet;
}
//########################################################################################################################
void			Gsm_SetTECharacterSet(GsmTECharacterSet_t GsmTECharacterSet)
{
	switch(GsmTECharacterSet)
	{
		case GsmTECharacterSet_GSM:
			Gsm_SerialSend("AT+CSCS=\"GSM\"\r\n");
		break;
		case GsmTECharacterSet_HEX:
			Gsm_SerialSend("AT+CSCS=\"HEX\"\r\n");
		break;
		case GsmTECharacterSet_8859_1:
			Gsm_SerialSend("AT+CSCS=\"8559-1\"\r\n");
		break;
		case GsmTECharacterSet_IRA:
			Gsm_SerialSend("AT+CSCS=\"IRA\"\r\n");
		break;
		case GsmTECharacterSet_PCCP:
			Gsm_SerialSend("AT+CSCS=\"PCCP\"\r\n");
		break;
		case GsmTECharacterSet_UCS2:
			Gsm_SerialSend("AT+CSCS=\"UCS2\"\r\n");
		break;
		case GsmTECharacterSet_PCDN:
			Gsm_SerialSend("AT+CSCS=\"PCDN\"\r\n");
		break;
	}
	osDelay(1000);
	Gsm_SerialSend("AT+CSCS?\r\n");	
}
//########################################################################################################################
GsmTypeOfAddress_t			Gsm_GetTypeOfAddress(void)
{
	Gsm_SerialSend("AT+CSTA?\r\n");	
	osDelay(1000);
	return Gsm.Config.TypeOfAddress;
}
//########################################################################################################################
void			Gsm_SetTypeOfAddress(GsmTypeOfAddress_t GsmTypeOfAddress)
{
	char str[16];
	sprintf(str,"AT+CSTA=%d\r\n",GsmTypeOfAddress);
	Gsm_SerialSend(str);
	osDelay(1000);
	Gsm_SerialSend("AT+CSTA?\r\n");
}
//########################################################################################################################
//########################################################################################################################
//########################################################################################################################
uint8_t		Gsm_GetSignalQuality(void)
{
	Gsm_SerialSend("AT+CSQ\r\n");
	osDelay(100);
	return Gsm.Answer.Signal;		
}
//########################################################################################################################
uint8_t			Gsm_GetBatteryStatus(void)
{
	Gsm_SerialSend("AT+CBC\r\n");
	osDelay(100);
	return Gsm.Answer.BatteryPercent;
}
//########################################################################################################################
//########################################################################################################################
//########################################################################################################################
bool			Gsm_MsgDelete(uint8_t index)
{
	char str[16];
	Gsm.Answer.MsgDeleteStatus=0;
	Gsm.LastMsgDelete=index;
	sprintf(str,"AT+CMGD=%d\r\n",index);
	Gsm_SerialSend(str);
	osDelay(500);	
	if(Gsm.Answer.MsgDeleteStatus==1)
		return true;
	else
		return false;
}
//########################################################################################################################
void			Gsm_MsgSetFormat(GsmMsgFormat_t GsmMsgFormat)
{
	if(GsmMsgFormat==GsmMsgFormat_PDU)
		Gsm_SerialSend("AT+CMGF=0\r\n");
	else
		Gsm_SerialSend("AT+CMGF=1\r\n");
	Gsm_MsgGetFormat();	
}
//########################################################################################################################
GsmMsgFormat_t		Gsm_MsgGetFormat(void)
{
	Gsm_SerialSend("AT+CMGF?\r\n");
	osDelay(500);
	return Gsm.Config.MsgFormat;
}
//########################################################################################################################
bool			Gsm_MsgRead(uint8_t index)
{
	char str[16];
	uint16_t	tryIndex=50;	// 5 seconds
	Gsm.Answer.MsgReadMsg=0;
	sprintf(str,"AT+CMGR=%d\r\n",index);
	Gsm_SerialSend(str);
	while(tryIndex > 0)
	{
		tryIndex--;
		osDelay(100);
		if(Gsm.Answer.MsgReadMsg==1)
			return true;
	}
	return false;
}
//########################################################################################################################
bool			Gsm_MsgSend(char *Number,char *Message)
{
	char str[32];
	uint16_t MsgSendTimeout=600; // 60 seconds

	
	if(Gsm.Config.MsgFormat == GsmMsgFormat_PDU)
	{		
		
		
	}
	else
	{
		Gsm.Answer.OK=0;
		Gsm.Answer.ERROR=0;
		Gsm.Answer.MsgReadyToSend=0;
		snprintf(str,sizeof(str),"AT+CMGS=\"%s\"\r\n",Number);
		Gsm_SerialSend(str);
		osDelay(1000);
		if(	Gsm.Answer.ERROR == 1)
			return false;
		while(MsgSendTimeout>0)
		{
			MsgSendTimeout--;
			osDelay(100);
			if(Gsm.Answer.MsgReadyToSend == 1)
			{		
				Gsm.Answer.OK=0;
				Gsm.Answer.ERROR=0;
				Gsm.Answer.MsgReadyToSend	=2;			
				Gsm_SerialSend(Message);
				sprintf(str,"%c",26);
				Gsm_SerialSend(str);
			}
			if(	Gsm.Answer.ERROR == 1)
				return false;
			if(	Gsm.Answer.OK == 1)
				return true;

		}
		return false;
		
	}
	return false;
}
//########################################################################################################################
void			Gsm_MsgSetStoreOnSim(bool	Enable)
{
	if(Enable == false)
		Gsm_SerialSend("AT+CPMS=\"ME\",\"ME\",\"ME\"\r\n");
	else
		Gsm_SerialSend("AT+CPMS=\"SM\",\"SM\",\"SM\"\r\n");	
}
//########################################################################################################################
void			Gsm_MsgSetServiceCenterAddress(char *ServiceCenter)
{
	char str[32];
	snprintf(str,sizeof(str),"AT+CSCA=\"%s\",145\r\n",ServiceCenter);
	Gsm_SerialSend(str);
	osDelay(200);
	Gsm_MsgGetServiceCenterAddress();
}
//########################################################################################################################
void			Gsm_MsgGetServiceCenterAddress(void)
{
	Gsm_SerialSend("AT+CSCA?\r\n");
}
//########################################################################################################################
uint8_t		Gsm_MsgGetFreeSpace(void)
{		
	Gsm_SerialSend("AT+CPMS?\r\n");
	osDelay(200);
	return Gsm.Answer.MsgCapacity-Gsm.Answer.MsgUsed;
}
//########################################################################################################################
void 	Gsm_MsgGetUnreadIndex(void)
{
	Gsm_SerialSend("AT+CMGL=\"ALL\"\r\n");	
}
//########################################################################################################################
void	Gsm_MsgSetTextModeParameter(uint8_t fo,uint8_t vp,uint8_t pid,uint8_t dcs)
{
	char str[32];
	snprintf(str,sizeof(str),"AT+CSMP=%d,%d,%d,%d\r\n",fo,vp,pid,dcs);
	Gsm_SerialSend(str);	
	osDelay(200);
	Gsm_MsgGetTextModeParameter();	
}
//########################################################################################################################
void	Gsm_MsgGetTextModeParameter(void)
{
	Gsm_SerialSend("AT+CSMP?\r\n");
	osDelay(200);
}
//########################################################################################################################


//########################################################################################################################
//########################################################################################################################
//########################################################################################################################
GsmRecord_t	Gsm_WavGetStatus(void)
{
	Gsm_SerialSend("AT+CREC?\r\n");
	osDelay(100);
	return Gsm.Answer.Record;	
}
//########################################################################################################################
bool	Gsm_WavRecord(uint8_t Index,uint16_t	RecordingLimitTime_Second)
{
	char str[32];		
	Gsm.Answer.OK = 0;
	Gsm.Answer.ERROR = 0;
	sprintf(str,"AT+CREC=1,%d,0,%d\r\n",Index,RecordingLimitTime_Second);
	Gsm_SerialSend(str);
	Gsm_SerialSend("AT+CREC?\r\n");
	osDelay(200);	
	if(Gsm.Answer.OK == 1)
		return true;
	else
		return false;
}
//########################################################################################################################
bool	Gsm_WavPlay(uint8_t Index,uint8_t Vol_0_to_100)
{
	char str[32];		
	Gsm.Answer.OK = 0;
	Gsm.Answer.ERROR = 0;
	sprintf(str,"AT+CREC=4,%d,0,%d\r\n",Index,Vol_0_to_100);
	Gsm_SerialSend(str);
	Gsm_SerialSend("AT+CREC?\r\n");	
	osDelay(200);
	if(Gsm.Answer.OK == 1)
		return true;
	else
		return false;
}
//########################################################################################################################


//########################################################################################################################
//########################################################################################################################
//########################################################################################################################



