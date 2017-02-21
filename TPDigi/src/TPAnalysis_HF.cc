#include "HCALStudies/TPDigi/interface/TPAnalysis_HF.h"
#include "HCALStudies/TPDigi/interface/TPAnalysis.h"



TPAnalysis_HF::TPAnalysis_HF(edm::ParameterSet const& ps):
  _emap(NULL)
{
  //std::cout << "TPAnalysis_HF::TPAnalysis_HF" << std::endl;
  
  _tagData_uTCA   = ps.getUntrackedParameter<edm::InputTag>("tagData_uTCA",edm::InputTag("hfDigis"));
  _tagData_emul   = ps.getUntrackedParameter<edm::InputTag>("tagData_emul",edm::InputTag("emulutcaTPs"));
  _tagL1          = ps.getUntrackedParameter<edm::InputTag>("tagL1",       edm::InputTag("l1tCaloLayer1Digis"));
  
  _tokData_uTCA   = consumes<HcalTrigPrimDigiCollection>(_tagData_uTCA);
  _tokData_emul   = consumes<HcalTrigPrimDigiCollection>(_tagData_emul);
  _tokL1          = consumes<HcalTrigPrimDigiCollection>(_tagL1);
}



TPAnalysis_HF::~TPAnalysis_HF()
{
  //std::cout << "TPAnalysis_HF::~TPAnalysis_HF" << std::endl;
}


void TPAnalysis_HF::beginJob()
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
  
}



void TPAnalysis_HF::endJob()
{
  delete _treeVars.vals1;
  delete _treeVars.vals2;
  delete _treeVars.emulVals1;
  delete _treeVars.emulVals2;
  delete _treeVars.l1Vals;
}


