#include "Sim80x.h"



void  Sim80x_UserInit(void)
{
  GPRS_ConnectToNetwork("mcinet","","",false);
}
