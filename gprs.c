#include "gsm.h"

#if (_GSM_GPRS == 1)
//###############################################################################################################
bool gsm_gprs_setApName(const char *apName)
{
  if (apName == NULL)
  {
    gsm_printf("[GSM] gprs_setApName(%s) failed!\r\n", apName);
    return false;
  }
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] gprs_setApName(%s) failed!\r\n", apName);
    return false;
  }
  if (gsm_command("AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n", 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
  {
    gsm_printf("[GSM] gprs_setApName(%s) failed!\r\n", apName);
    gsm_unlock();
    return false;
  }
  sprintf((char*)gsm.buffer, "AT+SAPBR=3,1,\"APN\",\"%s\"\r\n", apName);
  if (gsm_command((char*)gsm.buffer, 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") == 1)
  {
    gsm_printf("[GSM] gprs_setApName(%s) done\r\n", apName);
    gsm_unlock();
    return true;
  }
  gsm_printf("[GSM] gprs_setApName(%s) failed!\r\n", apName);
  gsm_unlock();
  return false;
}
//###############################################################################################################
bool gsm_gprs_connect(void)
{
  gsm_printf("[GSM] gprs_connect() begin\r\n");
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] gprs_connect() failed!\r\n");
    return false;
  }
  gsm_command("AT+SAPBR=0,1\r\n", 5000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n");
  gsm_delay(2000);
  if (gsm_command("AT+SAPBR=1,1\r\n", 90000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
  {
    gsm.gprs.connected = false;
    gsm.gprs.connectedLast = false;
    gsm_printf("[GSM] gprs_connect() failed!\r\n");
    gsm_unlock();
    return false;
  }
  gsm_delay(2000);
  if (gsm_command("AT+SAPBR=2,1\r\n", 1000, (char*)gsm.buffer, sizeof(gsm.buffer), 2, "\r\n+SAPBR: 1,1,", "\r\nERROR\r\n") != 1)
  {
    gsm_printf("[GSM] gprs_connect() failed!\r\n");
    gsm.gprs.connected = false;
    gsm.gprs.connectedLast = false;
    gsm_unlock();
    return false;
  }
  memset(gsm.gprs.ip, 0, sizeof(gsm.gprs.ip));
  sscanf((char*)gsm.buffer, "\r\n+SAPBR: 1,1,\"%[^\"\r\n]", gsm.gprs.ip);
  gsm.gprs.connected = true;
  gsm.gprs.connectedLast = true;
  gsm_printf("[GSM] gprs_connect() done\r\n");
  gsm_unlock();
  return true;
}
//###############################################################################################################
bool gsm_gprs_disconnect(void)
{
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] gprs_disconnect() failed!\r\n");
    return false;
  }
  gsm.gprs.connected = false;
  gsm.gprs.connectedLast = false;
  gsm_command("AT+CIPSHUT\r\n", 5000, NULL, 0, 2, "\r\nSHUT OK\r\n", "\r\nERROR\r\n");
  if (gsm_command("AT+SAPBR=0,1\r\n", 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") == 1)
  {
    gsm_printf("[GSM] gprs_disconnect() done\r\n");
    gsm_unlock();
    return true;
  }
  gsm_printf("[GSM] gprs_disconnect() failed!\r\n");
  gsm_unlock();
  return false;
}
//###############################################################################################################
bool gsm_gprs_httpInit(void)
{
  if (gsm.gprs.connected == false)
  {
    gsm_printf("[GSM] gprs_httpInit() failed!\r\n");
    return false;
  }
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] gprs_httpInit() failed!\r\n");
    return false;
  }
  gsm_command("AT+HTTPTERM\r\n", 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n");
  if (gsm_command("AT+HTTPINIT\r\n", 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
  {
    gsm_printf("[GSM] gprs_httpInit() failed!\r\n");
    gsm_unlock();
    return false;
  }
  gsm.gprs.dataCurrent = 0;
  gsm_printf("[GSM] gprs_httpInit() done\r\n");
  gsm_unlock();
  return true;
}
//###############################################################################################################
bool gsm_gprs_httpSetContent(const char *content)
{
  if (gsm.gprs.connected == false)
  {
    gsm_printf("[GSM] gprs_httpSetContent() failed!\r\n");
    return false;
  }
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] gprs_httpSetContent() failed!\r\n");
    return false;
  }
  sprintf((char*)gsm.buffer, "AT+HTTPPARA=\"CONTENT\",\"%s\"\r\n", content);
  if (gsm_command((char*)gsm.buffer, 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
  {
    gsm_printf("[GSM] gprs_httpSetContent() failed!\r\n");
    gsm_unlock();
    return false;
  }
  gsm_printf("[GSM] gprs_httpSetContent() done\r\n");
  gsm_unlock();
  return true;
}
//###############################################################################################################
bool gsm_gprs_httpSetUserData(const char *data)
{
  if (gsm.gprs.connected == false)
  {
    gsm_printf("[GSM] gprs_httpSetUserData() failed!\r\n");
    return false;
  }
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] gprs_httpSetUserData() failed!\r\n");
    return false;
  }
  gsm_transmit((uint8_t* ) "AT+HTTPPARA=\"USERDATA\",\"", strlen("AT+HTTPPARA=\"USERDATA\",\""));
  gsm_transmit((uint8_t* ) data, strlen(data));
  if (gsm_command("\"\r\n", 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
  {
    gsm_printf("[GSM] gprs_httpSetUserData() failed!\r\n");
    gsm_unlock();
    return false;
  }
  gsm_printf("[GSM] gprs_httpSetUserData() done\r\n");
  gsm_unlock();
  return true;
}
//###############################################################################################################
bool gsm_gprs_httpSendData(const char *data, uint16_t timeout_ms)
{
  if (gsm.gprs.connected == false)
  {
    gsm_printf("[GSM] gprs_httpSendData() failed!\r\n");
    return false;
  }
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] gprs_httpSendData() failed!\r\n");
    return false;
  }
  sprintf((char*)gsm.buffer, "AT+HTTPDATA=%d,%d\r\n", strlen(data), timeout_ms);
  do
  {
    if (gsm_command((char*)gsm.buffer, timeout_ms, NULL, 0, 2, "\r\nDOWNLOAD\r\n", "\r\nERROR\r\n") != 1)
      break;
    if (gsm_command(data, timeout_ms, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    gsm_delay(timeout_ms);
    gsm_printf("[GSM] gprs_httpSendData() done\r\n");
    gsm_unlock();
    return true;
  } while (0);
  gsm_printf("[GSM] gprs_httpSendData() failed!\r\n");
  gsm_unlock();
  return false;
}
//###############################################################################################################
int16_t gsm_gprs_httpGet(const char *url, bool ssl, uint16_t timeout_ms)
{
  if (gsm.gprs.connected == false)
  {
    gsm_printf("[GSM] gprs_httpGet(%s) failed!\r\n", url);
    return -1;
  }
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] gprs_httpGet(%s) failed!\r\n", url);
    return false;
  }
  gsm.gprs.code = -1;
  gsm.gprs.dataLen = 0;
  do
  {
    if (gsm_command("AT+HTTPPARA=\"CID\",1\r\n", 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    sprintf((char*)gsm.buffer, "AT+HTTPPARA=\"URL\",\"%s\"\r\n", url);
    if (gsm_command((char*)gsm.buffer, 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    if (gsm_command("AT+HTTPPARA=\"REDIR\",1\r\n", 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    if (ssl)
    {
      if (gsm_command("AT+HTTPSSL=1\r\n", 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
        break;
    }
    else
    {
      if (gsm_command("AT+HTTPSSL=0\r\n", 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
        break;
    }
    if (gsm_command("AT+HTTPACTION=0\r\n", timeout_ms , (char*)gsm.buffer, sizeof(gsm.buffer), 2, "\r\n+HTTPACTION:", "\r\nERROR\r\n") != 1)
      break;
    sscanf((char*)gsm.buffer, "\r\n+HTTPACTION: 0,%hd,%d\r\n", &gsm.gprs.code, &gsm.gprs.dataLen);
  } while (0);
  gsm_printf("[GSM] gprs_httpGet(%s) done. answer: %d\r\n", url, gsm.gprs.code);
  gsm_unlock();
  return gsm.gprs.code;
}
//###############################################################################################################
int16_t gsm_gprs_httpPost(const char *url, bool ssl, uint16_t timeout_ms)
{
  if (gsm.gprs.connected == false)
  {
    gsm_printf("[GSM] gprs_httpPost(%s) failed!\r\n", url);
    return -1;
  }
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] gprs_httpPost(%s) failed!\r\n", url);
    return false;
  }
  gsm.gprs.code = -1;
  gsm.gprs.dataLen = 0;
  do
  {
    if (gsm_command("AT+HTTPPARA=\"CID\",1\r\n", 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    sprintf((char*)gsm.buffer, "AT+HTTPPARA=\"URL\",\"%s\"\r\n", url);
    if (gsm_command((char*)gsm.buffer, 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    if (gsm_command("AT+HTTPPARA=\"REDIR\",1\r\n", 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    if (ssl)
    {
      if (gsm_command("AT+HTTPSSL=1\r\n", 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
        break;
    }
    else
    {
      if (gsm_command("AT+HTTPSSL=0\r\n", 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
        break;
    }
    if (gsm_command("AT+HTTPACTION=1\r\n", timeout_ms , (char*)gsm.buffer, sizeof(gsm.buffer), 2, "\r\n+HTTPACTION:", "\r\nERROR\r\n") != 1)
      break;
    sscanf((char*)gsm.buffer, "\r\n+HTTPACTION: 1,%hd,%d\r\n", &gsm.gprs.code, &gsm.gprs.dataLen);
  } while (0);
  gsm_printf("[GSM] gprs_httpPost(%s) done. answer: %d\r\n", url, gsm.gprs.code);
  gsm_unlock();
  return gsm.gprs.code;
}
//###############################################################################################################
uint32_t gsm_gprs_httpDataLen(void)
{
  return gsm.gprs.dataLen;
}
//###############################################################################################################
uint16_t gsm_gprs_httpRead(uint8_t *data, uint16_t len)
{
  if (gsm.gprs.connected == false)
  {
    gsm_printf("[GSM] gprs_httpRead() failed!\r\n");
    return 0;
  }
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] gprs_httpRead() failed!\r\n");
    return false;
  }
  memset(gsm.buffer, 0, sizeof(gsm.buffer));
  if (len >= sizeof(gsm.buffer) - 32)
    len = sizeof(gsm.buffer) - 32;
  char buf[32];
  sprintf(buf, "AT+HTTPREAD=%d,%d\r\n", gsm.gprs.dataCurrent, len);
  if (gsm_command(buf, 1000 , (char*)gsm.buffer, sizeof(gsm.buffer), 2, "\r\n+HTTPREAD: ", "\r\nERROR\r\n") != 1)
  {
    gsm_printf("[GSM] gprs_httpRead() failed!\r\n");
    gsm_unlock();
    return 0;
  }
  if (sscanf((char*)gsm.buffer, "\r\n+HTTPREAD: %hd\r\n", &len) != 1)
  {
    gsm_printf("[GSM] gprs_httpRead() failed!\r\n");
    gsm_unlock();
    return 0;
  }
  gsm.gprs.dataCurrent += len;
  if (gsm.gprs.dataCurrent >= gsm.gprs.dataLen)
    gsm.gprs.dataCurrent = gsm.gprs.dataLen;
  
  uint8_t *s = (uint8_t*)strchr((char*)&gsm.buffer[10], '\n');  
  if (s == NULL)
  {
    gsm_printf("[GSM] gprs_httpRead() failed!\r\n");
    gsm_unlock();
    return 0;
  }
  s++;
  for (uint16_t i = 0 ; i < len; i++)
    gsm.buffer[i] = *s++;
  if (data != NULL)
    memcpy(data, gsm.buffer, len);
  gsm_printf("[GSM] gprs_httpRead() done. length: %d\r\n", len);
  gsm_unlock();
  return len;      
}
//###############################################################################################################
bool gsm_gprs_httpTerminate(void)
{
  if (gsm.gprs.connected == false)
  {
    gsm_printf("[GSM] gprs_httpTerminate() failed!\r\n");
    return false;
  }
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] gprs_httpTerminate() failed!\r\n");
    return false;
  }
  if (gsm_command("AT+HTTPTERM\r\n", 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") == 1)
  {
    gsm_printf("[GSM] gprs_httpTerminate() done\r\n");
    gsm_unlock();
    return true;
  }
  gsm_printf("[GSM] gprs_httpTerminate() failed!\r\n");
  gsm_unlock();
  return false;
}
//###############################################################################################################
gsm_ftp_error_t gsm_gprs_ftpLogin(const char *ftpAddress, const char *ftpUserName, const char *ftpPassword,
    uint16_t port)
{
  if (gsm.gprs.connected == false)
  {
    gsm_printf("[GSM] gprs_ftpLogin() failed!\r\n");
    return gsm_ftp_error_error;
  }
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] gprs_ftpLogin() failed!\r\n");
    return gsm_ftp_error_error;
  }
  gsm_ftp_error_t ret = gsm_ftp_error_error;
  do
  {
    if (gsm_command("AT+FTPMODE=1\r\n", 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    if (gsm_command("AT+FTPCID=1\r\n", 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    sprintf((char*)gsm.buffer, "AT+FTPSERV=\"%s\"\r\n", ftpAddress);
    if (gsm_command((char*)gsm.buffer, 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    sprintf((char*)gsm.buffer, "AT+FTPPORT=%d\r\n", port);
    if (gsm_command((char*)gsm.buffer, 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    sprintf((char*)gsm.buffer, "AT+FTPUN=\"%s\"\r\n", ftpUserName);
    if (gsm_command((char*)gsm.buffer, 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    sprintf((char*)gsm.buffer, "AT+FTPPW=\"%s\"\r\n", ftpPassword);
    if (gsm_command((char*)gsm.buffer, 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    ret = gsm_ftp_error_none;
  } while (0);
  gsm_printf("[GSM] gprs_ftpLogin() done\r\n");
  gsm_unlock();
  return ret;
}
//###############################################################################################################
gsm_ftp_error_t gsm_gprs_ftpUploadBegin(bool asciiFile, bool append, const char *path, const char *fileName,
    const uint8_t *data, uint16_t len)
{
  if (gsm.gprs.connected == false)
  {
    gsm_printf("[GSM] gprs_ftpUploadBegin(%s/%s) failed!\r\n", path, fileName);
    return gsm_ftp_error_error;
  }
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] gprs_ftpUploadBegin(%s/%s) failed!\r\n", path, fileName);
    return gsm_ftp_error_error;
  }
  char *s;
  char answer[64];
  gsm_ftp_error_t error = gsm_ftp_error_error;
  do
  {
    gsm_command("AT+FTPEXTPUT=0\r\n", 5000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n");
    if (asciiFile)
      sprintf((char*)gsm.buffer, "AT+FTPTYPE=\"A\"\r\n");
    else
      sprintf((char*)gsm.buffer, "AT+FTPTYPE=\"I\"\r\n");
    if (gsm_command((char*)gsm.buffer, 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    if (append)
      sprintf((char*)gsm.buffer, "AT+FTPPUTOPT=\"APPE\"\r\n");
    else
      sprintf((char*)gsm.buffer, "AT+FTPPUTOPT=\"STOR\"\r\n");
    if (gsm_command((char*)gsm.buffer, 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    sprintf((char*)gsm.buffer, "AT+FTPPUTPATH=\"%s\"\r\n", path);
    if (gsm_command((char*)gsm.buffer, 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    sprintf((char*)gsm.buffer, "AT+FTPPUTNAME=\"%s\"\r\n", fileName);
    if (gsm_command((char*)gsm.buffer, 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    if (gsm_command("AT+FTPPUT=1\r\n", 75000 , answer, sizeof(answer), 2, "\r\n+FTPPUT: 1,", "\r\nERROR\r\n") != 1)
      break;
    s = strchr(answer, ',');
    if (s == NULL)
      break;
    s++;
    if (atoi(s) != 1)
    {
      error = (gsm_ftp_error_t) atoi(s);
      break;
    }
    sprintf((char*)gsm.buffer, "AT+FTPPUT=2,%d\r\n", len);
    if (gsm_command((char*)gsm.buffer, 5000 , answer, sizeof(answer), 2, "\r\n+FTPPUT: 2,", "\r\nERROR\r\n") != 1)
      break;
    s = strchr(answer, ',');
    if (s == NULL)
      break;
    s++;
    if (atoi(s) != len)
      break;
    gsm_transmit((uint8_t* )data, len);
    gsm_command("", 120 * 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n");
    error = gsm_ftp_error_none;
  } while (0);
  gsm_printf("[GSM] gprs_ftpUploadBegin(%s/%s) done\r\n", path, fileName);
  gsm_unlock();
  return error;
}
//###############################################################################################################
gsm_ftp_error_t gsm_gprs_ftpUpload(const uint8_t *data, uint16_t len)
{
  if (gsm.gprs.connected == false)
  {
    gsm_printf("[GSM] gprs_ftpUpload() failed!\r\n");
    return gsm_ftp_error_error;
  }
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] gprs_ftpUpload() failed!\r\n");
    return gsm_ftp_error_error;
  }
  char *s;
  char answer[64];
  gsm_ftp_error_t error = gsm_ftp_error_error;
  do
  {
    sprintf((char*)gsm.buffer, "AT+FTPPUT=2,%d\r\n", len);
    if (gsm_command((char*)gsm.buffer, 5000 , answer, sizeof(answer), 2, "\r\n+FTPPUT: 2,", "\r\nERROR\r\n") != 1)
      break;
    s = strchr(answer, ',');
    if (s == NULL)
      return gsm_ftp_error_error;
    s++;
    if (atoi(s) != len)
      return gsm_ftp_error_error;
    gsm_transmit((uint8_t* )data, len);
    if (gsm_command("", 120 * 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    error = gsm_ftp_error_none;
  } while (0);
  gsm_printf("[GSM] gprs_ftpUpload() done\r\n");
  gsm_unlock();
  return error;
}
//###############################################################################################################
gsm_ftp_error_t gsm_gprs_ftpUploadEnd(void)
{
  if (gsm.gprs.connected == false)
  {
    gsm_printf("[GSM] gprs_ftpUploadEnd() failed!\r\n");
    return gsm_ftp_error_error;
  }
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] gprs_ftpUploadEnd() failed!\r\n");
    return gsm_ftp_error_error;
  }
  if (gsm_command("AT+FTPPUT=2,0\r\n", 5000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
  {
    gsm_printf("[GSM] gprs_ftpUploadEnd() failed!\r\n");
    gsm_unlock();
    return gsm_ftp_error_error;
  }
  gsm_printf("[GSM] gprs_ftpUploadEnd() done\r\n");
  gsm_unlock();
  return gsm_ftp_error_none;
}
//###############################################################################################################
gsm_ftp_error_t gsm_gprs_ftpExtUploadBegin(bool asciiFile, bool append, const char *path, const char *fileName)
{
  if (gsm.gprs.connected == false)
  {
    gsm_printf("[GSM] gprs_ftpExtUploadBegin() failed!\r\n");
    return gsm_ftp_error_error;
  }
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] gprs_ftpExtUploadBegin() failed!\r\n");
    return gsm_ftp_error_error;
  }
  gsm_ftp_error_t error = gsm_ftp_error_error;
  do
  {
    gsm_command("AT+FTPEXTPUT=0\r\n", 5000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n");
    gsm_delay(100);
    if (asciiFile)
      sprintf((char*)gsm.buffer, "AT+FTPTYPE=\"A\"\r\n");
    else
      sprintf((char*)gsm.buffer, "AT+FTPTYPE=\"I\"\r\n");
    if (gsm_command((char*)gsm.buffer, 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    gsm_delay(100);
    if (append)
      sprintf((char*)gsm.buffer, "AT+FTPPUTOPT=\"APPE\"\r\n");
    else
      sprintf((char*)gsm.buffer, "AT+FTPPUTOPT=\"STOR\"\r\n");
    if (gsm_command((char*)gsm.buffer, 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    gsm_delay(100);
    sprintf((char*)gsm.buffer, "AT+FTPPUTPATH=\"%s\"\r\n", path);
    if (gsm_command((char*)gsm.buffer, 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    gsm_delay(100);
    sprintf((char*)gsm.buffer, "AT+FTPPUTNAME=\"%s\"\r\n", fileName);
    if (gsm_command((char*)gsm.buffer, 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    gsm_delay(100);
    if (gsm_command("AT+FTPEXTPUT=1\r\n", 5000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    gsm.gprs.ftpExtOffset = 0;
    error = gsm_ftp_error_none;
  } while (0);
  gsm_printf("[GSM] gprs_ftpExtUploadBegin() done. answer: %d\r\n", error);
  gsm_unlock();
  return error;
}
//###############################################################################################################
gsm_ftp_error_t gsm_gprs_ftpExtUpload(uint8_t *data, uint16_t len)
{
  if (gsm.gprs.connected == false)
  {
    gsm_printf("[GSM] gprs_ftpExtUpload() failed!\r\n");
    return gsm_ftp_error_error;
  }
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] gprs_ftpExtUpload() failed!\r\n");
    return gsm_ftp_error_error;
  }
  gsm_ftp_error_t error = gsm_ftp_error_error;
  char answer[64];
  do
  {
    sprintf((char*)gsm.buffer, "AT+FTPEXTPUT=2,%d,%d,5000\r\n", gsm.gprs.ftpExtOffset, len);
    if (gsm_command((char*)gsm.buffer, 5000, answer, sizeof(answer), 2, "\r\n+FTPEXTPUT: ", "\r\nERROR\r\n") != 1)
      break;
    char *s = strchr(answer, ',');
    if (s == NULL)
      break;
    s++;
    uint32_t d = atoi(s);
    if (d != len)
      break;
    gsm_delay(100);
    gsm_transmit(data, len);
    if (gsm_command("", 5000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    gsm.gprs.ftpExtOffset += len;
    error = gsm_ftp_error_none;
  } while (0);
  gsm_printf("[GSM] gprs_ftpExtUpload() done. answer: %d\r\n", error);
  gsm_unlock();
  return error;
}
//###############################################################################################################
gsm_ftp_error_t gsm_gprs_ftpExtUploadEnd(void)
{
  if (gsm.gprs.connected == false)
  {
    gsm_printf("[GSM] gprs_ftpExtUploadEnd() failed!\r\n");
    return gsm_ftp_error_error;
  }
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] gprs_ftpExtUploadEnd() failed!\r\n");
    return gsm_ftp_error_error;
  }
  gsm_ftp_error_t error = gsm_ftp_error_error;
  do
  {
    if (gsm_command("AT+FTPPUT=1\r\n", 75000, (char*)gsm.buffer, sizeof(gsm.buffer), 2, "\r\n+FTPPUT: 1,", "\r\nERROR\r\n") != 1)
      break;
    char *s = strchr((char*)gsm.buffer, ',');
    if (s == NULL)
      break;
    s++;
    error = (gsm_ftp_error_t) atoi(s);
    gsm_command("AT+FTPEXTPUT=0\r\n", 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n");
  } while (0);
  gsm_printf("[GSM] gprs_ftpExtUploadEnd() done. answer: %d\r\n", error);
  gsm_unlock();
  return error;
}
//###############################################################################################################
gsm_ftp_error_t gsm_gprs_ftpCreateDir(const char *path)
{
  if (gsm.gprs.connected == false)
  {
    gsm_printf("[GSM] gprs_ftpCreateDir(%s) failed!\r\n", path);
    return gsm_ftp_error_error;
  }
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] gprs_ftpCreateDir(%s) failed!\r\n", path);
    return gsm_ftp_error_error;
  }
  gsm_ftp_error_t error = gsm_ftp_error_error;
  do
  {
    sprintf((char*)gsm.buffer, "AT+FTPGETPATH=\"%s\"\r\n", path);
    if (gsm_command((char*)gsm.buffer, 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    if (gsm_command("AT+FTPMKD\r\n", 75000 , (char*)gsm.buffer, sizeof(gsm.buffer), 2, "\r\n+FTPMKD: 1,", "\r\nERROR\r\n") != 1)
      break;
    char *s = strchr((char*)gsm.buffer, ',');
    if (s == NULL)
      break;
    s++;
    error = (gsm_ftp_error_t) atoi(s);
  } while (0);
  gsm_printf("[GSM] gprs_ftpCreateDir(%s) done. answer: %d\r\n", path, error);
  gsm_unlock();
  return error;
}
//###############################################################################################################
gsm_ftp_error_t gsm_gprs_ftpRemoveDir(const char *path)
{
  if (gsm.gprs.connected == false)
  {
    gsm_printf("[GSM] gprs_ftpRemoveDir(%s) failed!\r\n", path);
    return gsm_ftp_error_error;
  }
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] gprs_ftpRemoveDir(%s) failed!\r\n", path);
    return gsm_ftp_error_error;
  }
  gsm_ftp_error_t error = gsm_ftp_error_error;
  do
  {
    sprintf((char*)gsm.buffer, "AT+FTPGETPATH=\"%s\"\r\n", path);
    if (gsm_command((char*)gsm.buffer, 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    if (gsm_command("AT+FTPRMD\r\n", 75000 , (char*)gsm.buffer, sizeof(gsm.buffer), 2, "\r\n+FTPRMD: 1,", "\r\nERROR\r\n") != 1)
      break;
    char *s = strchr((char*)gsm.buffer, ',');
    if (s == NULL)
      break;
    s++;
    error = (gsm_ftp_error_t) atoi(s);
  } while (0);
  gsm_printf("[GSM] gprs_ftpRemoveDir(%s) done. answer: %d\r\n", path, error);
  gsm_unlock();
  return error;
}
//###############################################################################################################
uint32_t gsm_gprs_ftpGetSize(const char *path, const char *name)
{
  if (gsm.gprs.connected == false)
  {
    gsm_printf("[GSM] gprs_ftpGetSize(%s, %s) failed!\r\n", path, name);
    return 0;
  }
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] gprs_ftpGetSize(%s, %s) failed!\r\n", path, name);
    return 0;
  }
  uint32_t error = 0;
  do
  {
    sprintf((char*)gsm.buffer, "AT+FTPGETPATH=\"%s\"\r\n", path);
    if (gsm_command((char*)gsm.buffer, 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    sprintf((char*)gsm.buffer, "AT+FTPGETNAME=\"%s\"\r\n", name);
    if (gsm_command((char*)gsm.buffer, 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    if (gsm_command("AT+FTPSIZE\r\n", 75000, (char*)gsm.buffer, sizeof(gsm.buffer), 2, "\r\n+FTPSIZE: 1,", "\r\nERROR\r\n") != 1)
      break;
    char *s = strchr((char*)gsm.buffer, ',');
    if (s == NULL)
      break;
    s++;
    if (atoi(s) == 0)
    {
      s = strchr((char*)gsm.buffer, ',');
      if (s == NULL)
        break;
      s++;
      error = atoi(s);
    }
  } while (0);
  gsm_printf("[GSM] gprs_ftpGetSize(%s, %s) done. answer: %d\r\n", path, name, error);
  gsm_unlock();
  return error;
}
//###############################################################################################################
gsm_ftp_error_t gsm_gprs_ftpRemove(const char *path, const char *name)
{
  if (gsm.gprs.connected == false)
  {
    gsm_printf("[GSM] gprs_ftpRemove(%s, %s) failed!\r\n", path, name);
    return gsm_ftp_error_error;
  }
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] gprs_ftpRemove(%s, %s) failed!\r\n", path, name);
    return gsm_ftp_error_error;
  }
  gsm_ftp_error_t error = gsm_ftp_error_error;
  do
  {
    sprintf((char*)gsm.buffer, "AT+FTPGETPATH=\"%s\"\r\n", path);
    if (gsm_command((char*)gsm.buffer, 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    sprintf((char*)gsm.buffer, "AT+FTPGETNAME=\"%s\"\r\n", name);
    if (gsm_command((char*)gsm.buffer, 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    if (gsm_command("AT+FTPDELE\r\n", 75000, (char*)gsm.buffer, sizeof(gsm.buffer), 2, "\r\n+FTPDELE: 1,", "\r\nERROR\r\n") != 1)
      break;
    char *s = strchr((char*)gsm.buffer, ',');
    if (s == NULL)
      break;
    s++;
    error = (gsm_ftp_error_t) atoi(s);
  } while (0);
  gsm_printf("[GSM] gprs_ftpRemove(%s, %s) done. answer: %d\r\n", path, name, error);
  gsm_unlock();
  return error;
}
//###############################################################################################################
gsm_ftp_error_t gsm_gprs_ftpIsExistFolder(const char *path)
{
  if (gsm.gprs.connected == false)
  {
    gsm_printf("[GSM] gprs_ftpIsExistFolder(%s) failed!\r\n", path);
    return gsm_ftp_error_error;
  }
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] gprs_ftpIsExistFolder(%s) failed!\r\n", path);
    return gsm_ftp_error_error;
  }
  gsm_ftp_error_t error = gsm_ftp_error_error;
  char answer[32];
  do
  {
    sprintf((char*)gsm.buffer, "AT+FTPGETPATH=\"%s\"\r\n", path);
    if (gsm_command((char*)gsm.buffer, 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    if (gsm_command("AT+FTPLIST=1\r\n", 75000, answer, sizeof(answer), 2, "\r\n+FTPLIST: ", "\r\nERROR\r\n") != 1)
      break;
    uint8_t i1 = 0, i2 = 0;
    if (sscanf(answer, "\r\n+FTPLIST: %hhd,%hhd", &i1, &i2) != 2)
      break;
    if (i1 == 1 && i2 == 1)
    {
      gsm_command("AT+FTPQUIT\r\n", 75000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n");
      error = gsm_ftp_error_none;
      break;
    }
    if (i1 == 1 && i2 == 77)
    {
      error = gsm_ftp_error_notExist;
      break;
    }
  } while (0);
  gsm_printf("[GSM] gprs_ftpIsExistFolder(%s) done. answer: %d\r\n", path, error);
  gsm_unlock();
  return error;
}
//###############################################################################################################
bool gsm_gprs_ftpIsBusy(void)
{
  if (gsm.gprs.connected == false)
  {
    gsm_printf("[GSM] gprs_ftpIsBusy() failed!\r\n");
    return false;
  }
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] gprs_ftpIsBusy() failed!\r\n");
    return false;
  }
  if (gsm_command("AT+FTPSTATE\r\n", 75000, NULL, 0, 3, "\r\n+FTPSTATE: 0\r\n", "\r\n+FTPSTATE: 1\r\n",
      "\r\nERROR\r\n") == 1)
  {
    gsm_printf("[GSM] gprs_ftpIsBusy() done. true\r\n");
    gsm_unlock();
    return true;
  }
  gsm_printf("[GSM] gprs_ftpIsBusy() done. false\r\n");
  gsm_unlock();
  return false;
}
//###############################################################################################################
gsm_ftp_error_t gsm_gprs_ftpQuit(void)
{
  if (gsm.gprs.connected == false)
  {
    gsm_printf("[GSM] gprs_ftpQuit() failed!\r\n");
    return gsm_ftp_error_error;
  }
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] gprs_ftpQuit() failed!\r\n");
    return gsm_ftp_error_error;
  }
  if (gsm_command("AT+FTPQUIT\r\n", 5000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
  {
    gsm_printf("[GSM] gprs_ftpQuit() failed!\r\n");
    gsm_unlock();
    return gsm_ftp_error_error;
  }
  gsm_printf("[GSM] gprs_ftpQuit() done\r\n");
  gsm_unlock();
  return gsm_ftp_error_none;
}
//###############################################################################################################
bool gsm_gprs_ntpServer(char *server, int8_t time_zone_in_quarter)
{
  if (gsm.gprs.connected == false)
  {
    gsm_printf("[GSM] gprs_ntpServer(%s, %d) failed!\r\n", server, time_zone_in_quarter);
    return false;
  }
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] gprs_ntpServer(%s, %d) failed!\r\n", server, time_zone_in_quarter);
    return false;
  }
  sprintf((char*)gsm.buffer, "AT+CNTP=\"%s\",%d\r\n", server, time_zone_in_quarter);
  if (gsm_command((char*)gsm.buffer, 10000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
  {
    gsm_printf("[GSM] gprs_ntpServer(%s, %d) failed!\r\n", server, time_zone_in_quarter);
    gsm_unlock();
    return false;
  }
  gsm_printf("[GSM] gprs_ntpServer(%s, %d) done\r\n", server, time_zone_in_quarter);
  gsm_unlock();
  return true;
}
//###############################################################################################################
bool gsm_gprs_ntpSyncTime(void)
{
  if (gsm.gprs.connected == false)
  {
    gsm_printf("[GSM] gprs_ntpSyncTime() failed!\r\n");
    return false;
  }
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] gprs_ntpSyncTime() failed!\r\n");
    return false;
  }
  if (gsm_command("AT+CNTP\r\n", 10000, NULL, 0, 2, "\r\n+CNTP: 1\r\n", "\r\nERROR\r\n") != 1)
  {
    gsm_printf("[GSM] gprs_ntpSyncTime() failed!\r\n");
    gsm_unlock();
    return false;
  }
  gsm_printf("[GSM] gprs_ntpSyncTime() done\r\n");
  gsm_unlock();
  return true;
}
//###############################################################################################################
bool gsm_gprs_ntpGetTime(char *string)
{
  if (string == NULL)
  {
    gsm_printf("[GSM] gprs_ntpGetTime() failed!\r\n");
    return false;
  }
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] gprs_ntpGetTime() failed!\r\n");
    return false;
  }
  if (gsm_command("AT+CCLK?\r\n", 10000, (char*)gsm.buffer, sizeof(gsm.buffer), 2, "\r\n+CCLK:", "\r\nERROR\r\n") != 1)
  {
    gsm_printf("[GSM] gprs_ntpGetTime() failed!\r\n");
    gsm_unlock();
    return false;
  }
  sscanf((char*)gsm.buffer, "\r\n+CCLK: \"%[^\"\r\n]", string);
  gsm_printf("[GSM] gprs_ntpGetTime() done. %s\r\n", string);
  gsm_unlock();
  return true;
}
//###############################################################################################################
bool gsm_gprs_mqttConnect(const char *url, uint16_t port, bool cleanFlag, const char *clientID, uint16_t keepAliveSec, const char *user, const char *pass, uint16_t timeoutSec)
{
  if ((gsm.gprs.connected == false) || (url == NULL))
  {
    gsm_printf("[GSM] gprs_mqttConnect() failed!\r\n");
    return false;
  }
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] gprs_mqttConnect() failed!\r\n");
    return false;
  }
  //  set CID to 1
  sprintf((char*)gsm.buffer, "AT+SMCONF=\"CID\",1\r\n");  
  if (gsm_command((char*)gsm.buffer, 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
  {
    gsm_printf("[GSM] gprs_mqttConnect() failed!\r\n");
    gsm_unlock();
    return false;
  }
  //  set keep alive time
  if (keepAliveSec < 60)
    keepAliveSec = 60;
  if (keepAliveSec > 3600)
    keepAliveSec = 3600;
  sprintf((char*)gsm.buffer, "AT+SMCONF=\"KEEPALIVE\",%d\r\n", keepAliveSec);
  if (gsm_command((char*)gsm.buffer, 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
  {
    gsm_printf("[GSM] gprs_mqttConnect() failed!\r\n");
    gsm_unlock();
    return false;
  }  
  //  set timeout
  sprintf((char*)gsm.buffer, "AT+SMCONF=\"TIMEOUT\",%d\r\n", timeoutSec);
  if (gsm_command((char*)gsm.buffer, 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
  {
    gsm_printf("[GSM] gprs_mqttConnect() failed!\r\n");
    gsm_unlock();
    return false;
  } 
  //  set client id
  if (clientID != NULL)
  {
    sprintf((char*)gsm.buffer, "AT+SMCONF=\"CLIENTID\",\"%s\"\r\n", clientID);
    if (gsm_command((char*)gsm.buffer, 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
    {
      gsm_printf("[GSM] gprs_mqttConnect() failed!\r\n");
      gsm_unlock();
      return false;
    }  
  }
  //  set user name
  if (user != NULL)
  {
    sprintf((char*)gsm.buffer, "AT+SMCONF=\"USERNAME\",\"%s\"\r\n", user);
    if (gsm_command((char*)gsm.buffer, 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
    {
      gsm_printf("[GSM] gprs_mqttConnect() failed!\r\n");
      gsm_unlock();
      return false;
    }  
  }
  //  set password
  if (pass != NULL)
  {
    sprintf((char*)gsm.buffer, "AT+SMCONF=\"PASSWORD\",\"%s\"\r\n", pass);
    if (gsm_command((char*)gsm.buffer, 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
    {
      gsm_printf("[GSM] gprs_mqttConnect() failed!\r\n");
      gsm_unlock();
      return false;
    }  
  }
  //  set URL and port
  sprintf((char*)gsm.buffer, "AT+SMCONF=\"URL\",\"%s:%d\"\r\n", url, port);
  if (gsm_command((char*)gsm.buffer, 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
  {
    gsm_printf("[GSM] gprs_mqttConnect() failed!\r\n");
    gsm_unlock();
    return false;
  }
  //  clear flag or not
  sprintf((char*)gsm.buffer, "AT+SMCONF=\"CLEANSS\",%d\r\n", cleanFlag);
  if (gsm_command((char*)gsm.buffer, 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
  {
    gsm_printf("[GSM] gprs_mqttConnect() failed!\r\n");
    gsm_unlock();
    return false;
  }
  //  connect to server
  if (gsm_command("AT+SMCONN\r\n", timeoutSec * 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
  {
    gsm_printf("[GSM] gprs_mqttConnect() failed!\r\n");
    gsm_unlock();
    return false;
  }
  gsm.gprs.mqttConnected = 1;
  gsm_printf("[GSM] gprs_mqttConnect() done\r\n");
  gsm_unlock();
  return true;   
}
//###############################################################################################################
bool gsm_gprs_mqttDisConnect(void)
{
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] gprs_mqttDisConnect() failed!\r\n");
    return false;
  }
  if (gsm_command("AT+SMDISC\r\n", 10000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
  {
    gsm_printf("[GSM] gprs_mqttDisConnect() failed!\r\n");
    gsm_unlock();
    return false;
  }
  gsm.gprs.mqttConnected = 0;
  gsm_printf("[GSM] gprs_mqttDisConnect() done\r\n");
  gsm_unlock();
  return true;   
}
//###############################################################################################################
bool gsm_gprs_mqttSubscribe(const char *topic, bool qos)
{
  if (gsm.gprs.connected == false)
  {
    gsm_printf("[GSM] gprs_mqttSubscribe() failed!\r\n");
    return false;
  }
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] gprs_mqttSubscribe() failed!\r\n");
    return false;
  }
  sprintf((char*)gsm.buffer, "AT+SMSUB=\"%s\",%d\r\n", topic, qos);
  if (gsm_command((char*)gsm.buffer, 65000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
  {
    gsm_printf("[GSM] gprs_mqttSubscribe() failed!\r\n");
    gsm_unlock();
    return false;
  }
  gsm_printf("[GSM] gprs_mqttSubscribe() done\r\n");
  gsm_unlock();
  return true;   
}
//###############################################################################################################
bool gsm_gprs_mqttUnSubscribe(const char *topic)
{
  if (gsm.gprs.connected == false)
  {
    gsm_printf("[GSM] gprs_mqttUnSubscribe() failed!\r\n");
    return false;
  }
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] gprs_mqttUnSubscribe() failed!\r\n");
    return false;
  }
  sprintf((char*)gsm.buffer, "AT+SMUNSUB=\"%s\"\r\n", topic);
  if (gsm_command((char*)gsm.buffer, 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
  {
    gsm_printf("[GSM] gprs_mqttUnSubscribe() failed!\r\n");
    gsm_unlock();
    return false;
  }
  gsm_printf("[GSM] gprs_mqttUnSubscribe() done\r\n");
  gsm_unlock();
  return true;   
}
//###############################################################################################################
bool gsm_gprs_mqttPublish(const char *topic, bool qos, bool retain, const char *message)
{
  if (gsm.gprs.connected == false)
  {
    gsm_printf("[GSM] gprs_mqttPublish() failed!\r\n");
    return false;
  }
  if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] gprs_mqttPublish() failed!\r\n");
    return false;
  }
  sprintf((char*)gsm.buffer, "AT+SMPUB=\"%s\",%d,%d,\"%s\"\r\n", topic, qos, retain, message);
  if (gsm_command((char*)gsm.buffer , 65000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
  {
    gsm_printf("[GSM] gprs_mqttPublish() failed!\r\n");
    gsm_unlock();
    return false;
  }
  gsm_printf("[GSM] gprs_mqttPublish() done\r\n");
  gsm_unlock();
  return true;   
}
//###############################################################################################################
bool gsm_gprs_cert(char *file_name)
{
	if (gsm_lock(10000) == false)
  {
    gsm_printf("[GSM] gprs_cert() failed!\r\n");
    return false;
  }
	sprintf((char*)gsm.buffer, "AT+SSLSETCERT=\"C:\\USER\\%s\"\r\n", file_name);
  if (gsm_command((char*)gsm.buffer , 2000 , NULL, 0, 2, "\r\n+SSLSETCERT: 0\r\n", "\r\nERROR\r\n") != 1)
  {
    gsm_printf("[GSM] gprs_cert() failed!\r\n");
    gsm_unlock();
    return false;
  }
  gsm_printf("[GSM] gprs_cert() done\r\n");
  gsm_unlock();
  return true;   	
}
//###############################################################################################################
#endif
