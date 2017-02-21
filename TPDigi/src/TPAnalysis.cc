#include "HCALStudies/TPDigi/interface/TPAnalysis.h"



bool isAllZero(const HcalTriggerPrimitiveDigi* tp1)
{
  
  for(int sIt = 0; sIt < tp1->size(); ++sIt)
  {
    if( (*tp1)[sIt].compressedEt() > 0 )
      return false;
  }
  
  return true;
}

bool matching(const HcalTriggerPrimitiveDigi* tp1, const HcalTriggerPrimitiveDigi* tp2)
{
  if( tp1->size() != tp2->size() ) return false;
  
  for(int sIt = 0; sIt < tp1->size(); ++sIt)
  {
    if( (*tp1)[sIt].compressedEt() != (*tp2)[sIt].compressedEt() )
      return false;
  }
  
  return true;
}

void FillTreeVars(treeVars& tv,coord& aCoord,
                  const HcalTriggerPrimitiveDigi* tp1, const HcalTriggerPrimitiveDigi* tp2,
                  const HcalTriggerPrimitiveDigi* etp1, const HcalTriggerPrimitiveDigi* etp2,
                  const HcalTriggerPrimitiveDigi* l1tp)
{
  tv.crate   = std::get<0>(aCoord);
  tv.slot    = std::get<1>(aCoord);
  tv.fiber   = std::get<2>(aCoord);
  tv.channel = std::get<3>(aCoord);
  
  if( tp1 )
  {
    tv.zsMnP1 = (*tp1).zsMarkAndPass();
    tv.zsUns1 = (*tp1).zsUnsuppressed();
  }
  else
  {
    tv.zsMnP1 = -1;
    tv.zsUns1 = -1;
  }

  if( tp2 )
  {
    tv.zsMnP2 = (*tp2).zsMarkAndPass();
    tv.zsUns2 = (*tp2).zsUnsuppressed();
  }
  else
  {
    tv.zsMnP2 = -1;
    tv.zsUns2 = -1;
  }
  
  int nTS = 4;
  
  if( tv.vals1 == NULL ) tv.vals1 = new int[nTS];
  if( tv.vals2 == NULL ) tv.vals2 = new int[nTS];
  if( tv.emulVals1 == NULL ) tv.emulVals1 = new int[nTS];
  if( tv.emulVals2 == NULL ) tv.emulVals2 = new int[nTS];
  if( tv.l1Vals == NULL ) tv.l1Vals = new int[nTS];
  
  for(int sIt = 0; sIt < nTS; ++sIt)
  {
    tv.vals1[sIt] = -1;
    tv.vals2[sIt] = -1;
    tv.emulVals1[sIt] = -1;
    tv.emulVals2[sIt] = -1;
    tv.l1Vals[sIt] = -1;
  }
  
  if( tp1 )
    for(int sIt = 0; sIt < (*tp1).size(); ++sIt)
      tv.vals1[sIt] = (*tp1)[sIt].compressedEt();
  
  if( tp2 )
    for(int sIt = 0; sIt < (*tp2).size(); ++sIt)
      tv.vals2[sIt] = (*tp2)[sIt].compressedEt();
  
  if( etp1 )
    for(int sIt = 0; sIt < (*etp1).size(); ++sIt)
      tv.emulVals1[sIt] = (*etp1)[sIt].compressedEt();
  
  if( etp2 )
    for(int sIt = 0; sIt < (*etp2).size(); ++sIt)
      tv.emulVals2[sIt] = (*etp2)[sIt].compressedEt();
  
  if( l1tp )
    for(int sIt = 0; sIt < (*l1tp).size(); ++sIt)
      tv.l1Vals[sIt] = (*l1tp)[0].compressedEt();
}



void PrintTP(const HcalTriggerPrimitiveDigi* tp)
{
  if( tp == NULL )
  {
    std::cout << "NULL";
    return;
  }
  
  std::cout << "(";
  for(int sIt = 0; sIt < (*tp).size(); ++sIt)
  {
    if( sIt < (((*tp).size())-1) )
      std::cout << std::setw(2) << (*tp)[sIt].compressedEt() << ",";
    else
      std::cout << std::setw(2) << (*tp)[sIt].compressedEt() << ")";
  }
}



