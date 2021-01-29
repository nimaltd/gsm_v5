#include "gsm.h"

#if (_GSM_CALL == 1)
//###############################################################################################################
bool gsm_call_answer(void)
{
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] call_answer() failed!\r\n");
    return false;
  }
  if (gsm_command("ATA\r\n", 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
  {
    gsm_printf("[GSM] call_answer() failed!\r\n");
    gsm_unlock();
    return false;
  }
  memset(gsm.call.dtmfBuffer, 0, sizeof(gsm.call.dtmfBuffer));
  gsm.call.dtmfCount = 0;
  gsm.call.dtmfUpdate = 0;
  gsm_printf("[GSM] call_answer() done\r\n");
  gsm_unlock();
  return true;
}
//###############################################################################################################
bool gsm_call_dial(const char *number, uint8_t waitSecond)
{
  gsm_printf("[GSM] call_dial() begin\r\n");
  char str[32];
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] call_dial() failed!\r\n");
    return false;
  }
  memset(gsm.call.dtmfBuffer, 0, sizeof(gsm.call.dtmfBuffer));
  gsm.call.dtmfCount = 0;
  gsm.call.dtmfUpdate = 0;
  sprintf(str, "ATD%s;\r\n", number);
  uint8_t ans = gsm_command(str, waitSecond * 1000, NULL, 0, 5, "\r\nNO DIALTONE\r\n", "\r\nBUSY\r\n",
      "\r\nNO CARRIER\r\n", "\r\nNO ANSWER\r\n", "\r\nOK\r\n");
  if (ans == 5)
  {
    gsm_printf("[GSM] call_dial() done\r\n");
    gsm_unlock();
    return true;
  }
  else
  {
    gsm_printf("[GSM] call_dial() failed!\r\n");
    gsm_unlock();
    gsm_call_end();
    return false;
  }
}
//###############################################################################################################
bool gsm_call_end(void)
{
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] call_end() failed!\r\n");
    return false;
  }
  if (gsm_command("ATH\r\n", 20000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
  {
    gsm_printf("[GSM] call_end() failed!\r\n");
    gsm_unlock();
    return false;
  }
  gsm_printf("[GSM] call_end() done\r\n");
  gsm_unlock();
  return true;
}
//#############################################################################################
#endif
