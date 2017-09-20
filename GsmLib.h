#ifndef	_GSMLIB_H
#define	_GSMLIB_H

//			www.GitHub.com/NimaLTD
//			GsmLib Version2				
//			2.006


#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "main.h"
#include "usart.h"
#include "gpio.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GsmLibConf.h"

//########################################################################################################################
typedef enum
{
	GsmRecord_Idle=0,
	GsmRecord_Recording,
	GsmRecord_Playing,
	
}GsmRecord_t;
//########################################################################################################################
typedef	struct
{	
	uint8_t			Signal;							//	0~100 
	
	uint8_t			MsgCapacity;
	uint8_t			MsgUsed;
	
	
	uint8_t			BatteryPercent;			//	0~100
	uint8_t			BatteryCharging:1;	
	uint8_t			BatteryFull:1;
	
	uint8_t			SmsReady:1;
	uint8_t			CallReady:1;
	uint8_t			Power:1;
	
	uint8_t			OK:1;
	uint8_t			ERROR:1;
	
	uint8_t			CallAnswerIncoming:1;
	uint8_t			CallAnswerOutgoing:1;
	uint8_t			CallDisconnect:1;
	uint8_t			Calling:1;
	uint8_t			CallNoDialTone:1;
	uint8_t			CallBusy:1;
	uint8_t			CallNoCarrier:1;
	uint8_t			CallNoAnswer:1;
	uint8_t			CallDialling:1;
	
	uint8_t			UssdAnswer:1;
	
	uint8_t			MsgDeleteStatus:1;
	uint8_t			MsgSendStatus:1;
	uint8_t			MsgReadMsg:1;	
	uint8_t			MsgReadyToSend:2;
	
	GsmRecord_t	Record;
	
}GsmAnswer_t;
//########################################################################################################################
typedef enum
{
	GsmTECharacterSet_GSM,
	GsmTECharacterSet_UCS2,
	GsmTECharacterSet_IRA,
	GsmTECharacterSet_HEX,
	GsmTECharacterSet_PCCP,
	GsmTECharacterSet_PCDN,
	GsmTECharacterSet_8859_1,	
	
}GsmTECharacterSet_t;
//########################################################################################################################
typedef enum
{
	GsmTypeOfAddress_UnknownType=129,
	GsmTypeOfAddress_NationalNumberType=161,
	GsmTypeOfAddress_InternationalNumberType=145,
	GsmTypeOfAddress_NetworkSpecificNumber=177,	
	
}GsmTypeOfAddress_t;
//########################################################################################################################
typedef enum
{
	GsmMsgFormat_PDU,
	GsmMsgFormat_Text,
	
}GsmMsgFormat_t;
//########################################################################################################################
typedef struct
{
	GsmTECharacterSet_t		TECharacterSet;	
	GsmTypeOfAddress_t 		TypeOfAddress;
	GsmMsgFormat_t				MsgFormat;
	char 									MsgSeriviceCenter[16];
	uint8_t								MsgFo;
	uint8_t								MsgVp;
	uint8_t								MsgPid;
	uint8_t								MsgDcs;
	
}GsmConfig_t;
//########################################################################################################################
typedef struct
{
	char				SerialRxBuff[_GSM_BUFFER_SIZE];
	uint8_t			SerialTmp;
	uint16_t		SerialIndex;
	uint32_t		SerialLastTimeReceived;
	uint32_t		SerialLastTimeSend;
	uint32_t		SerialLastTimeFindAnswer;

	char				LastCallerNumber[16];
	char				LastMsgNumber[16];
	char				LastMsgDate[16];
	char				LastMsgTime[16];
	char				LastMsg[_GSM_BUFFER_SIZE];
	
	uint8_t     LastMsgDelete;
	uint8_t			LastMsgNew[10];

	GsmConfig_t Config;
	GsmAnswer_t	Answer;
	
}Gsm_t;
//########################################################################################################################
//########################################################################################################################
//########################################################################################################################
typedef enum
{
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

//########################################################################################################################
typedef	struct
{
	uint8_t								Power:1;
	uint8_t								PairRequest:1;
	uint8_t								Connected;
	BluetoothStatus_t			Status;
	char									Profile[8];
	char									HostName[18];
	char									HostAddress[18];
	char									PairDeviceName[18];
	char									PairDeviceAddress[18];
	uint32_t							PairDevicePassword;
	
	
}Bluetooth_t;

//########################################################################################################################



extern 		Gsm_t	Gsm;
extern  	Bluetooth_t	 Bluetooth;
//########################################################################################################################
void										Gsm_RxCallBack(void);
//########################################################################################################################
void										Gsm_Init(osPriority Priority);
//########################################################################################################################
bool										Gsm_CallAnswer(void);
bool										Gsm_CallDisconnect(void);
bool										Gsm_CallDial(char	*DialNumber,uint8_t	WaitForAnswerInSecond);
//########################################################################################################################
bool										Gsm_Ussd(char *Request,char *Answer);
//########################################################################################################################
void										Gsm_SetFactoryReset(void);
void										Gsm_SetMonitorSpeakerLoudness(uint8_t	Vol_0_9);
void										Gsm_SetRingLevel(uint8_t	Vol_0_100);
void										Gsm_SetLoudSpeakerVolumeLevel(uint8_t	Vol_0_100);
void										Gsm_SetMute(bool	Enable);
GsmTECharacterSet_t			Gsm_GetTECharacterSet(void);
void										Gsm_SetTECharacterSet(GsmTECharacterSet_t GsmTECharacterSet);
GsmTypeOfAddress_t			Gsm_GetTypeOfAddress(void);
void										Gsm_SetTypeOfAddress(GsmTypeOfAddress_t GsmTypeOfAddress);
//########################################################################################################################
uint8_t									Gsm_GetSignalQuality(void);
uint8_t									Gsm_GetBatteryStatus(void);	
//########################################################################################################################
bool										Gsm_MsgDelete(uint8_t index);
void										Gsm_MsgSetFormat(GsmMsgFormat_t GsmMsgFormat);
GsmMsgFormat_t					Gsm_MsgGetFormat(void);
bool										Gsm_MsgRead(uint8_t index);
bool										Gsm_MsgSend(char *Number,char *Message);
void										Gsm_MsgSetStoreOnSim(bool	Enable);
void										Gsm_MsgSetServiceCenterAddress(char *ServiceCenter);
void										Gsm_MsgGetServiceCenterAddress(void);
uint8_t									Gsm_MsgGetFreeSpace(void);
void										Gsm_MsgGetUnreadIndex(void);
void										Gsm_MsgSetTextModeParameter(uint8_t fo,uint8_t vp,uint8_t pid,uint8_t dcs);
void										Gsm_MsgGetTextModeParameter(void);
//########################################################################################################################
GsmRecord_t							Gsm_WavGetStatus(void);
bool										Gsm_WavRecord(uint8_t Index,uint16_t	RecordingLimitTime_Second);
bool										Gsm_WavPlay(uint8_t Index,uint8_t Vol_0_to_100);
//########################################################################################################################
bool										Bluetooth_On(bool	TurnOnTurnOff);
BluetoothStatus_t				Bluetooth_GetStatus(void);
void										Bluetooth_SetHostName(char *Name);
void										Bluetooth_GetHostName(void);
void										Bluetooth_PairAccept(void);
//########################################################################################################################



//########################################################################################################################
void										Gsm_UserGetCall(char *Number);
void										Gsm_UserGetMsg(char *Number,char *date,char *time,char *msg);
void										Gsm_UserRunEverySecond(void);
//########################################################################################################################
void										Bluetooth_UserPairRequest(char *DeviceName,char *DeviceAddress,uint32_t DevicePassword);
//########################################################################################################################
#endif
