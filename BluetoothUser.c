#include "Sim80x.h"
#if (_SIM80X_USE_BLUETOOTH==1)
//###################################################################################################################
//I dont know why. not work yet . I set auto pair.
void   Bluetooth_UserNewPairingRequest(char *Name,char *Address,char *Pass)
{
  //Bluetooth_AcceptPair(true);  
  //Bluetooth_AcceptPair(false);
}
//###################################################################################################################
void  Bluetooth_UserConnectingSpp(void)
{
  Bluetooth_SppAllowConnection(true);  
  //Bluetooth_SppAllowConnection(false);
}
//###################################################################################################################
void  Bluetooth_UserNewSppData(char *NewData,uint16_t len)
{
  //Bluetooth_SppSend("Test Back\r\n");  
}
//###################################################################################################################
#endif
