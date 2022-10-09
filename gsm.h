
#ifndef _GSM_H_
#define _GSM_H_

/*
 *	Author:     Nima Askari
 *	WebSite:    https://www.github.com/NimaLTD
 *	Instagram:  https://www.instagram.com/github.NimaLTD
 *	LinkedIn:   https://www.linkedin.com/in/NimaLTD
 *	Youtube:    https://www.youtube.com/channel/UCUhY7qY1klJm1d2kulr9ckw
 */

/*
 * Version:	5.1.6
 *
 * History:
 *
 * (5.1.6): Add fs and ssl cert file.
 * (5.1.5): Fix ftp_upload, gprs_connect.
 * (5.1.4): Fix http_read.
 * (5.1.3): Auto turn on after turn off if needed
 * (5.1.2): Fix read MQTT message. Add MQTT disconnect callback.
 * (5.1.2): Fix HTTP GET/POST.
 * (5.1.0): Add MQTT. 
 * (5.0.1): Fix GPRS connecting. 
 * (5.0.0):	Rewrite again. Support NONE-RTOS, RTOS V1 and RTOS V2.
 */
 
#include "gsmConfig.h"
#include "atcConfig.h"
#include "atc.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#if (_GSM_DEBUG == 1)
#define gsm_printf(...)     printf(__VA_ARGS__)
#else
#define gsm_printf(...)     {};
#endif
#define _GSM_RTOS           _ATC_RTOS
typedef enum
{
  gsm_tone_dialTone = 1,
  gsm_tone_calledSubscriberBusy = 2,
  gsm_tone_congestion = 3,
  gsm_tone_radioPathAcknowledge = 4,
  gsm_tone_rdioPathNotAvailableOrCallDropped = 5,
  gsm_tone_errorOrSpecialinformation = 6,
  gsm_tone_callWaitingTone = 7,
  gsm_tone_ringingTone = 8,
  gsm_tone_generalBeep = 16,
  gsm_tone_positiveAcknowledgementTone = 17,
  gsm_tone_negativeAcknowledgementOrErrorTone = 18,
  gsm_tone_indianDialTone = 19,
  gsm_tone_americanDialTone = 20

}gsm_tone_t;

typedef enum
{
  gsm_msg_chSet_error = 0,
  gsm_msg_chSet_gsm,
  gsm_msg_chSet_ucs2,
  gsm_msg_chSet_ira,
  gsm_msg_chSet_hex,
  gsm_msg_chSet_pccp,
  gsm_msg_chSet_pcdn,
  gsm_msg_chSet_8859_1

}gsm_msg_chset_t;

typedef enum
{
  gsm_msg_store_error = 0,
  gsm_msg_store_simcard,
  gsm_msg_store_module,
  gsm_msg_store_simcard_preferred,
  gsm_msg_store_module_preferred,
  gsm_msg_store_simcard_or_module,

}gsm_msg_store_t;

typedef enum
{
  gsm_ftp_error_none = 0,
  gsm_ftp_error_error = 1,
  gsm_ftp_error_netError = 61,
  gsm_ftp_error_dnsError = 62,
  gsm_ftp_error_connectError = 63,
  gsm_ftp_error_timeout = 64,
  gsm_ftp_error_serverError = 65,
  gsm_ftp_error_operationNotAllow = 66,
  gsm_ftp_error_replayError = 70,
  gsm_ftp_error_userError = 71,
  gsm_ftp_error_passwordError = 72,
  gsm_ftp_error_typeError = 73,
  gsm_ftp_error_restError = 74,
  gsm_ftp_error_passiveError = 75,
  gsm_ftp_error_activeError = 76,
  gsm_ftp_error_operateError = 77,
  gsm_ftp_error_uploadError = 78,
  gsm_ftp_error_downloadError = 79,
  gsm_ftp_error_manualQuit = 86,
  gsm_ftp_error_notExist = 100,

}gsm_ftp_error_t;

typedef struct
{
  uint8_t           year;
  uint8_t           month;
  uint8_t           day;
  uint8_t           hour;
  uint8_t           minute;
  uint8_t           second;

}gsm_time_t;

#if (_GSM_CALL == 1)
typedef struct
{
  uint8_t           newCall;
  uint8_t           endCall;
  char              number[16];
  uint8_t           dtmfCount;
  uint8_t           dtmfUpdate;
  char              dtmfBuffer[16];

}gsm_call_t;
#endif

