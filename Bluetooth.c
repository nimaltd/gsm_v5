#include "Sim80x.h"


//#################################################################################################################
bool  Bluetooth_SetPower(bool TurnOn)
{
  uint8_t answer;
  osDelay(100);
  Bluetooth_GetStatus();
  if(TurnOn == true)
  {
    if(Sim80x.Bluetooth.Status == BluetoothStatus_Initial)
    {
      answer = Sim80x_SendAtCommand("AT+BTPOWER=1\r\n",5000,2,"\r\nOK\r\n","\r\nERROR\r\n");
      if(answer == 1)
      {
        for(uint8_t i=0 ;i<50 ;i++)
        {
          osDelay(100);
          if(Bluetooth_GetStatus()>BluetoothStatus_Initial)
          {
            Bluetooth_GetStatus();
            return true;          
          }
        }        
      }      
    }
    else if(Sim80x.Bluetooth.Status == BluetoothStatus_Error)
    {
      return false;
    }
    else
    {
      Bluetooth_GetStatus();
      return true;       
    }
  }
  else
  {
    for(uint8_t i=0 ;i<50 ;i++)
    {
      osDelay(100);
      answer = Sim80x_SendAtCommand("AT+BTPOWER=0\r\n",5000,2,"\r\nOK\r\n","\r\nERROR\r\n");
      if(Bluetooth_GetStatus()==BluetoothStatus_Initial)
      {
        Bluetooth_GetStatus();
        return true;      
      }
    }
    return false;     
  }
  return false; 
}
//#################################################################################################################
bool  Bluetooth_GetHostName(void)
{
  uint8_t answer;
  memset(Sim80x.Bluetooth.HostName,0,sizeof(Sim80x.Bluetooth.HostName));
  memset(Sim80x.Bluetooth.HostAddress,0,sizeof(Sim80x.Bluetooth.HostAddress));
  answer = Sim80x_SendAtCommand("AT+BTHOST?\r\n",1000,1,"\r\n+BTHOST:");
  if((answer == 1) && (Sim80x.Bluetooth.HostName[0] != 0) && (Sim80x.Bluetooth.HostAddress[0] != 0))
    return true;
  else
    return false;
}
//#################################################################################################################
bool  Bluetooth_SetHostName(char *HostName)
{
  uint8_t answer;
  char  str[32];
  char  strParam[32];
  snprintf(str,sizeof(str),"AT+BTHOST=%s\r\n",HostName);
  snprintf(strParam,sizeof(strParam),"AT+BTHOST=%s\r\r\nOK\r\n",HostName);
  answer = Sim80x_SendAtCommand(str,1000,1,strParam);
  if(answer == 1)
  {
    Bluetooth_GetHostName();
    return true;
  }
  else
    return false;
}
//#################################################################################################################
BluetoothStatus_t  Bluetooth_GetStatus(void)
{
  uint8_t answer;
  answer = Sim80x_SendAtCommand("AT+BTSTATUS?\r\n",1000,1,"\r\n+BTSTATUS:");
  if(answer == 1)
    return Sim80x.Bluetooth.Status;
  else
    return BluetoothStatus_Error;
}
//#################################################################################################################
bool  Bluetooth_AcceptPair(bool Accept)  
{
  uint8_t answer;
  if(Accept == true)
  {
    answer = Sim80x_SendAtCommand("AT+BTPAIR:1,1\r\n",1000,2,"\r\nOK\r\n","\r\nERROR\r\n");
    if(answer == 1)
      return true;
    else
      return false;
  }
  else
  {
    answer = Sim80x_SendAtCommand("AT+BTPAIR:1,0\r\n",1000,2,"\r\nOK\r\n","\r\nERROR\r\n");
    if(answer == 1)
      return true;
    else
      return false;    
  }  
}
//#################################################################################################################
bool  Bluetooth_AcceptPairWithPass(char *Pass)  
{
  uint8_t answer;
  char str[32];
  snprintf(str,sizeof(str),"AT+BTPAIR:2,%s\r\n",Pass);
  answer = Sim80x_SendAtCommand(str,1000,1,"\r\nOK\r\n");
  if(answer == 1)
    return true;
  else
    return false;
  
}

//#################################################################################################################
bool Bluetooth_SetAutoPair(bool  Enable)
{
  uint8_t answer;
  if(Enable==true)
    answer = Sim80x_SendAtCommand("AT+BTPAIRCFG=2\r\n",1000,2,"AT+BTPAIRCFG=2\r\r\nOK\r\n","AT+BTPAIRCFG=2\r\r\nERROR\r\n");
  else
    answer = Sim80x_SendAtCommand("AT+BTPAIRCFG=0\r\n",1000,2,"AT+BTPAIRCFG=2\r\r\nOK\r\n","AT+BTPAIRCFG=2\r\r\nERROR\r\n");
  if(answer == 1)
    return true;
  else
    return false;
}
//#################################################################################################################
bool Bluetooth_SetPairPassword(char  *Pass)
{
  uint8_t answer;
  char str[32];
  char strParam[32];
  snprintf(str,sizeof(str),"AT+BTPAIRCFG=1,%s\r\n",Pass);
  snprintf(strParam,sizeof(strParam),"AT+BTPAIRCFG=1,%s\r\r\nOK\r\n",Pass);
  answer = Sim80x_SendAtCommand(str,1000,1,strParam);
  if(answer == 1)
    return true;
  else
    return false;    
}
//#################################################################################################################
bool Bluetooth_Unpair(uint8_t  Unpair_0_to_all)
{
  uint8_t answer;
  char str[32];
  char strParam[32];
  snprintf(str,sizeof(str),"AT+BTUNPAIR=%d\r\n",Unpair_0_to_all);
  snprintf(strParam,sizeof(strParam),"AT+BTUNPAIR=%d\r\n",Unpair_0_to_all);
  answer = Sim80x_SendAtCommand(str,1000,1,strParam);
  if(answer == 1)
    return true;
  else
    return false;    
}
//#################################################################################################################
bool  Bluetooth_GetVisibility(void)
{
  Sim80x_SendAtCommand("AT+BTVIS?\r\n",1000,2,"\r\nOK\r\n","\r\nERROR\r\n");
  return Sim80x.Bluetooth.Visibility;
}
//#################################################################################################################
void  Bluetooth_SetVisibility(bool Visible)
{
 char str[16];
  snprintf(str,sizeof(str),"AT+BTVIS=%d\r\n",Visible);
  Sim80x_SendAtCommand(str,1000,2,"\r\nOK\r\n","\r\nERROR\r\n");
}
//#################################################################################################################
void  Bluetooth_SppAllowConnection(bool Accept)
{
  char str[16];
  snprintf(str,sizeof(str),"AT+BTACPT=%d\r\n",Accept);
  Sim80x_SendAtCommand(str,1000,2,"\r\nOK\r\n","\r\nERROR\r\n");  
}
//#################################################################################################################
bool  Bluetooth_SppSend(char *DataString)
{
  uint8_t answer;
  char str[2];
  answer = Sim80x_SendAtCommand("AT+BTSPPSEND\r\n",1000,2,"\r\r\n> ","\r\nERROR\r\n");
  if(answer == 1)
  {
    Sim80x_SendString(DataString);
    sprintf(str,"%c",26);
    answer = Sim80x_SendAtCommand(str,1000,2,"\r\nSEND OK\r\n","\r\nERROR\r\n");
    if(answer == 1)
      return true;
    else
      return false;
  }
  else
    return false;
}
//#################################################################################################################






