#include "HCALStudies/TPDigi/interface/Utils.h"



int GetRBXId(HcalSubdetector subdet, const int& iphi)
{
  if( subdet == HcalBarrel || subdet == HcalEndcap )
  {
    return int( (((iphi+2)%72) / 4) + 1);
  }
  
  return 99;
}
