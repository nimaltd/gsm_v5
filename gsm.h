
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
 * Version:	5.0.0
 *
 * History:
 *
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

typedef struct
{
  uint8_t           power:1;
  uint8_t           registerd:1;
  uint8_t           netReg:1;
  uint8_t           netChange:1;
  uint8_t           simcardChecked:1;
  uint8_t           powerDown:1;

}gsm_status_t;

typedef struct
{
  uint8_t           inited;
  uint8_t           signal;
  gsm_status_t      status;
  atc_t             atc;
  uint8_t           buffer[_ATC_RXSIZE - 16];

#if (_GSM_CALL == 1)
  gsm_call_t        call;
#endif
#if (_GSM_CALL == 1)
  gsm_msg_t         msg;
#endif

}gsm_t;

extern  gsm_t   gsm;
//###############################################################################################################
#define       gsm_delay(x)            atc_delay(x)
#define       gsm_command(...)        atc_command(&gsm.atc, __VA_ARGS__)
#define       gsm_rxCallback()        atc_rxCallback(&gsm.atc)

bool          gsm_init(void);
void          gsm_loop(void);
bool          gsm_power(bool on_off);

bool          gsm_registered(void);
bool          gsm_setDefault(void);
bool          gsm_saveProfile(void);
bool          gsm_enterPinPuk(const char* string);
bool          gsm_getIMEI(char* string, uint8_t sizeOfString);
bool          gsm_getVersion(char* string, uint8_t sizeOfString);
bool          gsm_getModel(char* string, uint8_t sizeOfString);
bool          gsm_getServiceProviderName(char* string, uint8_t sizeOfString);
uint8_t       gsm_getSignalQuality_0_to_100(void);
bool          gsm_waitForRegister(uint8_t waitSecond);
bool          gsm_tonePlay(gsm_tone_t gsm_tone_, uint32_t durationMiliSecond, uint8_t level_0_100);
bool          gsm_toneStop(void);
bool          gsm_dtmf(char *string, uint32_t durationMiliSecond);
bool          gsm_ussd(char *command, char *answer, uint16_t sizeOfAnswer, uint8_t waitSecond);
//###############################################################################################################
bool          gsm_call_answer(void);
bool          gsm_call_dial(const char *number, uint8_t waitSecond);
bool          gsm_call_end(void);
//###############################################################################################################
bool          gsm_msg_textMode(bool on_off);
bool          gsm_msg_isTextMode(void);
bool          gsm_msg_selectStorage(gsm_msg_store_t gsm_msg_store_);
bool          gsm_msg_selectCharacterSet(gsm_msg_chset_t gsm_msg_chset_);
bool          gsm_msg_deleteAll(void);
bool          gsm_msg_delete(uint16_t index);
bool          gsm_msg_send(const char *number,const char *msg);
bool          gsm_msg_read(uint16_t index);
bool          gsm_msg_updateStorage(void);
uint16_t      gsm_msg_getStorageUsed(void);
uint16_t      gsm_msg_getStorageTotal(void);
uint16_t      gsm_msg_getStorageFree(void);
//###############################################################################################################
void          gsm_callback_simcardReady(void);
void          gsm_callback_simcardPinRequest(void);
void          gsm_callback_simcardPukRequest(void);
void          gsm_callback_simcardNotInserted(void);
void          gsm_callback_networkRegister(void);
void          gsm_callback_networkUnregister(void);
void          gsm_callback_newCall(const char *number);
void          gsm_callback_endCall(void);
void          gsm_callback_dtmf(char *string, uint8_t len);
void          gsm_callback_newMsg(char *number, gsm_time_t time, char *msg);
//###############################################################################################################
#endif /* _GSM_H_ */
