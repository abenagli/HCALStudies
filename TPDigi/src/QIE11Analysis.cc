#include "HCALStudies/TPDigi/interface/QIE11Analysis.h"
#include "HCALStudies/TPDigi/interface/Utils.h"



QIE11Analysis::QIE11Analysis(edm::ParameterSet const& ps)
{
  //std::cout << "QIE11Analysis::QIE11Analysis" << std::endl;
  
  _tagQIE11 = ps.getUntrackedParameter<edm::InputTag>("tagQIE11",edm::InputTag("hcalDigis"));
  _tokQIE11 = consumes<QIE11DigiCollection>(_tagQIE11);
}



QIE11Analysis::~QIE11Analysis()
{
  //std::cout << "QIE11Analysis::~QIE11Analysis" << std::endl;
}


void QIE11Analysis::beginJob()
{
  // initialize variables
  _treeVars.iEntry = 0;
  
  
  // TFileService for output ntuples
  edm::Service<TFileService> _fs;
  if( !_fs )
  {
    throw edm::Exception(edm::errors::Configuration, "TFile Service is not registered in cfg file");
  }
  
  
  // output ntuple
  _tree = _fs->make<TTree>(Form("ntu"),Form("ntu"));  
  _tree -> SetAutoSave(10000000000);
  _tree -> SetAutoFlush(1000000);
  
  _tree -> Branch("iEntry",  &_treeVars.iEntry,  "iEntry/I");
  _tree -> Branch("EvN",     &_treeVars.EvN,        "EvN/I");
  _tree -> Branch("OrN",     &_treeVars.OrN,        "OrN/I");
  _tree -> Branch("BcN",     &_treeVars.BcN,        "BcN/I");
  _tree -> Branch("LsN",     &_treeVars.LsN,        "LsN/I");
  _tree -> Branch("crate",   &_treeVars.crate,    "crate/I");
  _tree -> Branch("slot",    &_treeVars.slot,      "slot/I");
  _tree -> Branch("fiber",   &_treeVars.fiber,    "fiber/I");
  _tree -> Branch("channel", &_treeVars.channel,"channel/I");
  _tree -> Branch("ieta",    &_treeVars.ieta,      "ieta/I");
  _tree -> Branch("iphi",    &_treeVars.iphi,      "iphi/I");
  _tree -> Branch("depth",   &_treeVars.depth,    "depth/I");
  
  
  // output plots
  _h1_ADC_11 = _fs->make<TH1F>("h1_ADC_11","",256,-0.5,255.5);
  _h1_fC_11 = _fs->make<TH1F>("h1_fC_11","",1000,0.,10000.);
  _h1_maxADC_11 = _fs->make<TH1F>("h1_maxADC_11","",256,-0.5,255.5);
  _h1_maxfC_11 = _fs->make<TH1F>("h1_maxfC_11","",1000,0.,10000.);
  _h1_LETDC = _fs->make<TH1F>("h1_LETDC","",64,-0.5,63.5);
  _h2_LETDC_vs_ADC = _fs->make<TH2F>("h2_LETDC_vs_ADC","",256,-0.5,255.5,64,-0.5,63.5);
  _h2_LETDC_vs_fC = _fs->make<TH2F>("h2_LETDC_vs_fC","",1000,0.,10000.,64,-0.5,63.5);
  _p1_shapeADC_11 = _fs->make<TProfile>("p1_shapeADC_11","",10,-0.5,9.5);
  _p1_shapefC_11 = _fs->make<TProfile>("p1_shapefC_11","",10,-0.5,9.5);
  
  for(int TSIt = 0; TSIt < 10; ++TSIt)
  {
    _h1_ADC_11_TS[TSIt] = _fs->make<TH1F>(Form("h1_ADC_11_TS%02d",TSIt),"",256,-0.5,255.5);
    _h1_fC_11_TS[TSIt] = _fs->make<TH1F>(Form("h1_fC_11_TS%02d",TSIt),"",1000,0.,10000.);
    _h1_LETDC_TS[TSIt] = _fs->make<TH1F>(Form("h1_LETDC_TS%02d",TSIt),"",64,-0.5,63.5);
    _h2_LETDC_vs_ADC_TS[TSIt] = _fs->make<TH2F>(Form("h2_LETDC_vs_ADC_TS%02d",TSIt),"",256,-0.5,255.5,64,-0.5,63.5);
    _h2_LETDC_vs_fC_TS[TSIt] = _fs->make<TH2F>(Form("h2_LETDC_vs_fC_TS%02d",TSIt),"",1000,0.,10000.,64,-0.5,63.5);
  }
  
}



void QIE11Analysis::endJob()
{}