TPAnalysis::TPAnalysis(edm::ParameterSet const& ps):
  _emap(NULL)
{
  //std::cout << "TPAnalysis::TPAnalysis" << std::endl;
  
  _tagData_VME    = ps.getUntrackedParameter<edm::InputTag>("tagData_VME", edm::InputTag("hcalDigis"));
  _tagData_uTCA   = ps.getUntrackedParameter<edm::InputTag>("tagData_uTCA",edm::InputTag("hcalDigis"));
  _tagEmul_VME    = ps.getUntrackedParameter<edm::InputTag>("tagEmul_VME", edm::InputTag("emulTPs"));
  _tagEmul_uTCA   = ps.getUntrackedParameter<edm::InputTag>("tagEmul_uTCA",edm::InputTag("emulTPs"));
  _tagL1          = ps.getUntrackedParameter<edm::InputTag>("tagL1",       edm::InputTag("l1tCaloLayer1Digis"));
  
  _tokData_VME    = consumes<HcalTrigPrimDigiCollection>(_tagData_VME);
  _tokData_uTCA   = consumes<HcalTrigPrimDigiCollection>(_tagData_uTCA);
  _tokEmul_VME    = consumes<HcalTrigPrimDigiCollection>(_tagEmul_VME);
  _tokEmul_uTCA   = consumes<HcalTrigPrimDigiCollection>(_tagEmul_uTCA);
  _tokL1          = consumes<HcalTrigPrimDigiCollection>(_tagL1);
}



TPAnalysis::~TPAnalysis()
{
  //std::cout << "TPAnalysis::~TPAnalysis" << std::endl;
}


