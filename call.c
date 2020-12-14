#include "gsm.h"

#if (_GSM_CALL == 1)
//###############################################################################################################
bool gsm_call_answer(void)
{
  if (gsm_command("ATA\r\n", 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
    return false;
  memset(gsm.call.dtmfBuffer, 0, sizeof(gsm.call.dtmfBuffer));
  gsm.call.dtmfCount = 0;
  gsm.call.dtmfUpdate = 0;
  return true;
}
//###############################################################################################################
bool gsm_call_dial(const char *number, uint8_t waitSecond)
{
  char str[32];
  memset(gsm.call.dtmfBuffer, 0, sizeof(gsm.call.dtmfBuffer));
  gsm.call.dtmfCount = 0;
  gsm.call.dtmfUpdate = 0;
  sprintf(str, "ATD%s;\r\n", number);
  uint8_t ans = gsm_command(str, waitSecond * 1000, NULL, 0, 5, "\r\nNO DIALTONE\r\n", "\r\nBUSY\r\n",
      "\r\nNO CARRIER\r\n", "\r\nNO ANSWER\r\n", "\r\nOK\r\n");
  if (ans == 5)
  {
    return true;
  }
  else
  {
    gsm_call_end();
    return false;
  }
}
//###############################################################################################################
bool gsm_call_end(void)
{
  if (gsm_command("ATH\r\n", 20000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
    return false;
  return true;
}
//#############################################################################################
#endif
