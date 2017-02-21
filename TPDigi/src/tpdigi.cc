#include "HCALStudies/TPDigi/interface/tpdigi.h"



//--------------------
// default constructor

tpdigi::tpdigi():
  m_type("none"),m_iEntry(-999),
  m_crate(-999),m_slot(-999),m_fiber(-999),m_channel(-999),
  m_subdet(-1),m_ieta(-999),m_iphi(-999),m_depth(-999),
  m_nTS(0),m_vals1(new std::vector<int>),m_vals2(new std::vector<int>)
{};



//-----------------
// copy constructor

tpdigi::tpdigi(const tpdigi& aTPDigi):
  m_type(aTPDigi.type()),m_iEntry(aTPDigi.iEntry()),
  m_crate(aTPDigi.crate()),m_slot(aTPDigi.slot()),m_fiber(aTPDigi.fiber()),m_channel(aTPDigi.channel()),
  m_subdet(aTPDigi.subdet()),m_ieta(aTPDigi.ieta()),m_iphi(aTPDigi.iphi()),m_depth(aTPDigi.depth()),
  m_nTS(aTPDigi.nTS()),m_vals1(new std::vector<int>(m_nTS)),m_vals2(new std::vector<int>(m_nTS))
{
  std::vector<int>* vals1 = aTPDigi.vals1();
  std::vector<int>* vals2 = aTPDigi.vals2();
  for(int it = 0; it < m_nTS; ++it)
  {
    m_vals1->at(it) = vals1->at(it);
    m_vals2->at(it) = vals2->at(it);
  }
}



//------------
// constructor

tpdigi::tpdigi(const std::string& type, const int& iEntry,
               const int& crate, const int& slot, const int& fiber, const int& channel,
               const int& subdet, const int& ieta, const int& iphi, const int& depth):
  m_type(type),m_iEntry(iEntry),
  m_crate(crate),m_slot(slot),m_fiber(fiber),m_channel(channel),
  m_subdet(subdet),m_ieta(ieta),m_iphi(iphi),m_depth(depth)
{
  if( type == "TP"  ) m_nTS = 4;
  if( type == "QIE" ) m_nTS = 10;
  m_vals1 = new std::vector<int>(m_nTS,-1);
  m_vals2 = new std::vector<int>(m_nTS,-1);
};



//-----------
// distructor

tpdigi::~tpdigi()
{
  // if( m_vals1 != NULL ) delete m_vals1;
  // if( m_vals2 != NULL ) delete m_vals2;
}



void tpdigi::SetVals(const int* vals1, const int* vals2)
{
  for(int it = 0; it < m_nTS; ++it)
  {
    m_vals1 -> at(it) = vals1[it];
    m_vals2 -> at(it) = vals2[it];
  }
}



//---------------------
// overload operator <<

std::ostream& operator<<(std::ostream& os, const crateslot& obj)
{
  os << "(crate:" << std::setw(2) << obj.first  << ",";
  os << "slot:"   << std::setw(2) << obj.second << ")";
  
  return os;
}



std::ostream& operator<<(std::ostream& os, const coord& obj)
{
  os << "(crate:"  << std::setw(2) << std::get<0>(obj) << ",";
  os << "slot:"    << std::setw(2) << std::get<1>(obj) << ",";
  os << "fiber:"   << std::setw(2) << std::get<2>(obj) << ",";
  os << "channel:" << std::setw(2) << std::get<3>(obj) << ")";
  
  return os;
}

bool operator==(const coord& obj1, const coord& obj2)
{
  return(
    ( std::get<0>(obj1) == std::get<0>(obj2) ) &&
    ( std::get<1>(obj1) == std::get<1>(obj2) ) &&
    ( std::get<2>(obj1) == std::get<2>(obj2) ) &&
    ( std::get<3>(obj1) == std::get<3>(obj2) )
    );
}
bool operator!=(const coord& obj1, const coord& obj2)
{
  return !(obj1 == obj2);
}

bool operator<(const coord& obj1, const coord& obj2)
{
  if( std::get<0>(obj1) < std::get<0>(obj2) ) return true;
  else
  {
    if( std::get<1>(obj1) < std::get<1>(obj2) ) return true;
    else
    {
      if( std::get<2>(obj1) < std::get<2>(obj2) ) return true;
      else
      {
        if( std::get<3>(obj1) < std::get<3>(obj2) ) return true;
      }
    }
  }
  return false;
}

