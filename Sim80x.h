#ifndef	_SIM80X_H
#define	_SIM80X_H

#include "usart.h"
#include "Cmsis_OS.h"
#include "Sim80xConfig.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
//######################################################################################################################
//######################################################################################################################
//######################################################################################################################
typedef enum
{
  GsmVoiceCallReturn_Idle,
  GsmVoiceCallReturn_Error,
  GsmVoiceCallReturn_NoDialTone,
  GsmVoiceCallReturn_NoCarrier,
  GsmVoiceCallReturn_NoAnswer,
  GsmVoiceCallReturn_Busy,
  GsmVoiceCallReturn_OK,
  
}GsmVoiceCallReturn_t;
//######################################################################################################################
typedef enum
{
  GsmTECharacterSet_Error,
	GsmTECharacterSet_GSM,
	GsmTECharacterSet_UCS2,
	GsmTECharacterSet_IRA,
	GsmTECharacterSet_HEX,
	GsmTECharacterSet_PCCP,
	GsmTECharacterSet_PCDN,
	GsmTECharacterSet_8859_1,	
	
}GsmTECharacterSet_t;
//######################################################################################################################
typedef enum
{
  GsmMsgMemory_Error,
  GsmMsgMemory_OnSim,
	GsmMsgMemory_OnModule,
	
}GsmMsgMemory_t;
//######################################################################################################################
typedef enum
{
  GsmMsgFormat_Error,
  GsmMsgFormat_PDU,
	GsmMsgFormat_Text,
	
}GsmMsgFormat_t;
//######################################################################################################################
typedef struct
{
  char                  SendCommand[64];
  char                  ReceiveAnswer[10][64];
  uint32_t              SendCommandStartTime;
  uint32_t              ReceiveAnswerExeTime;
  uint16_t              ReceiveAnswerMaxWaiting;  
  uint8_t               FindAnswer; 
  
}Sim80xAtCommand_t;
//######################################################################################################################
typedef struct
{
  uint8_t               Busy:1;
  uint8_t               Power:1;
  uint8_t               SmsReady:1;  
  uint8_t               CallReady:1;  
 
  uint8_t               BatteryCharging:1;
  uint8_t               BatteryFull:1;
  uint8_t               BatteryPercent;
  float                 BatteryVoltage;
  
  uint8_t               Signal; 
  
}Sim80xStatus_t;
//######################################################################################################################
typedef struct
{
  uint8_t               HaveNewCall:1;
  uint8_t               MsgReadIsOK:1;

  GsmVoiceCallReturn_t  GsmVoiceCallReturn;         
  char                  CallerNumber[16];
  char                  DiallingNumber[16]; 

  char                  MsgSentNumber[16];
  char                  MsgNumber[16];
  char                  MsgDate[8];
  char                  MsgTime[8];
  char                  Msg[_SIM80X_BUFFER_SIZE]; 
  GsmTECharacterSet_t   TeCharacterFormat;
  GsmMsgMemory_t        MsgMemory;
  GsmMsgFormat_t        MsgFormat;
  uint8_t               MsgCapacity;
  uint8_t               MsgUsed;
  uint8_t               HaveNewMsg;  
  
}Sim80xGsm_t;
//######################################################################################################################
typedef struct
{
	uint16_t	            UsartRxIndex;
	uint8_t		            UsartRxTemp;
	uint8_t		            UsartRxBuffer[_SIM80X_BUFFER_SIZE];
	uint32_t	            UsartRxLastTime;
	//
  Sim80xStatus_t        Status;
  //
  Sim80xAtCommand_t     AtCommand;
  //
  Sim80xGsm_t           Gsm;
	
}Sim80x_t;
//######################################################################################################################

extern Sim80x_t         Sim80x;
//######################################################################################################################
//######################################################################################################################
//######################################################################################################################

uint8_t                 Sim80x_SendAtCommand(char *AtCommand,int32_t  MaxWaiting_ms,uint8_t HowMuchAnswers,...);
//######################################################################################################################
void				            Sim80x_RxCallBack(void);
void				            Sim80x_Init(osPriority Priority);
void                    Sim80x_SetPower(bool TurnOn);
//######################################################################################################################
void                    Gsm_User(uint32_t StartupTime);
void                    Gsm_UserHaveNewCall(const char *CallerNumber);
void                    Gsm_UserHaveNewMsg(char *Number,char *Date,char *Time,char *msg);

bool                    Gsm_CallAnswer(void);
bool                    Gsm_CallDisconnect(void);
GsmVoiceCallReturn_t    Gsm_Dial(char *Number,uint8_t WaitForAnswer_second); 

GsmMsgFormat_t          Gsm_MsgGetFormat(void);
bool                    Gsm_MsgSetFormat(GsmMsgFormat_t GsmMsgFormat);  
GsmMsgMemory_t          Gsm_MsgGetMemoryStatus(void);
bool                    Gsm_MsgSetMemoryLocation(GsmMsgMemory_t GsmMsgMemory);
GsmTECharacterSet_t     Gsm_MsgGetCharacterFormat(void);  
bool                    Gsm_MsgSetCharacterFormat(GsmTECharacterSet_t GsmTECharacterSet);
bool                    Gsm_MsgRead(uint8_t index);
bool                    Gsm_MsgDelete(uint8_t index);
//######################################################################################################################




#endif
