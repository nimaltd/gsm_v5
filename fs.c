#include "gsm.h"

#if (_GSM_FS == 1)

//###############################################################################################################
bool gsm_fs_create(char *file_name)
{
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] fs_create() failed!\r\n");
    return false;
  }
	sprintf((char*)gsm.buffer, "AT+FSCREATE=C:\\USER\\%s\r\n", file_name);
  if (gsm_command((char*)gsm.buffer, 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
	{
		gsm_printf("[GSM] fs_create() failed!\r\n");
    gsm_unlock();
    return false;
	}
  gsm_printf("[GSM] fs_create() done\r\n");
  gsm_unlock();
  return true;
}
//###############################################################################################################
bool gsm_fs_write(char *file_name, bool append, char *data)
{
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] gsm_fs_write() failed!\r\n");
    return false;
  }
	sprintf((char*)gsm.buffer, "AT+FSWRITE=C:\\USER\\%s,%d,%d,10\r\n", file_name, append, strlen(data));
  if (gsm_command((char*)gsm.buffer, 2000, NULL, 0, 2, "\r\n>", "\r\nERROR\r\n") != 1)
	{
		gsm_printf("[GSM] gsm_fs_write() failed!\r\n");
    gsm_unlock();
    return false;
	}
	gsm_delay(500);
	if (gsm_command(data, 10000, NULL, 0, 3, "\r\nOK\r\n", "\r\nERROR\r\n", "\r\nTimeOut\r\n") != 1)
	{
		gsm_printf("[GSM] gsm_fs_write() failed!\r\n");
    gsm_unlock();
    return false;
	}
	gsm_delay(1000);
  gsm_printf("[GSM] gsm_fs_write() done\r\n");
  gsm_unlock();
  return true;
}
//###############################################################################################################
uint16_t gsm_fs_get_size(char *file_name)
{
	if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] fs_get_size() failed!\r\n");
    return 0;
  }
	char buf[32];
	sprintf((char*)gsm.buffer, "AT+FSFLSIZE=C:\\USER\\%s\r\n", file_name);
  if (gsm_command((char*)gsm.buffer, 1000, buf, sizeof(buf), 2, "\r\n+FSFLSIZE:", "\r\nERROR\r\n") != 1)
	{
		gsm_printf("[GSM] fs_get_size() failed!\r\n");
    gsm_unlock();
    return 0;
	}
	uint16_t size;
	if (sscanf(buf, "\r\n+FSFLSIZE: %hd", &size) != 1)
	{
		gsm_printf("[GSM] fs_get_size() failed!\r\n");
    gsm_unlock();
    return 0;
	}		
  gsm_printf("[GSM] fs_get_size() done\r\n");
  gsm_unlock();
  return size;
}
//###############################################################################################################
#endif