void TPAnalysis_HF::analyze(const edm::Event& ev, const edm::EventSetup& es)
{
  // std::cout << "TPAnalysis_HF::analyze" << std::endl;
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
    
    std::vector<uint32_t> vhashuTCA;
    
    vhashuTCA.push_back(HcalElectronicsId(hcaldqm::constants::CRATE_uTCA_MIN, hcaldqm::constants::SLOT_uTCA_MIN,
                                          hcaldqm::constants::FIBER_uTCA_MIN1, hcaldqm::constants::FIBERCH_MIN, false).rawId());
    
    _filter_uTCA.initialize(hcaldqm::filter::fFilter, hcaldqm::hashfunctions::fElectronics,vhashuTCA);
    
    _ehashmap_uTCA.initialize(_emap, hcaldqm::electronicsmap::fT2EHashMap,_filter_uTCA);
  }
  
  
  
  //----------------
  // get collections
  edm::Handle<HcalTrigPrimDigiCollection> cdata_utca;
  edm::Handle<HcalTrigPrimDigiCollection> cemul_utca;
  edm::Handle<HcalTrigPrimDigiCollection> cl1;
  
  if( !ev.getByToken(_tokData_uTCA, cdata_utca) )
    std::cout << "Collection HcalTrigPrimDigiCollection isn't available "
      + _tagData_uTCA.label() + " " + _tagData_uTCA.instance() << std::endl;
  
  if( !ev.getByToken(_tokData_emul, cemul_utca) )
    std::cout << "Collection HcalTrigPrimDigiCollection isn't available "
      + _tagData_emul.label() + " " + _tagData_emul.instance() << std::endl;
  
  // done below
  // if( !ev.getByToken(_tokL1, cl1) )
  //   std::cout << "Collection HcalTrigPrimDigiCollection isn't available "
  //     + _tagL1.label() + " " + _tagL1.instance() << std::endl;
  
  
  
  //-------------------
  // collectors for tps
  
  std::vector<coord> coords_uTCA;
  std::vector<coord> coords_emul;
  std::vector<coord> coords_common;
  std::vector<coord> coords_matched;
  std::vector<coord> coords_mismatched;
  std::vector<coord> coords_unique_emul;
  std::vector<coord> coords_unique_uTCA;
  std::map<coord,const HcalTriggerPrimitiveDigi*> tpMap_uTCA;
  std::map<coord,const HcalTriggerPrimitiveDigi*> tpMap_emul;
  
  
  
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
  
  
  for(HcalTrigPrimDigiCollection::const_iterator it = cemul_utca->begin(); it != cemul_utca->end(); ++it)
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
    
    std::map<coord,const HcalTriggerPrimitiveDigi*>::const_iterator mapIt = tpMap_emul.find(aCoord);
    if( mapIt == tpMap_emul.end() ) tpMap_emul[aCoord] = &(*it);
    else std::cout << "ERROR: emulated uTCA TP already exists" << std::endl;
  }
  
  
  //----------------------
  // collectors for l1 tps
  
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
  std::sort(coords_uTCA.begin(),coords_uTCA.end());
  std::sort(coords_emul.begin(),coords_emul.end());
  std::set_intersection(coords_emul.begin(),coords_emul.end(),
                        coords_uTCA.begin(),coords_uTCA.end(),
                        std::back_inserter(coords_common));
  std::set_difference(coords_emul.begin(),coords_emul.end(),
                      coords_common.begin(),coords_common.end(),
                      std::back_inserter(coords_unique_emul));
  std::set_difference(coords_uTCA.begin(),coords_uTCA.end(),
                      coords_common.begin(),coords_common.end(),
                      std::back_inserter(coords_unique_uTCA));
   
  
  // find matching and mismatching tps among common ones
  for(unsigned int it = 0; it < coords_common.size(); ++it)
  {
    coord aCoord = coords_common.at(it);
    bool isMatching = matching(tpMap_emul[aCoord],tpMap_uTCA[aCoord]);
    if( isMatching ) coords_matched.push_back(aCoord);
    else             coords_mismatched.push_back(aCoord);
  }
  
  
  // fill counter ntuple
  _treeVars.n1 = coords_uTCA.size();
  _treeVars.n2 = coords_emul.size();
  _treeVars.n_matched = coords_matched.size();
  _treeVars.n_mismatched = coords_mismatched.size();
  _treeVars.n1_unique = coords_unique_uTCA.size();
  _treeVars.n2_unique = coords_unique_emul.size();
  _trees["TPcounters"] -> Fill();
  
  
  // fill TP ntuple - matched
  for(unsigned int it = 0; it < coords_matched.size(); ++it)
  {
    coord aCoord = coords_matched.at(it);
    HcalTrigTowerDetId detId = tpMap_uTCA[aCoord] -> id();    
    _treeVars.ieta  = detId.ieta();
    _treeVars.iphi  = detId.iphi();
    _treeVars.depth = detId.depth();
    
    const HcalTriggerPrimitiveDigi* l1TP = NULL;
    std::map<coord,const HcalTriggerPrimitiveDigi*>::const_iterator mapIt_L1 = l1tpMap.find(aCoord);
    if( mapIt_L1 != l1tpMap.end() ) l1TP = mapIt_L1->second;
    
    FillTreeVars(_treeVars,aCoord,tpMap_uTCA[aCoord],tpMap_emul[aCoord],NULL,NULL,l1TP);
    _trees["TPmatched"] -> Fill();
  }
  
  
  // fill TP ntuple - mismatched
  for(unsigned int it = 0; it < coords_mismatched.size(); ++it)
  {
    coord aCoord = coords_mismatched.at(it);
    HcalTrigTowerDetId detId = tpMap_uTCA[aCoord] -> id();
    _treeVars.ieta  = detId.ieta();
    _treeVars.iphi  = detId.iphi();
    _treeVars.depth = detId.depth();
    
    const HcalTriggerPrimitiveDigi* l1TP = NULL;
    std::map<coord,const HcalTriggerPrimitiveDigi*>::const_iterator mapIt_L1 = l1tpMap.find(aCoord);
    if( mapIt_L1 != l1tpMap.end() ) l1TP = mapIt_L1->second;
    
    FillTreeVars(_treeVars,aCoord,tpMap_uTCA[aCoord],tpMap_emul[aCoord],NULL,NULL,l1TP);
    _trees["TPmismatched"] -> Fill();
  }
  
  
  // fill TP ntuple - unique emul
  for(unsigned int it = 0; it < coords_unique_emul.size(); ++it)
  {
    coord aCoord = coords_unique_emul.at(it);
    HcalTrigTowerDetId detId = tpMap_emul[aCoord] -> id();    
    _treeVars.ieta  = detId.ieta();
    _treeVars.iphi  = detId.iphi();
    _treeVars.depth = detId.depth();
    
    const HcalTriggerPrimitiveDigi* l1TP = NULL;
    std::map<coord,const HcalTriggerPrimitiveDigi*>::const_iterator mapIt_L1 = l1tpMap.find(aCoord);
    if( mapIt_L1 != l1tpMap.end() ) l1TP = mapIt_L1->second;
    
    FillTreeVars(_treeVars,aCoord,NULL,tpMap_emul[aCoord],NULL,NULL,l1TP);
    _trees["TPunique1"] -> Fill();
  }
  
  
  // fill TP ntuple - unique uTCA
  for(unsigned int it = 0; it < coords_unique_uTCA.size(); ++it)
  {
    coord aCoord = coords_unique_uTCA.at(it);
    HcalTrigTowerDetId detId = tpMap_uTCA[aCoord] -> id();
    _treeVars.ieta  = detId.ieta();
    _treeVars.iphi  = detId.iphi();
    _treeVars.depth = detId.depth();
    
    const HcalTriggerPrimitiveDigi* l1TP = NULL;
    std::map<coord,const HcalTriggerPrimitiveDigi*>::const_iterator mapIt_L1 = l1tpMap.find(aCoord);
    if( mapIt_L1 != l1tpMap.end() ) l1TP = mapIt_L1->second;
    
    FillTreeVars(_treeVars,aCoord,tpMap_uTCA[aCoord],NULL,NULL,NULL,l1TP);
    _trees["TPunique2"] -> Fill();
  }
  
  
  _treeVars.iEntry += 1;
}

DEFINE_FWK_MODULE(TPAnalysis_HF);
