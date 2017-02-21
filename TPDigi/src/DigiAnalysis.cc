#include "HCALStudies/TPDigi/interface/DigiAnalysis.h"



bool isAllZero(const HFDataFrame* df1)
{

  for(int sIt = 0; sIt < df1->size(); ++sIt)
  {
    if( (*df1)[sIt].adc() > 0 )
      return false;
  }

  return true;
}

bool matching(const HFDataFrame* df1, const HFDataFrame* df2)
{
  if( df1->size() != df2->size() ) return false;
  
  for(int sIt = 0; sIt < df1->size(); ++sIt)
  {
    if( (*df1)[sIt].adc() != (*df2)[sIt].adc() )
      return false;
  }
  
  return true;
}

void FillTreeVars(treeVars& tv, coord& aCoord, const HFDataFrame* df1, const HFDataFrame* df2)
{
  tv.crate   = std::get<0>(aCoord);
  tv.slot    = std::get<1>(aCoord);
  tv.fiber   = std::get<2>(aCoord);
  tv.channel = std::get<3>(aCoord);
  
  if( df1 )
  {
    tv.valid1 = (*df1).validate();
    tv.zsMnP1 = (*df1).zsMarkAndPass();
    tv.zsUns1 = (*df1).zsUnsuppressed();
    
    int capok = 1;
    int dverok = 1;
    int lastcapid=0, capid=0;
    for(int i = 0; i < (*df1).size(); ++i)
    {
      capid = (*df1)[i].capid();
      if( i != 0 && ((lastcapid+1)%4) != capid ) 
        capok = 0;
      if( (*df1)[i].er() || !(*df1)[i].dv() )
        dverok = 0;
      lastcapid = capid;
    }
    tv.capcheck1 = capok;
    tv.dvercheck1 = dverok;
  }
  else
  {
    tv.valid1 = -1;
    tv.zsMnP1 = -1;
    tv.zsUns1 = -1;
    tv.capcheck1 = -1; 
    tv.dvercheck1 = -1; 
  }
  
  if( df2 )
  {
    tv.valid2 = (*df2).validate();
    tv.zsMnP2 = (*df2).zsMarkAndPass();
    tv.zsUns2 = (*df2).zsUnsuppressed();
    
    int capok = 1;
    int dverok = 1;
    int lastcapid=0, capid=0;
    for(int i = 0; i < (*df2).size(); ++i)
    {
      capid = (*df2)[i].capid();
      if( i != 0 && ((lastcapid+1)%4) != capid ) 
        capok = 0;
      if( (*df2)[i].er() || !(*df2)[i].dv() )
        dverok = 0;
      lastcapid = capid;
    }
    tv.capcheck2 = capok;
    tv.dvercheck2 = dverok;
  }
  else
  {
    tv.valid2 = -1;
    tv.zsMnP2 = -1;
    tv.zsUns2 = -1;
    tv.capcheck2 = -1;
    tv.dvercheck2 = -1;
  }
  
  int nTS = -1;
  if( df1 ) nTS = df1 -> size();
  if( df2 ) nTS = df2 -> size();

  if( tv.vals1 == NULL ) tv.vals1 = new int[nTS];
  if( tv.vals2 == NULL ) tv.vals2 = new int[nTS];
  
  for(int sIt = 0; sIt < nTS; ++sIt)
  {
    if( df1 ) tv.vals1[sIt] = (*df1)[sIt].adc();
    else      tv.vals1[sIt] = -1;
    
    if( df2 ) tv.vals2[sIt] = (*df2)[sIt].adc();
    else      tv.vals2[sIt] = -1;
  }
}