#if (_GSM_MSG == 1)
typedef struct
{
  uint8_t           textMode;
  gsm_msg_chset_t   characterSet;
  gsm_msg_store_t   storage;
  uint16_t          storageTotal;
  uint16_t          storageUsed;
  gsm_time_t        time;
  int16_t           newMsg;
  char              status[16];
  char              number[16];

}gsm_msg_t;
#endif

#if (_GSM_GPRS == 1)
typedef struct
{
  bool              connected;
  bool              connectedLast;
  char              ip[16];
  uint32_t          dataLen;
  uint32_t          dataCurrent;
  int16_t           code;
  uint8_t           tcpConnection;
  uint8_t           gotData;
  uint32_t          ftpExtOffset;
  char              mqttTopic[64];
  char              mqttMessage[64];
  uint8_t           mqttData;
  uint8_t           mqttConnected;
  uint8_t           mqttConnectedLast;

}gsm_gprs_t;
#endif

typedef struct
{
  uint8_t           power:1;
  uint8_t           registerd:1;
  uint8_t           netReg:1;
  uint8_t           netChange:1;
  uint8_t           simcardChecked:1;
  uint8_t           turnOff:1;
  uint8_t           turnOn:1;
  #if(_GSM_SIM_DETECTOR == 1)
    uint8_t           simDetCangeInterruptFlag:1;
  #endif

}gsm_status_t;

typedef struct
{
  uint8_t           inited;
  uint8_t           lock;
  uint8_t           error;  
  uint8_t           signal;
  gsm_status_t      status;
  atc_t             atc;
  uint8_t           buffer[_ATC_RXSIZE - 16];

#if (_GSM_CALL == 1)
  gsm_call_t        call;
#endif
#if (_GSM_MSG == 1)
  gsm_msg_t         msg;
#endif
#if (_GSM_GPRS == 1)
  gsm_gprs_t       gprs;
#endif

}gsm_t;

extern  gsm_t   gsm;
//###############################################################################################################
#define         gsm_delay(x)            atc_delay(x)
#define         gsm_command(...)        atc_command(&gsm.atc, __VA_ARGS__)
#define         gsm_transmit(data,len)  atc_transmit(&gsm.atc,data,len)
#define         gsm_rxCallback()        atc_rxCallback(&gsm.atc)

bool            gsm_init(void);
void            gsm_loop(void);
bool            gsm_power(bool on_off);
bool            gsm_lock(uint32_t timeout_ms);
void            gsm_unlock(void);

bool            gsm_setDefault(void);
bool            gsm_registered(void);
bool            gsm_setDefault(void);
bool            gsm_saveProfile(void);
bool            gsm_enterPinPuk(const char* string);
bool            gsm_getIMEI(char* string, uint8_t sizeOfString);
bool            gsm_getVersion(char* string, uint8_t sizeOfString);
bool            gsm_getModel(char* string, uint8_t sizeOfString);
bool            gsm_getServiceProviderName(char* string, uint8_t sizeOfString);
uint8_t         gsm_getSignalQuality_0_to_100(void);
bool            gsm_waitForRegister(uint8_t waitSecond);
bool            gsm_tonePlay(gsm_tone_t gsm_tone_, uint32_t durationMiliSecond, uint8_t level_0_100);
bool            gsm_toneStop(void);
bool            gsm_dtmf(char *string, uint32_t durationMiliSecond);
bool            gsm_ussd(char *command, char *answer, uint16_t sizeOfAnswer, uint8_t waitSecond);
bool 		        gsm_isNumberExistInPhonebook(char* number, uint8_t from, uint8_t to);
bool 		        gsm_getPhonebookNumber(uint16_t index, char* getnumber);
//###############################################################################################################
bool            gsm_call_answer(void);
bool            gsm_call_dial(const char *number, uint8_t waitSecond);
bool            gsm_call_end(void);
//###############################################################################################################
bool            gsm_msg_textMode(bool on_off, bool integer);
bool            gsm_msg_isTextMode(void);
bool            gsm_msg_selectStorage(gsm_msg_store_t gsm_msg_store_);
bool            gsm_msg_selectCharacterSet(gsm_msg_chset_t gsm_msg_chset_);
bool            gsm_msg_deleteAll(void);
bool            gsm_msg_delete(uint16_t index);
bool            gsm_msg_send(const char *number,const char *msg);
bool            gsm_msg_read(uint16_t index);
bool            gsm_msg_updateStorage(void);
uint16_t        gsm_msg_getStorageUsed(void);
uint16_t        gsm_msg_getStorageTotal(void);
uint16_t        gsm_msg_getStorageFree(void);
//###############################################################################################################
bool            gsm_gprs_setApName(const char *apName);
bool            gsm_gprs_connect(void);
bool            gsm_gprs_disconnect(void);

