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
  Sim80xWave_Idle=0,
  Sim80xWave_Recording,
  Sim80xWave_Playing,
  
}Sim80xWave_t;
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
  GsmVoiceCallReturn_IAnswerCall,
  GsmVoiceCallReturn_Ringing,
  
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
typedef enum
{
  BluetoothStatus_Error=-1,
	BluetoothStatus_Initial=0,
	BluetoothStatus_Disactivating=1,
	BluetoothStatus_Activating=2,
	BluetoothStatus_Idle=5,
	BluetoothStatus_Scanning=6,
	BluetoothStatus_Inquiry_Res_Ind=7,
	BluetoothStatus_StoppingScanning=8,
	BluetoothStatus_Bonding=9,
	BluetoothStatus_Connecting=12,
	BluetoothStatus_Unpairing=13,
	BluetoothStatus_DeletingPairedDevice=14,
	BluetoothStatus_DeletingAllPairedDevice=15,
	BluetoothStatus_Disconnecting=16,
	BluetoothStatus_PairingConfirmWhilePassivePairing=19,
	BluetoothStatus_WaitingForRemoteConfirmWhilePassivePairing=20,
	BluetoothStatus_AcceptingConnection=25,
	BluetoothStatus_SDC_Refreshing=26,
	BluetoothStatus_SettingHostName=29,
	BluetoothStatus_ReleasingAllConnection=30,
	BluetoothStatus_ReleasingConnection=31,
	BluetoothStatus_ActivatingService=36,
	
}BluetoothStatus_t;
//######################################################################################################################
typedef struct
{
  char                  SendCommand[128];
  char                  ReceiveAnswer[10][64];
  uint32_t              SendCommandStartTime;
  uint32_t              ReceiveAnswerExeTime;
  uint16_t              ReceiveAnswerMaxWaiting;  
  uint8_t               FindAnswer; 
  
}Sim80xAtCommand_t;
//######################################################################################################################
typedef struct
{
  uint8_t               RegisterdToNetwork;
  uint8_t               DataTransferMode:1;
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

  uint8_t               MsgTextModeParameterFo;
  uint8_t               MsgTextModeParameterVp;
  uint8_t               MsgTextModeParameterPid;
  uint8_t               MsgTextModeParameterDcs;
  char                  MsgServiceNumber[16];
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
typedef enum
{
  BluetoothProfile_NotSet=0,
  BluetoothProfile_GAP,
  BluetoothProfile_SDAP,
  BluetoothProfile_SSP,
  BluetoothProfile_GOEP,
  BluetoothProfile_OPP,
  BluetoothProfile_HSP_HFP,
  BluetoothProfile_A2DP,  
  
}BluetoothProfile_t;
//######################################################################################################################
typedef struct
{
  uint8_t               Visibility:1;
  uint8_t               NeedGetStatus:1;
  BluetoothStatus_t     Status;
  char                  HostName[19];
  char                  HostAddress[19];
  char                  PairingPassword[17];
  uint8_t               ConnectedID;
  char                  ConnectedName[19];
  char                  ConnectedAddress[19];
  BluetoothProfile_t    ConnectedProfile[5];
  BluetoothProfile_t    ConnectingRequestProfile;
  uint16_t              SPPLen;
  char                  SPPBuffer[1024];  
  
}Sim80xBluetooth_t;
//######################################################################################################################
typedef enum
{
  GPRSConnection_Idle=0,
  GPRSConnection_AlreadyConnect,
  GPRSConnection_ConnectOK,
  GPRSConnection_ConnectFail,    
  
}GPRSConnection_t;
//######################################################################################################################
typedef enum
{
  GPRSSendData_Idle=0,
  GPRSSendData_SendOK,
  GPRSSendData_SendFail,
  
}GPRSSendData_t;
//######################################################################################################################
typedef enum
{
  GPRSHttpMethod_GET=0,
  GPRSHttpMethod_POST=1,
  GPRSHttpMethod_HEAD=2,
  GPRSHttpMethod_DELETE=3,
  
}GPRSHttpMethod_t;
//######################################################################################################################
typedef struct 
{
  uint8_t                 CopyToBuffer;
  GPRSHttpMethod_t        Method;
  uint16_t                ResultCode;
  uint32_t                DataLen;
  uint32_t                TransferStartAddress;
  uint16_t                TransferDataLen;
  char                    Data[256];
  
}GPRSHttpAction_t;
//######################################################################################################################
typedef struct
{
  uint8_t               MultiConnection:1;
  char                  APN[17];
  char                  APN_UserName[17];
  char                  APN_Password[17];
  char                  LocalIP[17]; 
  GPRSConnection_t      Connection[5];
  GPRSSendData_t        SendStatus[5];
  
  GPRSHttpAction_t      HttpAction;  
  
}GPRS_t;
//######################################################################################################################
typedef struct
{
  uint32_t              BufferStartTime;
  uint8_t               BufferExeTime;
  
	uint16_t	            UsartRxIndex;
	uint8_t		            UsartRxTemp;
	uint8_t		            UsartRxBuffer[_SIM80X_BUFFER_SIZE];
	uint32_t	            UsartRxLastTime;
  //
  Sim80xWave_t          WaveState;
  //
  char                  IMEI[16];
  uint8_t               RingVol;
  uint8_t               LoadVol;
	//
  Sim80xStatus_t        Status;
  //
  Sim80xAtCommand_t     AtCommand;
  //
  Sim80xGsm_t           Gsm;
  //
  #if (_SIM80X_USE_BLUETOOTH==1)
  Sim80xBluetooth_t     Bluetooth;
  #endif
	//
  #if (_SIM80X_USE_GPRS==1)
  GPRS_t                GPRS;
  #endif
  
}Sim80x_t;
//######################################################################################################################

extern Sim80x_t         Sim80x;
//######################################################################################################################
//######################################################################################################################
//######################################################################################################################
void	                  Sim80x_SendString(char *str);
void                    Sim80x_SendRaw(uint8_t *Data,uint16_t len);
uint8_t                 Sim80x_SendAtCommand(char *AtCommand,int32_t  MaxWaiting_ms,uint8_t HowMuchAnswers,...);
//######################################################################################################################
void                    Sim80x_UserInit(void);
void				            Sim80x_RxCallBack(void);
void				            Sim80x_Init(osPriority Priority);
void                    Sim80x_SetPower(bool TurnOn);
void                    Sim80x_SetFactoryDefault(void);
void                    Sim80x_GetIMEI(char *IMEI);
uint8_t                 Sim80x_GetRingVol(void);
bool                    Sim80x_SetRingVol(uint8_t Vol_0_to_100);
uint8_t                 Sim80x_GetLoadVol(void);
bool                    Sim80x_SetLoadVol(uint8_t Vol_0_to_100);
Sim80xWave_t            Sim80x_WaveGetState(void);
bool                    Sim80x_WaveRecord(uint8_t ID_1_to_10,uint8_t TimeLimitInSecond);  
bool                    Sim80x_WavePlay(uint8_t ID_1_to_10);  
bool                    Sim80x_WaveStop(void);  
bool                    Sim80x_WaveDelete(uint8_t ID_1_to_10);
//######################################################################################################################
void                    Gsm_User(uint32_t StartupTime);
void                    Gsm_UserNewCall(const char *CallerNumber);
void                    Gsm_UserNewMsg(char *Number,char *Date,char *Time,char *msg);

bool                    Gsm_Ussd(char *send,char *receive);

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
bool                    Gsm_MsgGetServiceNumber(void);
bool                    Gsm_MsgSetServiceNumber(char *ServiceNumber);
bool                    Gsm_MsgGetTextModeParameter(void);
bool                    Gsm_MsgSetTextModeParameter(uint8_t fo,uint8_t vp,uint8_t pid,uint8_t dcs);
bool                    Gsm_MsgSendText(char *Number,char *msg);  
//######################################################################################################################
void                    Bluetooth_UserNewPairingRequest(char *Name,char *Address,char *Pass);
void                    Bluetooth_UserConnectingSpp(void);
void                    Bluetooth_UserNewSppData(char *NewData,uint16_t len);
bool                    Bluetooth_SetPower(bool TurnOn);
bool                    Bluetooth_GetHostName(void);  
bool                    Bluetooth_SetHostName(char *HostName);
BluetoothStatus_t       Bluetooth_GetStatus(void);
bool                    Bluetooth_AcceptPair(bool Accept);  
bool                    Bluetooth_AcceptPairWithPass(char *Pass);  
bool                    Bluetooth_SetAutoPair(bool  Enable);
bool                    Bluetooth_SetPairPassword(char  *Pass);
bool                    Bluetooth_Unpair(uint8_t  Unpair_0_to_all);  
bool                    Bluetooth_GetVisibility(void);
void                    Bluetooth_SetVisibility(bool Visible);
void                    Bluetooth_SppAllowConnection(bool Accept);
bool                    Bluetooth_SppSend(char *DataString);
//######################################################################################################################
bool                    GPRS_DeactivatePDPContext(void);
bool                    GPRS_GetAPN(char *Name,char *username,char *password);
bool                    GPRS_SetAPN(char *Name,char *username,char *password);
bool                    GPRS_StartUpGPRS(void);
void                    GPRS_GetLocalIP(char *IP);
void                    GPRS_GetCurrentConnectionStatus(void);
bool                    GPRS_GetMultiConnection(void);
bool                    GPRS_SetMultiConnection(bool Enable);


void                    GPRS_UserHttpGetAnswer(char *data,uint32_t StartAddress,uint16_t dataLen);
bool                    GPRS_ConnectToNetwork(char *Name,char *username,char *password,bool EnableMultiConnection);
bool                    GPRS_HttpGet(char *URL);

//######################################################################################################################

#endif