DigiAnalysis::DigiAnalysis(edm::ParameterSet const& ps)
{
  //std::cout << "DigiAnalysis::DigiAnalysis" << std::endl;
  
  _tagHF_VME  = ps.getUntrackedParameter<edm::InputTag>("tagHF_VME", edm::InputTag("hfDigis"));
  _tagHF_uTCA = ps.getUntrackedParameter<edm::InputTag>("tagHF_uTCA",edm::InputTag("hfDigis"));
  _tagHFReport_VME  = ps.getUntrackedParameter<edm::InputTag>("tagHFReport", edm::InputTag("hfDigis"));
  _tagHFReport_uTCA = ps.getUntrackedParameter<edm::InputTag>("tagHFReport", edm::InputTag("hfDigis"));
  _tagHFReport        = ps.getUntrackedParameter<edm::InputTag>("tagHFReport",   edm::InputTag("hfDigis"));
  _tagHOReport        = ps.getUntrackedParameter<edm::InputTag>("tagHOReport",   edm::InputTag("hfDigis"));
  
  _tokHF_VME  = consumes<HFDigiCollection>(_tagHF_VME);
  _tokHF_uTCA = consumes<HFDigiCollection>(_tagHF_uTCA);
  _tokHFReport_VME  = consumes<HcalUnpackerReport>(_tagHFReport_VME);
  _tokHFReport_uTCA = consumes<HcalUnpackerReport>(_tagHFReport_uTCA);
  _tokHFReport        = consumes<HcalUnpackerReport>(_tagHFReport);
  _tokHOReport        = consumes<HcalUnpackerReport>(_tagHOReport);
}



DigiAnalysis::~DigiAnalysis()
{
  //std::cout << "DigiAnalysis::~DigiAnalysis" << std::endl;
}


void DigiAnalysis::beginJob()
{
  // initialize variables
  _treeVars.iEntry = 0;
  _treeVars.vals1 = new int[10];
  _treeVars.vals2 = new int[10];
  
  
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
  
  std::string typeLabel = "QIE";
  int nTS = 10;
  
  for(unsigned int it = 0; it < treeLabels.size(); ++it)
  {
    std::string treeLabel = treeLabels.at(it);
    std::string label = typeLabel + treeLabel;
    
    _trees[label] = fs->make<TTree>(Form("ntu_%s",label.c_str()),Form("ntu_%s",label.c_str()));  
    _trees[label] -> SetAutoSave(10000000000);
    _trees[label] -> SetAutoFlush(1000000);
    
    if( treeLabel == "counters" )
    {
      _trees[label] -> Branch(Form("n%s1",          typeLabel.c_str()),&_treeVars.n1,          Form("n%s1/I",typeLabel.c_str()));
      _trees[label] -> Branch(Form("n%s2",          typeLabel.c_str()),&_treeVars.n2,          Form("n%s2/I",typeLabel.c_str()));
      _trees[label] -> Branch(Form("n%s_matched",   typeLabel.c_str()),&_treeVars.n_matched,   Form("n%s_matched/I",typeLabel.c_str()));
      _trees[label] -> Branch(Form("n%s_mismatched",typeLabel.c_str()),&_treeVars.n_mismatched,Form("n%s_mismatched/I",typeLabel.c_str()));
      _trees[label] -> Branch(Form("n%s1_unique",   typeLabel.c_str()),&_treeVars.n1_unique,   Form("n%s1_unique/I",typeLabel.c_str()));
      _trees[label] -> Branch(Form("n%s2_unique",   typeLabel.c_str()),&_treeVars.n2_unique,   Form("n%s2_unique/I",typeLabel.c_str()));
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
      _trees[label] -> Branch("channel", &_treeVars.channel,"channel/I");
      _trees[label] -> Branch("ieta",    &_treeVars.ieta,      "ieta/I");
      _trees[label] -> Branch("iphi",    &_treeVars.iphi,      "iphi/I");
      _trees[label] -> Branch("depth",   &_treeVars.depth,    "depth/I");
      _trees[label] -> Branch("valid1",  &_treeVars.valid1,  "valid1/I");
      _trees[label] -> Branch("valid2",  &_treeVars.valid2,  "valid2/I");
      _trees[label] -> Branch("zsMnP1",  &_treeVars.zsMnP1,  "zsMnP1/I");
      _trees[label] -> Branch("zsMnP2",  &_treeVars.zsMnP2,  "zsMnP2/I");
      _trees[label] -> Branch("zsUns1",  &_treeVars.zsUns1,  "zsUns1/I");
      _trees[label] -> Branch("zsUns2",  &_treeVars.zsUns2,  "zsUns2/I");
      _trees[label] -> Branch("capcheck1",  &_treeVars.capcheck1,  "capcheck1/I");
      _trees[label] -> Branch("capcheck2",  &_treeVars.capcheck2,  "capcheck2/I");
      _trees[label] -> Branch("dvercheck1",  &_treeVars.dvercheck1,  "dvercheck1/I");
      _trees[label] -> Branch("dvercheck2",  &_treeVars.dvercheck2,  "dvercheck2/I");
      _trees[label] -> Branch(Form("%s1",typeLabel.c_str()),_treeVars.vals1,Form("%s1[%d]/I",typeLabel.c_str(),nTS));
      _trees[label] -> Branch(Form("%s2",typeLabel.c_str()),_treeVars.vals2,Form("%s2[%d]/I",typeLabel.c_str(),nTS));
    }
  }
  
}