bool operator>(const coord& obj1, const coord& obj2)
{
  return obj2 < obj1;
}

bool operator<=(const coord& obj1, const coord& obj2)
{
  return !(obj1 > obj2);
}

bool operator>=(const coord& obj1, const coord& obj2)
{
  return !(obj1 < obj2);
}



std::ostream& operator<<(std::ostream& os, const tpdigi& obj)
{
  os << "iEntry:" << std::setw(5) << obj.iEntry();
  
  os << "   ";
  
  os << "(crate:"  << std::setw(2) << obj.crate()   << ",";
  os << "slot:"    << std::setw(2) << obj.slot()    << ",";
  os << "fiber:"   << std::setw(2) << obj.fiber()   << ",";
  os << "channel:" << std::setw(2) << obj.channel() << ")";
  
  os << "   ";
  
  os << "(subdet:" << std::setw(1) << obj.subdet() << ",";
  os << "ieta:"    << std::setw(3) << obj.ieta()   << ",";
  os << "iphi:"    << std::setw(3) << obj.iphi()   << ",";
  os << "depth:"   << std::setw(2) << obj.depth()  << ")";
  
  os << "   ";
  
  os << obj.type() << "1=(";
  for(int it = 0; it < obj.nTS(); ++it)
  {
    if( it < obj.nTS()-1 )
      os << std::setw(2) << obj.vals1()->at(it) << ",";
    else
      os << std::setw(2) << obj.vals1()->at(it);
  }
  os << ")";
  
  os << "   ";
  
  os << obj.type() << "2=(";
  for(int it = 0; it < obj.nTS(); ++it)
  {
    if( it < obj.nTS()-1 )
      os << std::setw(2) << obj.vals2()->at(it) << ",";
    else
      os << std::setw(2) << obj.vals2()->at(it);
  }
  os << ")";
  
  return os;
}




coord GetCoord(const std::string& type, const int& crate, const int& slot, const int& tb, const int& fiber, const int& channel)
{
  if( tb == -1 ) // uTCA
  {
    coord result(crate,slot,fiber,channel);
    return result;
  }
  
  
  else // VME
  {
    int crate_new = crate + 20;
    
    int slot_new = slot - 1;
    if( slot >= 13) slot_new -= 5;
    
    int fiber_new = fiber;
    int channel_new = channel;
    
    if( type == "QIE" ) // remapping for digis
    {
      fiber_new += 1;
      
      if( tb == 1 ) // top
      {
        fiber_new += 12;
      }
    }
    
    if( type == "TP" ) // remapping for TPs
    {
      fiber_new -= 1;
      
      if( fiber_new == 3 && (slot_new == 2 || slot_new == 3 || slot_new == 8 || slot_new == 9) )
      {
        if( channel == 0 && tb == 1 ) channel_new = 4;
        if( channel == 1 && tb == 1 ) channel_new = 5;
        if( channel == 2 && tb == 1 ) channel_new = 0;
        if( channel == 3 && tb == 1 ) channel_new = 1;
        if( channel == 0 && tb == 0 ) channel_new = 6;
        if( channel == 1 && tb == 0 ) channel_new = 7;
        if( channel == 2 && tb == 0 ) channel_new = 2;
        if( channel == 3 && tb == 0 ) channel_new = 3;
      }
      else
      {
        if( channel == 0 && tb == 1 ) channel_new = 0;
        if( channel == 1 && tb == 1 ) channel_new = 1;
        if( channel == 2 && tb == 1 ) channel_new = 4;
        if( channel == 3 && tb == 1 ) channel_new = 5;
        if( channel == 0 && tb == 0 ) channel_new = 2;
        if( channel == 1 && tb == 0 ) channel_new = 3;
        if( channel == 2 && tb == 0 ) channel_new = 6;
        if( channel == 3 && tb == 0 ) channel_new = 7;
      }
    }
    
    coord result(crate_new,slot_new,fiber_new,channel_new);
    return result;
  }
}