void TPAnalysis::beginJob()
{
  // initialize variables
  _treeVars.iEntry = 0;
  _treeVars.vals1 = new int[4];
  _treeVars.vals2 = new int[4];
  _treeVars.emulVals1 = new int[4];
  _treeVars.emulVals2 = new int[4];
  _treeVars.l1Vals = new int[4];
  
  
  // TFileService for output ntuples
  edm::Service<TFileService> fs;
  if( !fs )
  {
    throw edm::Exception(edm::errors::Configuration, "TFile Service is not registered in cfg file");
  }
  
  
  // output ntuples
  std::vector<std::string> treeLabels;
  treeLabels.push_back("counters");
  treeLabels.push_back("matched");
  treeLabels.push_back("mismatched");
  treeLabels.push_back("unique1");
  treeLabels.push_back("unique2");
  
  for(unsigned int it2 = 0; it2 < treeLabels.size(); ++it2)
  {
    std::string treeLabel = treeLabels.at(it2);
    std::string label = "TP" + treeLabel;
    
    _trees[label] = fs->make<TTree>(Form("ntu_%s",label.c_str()),Form("ntu_%s",label.c_str()));  
    _trees[label] -> SetAutoSave(10000000000);
    _trees[label] -> SetAutoFlush(1000000);
    
    if( treeLabel == "counters" )
    {
      _trees[label] -> Branch("iEntry",  &_treeVars.iEntry,  "iEntry/I");
      _trees[label] -> Branch("EvN",     &_treeVars.EvN,        "EvN/I");
      _trees[label] -> Branch("OrN",     &_treeVars.OrN,        "OrN/I");
      _trees[label] -> Branch("BcN",     &_treeVars.BcN,        "BcN/I");
      _trees[label] -> Branch("LsN",     &_treeVars.LsN,        "LsN/I");
      _trees[label] -> Branch(Form("nTP1"          ),&_treeVars.n1,          Form("nTP1/I"));
      _trees[label] -> Branch(Form("nTP2"          ),&_treeVars.n2,          Form("nTP2/I"));
      _trees[label] -> Branch(Form("nTP_matched"   ),&_treeVars.n_matched,   Form("nTP_matched/I"));
      _trees[label] -> Branch(Form("nTP_mismatched"),&_treeVars.n_mismatched,Form("nTP_mismatched/I"));
      _trees[label] -> Branch(Form("nTP1_unique"   ),&_treeVars.n1_unique,   Form("nTP1_unique/I"));
      _trees[label] -> Branch(Form("nTP2_unique"   ),&_treeVars.n2_unique,   Form("nTP2_unique/I"));
    }
    else
    {
      _trees[label] -> Branch("iEntry",  &_treeVars.iEntry,  "iEntry/I");
      _trees[label] -> Branch("EvN",     &_treeVars.EvN,        "EvN/I");
      _trees[label] -> Branch("OrN",     &_treeVars.OrN,        "OrN/I");
      _trees[label] -> Branch("BcN",     &_treeVars.BcN,        "BcN/I");
      _trees[label] -> Branch("LsN",     &_treeVars.LsN,        "LsN/I");
      _trees[label] -> Branch("crate",   &_treeVars.crate,    "crate/I");
      _trees[label] -> Branch("slot",    &_treeVars.slot,      "slot/I");
      _trees[label] -> Branch("fiber",   &_treeVars.fiber,    "fiber/I");
      _trees[label] -> Branch("ieta",    &_treeVars.ieta,      "ieta/I");
      _trees[label] -> Branch("iphi",    &_treeVars.iphi,      "iphi/I");
      _trees[label] -> Branch("depth",   &_treeVars.depth,    "depth/I");
      _trees[label] -> Branch("channel", &_treeVars.channel,"channel/I");
      _trees[label] -> Branch("zsMnP1",  &_treeVars.zsMnP1,  "zsMnP1/I");
      _trees[label] -> Branch("zsMnP2",  &_treeVars.zsMnP2,  "zsMnP2/I");
      _trees[label] -> Branch("zsUns1",  &_treeVars.zsUns1,  "zsUns1/I");
      _trees[label] -> Branch("zsUns2",  &_treeVars.zsUns2,  "zsUns2/I");
      _trees[label] -> Branch(Form("TP1"),_treeVars.vals1,Form("TP1[4]/I"));
      _trees[label] -> Branch(Form("TP2"),_treeVars.vals2,Form("TP2[4]/I"));
      _trees[label] -> Branch(Form("emulTP1"),_treeVars.emulVals1,Form("emulTP1[4]/I"));
      _trees[label] -> Branch(Form("emulTP2"),_treeVars.emulVals2,Form("emulTP2[4]/I"));
      _trees[label] -> Branch(Form("L1TP"),_treeVars.l1Vals,Form("L1TP1[4]/I"));
    }
  }
  
  _p2_Et_phi_vs_eta = fs->make<TProfile2D>("h2_Et_phi_vs_eta","",83,-41.5,41.5,73,-0.5,72.5);
  
  for(int iBit = 0; iBit < 6; ++iBit)
  {
    _h2_FG_phi_vs_eta[iBit] = fs->make<TH2F>(Form("h2_FG%d_phi_vs_eta",iBit),"",83,-41.5,41.5,73,-0.5,72.5);
    _h2_FG_phi_vs_eta_cut[iBit] = fs->make<TH2F>(Form("h2_FG%d_phi_vs_eta_cut",iBit),"",83,-41.5,41.5,73,-0.5,72.5);
    _p2_FG_phi_vs_eta[iBit] = fs->make<TProfile2D>(Form("p2_FG%d_phi_vs_eta",iBit),"",83,-41.5,41.5,73,-0.5,72.5);
    
    _h2_FG_phi_vs_eta_emul[iBit] = fs->make<TH2F>(Form("h2_FG%d_phi_vs_eta_emul",iBit),"",83,-41.5,41.5,73,-0.5,72.5);
    _h2_FG_phi_vs_eta_cut_emul[iBit] = fs->make<TH2F>(Form("h2_FG%d_phi_vs_eta_cut_emul",iBit),"",83,-41.5,41.5,73,-0.5,72.5);
    _p2_FG_phi_vs_eta_emul[iBit] = fs->make<TProfile2D>(Form("p2_FG%d_phi_vs_eta_emul",iBit),"",83,-41.5,41.5,73,-0.5,72.5);
  }
}



void TPAnalysis::endJob()
{
  delete _treeVars.vals1;
  delete _treeVars.vals2;
  delete _treeVars.emulVals1;
  delete _treeVars.emulVals2;
  delete _treeVars.l1Vals;
}


