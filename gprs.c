#include "gsm.h"

#if (_GSM_GPRS == 1)
//###############################################################################################################
bool gsm_gprs_setApName(const char *apName)
{
  char str[64];
  if (apName == NULL)
    return false;
  if (gsm_lock(10000) == false)
    return false;
  if (gsm_command("AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n", 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
  {
    gsm_unlock();
    return false;
  }
  sprintf(str, "AT+SAPBR=3,1,\"APN\",\"%s\"\r\n", apName);
  if (gsm_command(str, 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") == 1)
  {
    gsm_unlock();
    return true;
  }
  gsm_unlock();
  return false;
}
//###############################################################################################################
bool gsm_gprs_connect(void)
{
  if (gsm_lock(10000) == false)
    return false;
  char str[32];
  gsm_command("AT+SAPBR=0,1\r\n", 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n");
  if (gsm_command("AT+SAPBR=1,1\r\n", 85000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
  {
    gsm_unlock();
    return false;
  }
  if (gsm_command("AT+SAPBR=2,1\r\n", 1000, str, sizeof(str), 2, "\r\n+SAPBR: 1,1,", "\r\nERROR\r\n") != 1)
  {
    gsm.gprs.connected = false;
    gsm_unlock();
    return false;
  }
  memset(gsm.gprs.ip, 0, sizeof(gsm.gprs.ip));
  sscanf(str, "\r\n+SAPBR: 1,1,\"%[^\"\r\n]", gsm.gprs.ip);
  gsm.gprs.connected = true;
  gsm.gprs.connectedLast = true;
  gsm_unlock();
  return true;
}
//###############################################################################################################
bool gsm_gprs_disconnect(void)
{
  if (gsm_lock(10000) == false)
    return false;
  if (gsm_command("AT+SAPBR=0,1\r\n", 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") == 1)
  {
    gsm.gprs.connected = false;
    gsm.gprs.connectedLast = false;
    gsm_unlock();
    return true;
  }
  gsm_unlock();
  return false;
}
//###############################################################################################################
bool gsm_gprs_httpInit(void)
{
  if (gsm.gprs.connected == false)
    return false;
  if (gsm_lock(10000) == false)
    return false;
  if (gsm_command("AT+HTTPINIT\r\n", 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
  {
    gsm_unlock();
    return false;
  }
  gsm_unlock();
  return true;
}
//###############################################################################################################
bool gsm_gprs_httpSetContent(const char *content)
{
  if (gsm.gprs.connected == false)
    return false;
  if (gsm_lock(10000) == false)
    return false;
  char str[strlen(content) + 32];
  sprintf(str, "AT+HTTPPARA=\"CONTENT\",\"%s\"\r\n", content);
  if (gsm_command(str, 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
  {
    gsm_unlock();
    return false;
  }
  gsm_unlock();
  return true;
}
//###############################################################################################################
bool gsm_gprs_httpSetUserData(const char *data)
{
  if (gsm.gprs.connected == false)
    return false;
  if (gsm_lock(10000) == false)
    return false;
  gsm_transmit((uint8_t* ) "AT+HTTPPARA=\"USERDATA\",\"", strlen("AT+HTTPPARA=\"USERDATA\",\""));
  gsm_transmit((uint8_t* ) data, strlen(data));
  if (gsm_command("\"\r\n", 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
  {
    gsm_unlock();
    return false;
  }
  gsm_unlock();
  return true;
}
//###############################################################################################################
bool gsm_gprs_httpSendData(const char *data, uint16_t timeout_ms)
{
  if (gsm.gprs.connected == false)
    return false;
  if (gsm_lock(10000) == false)
    return false;
  char str[32];
  sprintf(str, "AT+HTTPDATA=%d,%d\r\n", strlen(data), timeout_ms);
  do
  {
    if (gsm_command(str, timeout_ms, NULL, 0, 2, "\r\nDOWNLOAD\r\n", "\r\nERROR\r\n") != 1)
      break;
    if (gsm_command(data, timeout_ms, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    gsm_delay(timeout_ms);
    gsm_unlock();
    return true;
  } while (0);
  gsm_unlock();
  return false;
}
//###############################################################################################################
int16_t gsm_gprs_httpGet(const char *url, bool ssl, uint16_t timeout_ms)
{
  if (gsm.gprs.connected == false)
    return -1;
  if (gsm_lock(10000) == false)
    return false;
  gsm.gprs.code = -1;
  gsm.gprs.dataLen = 0;
  do
  {
    if (gsm_command("AT+HTTPPARA=\"CID\",1\r\n", 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    char str[strlen(url) + 32];
    sprintf(str, "AT+HTTPPARA=\"URL\",\"%s\"\r\n", url);
    if (gsm_command(str, 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
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
    if (gsm_command("AT+HTTPACTION=0\r\n", timeout_ms , str, sizeof(str), 2, "\r\n+HTTPACTION:", "\r\nERROR\r\n") != 1)
      break;
    sscanf(str, "\r\n+HTTPACTION: 0,%hd,%ld\r\n", &gsm.gprs.code, &gsm.gprs.dataLen);
  } while (0);
  gsm_unlock();
  return gsm.gprs.code;
}
//###############################################################################################################
int16_t gsm_gprs_httpPost(const char *url, bool ssl, uint16_t timeout_ms)
{
  if (gsm.gprs.connected == false)
    return -1;
  if (gsm_lock(10000) == false)
    return false;
  gsm.gprs.code = -1;
  gsm.gprs.dataLen = 0;
  do
  {
    if (gsm_command("AT+HTTPPARA=\"CID\",1\r\n", 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    char str[strlen(url) + 32];
    sprintf(str, "AT+HTTPPARA=\"URL\",\"%s\"\r\n", url);
    if (gsm_command(str, 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
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
    if (gsm_command("AT+HTTPACTION=1\r\n", timeout_ms , str, sizeof(str), 2, "\r\n+HTTPACTION:", "\r\nERROR\r\n") != 1)
      break;
    sscanf(str, "\r\n+HTTPACTION: 1,%hd,%ld\r\n", &gsm.gprs.code, &gsm.gprs.dataLen);
  } while (0);
  gsm_unlock();
  return gsm.gprs.code;
}
//###############################################################################################################
uint32_t gsm_gprs_httpDataLen(void)
{
  return gsm.gprs.dataLen;
}
//###############################################################################################################
uint16_t gsm_gprs_httpRead(uint16_t len)
{
  if (gsm.gprs.connected == false)
    return 0;
  if (gsm_lock(10000) == false)
    return false;
  memset(gsm.buffer, 0, sizeof(gsm.buffer));
  char str[32];
  if (len >= sizeof(gsm.buffer))
    len = sizeof(gsm.buffer);
  sprintf(str, "AT+HTTPREAD=%ld,%d\r\n", gsm.gprs.dataCurrent, len);
  if (gsm_command(str, 1000 , (char*)gsm.buffer, sizeof(gsm.buffer), 2, "\r\n+HTTPREAD: ", "\r\nERROR\r\n") != 1)
  {
    gsm_unlock();
    return 0;
  }
  gsm.gprs.dataCurrent += len;
  if (gsm.gprs.dataCurrent >= gsm.gprs.dataLen)
    gsm.gprs.dataCurrent = gsm.gprs.dataLen;
  uint16_t readLen;
  char *s = strchr((char*) gsm.buffer, ':');
  s++;
  readLen = atoi(s);
  s = strchr(s, '\n');
  if (s != NULL)
  {
    s++;
    for (uint16_t i = 0; i < readLen; i++)
      gsm.buffer[i] = *s++;
    gsm_unlock();
    return readLen;
  }
  gsm_unlock();
  return 0;
}
//###############################################################################################################
bool gsm_gprs_httpTerminate(void)
{
  if (gsm.gprs.connected == false)
    return false;
  if (gsm_lock(10000) == false)
    return false;
  if (gsm_command("AT+HTTPTERM\r\n", 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") == 1)
  {
    gsm_unlock();
    return true;
  }
  gsm_unlock();
  return false;
}
//###############################################################################################################
gsm_ftp_error_t gsm_gprs_ftpLogin(const char *ftpAddress, const char *ftpUserName, const char *ftpPassword,
    uint16_t port)
{
  if (gsm.gprs.connected == false)
    return false;
  if (gsm_lock(10000) == false)
    return false;
  gsm_ftp_error_t ret = gsm_ftp_error_error;
  char str[128];
  do
  {
    if (gsm_command("AT+FTPMODE=1\r\n", 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    if (gsm_command("AT+FTPCID=1\r\n", 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    sprintf(str, "AT+FTPSERV=\"%s\"\r\n", ftpAddress);
    if (gsm_command(str, 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    sprintf(str, "AT+FTPPORT=%d\r\n", port);
    if (gsm_command(str, 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    sprintf(str, "AT+FTPUN=\"%s\"\r\n", ftpUserName);
    if (gsm_command(str, 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    sprintf(str, "AT+FTPPW=\"%s\"\r\n", ftpPassword);
    if (gsm_command(str, 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    ret = gsm_ftp_error_none;
  } while (0);
  gsm_unlock();
  return ret;
}
//###############################################################################################################
gsm_ftp_error_t gsm_gprs_ftpUploadBegin(bool asciiFile, bool append, const char *path, const char *fileName,
    const uint8_t *data, uint16_t len)
{
  if (gsm.gprs.connected == false)
    return false;
  if (gsm_lock(10000) == false)
    return gsm_ftp_error_error;
  char *s;
  char str[128];
  char answer[64];
  gsm_ftp_error_t error = gsm_ftp_error_error;
  do
  {
    gsm_command("AT+FTPEXTPUT=0\r\n", 5000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n");
    if (asciiFile)
      sprintf(str, "AT+FTPTYPE=\"A\"\r\n");
    else
      sprintf(str, "AT+FTPTYPE=\"I\"\r\n");
    if (gsm_command(str, 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    if (append)
      sprintf(str, "AT+FTPPUTOPT=\"APPE\"\r\n");
    else
      sprintf(str, "AT+FTPPUTOPT=\"STOR\"\r\n");
    if (gsm_command(str, 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    sprintf(str, "AT+FTPPUTPATH=\"%s\"\r\n", path);
    if (gsm_command(str, 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    sprintf(str, "AT+FTPPUTNAME=\"%s\"\r\n", fileName);
    if (gsm_command(str, 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
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
    sprintf(str, "AT+FTPPUT=2,%d\r\n", len);
    if (gsm_command(str, 5000 , answer, sizeof(answer), 2, "\r\n+FTPPUT: 2,", "\r\nERROR\r\n") != 1)
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
  gsm_unlock();
  return error;
}
//###############################################################################################################
gsm_ftp_error_t gsm_gprs_ftpUpload(const uint8_t *data, uint16_t len)
{
  if (gsm.gprs.connected == false)
    return gsm_ftp_error_error;
  if (gsm_lock(10000) == false)
    return gsm_ftp_error_error;
  char *s;
  char str[128];
  char answer[64];
  gsm_ftp_error_t error = gsm_ftp_error_error;
  do
  {
    sprintf(str, "AT+FTPPUT=2,%d\r\n", len);
    if (gsm_command(str, 5000 , answer, sizeof(answer), 2, "\r\n+FTPPUT: 2,", "\r\nERROR\r\n") != 1)
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
  gsm_unlock();
  return error;
}
//###############################################################################################################
gsm_ftp_error_t gsm_gprs_ftpUploadEnd(void)
{
  if (gsm.gprs.connected == false)
    return gsm_ftp_error_error;
  if (gsm_lock(10000) == false)
    return gsm_ftp_error_error;
  if (gsm_command("AT+FTPPUT=2,0\r\n", 5000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
  {
    gsm_unlock();
    return gsm_ftp_error_error;
  }
  gsm_unlock();
  return gsm_ftp_error_none;
}
//###############################################################################################################
gsm_ftp_error_t gsm_gprs_ftpExtUploadBegin(bool asciiFile, bool append, const char *path, const char *fileName)
{
  if (gsm.gprs.connected == false)
    return gsm_ftp_error_error;
  if (gsm_lock(10000) == false)
    return gsm_ftp_error_error;
  gsm_ftp_error_t error = gsm_ftp_error_error;
  char str[128];
  do
  {
    gsm_command("AT+FTPEXTPUT=0\r\n", 5000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n");
    if (asciiFile)
      sprintf(str, "AT+FTPTYPE=\"A\"\r\n");
    else
      sprintf(str, "AT+FTPTYPE=\"I\"\r\n");
    if (gsm_command(str, 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    if (append)
      sprintf(str, "AT+FTPPUTOPT=\"APPE\"\r\n");
    else
      sprintf(str, "AT+FTPPUTOPT=\"STOR\"\r\n");
    if (gsm_command(str, 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    sprintf(str, "AT+FTPPUTPATH=\"%s\"\r\n", path);
    if (gsm_command(str, 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    sprintf(str, "AT+FTPPUTNAME=\"%s\"\r\n", fileName);
    if (gsm_command(str, 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    if (gsm_command("AT+FTPEXTPUT=1\r\n", 5000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    gsm.gprs.ftpExtOffset = 0;
    error = gsm_ftp_error_none;
  } while (0);
  gsm_unlock();
  return error;
}
//###############################################################################################################
gsm_ftp_error_t gsm_gprs_ftpExtUpload(uint8_t *data, uint16_t len)
{
  if (gsm.gprs.connected == false)
    return false;
  if (gsm_lock(10000) == false)
    return gsm_ftp_error_error;
  gsm_ftp_error_t error = gsm_ftp_error_error;
  char str[64];
  char answer[64];
  do
  {
    sprintf(str, "AT+FTPEXTPUT=2,%ld,%d,5000\r\n", gsm.gprs.ftpExtOffset, len);
    if (gsm_command(str, 5000, answer, sizeof(answer), 2, "\r\n+FTPEXTPUT: ", "\r\nERROR\r\n") != 1)
      break;
    char *s = strchr(answer, ',');
    if (s == NULL)
      break;
    s++;
    uint32_t d = atoi(s);
    if (d != len)
      break;
    gsm_delay(100);
    gsm_transmit((uint8_t* )data, len);
    if (gsm_command("", 5000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    gsm.gprs.ftpExtOffset += len;
    error = gsm_ftp_error_none;
  } while (0);
  gsm_unlock();
  return error;
}
//###############################################################################################################
gsm_ftp_error_t gsm_gprs_ftpExtUploadEnd(void)
{
  if (gsm.gprs.connected == false)
    return gsm_ftp_error_error;
  if (gsm_lock(10000) == false)
    return gsm_ftp_error_error;
  gsm_ftp_error_t error = gsm_ftp_error_error;
  char answer[64];
  do
  {
    if (gsm_command("AT+FTPPUT=1\r\n", 75000, answer, sizeof(answer), 2, "\r\n+FTPPUT: 1,", "\r\nERROR\r\n") != 1)
      break;
    char *s = strchr(answer, ',');
    if (s == NULL)
      break;
    s++;
    error = (gsm_ftp_error_t) atoi(s);
    gsm_command("AT+FTPEXTPUT=0\r\n", 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n");
  } while (0);
  gsm_unlock();
  return error;
}
//###############################################################################################################
gsm_ftp_error_t gsm_gprs_ftpCreateDir(const char *path)
{
  if (gsm.gprs.connected == false)
    return gsm_ftp_error_error;
  if (gsm_lock(10000) == false)
    return gsm_ftp_error_error;
  gsm_ftp_error_t error = gsm_ftp_error_error;
  char str[128];
  do
  {
    sprintf(str, "AT+FTPGETPATH=\"%s\"\r\n", path);
    if (gsm_command(str, 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    if (gsm_command("AT+FTPMKD\r\n", 75000 , str, sizeof(str), 2, "\r\n+FTPMKD: 1,", "\r\nERROR\r\n") != 1)
      break;
    char *s = strchr(str, ',');
    if (s == NULL)
      break;
    s++;
    error = (gsm_ftp_error_t) atoi(s);
  } while (0);
  gsm_unlock();
  return error;
}
//###############################################################################################################
gsm_ftp_error_t gsm_gprs_ftpRemoveDir(const char *path)
{
  if (gsm.gprs.connected == false)
    return gsm_ftp_error_error;
  if (gsm_lock(10000) == false)
    return gsm_ftp_error_error;
  gsm_ftp_error_t error = gsm_ftp_error_error;
  char str[128];
  do
  {
    sprintf(str, "AT+FTPGETPATH=\"%s\"\r\n", path);
    if (gsm_command(str, 1000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    if (gsm_command("AT+FTPRMD\r\n", 75000 , str, sizeof(str), 2, "\r\n+FTPRMD: 1,", "\r\nERROR\r\n") != 1)
      break;
    char *s = strchr(str, ',');
    if (s == NULL)
      break;
    s++;
    error = (gsm_ftp_error_t) atoi(s);
  } while (0);
  gsm_unlock();
  return error;
}
//###############################################################################################################
uint32_t gsm_gprs_ftpGetSize(const char *path, const char *name)
{
  if (gsm.gprs.connected == false)
    return 0;
  if (gsm_lock(10000) == false)
    return 0;
  uint32_t error = 0;
  char str[128];
  do
  {
    sprintf(str, "AT+FTPGETPATH=\"%s\"\r\n", path);
    if (gsm_command(str, 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    sprintf(str, "AT+FTPGETNAME=\"%s\"\r\n", name);
    if (gsm_command(str, 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    if (gsm_command("AT+FTPSIZE\r\n", 75000, str, sizeof(str), 2, "\r\n+FTPSIZE: 1,", "\r\nERROR\r\n") != 1)
      break;
    char *s = strchr(str, ',');
    if (s == NULL)
      break;
    s++;
    if (atoi(s) == 0)
    {
      s = strchr(str, ',');
      if (s == NULL)
        break;
      s++;
      error = atoi(s);
    }
  } while (0);
  gsm_unlock();
  return error;
}
//###############################################################################################################
gsm_ftp_error_t gsm_gprs_ftpRemove(const char *path, const char *name)
{
  if (gsm.gprs.connected == false)
    return gsm_ftp_error_error;
  if (gsm_lock(10000) == false)
    return gsm_ftp_error_error;
  gsm_ftp_error_t error = gsm_ftp_error_error;
  char str[128];
  do
  {
    sprintf(str, "AT+FTPGETPATH=\"%s\"\r\n", path);
    if (gsm_command(str, 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    sprintf(str, "AT+FTPGETNAME=\"%s\"\r\n", name);
    if (gsm_command(str, 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
      break;
    if (gsm_command("AT+FTPDELE\r\n", 75000, str, sizeof(str), 2, "\r\n+FTPDELE: 1,", "\r\nERROR\r\n") != 1)
      break;
    char *s = strchr(str, ',');
    if (s == NULL)
      break;
    s++;
    error = (gsm_ftp_error_t) atoi(s);
  } while (0);
  gsm_unlock();
  return error;
}
//###############################################################################################################
gsm_ftp_error_t gsm_gprs_ftpIsExistFolder(const char *path)
{
  if (gsm.gprs.connected == false)
    return gsm_ftp_error_error;
  if (gsm_lock(10000) == false)
    return gsm_ftp_error_error;
  gsm_ftp_error_t error = gsm_ftp_error_error;
  char str[strlen(path) + 16];
  char answer[32];
  do
  {
    sprintf(str, "AT+FTPGETPATH=\"%s\"\r\n", path);
    if (gsm_command(str, 1000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
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
  gsm_unlock();
  return error;
}
//###############################################################################################################
bool gsm_gprs_ftpIsBusy(void)
{
  if (gsm.gprs.connected == false)
    return false;
  if (gsm_lock(10000) == false)
    return false;
  if (gsm_command("AT+FTPSTATE\r\n", 75000, NULL, 0, 3, "\r\n+FTPSTATE: 0\r\n", "\r\n+FTPSTATE: 1\r\n",
      "\r\nERROR\r\n") == 1)
  {
    gsm_unlock();
    return true;
  }
  gsm_unlock();
  return false;
}
//###############################################################################################################
gsm_ftp_error_t gsm_gprs_ftpQuit(void)
{
  if (gsm.gprs.connected == false)
    return gsm_ftp_error_error;
  if (gsm_lock(10000) == false)
    return gsm_ftp_error_error;
  if (gsm_command("AT+FTPQUIT\r\n", 5000, NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
  {
    gsm_unlock();
    return gsm_ftp_error_error;
  }
  gsm_unlock();
  return gsm_ftp_error_none;
}
//###############################################################################################################
bool gsm_gprs_ntpServer(char *server, int8_t time_zone_in_quarter)
{
  if (gsm.gprs.connected == false)
    return false;
  if (gsm_lock(10000) == false)
    return false;
  char str[64];
  sprintf(str, "AT+CNTP=\"%s\",%d\r\n", server, time_zone_in_quarter);
  if (gsm_command(str, 10000 , NULL, 0, 2, "\r\nOK\r\n", "\r\nERROR\r\n") != 1)
  {
    gsm_unlock();
    return false;
  }
  gsm_unlock();
  return true;
}
//###############################################################################################################
bool gsm_gprs_ntpSyncTime(void)
{
  if (gsm.gprs.connected == false)
    return false;
  if (gsm_lock(10000) == false)
    return false;
  if (gsm_command("AT+CNTP\r\n", 10000, NULL, 0, 2, "\r\n+CNTP: 1\r\n", "\r\nERROR\r\n") != 1)
  {
    gsm_unlock();
    return false;
  }
  gsm_unlock();
  return true;
}
//###############################################################################################################
bool gsm_gprs_ntpGetTime(char *string)
{
  if (string == NULL)
    return false;
  if (gsm_lock(10000) == false)
    return false;
  char str[32];
  if (gsm_command("AT+CCLK?\r\n", 10000, str, sizeof(str), 2, "\r\n+CCLK:", "\r\nERROR\r\n") != 1)
  {
    gsm_unlock();
    return false;
  }
  sscanf(str, "\r\n+CCLK: \"%[^\"\r\n]", string);
  gsm_unlock();
  return true;
}
//###############################################################################################################
#endif
