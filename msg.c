#include "gsm.h"
#include "pdu.h"

#if (_GSM_MSG == 1)
//###############################################################################################################
bool gsm_msg_updateStorage(void)
{
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] msg_updateStorage() failed!\r\n");
    return false;
  }
  char str[64];
  char s[5];
  if (gsm_command("AT+CPMS?\r\n", 1000 , str, sizeof(str), 2, "\r\n+CPMS:", "\r\nERROR\r\n") != 1)
  {
    gsm_printf("[GSM] msg_updateStorage() failed!\r\n");
    gsm_unlock();
    return false;
  }
  if (sscanf(str, "\r\n+CPMS: \"%[^\"]\",%hd,%hd,", s, &gsm.msg.storageUsed, &gsm.msg.storageTotal) != 3)
  {
    gsm_printf("[GSM] msg_updateStorage() failed!\r\n");
    gsm_unlock();
    return false;
  }
  if (strcmp(s, "SM") == 0)
    gsm.msg.storage = gsm_msg_store_simcard;
  else if (strcmp(s, "ME") == 0)
    gsm.msg.storage = gsm_msg_store_module;
  else if (strcmp(s, "SM_P") == 0)
    gsm.msg.storage = gsm_msg_store_simcard_preferred;
  else if (strcmp(s, "ME_P") == 0)
    gsm.msg.storage = gsm_msg_store_module_preferred;
  else if (strcmp(s, "MT") == 0)
    gsm.msg.storage = gsm_msg_store_simcard_or_module;
  else
    gsm.msg.storage = gsm_msg_store_error;
  gsm_printf("[GSM] msg_updateStorage() done\r\n");
  gsm_unlock();
  return true;
}
//###############################################################################################################
uint16_t gsm_msg_getStorageUsed(void)
{
  gsm_msg_updateStorage();
  return gsm.msg.storageUsed;
}
//###############################################################################################################
uint16_t gsm_msg_getStorageTotal(void)
{
  gsm_msg_updateStorage();
  return gsm.msg.storageTotal;
}
//###############################################################################################################
uint16_t gsm_msg_getStorageFree(void)
{
  gsm_msg_updateStorage();
  return gsm.msg.storageTotal - gsm.msg.storageUsed;
}
//###############################################################################################################
bool gsm_msg_textMode(bool on_off, bool integer)
{
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] msg_textMode() failed!\r\n");
    return false;
  }
  if (on_off)
  {
    if (gsm_command("AT+CMGF=1\r\n", 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") == 1)
    {
      gsm.msg.textMode = 1;
      if (integer == false)
      {
        gsm_command("AT+CSMP=17,167,0,0\r\n", 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n");
        gsm_printf("[GSM] msg_textMode() done. text: true, integer: false \r\n");
      }
      else
      {
        gsm_command("AT+CSMP=17,167,0,8\r\n", 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n");
        gsm_printf("[GSM] msg_textMode() done. text: true, integer: true \r\n");
      }
      gsm_unlock();
      return true;
    }
  }
  else
  {
    if (gsm_command("AT+CMGF=0\r\n", 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") == 1)
    {
      gsm.msg.textMode = 0;
      gsm_printf("[GSM] msg_textMode() done. text: false\r\n");
      gsm_unlock();
      return true;
    }
  }
  gsm_printf("[GSM] msg_textMode() failed!\r\n");
  gsm_unlock();
  return false;
}
//###############################################################################################################
bool gsm_msg_isTextMode(void)
{
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] msg_isTextMode() failed!\r\n");
    return false;
  }
  uint8_t ans;
  ans = gsm_command("AT+CMGF?\r\n", 1000, NULL, 0, 3, "\r\n+CMGF: 0\r\n", "\r\n+CMGF: 1\r\n", "\r\nERROR\r\n");
  if (ans == 1)
  {
    gsm.msg.textMode = 0;
    gsm_printf("[GSM] msg_isTextMode() done. false\r\n");
    gsm_unlock();
    return false;
  }
  else if (ans == 1)
  {
    gsm.msg.textMode = 1;
    gsm_printf("[GSM] msg_isTextMode() done. true\r\n");
    gsm_unlock();
    return true;
  }
  else
  {
    gsm_printf("[GSM] msg_isTextMode() failed!\r\n");
    gsm_unlock();
    return false;
  }
}
//###############################################################################################################
bool gsm_msg_deleteAll(void)
{
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] msg_deleteAll() failed!\r\n");
    return false;
  }
  if (gsm.msg.textMode)
  {
    if (gsm_command("AT+CMGDA=\"DEL ALL\"\r\n", 25000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
    {
      gsm_printf("[GSM] msg_deleteAll() failed!\r\n");
      gsm_unlock();
      return false;
    }
    gsm_printf("[GSM] msg_deleteAll() done\r\n");
    gsm_unlock();
    return true;
  }
  else
  {
    if (gsm_command("AT+CMGDA=6\r\n", 25000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
    {
      gsm_printf("[GSM] msg_deleteAll() failed!\r\n");
      gsm_unlock();
      return false;
    }
    gsm_printf("[GSM] msg_deleteAll() done\r\n");
    gsm_unlock();
    return true;
  }
}
//###############################################################################################################
bool gsm_msg_delete(uint16_t index)
{
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] msg_delete(%d) failed!\r\n", index);
    return false;
  }
  char str[32];
  sprintf(str, "AT+CMGD=%d\r\n", index);
  if (gsm_command(str, 5000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") == 1)
  {
    gsm_printf("[GSM] msg_delete(%d) done\r\n", index);
    gsm_unlock();
    return true;
  }
  gsm_printf("[GSM] msg_delete(%d) failed!\r\n", index);
  gsm_unlock();
  return false;
}
//###############################################################################################################
bool gsm_msg_send(const char *number, const char *msg)
{
  gsm_printf("[GSM] msg_send() begin\r\n");
  if ((number == NULL) || (msg == NULL))
  {
    gsm_printf("[GSM] msg_send() failed!\r\n");
    return false;
  }
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] msg_send() failed!\r\n");
    return false;
  }

  char str[32];
  // +++ text mode
  if (gsm.msg.textMode == 1)
  {
    sprintf(str, "AT+CMGS=\"%s\"\r\n", number);
    if (gsm_command(str, 5000 , NULL, 0, 2, "\r\r\n> ", "\r\nERROR\r\n") != 1)
    {
      sprintf(str, "%c", 27);
      gsm_command(str, 1000, NULL, 0, 0);
      gsm_printf("[GSM] msg_send() failed!\r\n");
      gsm_unlock();
      return false;
    }
    sprintf((char*) gsm.buffer, "%s%c", msg, 26);
    if (gsm_command((char*)gsm.buffer, 80000 , NULL, 0, 2, "\r\n+CMGS:", "\r\nERROR\r\n") != 1)
    {
      gsm_printf("[GSM] msg_send() failed!\r\n");
      gsm_unlock();
      return false;
    }
    gsm_printf("[GSM] msg_send() done\r\n");
    gsm_unlock();
    return true;
  }
  // --- text mode

  // +++ pdu mode
  else if(gsm.msg.textMode == 0)
  {		
    int messageLen = PDU_encode(NULL, false, false, false, 0, 0, number, 0, msg, 0);
    if(messageLen > 0)
    {
      sprintf(str, "AT+CMGS=%d\r\n", messageLen);
      if (gsm_command(str, 5000 , NULL, 0, 2, "\r\r\n> ", "\r\nERROR\r\n") != 1)
      {
        sprintf(str, "%c", 27);
        gsm_command(str, 1000, NULL, 0, 0);
        gsm_printf("[GSM] msg_send() failed!\r\n");
        gsm_unlock();
        return false;
      }
      sprintf((char *)gsm.buffer, "%s%c", PDU_getPDUBuffer(), 26);
      if(gsm_command((char *)gsm.buffer, 8000, NULL, 0, 2, "\r\n+CMGS:", "\r\nERROR\r\n") != 1)
      {
        gsm_printf("[GSM] msg_send() failed!\r\n");
        gsm_unlock();
        return false;
      }
      gsm_printf("[GSM] msg_send() done\r\n");
      gsm_unlock();
      return true;

    }
    gsm_printf("[GSM] msg_send() failed!\r\n");
    gsm_unlock();
    return false;
  }
  // --- pdu mode
}
//###############################################################################################################
bool gsm_msg_selectStorage(gsm_msg_store_t gsm_msg_store_)
{
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] msg_selectStorage() failed!\r\n");
    return false;
  }
  char str[64];
  switch (gsm_msg_store_)
  {
  case gsm_msg_store_simcard:
    sprintf(str, "AT+CPMS=\"SM\",\"SM\",\"SM\"\r\n");
    break;
  case gsm_msg_store_module:
    sprintf(str, "AT+CPMS=\"ME\",\"ME\",\"ME\"\r\n");
    break;
  case gsm_msg_store_simcard_preferred:
    sprintf(str, "AT+CPMS=\"SM_P\",\"SM_P\",\"SM_P\"\r\n");
    break;
  case gsm_msg_store_module_preferred:
    sprintf(str, "AT+CPMS=\"ME_P\",\"ME_P\",\"ME_P\"\r\n");
    break;
  case gsm_msg_store_simcard_or_module:
    sprintf(str, "AT+CPMS=\"MT\",\"MT\",\"MT\"\r\n");
    break;
  default:
    gsm_printf("[GSM] msg_selectStorage() failed!\r\n");
    gsm_unlock();
    return false;
  }
  if (gsm_command(str, 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
  {
    gsm_printf("[GSM] msg_selectStorage() failed!\r\n");
    gsm_unlock();
    return false;
  }
  gsm.msg.storage = gsm_msg_store_;
  gsm_printf("[GSM] msg_selectStorage() done\r\n");
  gsm_unlock();
  gsm_msg_updateStorage();
  return true;
}
//###############################################################################################################
bool gsm_msg_selectCharacterSet(gsm_msg_chset_t gsm_msg_chSet_)
{
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] msg_selectCharacterSet() failed!\r\n");
    return false;
  }
  char str[64];
  switch (gsm_msg_chSet_)
  {
  case gsm_msg_chSet_8859_1:
    sprintf(str, "AT+CSCS=\"8859-1\"\r\n");
    break;
  case gsm_msg_chSet_gsm:
    sprintf(str, "AT+CSCS=\"GSM\"\r\n");
    break;
  case gsm_msg_chSet_ira:
    sprintf(str, "AT+CSCS=\"IRA\"\r\n");
    break;
  case gsm_msg_chSet_pccp:
    sprintf(str, "AT+CSCS=\"PCCP\"\r\n");
    break;
  case gsm_msg_chSet_hex:
    sprintf(str, "AT+CSCS=\"HEX\"\r\n");
    break;
  case gsm_msg_chSet_ucs2:
    sprintf(str, "AT+CSCS=\"UCS2\"\r\n");
    break;
  case gsm_msg_chSet_pcdn:
    sprintf(str, "AT+CSCS=\"PCDN\"\r\n");
    break;
  default:
    gsm_printf("[GSM] msg_selectCharacterSet() failed!\r\n");
    gsm_unlock();
    return false;
  }
  if (gsm_command(str, 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
  {
    gsm_printf("[GSM] msg_selectCharacterSet() failed!\r\n");
    gsm_unlock();
    return false;
  }
  gsm.msg.characterSet = gsm_msg_chSet_;
  gsm_printf("[GSM] msg_selectCharacterSet() done\r\n");
  gsm_unlock();
  return true;
}
//###############################################################################################################
bool gsm_msg_read(uint16_t index)
{
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] msg_read(%d) failed!\r\n", index);
    return false;
  }
  char str[20];
  //  +++ text mode
  if (gsm.msg.textMode == 1)
  {
    uint16_t d[6];
    sprintf(str, "AT+CMGR=%d\r\n", index);
    if (gsm_command(str, 5000, (char* )gsm.buffer, sizeof(gsm.buffer), 3, "\r\n+CMGR:", "\r\nOK\r\n", "\r\nERROR\r\n")
        != 1)
    {
      gsm_printf("[GSM] msg_read(%d) failed!\r\n", index);
      gsm_unlock();
      return false;
    }
    sscanf((char*) gsm.buffer, "\r\n+CMGR: \"%[^\"]\",\"%[^\"]\",\"\",\"%hd/%hd/%hd,%hd:%hd:%hd%*d\"", gsm.msg.status,
        gsm.msg.number, &d[0], &d[1], &d[2], &d[3], &d[4], &d[5]);
    gsm.msg.time.year = d[0];
    gsm.msg.time.month = d[1];
    gsm.msg.time.day = d[2];
    gsm.msg.time.hour = d[3];
    gsm.msg.time.minute = d[4];
    gsm.msg.time.second = d[5];
    uint8_t cnt = 0;
    char *s = strtok((char*) gsm.buffer, "\"");
    while (s != NULL)
    {
      s = strtok(NULL, "\"");
      if (cnt == 6)
      {
        s += 2;
        char *end = strstr(s, "\r\nOK\r\n");
        if (end != NULL)
        {
          strncpy((char*) &gsm.buffer[0], s, end - s);
          memset(&gsm.buffer[end - s], 0, sizeof(gsm.buffer) - (end - s));
          gsm_printf("[GSM] msg_read(%d) done\r\n", index);
          gsm_unlock();
          return true;
        }
        else
        {
          gsm_printf("[GSM] msg_read(%d) failed!\r\n", index);
          gsm_unlock();
          return false;
        }
      }
      cnt++;
    }
  }
  //  --- text mode

  //  +++ pdu mode
  else if(gsm.msg.textMode == 0)
  {
		
    sprintf(str, "AT+CMGR=%d\r\n", index);
    if (gsm_command(str, 5000, (char* )gsm.buffer, sizeof(gsm.buffer), 3, "\r\n+CMGR:", "\r\nOK\r\n", "\r\nERROR\r\n")
        != 1)
    {
      gsm_printf("[GSM] msg_read(%d) failed!\r\n", index);
      gsm_unlock();
      return false;
    }
    char *end = strstr(gsm.buffer, "\r\n\r\nOK\r\n");
    char *s = strtok((char*) gsm.buffer, "\r\n");
		s = strtok(NULL, "\r\n");
		strncpy((char*) &gsm.buffer[0], s, end - s);
		memset(&gsm.buffer[end - s], 0, sizeof(gsm.buffer) - (end - s));
		
		gsm_printf("[GSM] msg_read(%d) done\r\n", index);
		gsm_unlock();
		return true;
  }
  //  --- pdu mode

  gsm_printf("[GSM] msg_read(%d) failed!\r\n", index);
  gsm_unlock();
  return false;
}
//###############################################################################################################
#endif
