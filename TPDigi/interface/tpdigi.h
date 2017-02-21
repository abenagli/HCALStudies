#ifndef tpdigi_hh
#define tpdigi_hh

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <tuple>



typedef std::pair<int,int> crateslot;

typedef std::tuple<int,int,int,int> coord;
typedef std::vector<coord> coords;

coord GetCoord(const std::string& type, const int& crate, const int& slot, const int& tb, const int& fiber, const int& channel);



struct treeVars {
  int iEntry;
  int EvN;
  int OrN;
  int BcN;
  int LsN;
  int crate;
  int slot;
  int fiber;
  int channel;
  int ieta;
  int iphi;
  int depth;
  int valid1;
  int valid2;
  int zsMnP1;
  int zsMnP2;
  int zsUns1;
  int zsUns2;
  int capcheck1;
  int capcheck2;
  int dvercheck1;
  int dvercheck2;
  int* vals1;
  int* vals2;
  int* emulVals1;
  int* emulVals2;
  int* l1Vals;
  int n1;
  int n2;
  int n_matched;
  int n_mismatched;
  int n1_unique;
  int n2_unique;
};



class tpdigi {
  
public:
  
  // default constructor
  tpdigi();
  
  // copy constructor
  tpdigi(const tpdigi& aTPDigi);
  
  // constructor
  tpdigi(const std::string& type, const int& iEntry,
         const int& crate, const int& slot, const int& fiber, const int& channel,
         const int& subdet, const int& ieta, const int& iphi, const int& depth);
  
  // distructor
  ~tpdigi();
  
  // methods
  std::string type() const { return m_type;   };
  int       iEntry() const { return m_iEntry; };
  
  int   crate() const { return m_crate;   };
  int    slot() const { return m_slot;    };
  int   fiber() const { return m_fiber;   };
  int channel() const { return m_channel; };
  
  int subdet() const { return m_subdet; };
  int   ieta() const { return m_ieta;   };
  int   iphi() const { return m_iphi;   };
  int  depth() const { return m_depth;  };
  
  int nTS() const { return m_nTS; };
  std::vector<int>* vals1() const { return m_vals1; };
  std::vector<int>* vals2() const { return m_vals2; };
  
  void SetVals(const int* vals1, const int* vals2);
  
private:
  
  std::string m_type;
  int m_iEntry;
  
  int m_crate;
  int m_slot;
  int m_fiber;
  int m_channel;
  
  int m_subdet;
  int m_ieta;
  int m_iphi;
  int m_depth;
  
  int m_nTS;
  std::vector<int>* m_vals1;
  std::vector<int>* m_vals2;
};

typedef std::vector<tpdigi> tpdigis;



// overload operator <<
std::ostream& operator<<(std::ostream& os, const crateslot& obj);

std::ostream& operator<<(std::ostream& os, const coord& obj);
bool operator==(const coord& obj1, const coord& obj2);
bool operator!=(const coord& obj1, const coord& obj2);
bool operator<(const coord& obj1, const coord& obj2);
bool operator>(const coord& obj1, const coord& obj2);
bool operator<=(const coord& obj1, const coord& obj2);
bool operator>=(const coord& obj1, const coord& obj2);

std::ostream& operator<<(std::ostream& os, const tpdigi& obj);

#endif
