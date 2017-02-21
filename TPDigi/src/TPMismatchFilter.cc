#include "HCALStudies/TPDigi/interface/TPMismatchFilter.h"



//
// constructors and destructor
//
TPMismatchFilter::TPMismatchFilter(const edm::ParameterSet& config):
  _emap(NULL)
{
  //now do what ever initialization is needed
  _tagData_VME    = config.getUntrackedParameter<edm::InputTag>("tagData_VME", edm::InputTag("hbhevmeDigis"));
  _tagData_uTCA   = config.getUntrackedParameter<edm::InputTag>("tagData_uTCA",edm::InputTag("hbheutcaDigis"));
  
  _tokData_VME    = consumes<HcalTrigPrimDigiCollection>(_tagData_VME);
  _tokData_uTCA   = consumes<HcalTrigPrimDigiCollection>(_tagData_uTCA);
}


TPMismatchFilter::~TPMismatchFilter()
{
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)
}


//
// member functions
//

// ------------ method called on each new Event  ------------
bool TPMismatchFilter::filter(edm::Event& ev, const edm::EventSetup& es)
{
  using namespace edm;
  
  //------------------------
  // initialize the hash map
  if( _emap == NULL )
  {
    edm::ESHandle<HcalDbService> dbs;
    es.get<HcalDbRecord>().get(dbs);
    _emap = dbs->getHcalMapping();
    
    std::vector<uint32_t> vhashVME;
    std::vector<uint32_t> vhashuTCA;

    vhashVME.push_back(HcalElectronicsId(hcaldqm::constants::FIBERCH_MIN,
                                         hcaldqm::constants::FIBER_VME_MIN, hcaldqm::constants::SPIGOT_MIN, hcaldqm::constants::CRATE_VME_MIN).rawId());
    vhashuTCA.push_back(HcalElectronicsId(hcaldqm::constants::CRATE_uTCA_MIN, hcaldqm::constants::SLOT_uTCA_MIN,
                                          hcaldqm::constants::FIBER_uTCA_MIN1, hcaldqm::constants::FIBERCH_MIN, false).rawId());

    _filter_VME.initialize(hcaldqm::filter::fFilter, hcaldqm::hashfunctions::fElectronics,vhashVME);
    _filter_uTCA.initialize(hcaldqm::filter::fFilter, hcaldqm::hashfunctions::fElectronics,vhashuTCA);

    _ehashmap_VME.initialize(_emap, hcaldqm::electronicsmap::fT2EHashMap,_filter_uTCA);
    _ehashmap_uTCA.initialize(_emap, hcaldqm::electronicsmap::fT2EHashMap,_filter_VME);
  }
  
  
  //----------------
  // get collections
  edm::Handle<HcalTrigPrimDigiCollection> cdata_vme;
  edm::Handle<HcalTrigPrimDigiCollection> cdata_utca;
  
  if( !ev.getByToken(_tokData_VME, cdata_vme) )
    std::cout << "Collection HcalTrigPrimDigiCollection isn't available "
      + _tagData_VME.label() + " " + _tagData_VME.instance() << std::endl;
  
  if( !ev.getByToken(_tokData_uTCA, cdata_utca) )
    std::cout << "Collection HcalTrigPrimDigiCollection isn't available "
      + _tagData_uTCA.label() + " " + _tagData_uTCA.instance() << std::endl;
  
  
  
  //-------------------
  // collectors for tps
  std::vector<coord> coords_VME;
  std::map<coord,const HcalTriggerPrimitiveDigi*> tpMap_VME;
  for(HcalTrigPrimDigiCollection::const_iterator it = cdata_vme->begin(); it != cdata_vme->end(); ++it)
  {
    HcalTrigTowerDetId detId = it -> id();
    HcalElectronicsId eId = HcalElectronicsId(_ehashmap_VME.lookup(detId));

    int crate   = eId.readoutVMECrateId();
    int slot    = eId.htrSlot();
    int tb      = eId.htrTopBottom();
    int fiber   = eId.slbSiteNumber();
    int channel = eId.slbChannelIndex();

    if( eId.isUTCAid() )
    {
      std::cout << "ERROR: uTCA eId found when VME expected";
      continue;
    }

    if( isAllZero(&(*it)) ) continue;

    // construct maps
    coord aCoord = GetCoord("TP",crate,slot,tb,fiber,channel);

    coords_VME.push_back(aCoord);  // VME
    
    std::map<coord,const HcalTriggerPrimitiveDigi*>::const_iterator mapIt = tpMap_VME.find(aCoord);
    if( mapIt == tpMap_VME.end() ) tpMap_VME[aCoord] = &(*it);
    else std::cout << "ERROR: VME TP already exists" << std::endl;
  }
  
  std::vector<coord> coords_uTCA;
  std::map<coord,const HcalTriggerPrimitiveDigi*> tpMap_uTCA;
  for(HcalTrigPrimDigiCollection::const_iterator it = cdata_utca->begin(); it != cdata_utca->end(); ++it)
  {
    HcalTrigTowerDetId detId = it -> id();
    HcalElectronicsId eId = HcalElectronicsId(_ehashmap_uTCA.lookup(detId));

    int crate   = eId.readoutVMECrateId();
    int slot    = eId.htrSlot();
    int tb      = eId.htrTopBottom();
    int fiber   = eId.fiberIndex();
    int channel = eId.fiberChanId();

    if( !eId.isUTCAid() )
    {
      std::cout << "ERROR: VME eId found when uTCA expected";
      continue;
    }

    if( isAllZero(&(*it)) ) continue;

    // construct maps
    coord aCoord = GetCoord("TP",crate,slot,tb,fiber,channel);
    coords_uTCA.push_back(aCoord);  // uTCA
    
    std::map<coord,const HcalTriggerPrimitiveDigi*>::const_iterator mapIt = tpMap_uTCA.find(aCoord);
    if( mapIt == tpMap_uTCA.end() ) tpMap_uTCA[aCoord] = &(*it);
    else std::cout << "ERROR: uTCA TP already exists" << std::endl;
  }
  
  
  
  // find common and unique tps
  std::vector<coord> coords_common;
  std::vector<coord> coords_matched;
  std::vector<coord> coords_mismatched;
  std::vector<coord> coords_unique_VME;
  std::vector<coord> coords_unique_uTCA;
  std::sort(coords_VME.begin(),coords_VME.end());
  std::sort(coords_uTCA.begin(),coords_uTCA.end());
  std::set_intersection(coords_VME.begin(),coords_VME.end(),
                        coords_uTCA.begin(),coords_uTCA.end(),
                        std::back_inserter(coords_common));
  std::set_difference(coords_VME.begin(),coords_VME.end(),
                      coords_common.begin(),coords_common.end(),
                      std::back_inserter(coords_unique_VME));
  std::set_difference(coords_uTCA.begin(),coords_uTCA.end(),
                      coords_common.begin(),coords_common.end(),
                      std::back_inserter(coords_unique_uTCA));
  
  // find matching and mismatching tps among common ones 
  for(unsigned int it = 0; it < coords_common.size(); ++it)
  {
    coord aCoord = coords_common.at(it);
    bool isMatching = matching(tpMap_VME[aCoord],tpMap_uTCA[aCoord]);
    if( isMatching ) coords_matched.push_back(aCoord);
    else             coords_mismatched.push_back(aCoord);
  }
  
  if( coords_unique_VME.size()  > 0 )
  {
    for(unsigned int it = 0; it < coords_unique_VME.size(); ++it)
    {
      coord aCoord = coords_unique_VME.at(it);
      //std::cout << "AAAA" << aCoord << std::endl;
      if( std::get<0>(aCoord) != 24 && std::get<1>(aCoord) != 5 )
        //if( std::get<0>(aCoord) == 24 && std::get<1>(aCoord) == 5 )
        return true;
    }
  }
  if( coords_unique_uTCA.size() > 0 )
  {
    for(unsigned int it = 0; it < coords_unique_uTCA.size(); ++it)
    {
      coord aCoord = coords_unique_uTCA.at(it);
      //std::cout << "BBBB" << aCoord << std::endl;
      if( std::get<0>(aCoord) != 24 && std::get<1>(aCoord) != 5 )
        //if( std::get<0>(aCoord) == 24 && std::get<1>(aCoord) == 5 )
        return true;
    }  
  }
  if( coords_mismatched.size()  > 0 )
  {
    for(unsigned int it = 0; it < coords_mismatched.size(); ++it)
    {
      coord aCoord = coords_mismatched.at(it);
      //std::cout << "CCCC" << aCoord << std::endl;
      if( std::get<0>(aCoord) != 24 && std::get<1>(aCoord) != 5 )
        //if( std::get<0>(aCoord) == 24 && std::get<1>(aCoord) == 5 )
        return true;
    }
  }
  return false;
}



DEFINE_FWK_MODULE(TPMismatchFilter);
