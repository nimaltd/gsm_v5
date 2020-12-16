#include "gsm.h"

#if (_GSM_CALL == 1)
//###############################################################################################################
bool gsm_call_answer(void)
{
  if (gsm_lock(10000) == false)
    return false;
  if (gsm_command("ATA\r\n", 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
  {
    gsm_unlock();
    return false;
  }
  memset(gsm.call.dtmfBuffer, 0, sizeof(gsm.call.dtmfBuffer));
  gsm.call.dtmfCount = 0;
  gsm.call.dtmfUpdate = 0;
  gsm_unlock();
  return true;
}
//###############################################################################################################
bool gsm_call_dial(const char *number, uint8_t waitSecond)
{
  char str[32];
  if (gsm_lock(10000) == false)
    return false;
  memset(gsm.call.dtmfBuffer, 0, sizeof(gsm.call.dtmfBuffer));
  gsm.call.dtmfCount = 0;
  gsm.call.dtmfUpdate = 0;
  sprintf(str, "ATD%s;\r\n", number);
  uint8_t ans = gsm_command(str, waitSecond * 1000, NULL, 0, 5, "\r\nNO DIALTONE\r\n", "\r\nBUSY\r\n",
      "\r\nNO CARRIER\r\n", "\r\nNO ANSWER\r\n", "\r\nOK\r\n");
  if (ans == 5)
  {
    gsm_unlock();
    return true;
  }
  else
  {
    gsm_unlock();
    gsm_call_end();
    return false;
  }
}
//###############################################################################################################
bool gsm_call_end(void)
{
  if (gsm_lock(10000) == false)
    return false;
  if (gsm_command("ATH\r\n", 20000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
  {
    gsm_unlock();
    return false;
  }
  gsm_unlock();
  return true;
}
//#############################################################################################
#endif
