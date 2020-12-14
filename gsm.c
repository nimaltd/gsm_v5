#include "gsm.h"

gsm_t gsm;

//###############################################################################################################
void gsm_found(char *found_str)
{
  char *str;

  str = strstr(found_str, "POWER DOWN\r\n");
  if (str != NULL)
  {
    gsm.status.powerDown = 1;
    return;
  }
  str = strstr(found_str, "\r\n+CREG: ");
  if (str != NULL)
  {
    if (strstr(str, "\r\n+CREG: 1\r\n") != NULL)
    {
      gsm.status.netReg = 1;
      gsm.status.netChange = 1;
      return;
    }
    if (strstr(str, "\r\n+CREG: 0\r\n") != NULL)
    {
      gsm.status.netReg = 0;
      gsm.status.netChange = 1;
      return;
    }
  }
#if (_GSM_CALL == 1)
  str = strstr(found_str, "\r\n+CLIP:");
  if (str != NULL)
  {
    if (sscanf(str, "\r\n+CLIP: \"%[^\"]\"", gsm.call.number) == 1)
      gsm.call.newCall = 1;
    return;
  }
  str = strstr(found_str, "\r\nNO CARRIER\r\n");
  if (str != NULL)
  {
    gsm.call.endCall = 1;
    return;
  }
  str = strstr(found_str, "\r\n+DTMF:");
  if (str != NULL)
  {
    char c = 0;
    if (sscanf(str, "\r\n+DTMF: %c\r\n", &c) == 1)
    {
      if (gsm.call.dtmfCount < sizeof(gsm.call.dtmfBuffer) - 1)
      {
        gsm.call.dtmfBuffer[gsm.call.dtmfCount] = c;
        gsm.call.dtmfCount++;
        gsm.call.dtmfUpdate = 1;
      }
    }
    return;
  }
#endif
#if (_GSM_MSG == 1)
  str = strstr(found_str, "\r\n+CMTI:");
  if (str != NULL)
  {
    str = strchr(str, ',');
    if (str != NULL)
    {
      str++;
      gsm.msg.newMsg = atoi(str);
      return;
    }
  }
#endif
#if (_GSM_GPRS == 1)
  str = strstr(found_str, "");
#endif
#if (_GSM_BLUETOOTH == 1)
  str = strstr(found_str, "");
#endif
}
//###############################################################################################################
void gsm_init_commands(void)
{
  gsm_command("AT&F0\r\n", 5000, NULL, 0, 1, "\r\nOK\r\n");
  gsm_command("ATE1\r\n", 1000, NULL, 0, 1, "\r\nOK\r\n");
  gsm_command("AT+CREG=1\r\n", 1000, NULL, 0, 1, "\r\nOK\r\n");
  gsm_command("AT+FSHEX=0\r\n", 1000, NULL, 0, 1, "\r\nOK\r\n");
#if (_GSM_CALL == 1)
  gsm_command("AT+COLP=1\r\n", 1000, NULL, 0, 1, "\r\nOK\r\n");
  gsm_command("AT+CLIP=1\r\n", 1000, NULL, 0, 1, "\r\nOK\r\n");
  gsm_command("AT+DDET=1\r\n", 1000, NULL, 0, 1, "\r\nOK\r\n");
#endif
#if (_GSM_MSG == 1)
  gsm_msg_textMode(true);
  gsm_msg_selectStorage(gsm_msg_store_module);
  gsm_msg_selectCharacterSet(gsm_msg_chSet_ira);
#endif
#if (_GSM_GPRS == 1)

#endif
#if (_GSM_BLUETOOTH == 1)

#endif

}
//###############################################################################################################
bool gsm_init(void)
{
  if (gsm.inited == 1)
    return true;
  gsm_printf("[GSM] init begin\r\n");
  HAL_GPIO_WritePin(_GSM_KEY_GPIO, _GSM_KEY_PIN, GPIO_PIN_SET);
  memset(&gsm, 0, sizeof(gsm));
  atc_init(&gsm.atc, "GSM ATC", _GSM_USART, gsm_found);
  if (atc_addSearch(&gsm.atc, "POWER DOWN\r\n") == false)
    return false;
  if (atc_addSearch(&gsm.atc, "\r\n+CREG:") == false)
    return false;
#if (_GSM_CALL == 1)
  if (atc_addSearch(&gsm.atc, "\r\n+CLIP:") == false)
    return false;
  if (atc_addSearch(&gsm.atc, "\r\nNO CARRIER\r\n") == false)
    return false;
  if (atc_addSearch(&gsm.atc, "\r\n+DTMF:") == false)
    return false;
#endif
#if (_GSM_MSG == 1)
  if (atc_addSearch(&gsm.atc, "\r\n+CMTI:") == false)
    return false;
  gsm.msg.newMsg = -1;
#endif
#if (_GSM_GPRS == 1)

#endif
#if (_GSM_BLUETOOTH == 1)

#endif
  gsm_printf("[GSM] init done\r\n");
  gsm.inited = 1;
  return true;
}
//###############################################################################################################
void gsm_loop(void)
{
  static uint32_t gsm_time_1s = 0;
  static uint32_t gsm_time_10s = 0;
  static uint32_t gsm_time_60s = 0;
  atc_loop(&gsm.atc);
  //  +++ 1s timer  ######################
  if (HAL_GetTick() - gsm_time_1s > 1000)
  {
    gsm_time_1s = HAL_GetTick();

    //  +++ simcard check
    if (gsm.status.power == 1 && gsm.status.simcardChecked == 0)
    {
      char str1[64];
      char str2[16];
      if (gsm_command("AT+CPIN?\r\n", 1000, str1, sizeof(str1), 2, "\r\n+CPIN:", "\r\nERROR\r\n") == 1)
      {
        if (sscanf(str1, "\r\n+CPIN: %[^\r\n]", str2) == 1)
        {
          if (strcmp(str2, "READY") == 0)
          {
            gsm_callback_simcardReady();
            gsm.status.simcardChecked = 1;
          }
          if (strcmp(str2, "SIM PIN") == 0)
          {
            gsm_callback_simcardPinRequest();
            gsm.status.simcardChecked = 1;
          }
          if (strcmp(str2, "SIM PUK") == 0)
          {
            gsm_callback_simcardPukRequest();
            gsm.status.simcardChecked = 1;
          }
        }
      }
      else
      {
        gsm_callback_simcardNotInserted();
      }
    }
    //  --- simcard check

    //  +++ network check
    if (gsm.status.netChange == 1)
    {
      gsm.status.netChange = 0;
      if (gsm.status.netReg == 1)
      {
        gsm.status.registerd = 1;
        gsm_callback_networkRegister();
      }
      else
      {
        gsm.status.registerd = 0;
        gsm_callback_networkUnregister();
      }
    }
    //  --- network check

    //  +++ call check
#if (_GSM_CALL == 1)
    if (gsm.call.newCall == 1)
    {
      gsm.call.newCall = 0;
      gsm_callback_newCall(gsm.call.number);
    }
    if (gsm.call.endCall == 1)
    {
      gsm.call.endCall = 0;
      gsm_callback_endCall();
    }
    if (gsm.call.dtmfUpdate == 1)
    {
      gsm.call.dtmfUpdate = 0;
      gsm_callback_dtmf(gsm.call.dtmfBuffer, gsm.call.dtmfCount);
    }
#endif
    //  --- call check

    //  +++ msg check
#if (_GSM_MSG == 1)
    if (gsm.msg.newMsg >= 0)
    {
      if (gsm_msg_read(gsm.msg.newMsg))
      {
        gsm_callback_newMsg(gsm.msg.number, gsm.msg.time, (char*) gsm.buffer);
        gsm_msg_delete(gsm.msg.newMsg);
      }
      gsm.msg.newMsg = -1;
    }
#endif

  }
  //  --- 1s timer  ######################

  //  +++ 10s timer ######################
  if (HAL_GetTick() - gsm_time_10s > 10000)
  {
    gsm_time_10s = HAL_GetTick();

    //  +++ check network
    gsm_getSignalQuality_0_to_100();
    if (gsm.status.netReg == 0)
    {
      if (gsm_command("AT+CREG?\r\n", 1000 , NULL, 0, 1, "\r\n+CREG: 1,1\r\n") == 1)
      {
        gsm.status.netReg = 1;
        gsm.status.netChange = 1;
      }
    }
    //  --- check network

    //  +++ msg check
#if (_GSM_MSG == 1)
    if (gsm.msg.storageUsed > 0)
    {
      for (uint16_t i = 0; i < 150; i++)
      {
        if (gsm_msg_read(i))
        {
          gsm_callback_newMsg(gsm.msg.number, gsm.msg.time, (char*) gsm.buffer);
          gsm_msg_delete(i);
        }
      }
      gsm_msg_updateStorage();
    }
#endif
    //  --- msg check

  }
  //  --- 10s timer ######################

  //  +++ 60s timer  ######################
  if (HAL_GetTick() - gsm_time_60s > 60000)
  {
    gsm_time_60s = HAL_GetTick();

    //  +++ msg check
#if (_GSM_MSG == 1)
    gsm_msg_updateStorage();
#endif
    //  --- msg check

  }
  //  --- 60s timer  ######################

}
//###############################################################################################################
bool gsm_power(bool on_off)
{
  uint8_t state = 0;
  gsm.status.power = on_off;
  for (uint8_t i = 0; i < 5; i++)
  {
    if (gsm_command("AT\r\n", 1000, NULL, 0, 1, "\r\nOK\r\n") == 1)
    {
      state = 1;
      break;
    }
  }
  if (on_off == true && state == 1)
  {
    memset(&gsm.status, 0, sizeof(gsm.status));
    gsm_init_commands();
    return true;
  }
  if (on_off == true && state == 0)
  {
    memset(&gsm.status, 0, sizeof(gsm.status));
    HAL_GPIO_WritePin(_GSM_KEY_GPIO, _GSM_KEY_PIN, GPIO_PIN_RESET);
    gsm_delay(1200);
    HAL_GPIO_WritePin(_GSM_KEY_GPIO, _GSM_KEY_PIN, GPIO_PIN_SET);
    gsm_delay(2000);
    for (uint8_t i = 0; i < 5; i++)
    {
      if (gsm_command("AT\r\n", 1000, NULL, 0, 1, "\r\nOK\r\n") == 1)
      {
        state = 1;
        break;
      }
    }
    if (state == 1)
    {
      gsm_delay(5000);
      gsm_init_commands();
      return true;
    }
    else
    {
      return false;
    }
  }
  if (on_off == false && state == 0)
  {
    return true;
  }
  if (on_off == false && state == 1)
  {
    HAL_GPIO_WritePin(_GSM_KEY_GPIO, _GSM_KEY_PIN, GPIO_PIN_RESET);
    gsm_delay(1200);
    HAL_GPIO_WritePin(_GSM_KEY_GPIO, _GSM_KEY_PIN, GPIO_PIN_SET);
    gsm_delay(2000);
    return true;
  }
  return false;
}
//###############################################################################################################
bool gsm_registered(void)
{
  return gsm.status.registerd;
}
//###############################################################################################################
bool gsm_setDefault(void)
{
  if (gsm_command("AT&F0\r\n", 5000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
    return true;
  return false;
}
//###############################################################################################################
bool gsm_saveProfile(void)
{
  if (gsm_command("AT&W\r\n", 5000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
    return true;
  return false;
}
//###############################################################################################################
bool gsm_enterPinPuk(const char *string)
{
  char str[32];
  if (string == NULL)
    return false;
  sprintf(str, "AT+CPIN=%s\r\n", string);
  if (gsm_command(str, 5000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
    return false;
  return true;
}
//###############################################################################################################
bool gsm_getIMEI(char *string, uint8_t sizeOfString)
{
  if ((string == NULL) || (sizeOfString < 15))
    return false;
  char str[32];
  if (gsm_command("AT+GSN\r\n", 1000 , str, sizeof(str), 2, "AT+GSN", "\r\nERROR\r\n") != 1)
    return false;
  if (sscanf(str, "\r\nAT+GSN\r\n %[^\r\n]", string) != 1)
    return false;
  return true;
}
//###############################################################################################################
bool gsm_getVersion(char *string, uint8_t sizeOfString)
{
  if (string == NULL)
    return false;
  char str1[16 + sizeOfString];
  char str2[sizeOfString + 1];
  if (gsm_command("AT+CGMR\r\n", 1000 , str1, sizeof(str1), 2, "AT+GMM", "\r\nERROR\r\n") != 1)
    return false;
  if (sscanf(str1, "\r\nAT+CGMR\r\n %[^\r\n]", str2) != 1)
    return false;
  strncpy(string, str2, sizeOfString);
  return true;
}
//###############################################################################################################
bool gsm_getModel(char *string, uint8_t sizeOfString)
{
  if (string == NULL)
    return false;
  char str1[16 + sizeOfString];
  char str2[sizeOfString + 1];
  if (gsm_command("AT+GMM\r\n", 1000 , str1, sizeof(str1), 2, "AT+GMM", "\r\nERROR\r\n") != 1)
    return false;
  if (sscanf(str1, "\r\nAT+GMM\r\n %[^\r\n]", str2) != 1)
    return false;
  strncpy(string, str2, sizeOfString);
  return true;
}
//###############################################################################################################
bool gsm_getServiceProviderName(char *string, uint8_t sizeOfString)
{
  if (string == NULL)
    return false;
  char str1[16 + sizeOfString];
  char str2[sizeOfString + 1];
  if (gsm_command("AT+CSPN?\r\n", 1000 , str1, sizeof(str1), 2, "\r\n+CSPN:", "\r\nERROR\r\n") != 1)
    return false;
  if (sscanf(str1, "\r\n+CSPN: \"%[^\"]\"", str2) != 1)
    return false;
  strncpy(string, str2, sizeOfString);
  return true;
}
//###############################################################################################################
uint8_t gsm_getSignalQuality_0_to_100(void)
{
  char str[32];
  int16_t p1, p2;
  if (gsm_command("AT+CSQ\r\n", 1000, str, sizeof(str), 2, "\r\n+CSQ:", "\r\nERROR\r\n") != 1)
    return 0;
  if (sscanf(str, "\r\n+CSQ: %hd,%hd\r\n", &p1, &p2) != 2)
    return 0;
  if (p1 == 99)
    gsm.signal = 0;
  else
    gsm.signal = (p1 * 100) / 31;
  return gsm.signal;
}
//###############################################################################################################
bool gsm_waitForRegister(uint8_t waitSecond)
{
  uint32_t startTime = HAL_GetTick();
  uint8_t idx = 0;
  while (HAL_GetTick() - startTime < (waitSecond * 1000))
  {
    gsm_delay(100);
    if (gsm.status.netReg == 1)
      return true;
    if (gsm.inited == 0)
      continue;
    idx++;
    if (idx % 10 == 0)
    {
      if (gsm_command("AT+CREG?\r\n", 1000, NULL, 0, 1, "\r\n+CREG: 1,1\r\n") == 1)
      {
        gsm.status.netReg = 1;
        gsm.status.netChange = 1;
        return true;
      }
    }
  }
  return false;
}
//###############################################################################################################
bool gsm_tonePlay(gsm_tone_t gsm_tone_, uint32_t durationMiliSecond, uint8_t level_0_100)
{
  char str[32];
  sprintf(str, "AT+SNDLEVEL=0,%d\r\n", level_0_100);
  if (gsm_command(str, 5000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
    return false;
  sprintf(str, "AT+STTONE=1,%d,%d\r\n", gsm_tone_, (int) durationMiliSecond);
  if (gsm_command(str, 5000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
    return false;
  return true;
}
//###############################################################################################################
bool gsm_toneStop(void)
{
  if (gsm_command("AT+STTONE=0\r\n", 5000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
    return true;
  return false;
}
//###############################################################################################################
bool gsm_dtmf(char *string, uint32_t durationMiliSecond)
{
  char str[32];
  sprintf(str, "AT+VTS=\"%s\",%d\r\n", string, (int) (durationMiliSecond / 100));
  if (gsm_command(str, 5000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
    return false;
  return true;
}
//###############################################################################################################
bool gsm_ussd(char *command, char *answer, uint16_t sizeOfAnswer, uint8_t waitSecond)
{
  if (command == NULL)
  {
    if (gsm_command("AT+CUSD=2\r\n", 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      return false;
    return true;
  }
  else if (answer == NULL)
  {
    char str[16 + strlen(command)];
    sprintf(str, "AT+CUSD=0,\"%s\"\r\n", command);
    if (gsm_command(str, waitSecond * 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      return false;
    return true;
  }
  else
  {
    memset(answer, 0, sizeOfAnswer);
    char str[16 + strlen(command)];
    sprintf(str, "AT+CUSD=1,\"%s\"\r\n", command);
    if (gsm_command(str, waitSecond * 1000, (char* )gsm.buffer, sizeof(gsm.buffer), 2, "\r\n+CUSD:", "\r\nERROR\r\n")
        != 1)
    {
      gsm_command("AT+CUSD=2\r\n", 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n");
      return false;
    }
    gsm_command("AT+CUSD=2\r\n", 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n");
    char *start = strstr((char*) gsm.buffer, "\"");
    if (start != NULL)
    {
      char *end = strstr(start, "\", ");
      if (end != NULL)
      {
        start++;
        strncpy(answer, start, end - start);
        return true;
      }
      else
        return false;
    }
    else
      return false;
  }
}
//###############################################################################################################