void TPAnalysis::analyze(const edm::Event& ev, const edm::EventSetup& es)
{
  // std::cout << "TPAnalysis::analyze" << std::endl;
  // std::cout << "------------------------------" << std::endl;
  // std::cout << ">>> run: "   << ev.id().run() << std::endl;
  // std::cout << ">>> LS:  "   << ev.luminosityBlock() << std::endl;
  // std::cout << ">>> BX: "    << ev.bunchCrossing() << std::endl;
  // std::cout << ">>> event: " << ev.id().event() << std::endl;
  // std::cout << "------------------------------" << std::endl;
  _treeVars.EvN = ev.id().event();
  _treeVars.OrN = ev.orbitNumber();
  _treeVars.BcN = ev.bunchCrossing();
  _treeVars.LsN = ev.luminosityBlock();
  
  
  
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
  edm::Handle<HcalTrigPrimDigiCollection> cemul_vme;
  edm::Handle<HcalTrigPrimDigiCollection> cemul_utca;
  edm::Handle<HcalTrigPrimDigiCollection> cl1;
  
  if( !ev.getByToken(_tokData_VME, cdata_vme) )
    std::cout << "Collection HcalTrigPrimDigiCollection isn't available "
      + _tagData_VME.label() + " " + _tagData_VME.instance() << std::endl;
  
  if( !ev.getByToken(_tokData_uTCA, cdata_utca) )
    std::cout << "Collection HcalTrigPrimDigiCollection isn't available "
      + _tagData_uTCA.label() + " " + _tagData_uTCA.instance() << std::endl;
  
  if( !ev.getByToken(_tokEmul_VME, cemul_vme) )
    std::cout << "Collection HcalTrigPrimDigiCollection isn't available "
      + _tagEmul_VME.label() + " " + _tagEmul_VME.instance() << std::endl;
  
  if( !ev.getByToken(_tokEmul_uTCA, cemul_utca) )
    std::cout << "Collection HcalTrigPrimDigiCollection isn't available "
      + _tagEmul_uTCA.label() + " " + _tagEmul_uTCA.instance() << std::endl;
  
  // done below
  // if( !ev.getByToken(_tokL1, cl1) )
  //   std::cout << "Collection HcalTrigPrimDigiCollection isn't available "
  //     + _tagL1.label() + " " + _tagL1.instance() << std::endl;
  
  
  
  //-------------------
  // collectors for tps
  
  std::vector<coord> coords_VME;
  std::vector<coord> coords_uTCA;
  std::vector<coord> coords_common;
  std::vector<coord> coords_matched;
  std::vector<coord> coords_mismatched;
  std::vector<coord> coords_unique_VME;
  std::vector<coord> coords_unique_uTCA;
  std::map<coord,const HcalTriggerPrimitiveDigi*> tpMap_VME;
  std::map<coord,const HcalTriggerPrimitiveDigi*> tpMap_uTCA;
  
  
  // for(HcalTrigPrimDigiCollection::const_iterator it = cdata_vme->begin(); it != cdata_vme->end(); ++it)
  // {
  //   HcalTrigTowerDetId detId = it -> id();
  //   HcalElectronicsId eId = HcalElectronicsId(_ehashmap_VME.lookup(detId));
    
  //   int crate   = eId.readoutVMECrateId();
  //   int slot    = eId.htrSlot();
  //   int tb      = eId.htrTopBottom();
  //   int fiber   = eId.slbSiteNumber();
  //   int channel = eId.slbChannelIndex();
    
  //   if( eId.isUTCAid() )
  //   {
  //     std::cout << "ERROR: uTCA eId found when VME expected";
  //     continue;
  //   }
    
  //   if( isAllZero(&(*it)) ) continue;
    
  //   // construct maps
  //   coord aCoord = GetCoord("TP",crate,slot,tb,fiber,channel);
    
  //   coords_VME.push_back(aCoord);  // VME
    
  //   std::map<coord,const HcalTriggerPrimitiveDigi*>::const_iterator mapIt = tpMap_VME.find(aCoord);
  //   if( mapIt == tpMap_VME.end() ) tpMap_VME[aCoord] = &(*it);
  //   else std::cout << "ERROR: VME TP already exists" << std::endl;
  // }
  
  
  for(HcalTrigPrimDigiCollection::const_iterator it = cdata_vme->begin(); it != cdata_vme->end(); ++it)
  {
    HcalTrigTowerDetId detId = it -> id();
    HcalElectronicsId eId = HcalElectronicsId(_ehashmap_uTCA.lookup(detId));
    
    if( detId.version() == 0 && detId.ietaAbs() >= 29 ) continue;
    
    int crate   = eId.readoutVMECrateId();
    int slot    = eId.htrSlot();
    int tb      = eId.htrTopBottom();
    int fiber   = eId.fiberIndex();
    int channel = eId.fiberChanId();
    
    if( crate == 36 ) std::cout << "!!! CRATE 36 !!!" << std::endl;
    
    if( !eId.isUTCAid() )
    {
      std::cout << "ERROR: VME eId found when uTCA expected";
      continue;
    }
    
    if( isAllZero(&(*it)) ) continue;
    
    // construct maps
    coord aCoord = GetCoord("TP",crate,slot,tb,fiber,channel);
    coords_VME.push_back(aCoord);  // uTCA
    
    std::map<coord,const HcalTriggerPrimitiveDigi*>::const_iterator mapIt = tpMap_VME.find(aCoord);
    if( mapIt == tpMap_VME.end() ) tpMap_VME[aCoord] = &(*it);
    else std::cout << "ERROR: uTCA TP already exists" << std::endl;
  }
  
  
  for(HcalTrigPrimDigiCollection::const_iterator it = cdata_utca->begin(); it != cdata_utca->end(); ++it)
  {
    HcalTrigTowerDetId detId = it -> id();
    HcalElectronicsId eId = HcalElectronicsId(_ehashmap_uTCA.lookup(detId));
    
    if( detId.version() == 0 && detId.ietaAbs() >= 29 ) continue;
    
    int crate   = eId.readoutVMECrateId();
    int slot    = eId.htrSlot();
    int tb      = eId.htrTopBottom();
    int fiber   = eId.fiberIndex();
    int channel = eId.fiberChanId();

    
    for (uint32_t ibit=0; ibit<5; ibit++)
      std::cout << it->t0().fineGrain(ibit) << " ";
    std::cout << " " << crate << " " << slot << " " << fiber << " " << channel << " " << detId.ieta() << " " << detId.iphi() << std::endl;
    
    
    // Fine-Grain bits
    _p2_Et_phi_vs_eta -> Fill(detId.ieta(),detId.iphi(),it->SOI_compressedEt());
    for (int iBit = 0; iBit < 5; ++iBit)
    {
      if( it->t0().fineGrain(iBit) ) _h2_FG_phi_vs_eta_cut[iBit] -> Fill(detId.ieta(),detId.iphi());
      _h2_FG_phi_vs_eta[iBit] -> Fill(detId.ieta(),detId.iphi());
      _p2_FG_phi_vs_eta[iBit] -> Fill(detId.ieta(),detId.iphi(),it->t0().fineGrain(iBit));
    }
    
    
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
  
  
  
  //----------------------------
  // collectors for emulated tps
  
  std::map<coord,const HcalTriggerPrimitiveDigi*> emultpMap_VME;
  std::map<coord,const HcalTriggerPrimitiveDigi*> emultpMap_uTCA;
  
  for(HcalTrigPrimDigiCollection::const_iterator it = cemul_vme->begin(); it != cemul_vme->end(); ++it)
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
    
    // construct maps
    coord aCoord = GetCoord("TP",crate,slot,tb,fiber,channel);
    
    std::map<coord,const HcalTriggerPrimitiveDigi*>::const_iterator mapIt = emultpMap_VME.find(aCoord);
    if( mapIt == emultpMap_VME.end() ) emultpMap_VME[aCoord] = &(*it);
    else
    {
      // std::cout << "ERROR: emulated VME TP already exists" << std::endl;
      // std::cout << "aCoord: " << aCoord << std::endl;
      // std::cout << "detId: " << detId << std::endl;
      // std::cout << "eId: " << eId << std::endl;
    }
  }
  
  
  for(HcalTrigPrimDigiCollection::const_iterator it = cemul_utca->begin(); it != cemul_utca->end(); ++it)
  {
    HcalTrigTowerDetId detId = it -> id();
    HcalElectronicsId eId = HcalElectronicsId(_ehashmap_uTCA.lookup(detId));
    
    int crate   = eId.readoutVMECrateId();
    int slot    = eId.htrSlot();
    int tb      = eId.htrTopBottom();
    int fiber   = eId.fiberIndex();
    int channel = eId.fiberChanId();
    
    //std::cout << "emul sample: " << (*it) << std::endl;
    // Fine-Grain bits
    for (int iBit = 0; iBit < 5; ++iBit)
    {
      if( it->t0().fineGrain(iBit) != 0 ) _h2_FG_phi_vs_eta_cut_emul[iBit] -> Fill(detId.ieta(),detId.iphi());
      _h2_FG_phi_vs_eta_emul[iBit] -> Fill(detId.ieta(),detId.iphi());
      _p2_FG_phi_vs_eta_emul[iBit] -> Fill(detId.ieta(),detId.iphi(),it->t0().fineGrain(iBit));
    }
    
    if( !eId.isUTCAid() )
    {
      std::cout << "ERROR: VME eId found when uTCA expected";
      continue;
    }
    
    // construct maps
    coord aCoord = GetCoord("TP",crate,slot,tb,fiber,channel);
    
    std::map<coord,const HcalTriggerPrimitiveDigi*>::const_iterator mapIt = emultpMap_uTCA.find(aCoord);
    if( mapIt == emultpMap_uTCA.end() ) emultpMap_uTCA[aCoord] = &(*it);
    // else std::cout << "ERROR: emulated uTCA TP already exists" << std::endl;
  }
  
  
  //----------------------------
  // collectors for emulated tps
  
  std::map<coord,const HcalTriggerPrimitiveDigi*> l1tpMap;
  
  if( ev.getByToken(_tokL1, cl1) )
  {    
    for(HcalTrigPrimDigiCollection::const_iterator it = cl1->begin(); it != cl1->end(); ++it)
    {
      HcalTrigTowerDetId detId = it -> id();
      HcalElectronicsId eId = HcalElectronicsId(_ehashmap_uTCA.lookup(detId));
      
      int crate   = eId.readoutVMECrateId();
      int slot    = eId.htrSlot();
      int tb      = eId.htrTopBottom();
      int fiber   = eId.fiberIndex();
      int channel = eId.fiberChanId();
      
      if( isAllZero(&(*it)) ) continue;
      
      // construct maps
      coord aCoord = GetCoord("TP",crate,slot,tb,fiber,channel);

      std::map<coord,const HcalTriggerPrimitiveDigi*>::const_iterator mapIt = l1tpMap.find(aCoord);
      if( mapIt == l1tpMap.end() ) l1tpMap[aCoord] = &(*it);
      else std::cout << "ERROR: L1 TP already exists" << std::endl;
    }
  }
  
  
  // find common and unique tps
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
  
  
  // fill counter ntuple
  _treeVars.n1 = coords_VME.size();
  _treeVars.n2 = coords_uTCA.size();
  _treeVars.n_matched = coords_matched.size();
  _treeVars.n_mismatched = coords_mismatched.size();
  _treeVars.n1_unique = coords_unique_VME.size();
  _treeVars.n2_unique = coords_unique_uTCA.size();
  _trees["TPcounters"] -> Fill();
  
  
  // fill TP ntuple - matched
  for(unsigned int it = 0; it < coords_matched.size(); ++it)
  {
    coord aCoord = coords_matched.at(it);
    HcalTrigTowerDetId detId = tpMap_VME[aCoord] -> id();    
    _treeVars.ieta  = detId.ieta();
    _treeVars.iphi  = detId.iphi();
    _treeVars.depth = detId.depth();
    
    const HcalTriggerPrimitiveDigi* emulTP_VME = NULL;
    std::map<coord,const HcalTriggerPrimitiveDigi*>::const_iterator mapIt_VME = emultpMap_VME.find(aCoord);
    if( mapIt_VME != emultpMap_VME.end() ) emulTP_VME = mapIt_VME->second;
    const HcalTriggerPrimitiveDigi* emulTP_uTCA = NULL;
    std::map<coord,const HcalTriggerPrimitiveDigi*>::const_iterator mapIt_uTCA = emultpMap_uTCA.find(aCoord);
    if( mapIt_uTCA != emultpMap_uTCA.end() ) emulTP_uTCA = mapIt_uTCA->second;
    const HcalTriggerPrimitiveDigi* l1TP = NULL;
    std::map<coord,const HcalTriggerPrimitiveDigi*>::const_iterator mapIt_L1 = l1tpMap.find(aCoord);
    if( mapIt_L1 != l1tpMap.end() ) l1TP = mapIt_L1->second;
    
    FillTreeVars(_treeVars,aCoord,tpMap_VME[aCoord],tpMap_uTCA[aCoord],emulTP_VME,emulTP_uTCA,l1TP);
    _trees["TPmatched"] -> Fill();
    
    if( _treeVars.EvN == 57056 )
    {
      std::cout << "matched: " << aCoord << " --->   VME=";
      PrintTP(tpMap_VME[aCoord]);
      std::cout << "   uTCA=";
      PrintTP(tpMap_uTCA[aCoord]);
      std::cout << "   VME_emul=";
      PrintTP(emulTP_VME);
      std::cout << "   uTCA_emul=";
      PrintTP(emulTP_uTCA);
      std::cout << std::endl;
    }
  }
  
  
  // fill TP ntuple - mismatched
  for(unsigned int it = 0; it < coords_mismatched.size(); ++it)
  {
    coord aCoord = coords_mismatched.at(it);
    HcalTrigTowerDetId detId = tpMap_VME[aCoord] -> id();    
    _treeVars.ieta  = detId.ieta();
    _treeVars.iphi  = detId.iphi();
    _treeVars.depth = detId.depth();
    
    const HcalTriggerPrimitiveDigi* emulTP_VME = NULL;
    std::map<coord,const HcalTriggerPrimitiveDigi*>::const_iterator mapIt_VME = emultpMap_VME.find(aCoord);
    if( mapIt_VME != emultpMap_VME.end() ) emulTP_VME = mapIt_VME->second;
    const HcalTriggerPrimitiveDigi* emulTP_uTCA = NULL;
    std::map<coord,const HcalTriggerPrimitiveDigi*>::const_iterator mapIt_uTCA = emultpMap_uTCA.find(aCoord);
    if( mapIt_uTCA != emultpMap_uTCA.end() ) emulTP_uTCA = mapIt_uTCA->second;
    const HcalTriggerPrimitiveDigi* l1TP = NULL;
    std::map<coord,const HcalTriggerPrimitiveDigi*>::const_iterator mapIt_L1 = l1tpMap.find(aCoord);
    if( mapIt_L1 != l1tpMap.end() ) l1TP = mapIt_L1->second;
    
    FillTreeVars(_treeVars,aCoord,tpMap_VME[aCoord],tpMap_uTCA[aCoord],emulTP_VME,emulTP_uTCA,l1TP);
    _trees["TPmismatched"] -> Fill();
    
    if( _treeVars.EvN == 57056 )
    {
      std::cout << "mismatched: " << aCoord << " --->   VME=";
      PrintTP(tpMap_VME[aCoord]);
      std::cout << "   uTCA=";
      PrintTP(tpMap_uTCA[aCoord]);
      std::cout << "   VME_emul=";
      PrintTP(emulTP_VME);
      std::cout << "   uTCA_emul=";
      PrintTP(emulTP_uTCA);
      std::cout << std::endl;
    }
  }
  
  
  // fill TP ntuple - unique VME
  for(unsigned int it = 0; it < coords_unique_VME.size(); ++it)
  {
    coord aCoord = coords_unique_VME.at(it);
    HcalTrigTowerDetId detId = tpMap_VME[aCoord] -> id();    
    _treeVars.ieta  = detId.ieta();
    _treeVars.iphi  = detId.iphi();
    _treeVars.depth = detId.depth();
    
    const HcalTriggerPrimitiveDigi* emulTP_VME = NULL;
    std::map<coord,const HcalTriggerPrimitiveDigi*>::const_iterator mapIt_VME = emultpMap_VME.find(aCoord);
    if( mapIt_VME != emultpMap_VME.end() ) emulTP_VME = mapIt_VME->second;
    const HcalTriggerPrimitiveDigi* emulTP_uTCA = NULL;
    std::map<coord,const HcalTriggerPrimitiveDigi*>::const_iterator mapIt_uTCA = emultpMap_uTCA.find(aCoord);
    if( mapIt_uTCA != emultpMap_uTCA.end() ) emulTP_uTCA = mapIt_uTCA->second;
    const HcalTriggerPrimitiveDigi* l1TP = NULL;
    std::map<coord,const HcalTriggerPrimitiveDigi*>::const_iterator mapIt_L1 = l1tpMap.find(aCoord);
    if( mapIt_L1 != l1tpMap.end() ) l1TP = mapIt_L1->second;
    
    FillTreeVars(_treeVars,aCoord,tpMap_VME[aCoord],NULL,emulTP_VME,emulTP_uTCA,l1TP);
    _trees["TPunique1"] -> Fill();
    
    if( _treeVars.EvN == 57056 )
    {
      std::cout << "unique: " << aCoord << " --->   VME=";
      PrintTP(tpMap_VME[aCoord]);
      std::cout << "   uTCA=";
      PrintTP(tpMap_uTCA[aCoord]);
      std::cout << "   VME_emul=";
      PrintTP(emulTP_VME);
      std::cout << "   uTCA_emul=";
      PrintTP(emulTP_uTCA);
      std::cout << std::endl;
    }
  }
  
  
  // fill TP ntuple - unique uTCA
  for(unsigned int it = 0; it < coords_unique_uTCA.size(); ++it)
  {
    coord aCoord = coords_unique_uTCA.at(it);
    HcalTrigTowerDetId detId = tpMap_uTCA[aCoord] -> id();
    _treeVars.ieta  = detId.ieta();
    _treeVars.iphi  = detId.iphi();
    _treeVars.depth = detId.depth();
    
    const HcalTriggerPrimitiveDigi* emulTP_VME = NULL;
    std::map<coord,const HcalTriggerPrimitiveDigi*>::const_iterator mapIt_VME = emultpMap_VME.find(aCoord);
    if( mapIt_VME != emultpMap_VME.end() ) emulTP_VME = mapIt_VME->second;
    const HcalTriggerPrimitiveDigi* emulTP_uTCA = NULL;
    std::map<coord,const HcalTriggerPrimitiveDigi*>::const_iterator mapIt_uTCA = emultpMap_uTCA.find(aCoord);
    if( mapIt_uTCA != emultpMap_uTCA.end() ) emulTP_uTCA = mapIt_uTCA->second;
    const HcalTriggerPrimitiveDigi* l1TP = NULL;
    std::map<coord,const HcalTriggerPrimitiveDigi*>::const_iterator mapIt_L1 = l1tpMap.find(aCoord);
    if( mapIt_L1 != l1tpMap.end() ) l1TP = mapIt_L1->second;
    
    FillTreeVars(_treeVars,aCoord,NULL,tpMap_uTCA[aCoord],emulTP_VME,emulTP_uTCA,l1TP);
    _trees["TPunique2"] -> Fill();
    
    if( _treeVars.EvN == 57056 )
    {
      std::cout << "unique: " << aCoord << " --->   VME=";
      PrintTP(tpMap_VME[aCoord]);
      std::cout << "   uTCA=";
      PrintTP(tpMap_uTCA[aCoord]);
      std::cout << "   VME_emul=";
      PrintTP(emulTP_VME);
      std::cout << "   uTCA_emul=";
      PrintTP(emulTP_uTCA);
      PrintTP(emulTP_VME);
      std::cout << "   L1=";
      PrintTP(l1TP);
      std::cout << std::endl;
    }
  }
  
  
  _treeVars.iEntry += 1;
}

DEFINE_FWK_MODULE(TPAnalysis);
