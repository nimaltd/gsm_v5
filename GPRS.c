#include "Sim80x.h"
#if (_SIM80X_USE_GPRS==1)


//####################################################################################################
bool  GPRS_StartUpGPRS(void)
{
  uint8_t answer;
  answer = Sim80x_SendAtCommand("AT+CIICR\r\n",85000,2,"\r\nOK\r\n","\r\nERROR\r\n");
  if(answer == 1)
  {
    #if (_SIM80X_DEBUG==1)
    printf("\r\nGPRS_StartUpGPRS() ---> OK\r\n");
    #endif
    return true;
  }
  else  
  {
    #if (_SIM80X_DEBUG==1)
    printf("\r\nGPRS_StartUpGPRS() --->ERROR\r\n");
    #endif
    return false;
  }
}  
//####################################################################################################
bool  GPRS_GetAPN(char *Name,char *username,char *password)
{
  uint8_t answer;
  answer = Sim80x_SendAtCommand("AT+CSTT?\r\n",1000,2,"\r\nOK\r\n","\r\nERROR\r\n");
  if(answer==1)
  {
    if(Name!=NULL)
      strcpy(Name,Sim80x.GPRS.APN);
    if(username!=NULL)
      strcpy(username,Sim80x.GPRS.APN_UserName);
    if(password!=NULL)
      strcpy(password,Sim80x.GPRS.APN_Password);
    #if (_SIM80X_DEBUG==1)
    printf("\r\nGPRS_GetAPN(%s,%s,%s) <--- OK\r\n",Sim80x.GPRS.APN,Sim80x.GPRS.APN_UserName,Sim80x.GPRS.APN_Password);
    #endif
    return true;
  }
  else
  {
    #if (_SIM80X_DEBUG==1)
    printf("\r\nGPRS_GetAPN() <--- ERROR\r\n");
    #endif
    return false;
  }
}
//####################################################################################################
bool  GPRS_SetAPN(char *Name,char *username,char *password)
{
  char str[64];
  uint8_t answer;
  sprintf(str,"AT+CSTT=\"%s\",\"%s\",\"%s\"\r\n",Name,username,password);
  answer = Sim80x_SendAtCommand(str,1000,2,"\r\nOK\r\n","\r\nERROR\r\n");
  if(answer == 1)
  {
    strcpy(Sim80x.GPRS.APN,Name);
    strcpy(Sim80x.GPRS.APN_UserName,username);
    strcpy(Sim80x.GPRS.APN_Password,password);
    #if (_SIM80X_DEBUG==1)
    printf("\r\nGPRS_SetAPN(\"%s\",\"%s\",\"%s\") ---> OK\r\n",Name,username,password);
    #endif    
    return true;
  }
  else
  {
    #if (_SIM80X_DEBUG==1)
    printf("\r\nGPRS_SetAPN(\"%s\",\"%s\",\"%s\") ---> ERROR\r\n",Name,username,password);
    #endif    
    return false;  
  }  
}
//####################################################################################################
bool  GPRS_DeactivatePDPContext(void)
{
  uint8_t answer;
  answer = Sim80x_SendAtCommand("AT+CIPSHUT\r\n",65000,2,"\r\nSHUT OK\r\n","\r\nERROR\r\n");
  if(answer == 1)
  {
    #if (_SIM80X_DEBUG==1)
    printf("\r\nGPRS_DeactivatePDPContext() ---> OK\r\n");
    #endif    
    return true;
  }
  else
  {
    #if (_SIM80X_DEBUG==1)
    printf("\r\nGPRS_DeactivatePDPContext() ---> ERROR\r\n");
    #endif    
    return false;  
  }
}
//####################################################################################################
void  GPRS_GetLocalIP(char *IP)
{
  uint8_t answer;
  answer = Sim80x_SendAtCommand("AT+CIFSR\r\n",1000,2,"\r\nOK\r\n","\r\nERROR\r\n");
  if((IP!=NULL) && (answer==1))
    strcpy(IP,Sim80x.GPRS.LocalIP);
  #if (_SIM80X_DEBUG==1)
  printf("\r\nGPRS_GetLocalIP(%s) <--- OK\r\n",Sim80x.GPRS.LocalIP);
  #endif    
}
//####################################################################################################
void  GPRS_GetCurrentConnectionStatus(void)
{
  uint8_t answer;
  answer = Sim80x_SendAtCommand(" AT+CIPSTATUS\r\n",1000,2,"\r\nOK\r\n","\r\nERROR\r\n");  
  #if (_SIM80X_DEBUG==1)
  printf("\r\nGPRS_GetCurrentConnectionStatus() <--- OK\r\n");
  #endif    
  
}
//####################################################################################################
bool  GPRS_GetMultiConnection(void)
{
  Sim80x_SendAtCommand(" AT+CIPMUX?\r\n",1000,2,"\r\nOK\r\n","\r\nERROR\r\n");  
  #if (_SIM80X_DEBUG==1)
  printf("\r\nGPRS_GetMultiConnection(%d) <--- OK\r\n",Sim80x.GPRS.MultiConnection);
  #endif  
  return Sim80x.GPRS.MultiConnection;
}
//####################################################################################################
bool  GPRS_SetMultiConnection(bool Enable)
{
  uint8_t answer;
  if(Enable==true)
    answer = Sim80x_SendAtCommand(" AT+CIPMUX=1\r\n",1000,2,"\r\nOK\r\n","\r\nERROR\r\n");  
  else
    answer = Sim80x_SendAtCommand(" AT+CIPMUX=0\r\n",1000,2,"\r\nOK\r\n","\r\nERROR\r\n");  
  if(answer == 1)
  {
    #if (_SIM80X_DEBUG==1)
    printf("\r\nGPRS_SetMultiConnection(%d) ---> OK\r\n",Enable);
    #endif  
    Sim80x.GPRS.MultiConnection=Enable;
    return true;
  }
  else
  {
    #if (_SIM80X_DEBUG==1)
    printf("\r\nGPRS_SetMultiConnection(%d) ---> ERROR\r\n",Enable);
    #endif  
    return false;
  }
}
//####################################################################################################
//####################################################################################################
//####################################################################################################
bool  GPRS_ConnectToNetwork(char *Name,char *username,char *password,bool EnableMultiConnection)
{
  
  if(GPRS_DeactivatePDPContext()==false)
  {
    #if (_SIM80X_DEBUG==1)
    printf("\r\nGPRS_ConnectToNetwork() ---> ERROR\r\n");
    #endif 
    return false;
  }
  GPRS_SetMultiConnection(EnableMultiConnection);
  if(GPRS_SetAPN(Name,username,password)==false)
  {
    #if (_SIM80X_DEBUG==1)
    printf("\r\nGPRS_ConnectToNetwork() ---> ERROR\r\n");
    #endif 
    return false;
  }
  if(GPRS_StartUpGPRS()==false)
  {
    #if (_SIM80X_DEBUG==1)
    printf("\r\nGPRS_ConnectToNetwork() ---> ERROR\r\n");
    #endif 
    return false;
  }
  GPRS_GetLocalIP(NULL);  
  #if (_SIM80X_DEBUG==1)
  printf("\r\nGPRS_ConnectToNetwork() ---> OK\r\n");
  #endif 
  return true;
}
//####################################################################################################
//####################################################################################################
//####################################################################################################
#endif