bool 						gsm_gprs_cert(char *file_name);

bool            gsm_gprs_httpInit(void);
bool            gsm_gprs_httpSetContent(const char *content);
bool            gsm_gprs_httpSetUserData(const char *data);
bool            gsm_gprs_httpSendData(const char *data, uint16_t timeout_ms);
int16_t         gsm_gprs_httpGet(const char *url, bool ssl, uint16_t timeout_ms);
int16_t         gsm_gprs_httpPost(const char *url, bool ssl, uint16_t timeout_ms);
uint32_t        gsm_gprs_httpDataLen(void);
uint16_t        gsm_gprs_httpRead(uint8_t *data, uint16_t len);
bool            gsm_gprs_httpTerminate(void);

gsm_ftp_error_t gsm_gprs_ftpLogin(const char *ftpAddress, const char *ftpUserName, const char *ftpPassword, uint16_t port);
gsm_ftp_error_t gsm_gprs_ftpUploadBegin(bool asciiFile, bool append, const char *path, const char *fileName, const uint8_t *data, uint16_t len);
gsm_ftp_error_t gsm_gprs_ftpUpload(const uint8_t *data, uint16_t len);
gsm_ftp_error_t gsm_gprs_ftpUploadEnd(void);
gsm_ftp_error_t gsm_gprs_ftpExtUploadBegin(bool asciiFile, bool append, const char *path, const char *fileName);
gsm_ftp_error_t gsm_gprs_ftpExtUpload(uint8_t *data, uint16_t len);
gsm_ftp_error_t gsm_gprs_ftpExtUploadEnd(void);
gsm_ftp_error_t gsm_gprs_ftpCreateDir(const char *path);
gsm_ftp_error_t gsm_gprs_ftpRemoveDir(const char *path);
uint32_t        gsm_gprs_ftpGetSize(const char *path, const char *name);
gsm_ftp_error_t gsm_gprs_ftpRemove(const char *path, const char *name);
gsm_ftp_error_t gsm_gprs_ftpIsExistFolder(const char *path);
bool            gsm_gprs_ftpIsBusy(void);
gsm_ftp_error_t gsm_gprs_ftpQuit(void);

bool            gsm_gprs_ntpServer(char *server, int8_t time_zone_in_quarter);
bool            gsm_gprs_ntpSyncTime(void);
bool            gsm_gprs_ntpGetTime(char *string);

bool            gsm_gprs_mqttConnect(const char *url, uint16_t port, bool cleanFlag, const char *clientID, uint16_t keepAliveSec, const char *user, const char *pass, uint16_t timeoutSec);
bool            gsm_gprs_mqttDisConnect(void);
bool            gsm_gprs_mqttSubscribe(const char *topic, bool qos);
bool            gsm_gprs_mqttUnSubscribe(const char *topic);
bool            gsm_gprs_mqttPublish(const char *topic, bool qos, bool retain, const char *message);
//###############################################################################################################
bool 						gsm_fs_create(char *file_name);
bool 						gsm_fs_write(char *file_name, bool append, char *data);
uint16_t 				gsm_fs_get_size(char *file_name);
//###############################################################################################################
void            gsm_callback_simcardReady(void);
void            gsm_callback_simcardPinRequest(void);
void            gsm_callback_simcardPukRequest(void);
void            gsm_callback_simcardNotInserted(void);
void            gsm_callback_networkRegister(void);
void            gsm_callback_networkUnregister(void);
void            gsm_callback_newCall(const char *number);
void            gsm_callback_endCall(void);
void            gsm_callback_dtmf(char *string, uint8_t len);
void            gsm_callback_newMsg(char *number, gsm_time_t time, char *msg);
void            gsm_callback_gprsConnected(void);
void            gsm_callback_gprsDisconnected(void);
void            gsm_callback_mqttMessage(char *topic, char *message);
void            gsm_callback_mqttDisconnect(void);
void            gsm_callback_networkNotFound(void);
void            gsm_callback_simDetectorISR(void);
//###############################################################################################################
#endif /* _GSM_H_ */