void DigiAnalysis::endJob()
{}


void DigiAnalysis::analyze(const edm::Event& ev, const edm::EventSetup& es)
{
  // std::cout << "DigiAnalysis::analyze" << std::endl;
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
  
  
  //----------------
  // get collections
  edm::Handle<HFDigiCollection> chbhe_vme;
  edm::Handle<HFDigiCollection> chbhe_utca;
  edm::Handle<HcalUnpackerReport> chbhereport_vme;
  edm::Handle<HcalUnpackerReport> chbhereport_utca;  
  edm::Handle<HcalUnpackerReport> chfreport;
  edm::Handle<HcalUnpackerReport> choreport;
  
  if( !ev.getByToken(_tokHF_VME, chbhe_vme) )
    std::cout << "Collection HFDigiCollection isn't available "
      + _tagHF_VME.label() + " " + _tagHF_VME.instance() << std::endl;
  
  if( !ev.getByToken(_tokHF_uTCA, chbhe_utca) )
    std::cout << "Collection HFDigiCollection isn't available "
      + _tagHF_uTCA.label() + " " + _tagHF_uTCA.instance() << std::endl;
  
  if( !ev.getByToken(_tokHFReport_VME, chbhereport_vme) )
    std::cout << "Collection HcalUnpackerReport isn't available " + _tagHFReport_VME.label() + " " + _tagHFReport_VME.instance() << std::endl;
  
  if( !ev.getByToken(_tokHFReport_uTCA, chbhereport_utca) )
    std::cout << "Collection HcalUnpackerReport isn't available " + _tagHFReport_uTCA.label() + " " + _tagHFReport_uTCA.instance() << std::endl;
  
  if( !ev.getByToken(_tokHFReport, chfreport) )
    std::cout << "Collection HcalUnpackerReport isn't available " + _tagHFReport.label() + " " + _tagHFReport.instance() << std::endl;
  
  if( !ev.getByToken(_tokHOReport, choreport) )
    std::cout << "Collection HcalUnpackerReport isn't available " + _tagHOReport.label() + " " + _tagHOReport.instance() << std::endl;

  if( chbhereport_vme  -> badQualityDigis() > 0 || 
      chbhereport_utca -> badQualityDigis() > 0 || 
      chfreport -> badQualityDigis() > 0 || 
      choreport -> badQualityDigis() > 0
    )
    std::cout << "LS: "    << std::setw(3) << _treeVars.LsN << " "
              << "BX: " << std::setw(4) << _treeVars.BcN << "   "
              << "OR: " << std::setw(4) << _treeVars.OrN << "   "
              << chbhereport_vme -> badQualityDigis() << " "
              << chbhereport_utca -> badQualityDigis() << " "
              << chfreport -> badQualityDigis() << " "
              << choreport -> badQualityDigis()
              << std::endl;
  
  //---------------------
  // collectors for digis
  
  std::vector<coord> coords_VME;
  std::vector<coord> coords_uTCA;
  std::vector<coord> coords_common;
  std::vector<coord> coords_matched;
  std::vector<coord> coords_mismatched;
  std::vector<coord> coords_unique_VME;
  std::vector<coord> coords_unique_uTCA;
  std::map<coord,const HFDataFrame*> digiMap_VME;
  std::map<coord,const HFDataFrame*> digiMap_uTCA;
  
  
  for(HFDigiCollection::const_iterator it = chbhe_vme->begin(); it != chbhe_vme->end(); ++it)
  {
    HcalElectronicsId const& eId = it->elecId();
    int crate   = eId.readoutVMECrateId();
    int slot    = eId.htrSlot();
    int tb      = eId.htrTopBottom();
    int fiber   = eId.fiberIndex();
    int channel = eId.fiberChanId();
    
    if( isAllZero(&(*it)) ) continue;
    
    // construct maps
    coord aCoord = GetCoord("QIE",crate,slot,tb,fiber,channel);
    
    if( eId.isUTCAid() ) continue;
    
    coords_VME.push_back(aCoord);  // VME
    
    std::map<coord,const HFDataFrame*>::const_iterator mapIt = digiMap_VME.find(aCoord);
    if( mapIt == digiMap_VME.end() ) digiMap_VME[aCoord] = &(*it);
    else std::cout << "ERROR: digi already exists" << std::endl;
  }
  
  for(HFDigiCollection::const_iterator it = chbhe_utca->begin(); it != chbhe_utca->end(); ++it)
  {
    HcalElectronicsId const& eId = it->elecId();
    int crate   = eId.readoutVMECrateId();
    int slot    = eId.htrSlot();
    int tb      = eId.htrTopBottom();
    int fiber   = eId.fiberIndex();
    int channel = eId.fiberChanId();
    
    if( isAllZero(&(*it)) ) continue;
    
    // construct maps
    coord aCoord = GetCoord("QIE",crate,slot,tb,fiber,channel);
    
    if( !eId.isUTCAid() ) continue;
    
    coords_uTCA.push_back(aCoord); // uTCA
    
    std::map<coord,const HFDataFrame*>::const_iterator mapIt = digiMap_uTCA.find(aCoord);
    if( mapIt == digiMap_uTCA.end() ) digiMap_uTCA[aCoord] = &(*it);
    else std::cout << "ERROR: digi already exists" << std::endl;
  }
  
  
  // find common and unique digis
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
  
  
  // find matching and mismatching digis among common ones
  for(unsigned int it = 0; it < coords_common.size(); ++it)
  {
    coord aCoord = coords_common.at(it);
    bool isMatching = matching(digiMap_VME[aCoord],digiMap_uTCA[aCoord]);
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
  _trees["QIEcounters"] -> Fill();
  
  // fill QIE ntuple - matched
  for(unsigned int it = 0; it < coords_matched.size(); ++it)
  {
    coord aCoord = coords_matched.at(it);
    HcalDetId detId = digiMap_VME[aCoord] -> id();
    _treeVars.ieta  = detId.ieta();
    _treeVars.iphi  = detId.iphi();
    _treeVars.depth = detId.depth();
    
    FillTreeVars(_treeVars,aCoord,digiMap_VME[aCoord],digiMap_uTCA[aCoord]);
    _trees["QIEmatched"] -> Fill();
  }
  // fill QIE ntuple - mismatched
  for(unsigned int it = 0; it < coords_mismatched.size(); ++it)
  {
    coord aCoord = coords_mismatched.at(it);
    HcalDetId detId = digiMap_VME[aCoord] -> id();
    _treeVars.ieta  = detId.ieta();
    _treeVars.iphi  = detId.iphi();
    _treeVars.depth = detId.depth();
    
    FillTreeVars(_treeVars,aCoord,digiMap_VME[aCoord],digiMap_uTCA[aCoord]);
    _trees["QIEmismatched"] -> Fill();
  }
  // fill QIE ntuple - unique VME
  for(unsigned int it = 0; it < coords_unique_VME.size(); ++it)
  {
    coord aCoord = coords_unique_VME.at(it);
    HcalDetId detId = digiMap_VME[aCoord] -> id();
    _treeVars.ieta  = detId.ieta();
    _treeVars.iphi  = detId.iphi();
    _treeVars.depth = detId.depth();
    
    FillTreeVars(_treeVars,aCoord,digiMap_VME[aCoord],NULL);
    _trees["QIEunique1"] -> Fill();
  }
  // fill QIE ntuple - unique uTCA
  for(unsigned int it = 0; it < coords_unique_uTCA.size(); ++it)
  {
    coord aCoord = coords_unique_uTCA.at(it);
    HcalDetId detId = digiMap_uTCA[aCoord] -> id();
    _treeVars.ieta  = detId.ieta();
    _treeVars.iphi  = detId.iphi();
    _treeVars.depth = detId.depth();
    
    FillTreeVars(_treeVars,aCoord,NULL,digiMap_uTCA[aCoord]);
    _trees["QIEunique2"] -> Fill();
  }
  
  
  _treeVars.iEntry += 1;
}

DEFINE_FWK_MODULE(DigiAnalysis);