void QIE11Analysis::analyze(const edm::Event& ev, const edm::EventSetup& es)
{
  // std::cout << "QIE11Analysis::analyze" << std::endl;
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
  
  
  //-------------
  // get the emap
  edm::ESHandle<HcalDbService> dbs;
  es.get<HcalDbRecord>().get(dbs);
  _emap = dbs->getHcalMapping();
  std::vector<uint32_t> vhashC36;
  vhashC36.push_back(HcalElectronicsId(36, 4, hcaldqm::constants::FIBER_uTCA_MIN1, hcaldqm::constants::FIBERCH_MIN, false).rawId());
  _filter_C36.initialize(hcaldqm::filter::fPreserver, hcaldqm::hashfunctions::fCrateSlot, vhashC36);
  _ehashmap.initialize(_emap, hcaldqm::electronicsmap::fD2EHashMap, _filter_C36);
  
  
  //----------------
  // get collections
  edm::Handle<QIE11DigiCollection> cqie11;
  
  if( !ev.getByToken(_tokQIE11,cqie11) )
    std::cout << "Collection QIE11DigiCollection isn't available " + _tagQIE11.label() + " " + _tagQIE11.instance() << std::endl;
  
  
  //------------
  // QIE11 plots
  std::map<std::string,QIE11DataFrame> mqie11;
  for(unsigned int i = 0; i < cqie11->size(); ++i)
  {
    QIE11DataFrame frame = static_cast<QIE11DataFrame>((*cqie11)[i]);
    HcalDetId did = frame.detid();
    HcalElectronicsId eid = HcalElectronicsId(_ehashmap.lookup(did));  
    std::string label(Form("%d-%d-%d-%d",eid.readoutVMECrateId(),eid.htrSlot(),eid.fiberIndex(),eid.fiberChanId()));
    std::cout << "detid: " << did << std::endl;
    std::cout << "electronicsid: " << eid << std::endl;
    std::cout << "label: " << label << std::endl;
    std::cout << std::endl;
    
    
    //iterate thru all TS and fill
    float maxADC = -999999.;
    float maxfC = -999999.;
    std::map<std::string,float> maxADC_channel;
    std::map<std::string,float> maxfC_channel;
    for( int j = 0; j < frame.samples(); ++j)
    {
      if( frame[j].adc() > maxADC ) maxADC = frame[j].adc();
      if( new_adc2fC[frame[j].adc()] > maxADC ) maxfC = new_adc2fC[frame[j].adc()];
      
      _h1_ADC_11 -> Fill( frame[j].adc() );
      _h1_fC_11 -> Fill( new_adc2fC[frame[j].adc()] );
      _h1_LETDC -> Fill( frame[j].tdc() );
      _h2_LETDC_vs_ADC -> Fill( frame[j].adc(),frame[j].tdc() );
      _h2_LETDC_vs_fC -> Fill( new_adc2fC[frame[j].adc()],frame[j].tdc() );

      _h1_ADC_11_TS[j] -> Fill( frame[j].adc() );
      _h1_fC_11_TS[j] -> Fill( new_adc2fC[frame[j].adc()] );
      _h1_LETDC_TS[j] -> Fill( frame[j].tdc() );
      _h2_LETDC_vs_ADC_TS[j] -> Fill( frame[j].adc(),frame[j].tdc() );
      _h2_LETDC_vs_fC_TS[j] -> Fill( new_adc2fC[frame[j].adc()],frame[j].tdc() );
      
      if( _h1_ADC_11_channel[label] == NULL )
      {
        _h1_ADC_11_channel[label] = _fs->make<TH1F>(Form("h1_ADC_11_%s",label.c_str()),"",256,-0.5,255.5);
        _h1_fC_11_channel[label] = _fs->make<TH1F>(Form("h1_fC_11_%s",label.c_str()),"",1000,0.,10000.);
        _h1_maxADC_11_channel[label] = _fs->make<TH1F>(Form("h1_maxADC_11_%s",label.c_str()),"",256,-0.5,255.5);
        _h1_maxfC_11_channel[label] = _fs->make<TH1F>(Form("h1_maxfC_11_%s",label.c_str()),"",1000,0.,10000.);
        _h1_LETDC_channel[label] = _fs->make<TH1F>(Form("h1_LETDC_%s",label.c_str()),"",64,-0.5,63.5);
        _h2_LETDC_vs_ADC_channel[label] = _fs->make<TH2F>(Form("h2_LETDC_vs_ADC_%s",label.c_str()),"",256,-0.5,255.5,64,-0.5,63.5);
        _h2_LETDC_vs_fC_channel[label] = _fs->make<TH2F>(Form("h2_LETDC_vs_fC_%s",label.c_str()),"",1000,0.,10000.,64,-0.5,63.5);
        _p1_shapeADC_11_channel[label] = _fs->make<TProfile>(Form("p1_shapeADC_11_%s",label.c_str()),"",10,-0.5,9.5);
        _p1_shapefC_11_channel[label] = _fs->make<TProfile>(Form("p1_shapefC_11_%s",label.c_str()),"",10,-0.5,9.5);
        
        for(int TSIt = 0; TSIt < 10; ++TSIt)
        {
          _h1_ADC_11_channel_TS[label][TSIt] = _fs->make<TH1F>(Form("h1_ADC_11_%s_TS%02d",label.c_str(),TSIt),"",256,-0.5,255.5);
          _h1_fC_11_channel_TS[label][TSIt] = _fs->make<TH1F>(Form("h1_fC_11_%s_TS%02d",label.c_str(),TSIt),"",1000,0.,10000.);
          _h1_LETDC_channel_TS[label][TSIt] = _fs->make<TH1F>(Form("h1_LETDC_%s_TS%02d",label.c_str(),TSIt),"",64,-0.5,63.5);
          _h2_LETDC_vs_ADC_channel_TS[label][TSIt] = _fs->make<TH2F>(Form("h2_LETDC_vs_ADC_%s_TS%02d",label.c_str(),TSIt),"",256,-0.5,255.5,64,-0.5,63.5);
          _h2_LETDC_vs_fC_channel_TS[label][TSIt] = _fs->make<TH2F>(Form("h2_LETDC_vs_fC_%s_TS%02d",label.c_str(),TSIt),"",1000,0.,10000.,64,-0.5,63.5);
        }
      }
      
      if( frame[j].adc() > maxADC_channel[label] ) maxADC_channel[label] = frame[j].adc();
      if( new_adc2fC[frame[j].adc()] > maxfC_channel[label] ) maxfC_channel[label] = new_adc2fC[frame[j].adc()];
      
      _h1_ADC_11_channel[label] -> Fill( frame[j].adc() );
      _h1_fC_11_channel[label] -> Fill( new_adc2fC[frame[j].adc()] );
      _h1_LETDC_channel[label] -> Fill( frame[j].tdc() );
      _h2_LETDC_vs_ADC_channel[label] -> Fill( frame[j].adc(),frame[j].tdc() );
      _h2_LETDC_vs_fC_channel[label] -> Fill( new_adc2fC[frame[j].adc()],frame[j].tdc() );
      
      _h1_ADC_11_channel_TS[label][j] -> Fill( frame[j].adc() );
      _h1_fC_11_channel_TS[label][j] -> Fill( new_adc2fC[frame[j].adc()] );
      _h1_LETDC_channel_TS[label][j] -> Fill( frame[j].tdc() );
      _h2_LETDC_vs_ADC_channel_TS[label][j] -> Fill( frame[j].adc(),frame[j].tdc() );
      _h2_LETDC_vs_fC_channel_TS[label][j] -> Fill( new_adc2fC[frame[j].adc()],frame[j].tdc() );
    }
    
    _h1_maxADC_11 -> Fill( maxADC );
    _h1_maxfC_11 -> Fill( maxfC );
    if( maxADC > 0. )
      for( int j = 0; j < frame.samples(); ++j)
        _p1_shapeADC_11 -> Fill( j,frame[j].adc() );
    if( maxfC > 0. )
      for( int j = 0; j < frame.samples(); ++j)
        _p1_shapefC_11 -> Fill( j,new_adc2fC[frame[j].adc()] );
    for(std::map<std::string,float>::const_iterator mapIt = maxADC_channel.begin(); mapIt != maxADC_channel.end(); ++mapIt)
    {
      _h1_maxADC_11_channel[mapIt->first] -> Fill( mapIt->second );
      if( mapIt->second > 0. )
        for( int j = 0; j < frame.samples(); ++j)
          _p1_shapeADC_11_channel[mapIt->first] -> Fill( j,frame[j].adc() );
    }
    for(std::map<std::string,float>::const_iterator mapIt = maxfC_channel.begin(); mapIt != maxfC_channel.end(); ++mapIt)
    {
      _h1_maxfC_11_channel[mapIt->first] -> Fill( mapIt->second );
      if( mapIt->second > 0. )
        for( int j = 0; j < frame.samples(); ++j)
          _p1_shapefC_11_channel[mapIt->first] -> Fill( j,new_adc2fC[frame[j].adc()] );
    }
  }
  
  
  _treeVars.iEntry += 1;
}

DEFINE_FWK_MODULE(QIE11Analysis);
