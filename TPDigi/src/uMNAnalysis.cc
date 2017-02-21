#include "HCALStudies/TPDigi/interface/uMNAnalysis.h"



uMNAnalysis::uMNAnalysis(edm::ParameterSet const& ps)
{
  //std::cout << "uMNAnalysis::uMNAnalysis" << std::endl;
  
  _taguMNio = ps.getUntrackedParameter<edm::InputTag>("taguMNio",edm::InputTag("hcalDigis"));
  _taguMNQIE = ps.getUntrackedParameter<edm::InputTag>("taguMNQIE",edm::InputTag("hcalDigis"));
  
  _tokuMNio = consumes<HcalUMNioDigi>(_taguMNio);
  _tokuMNQIE = consumes<QIE10DigiCollection>(_taguMNQIE);
}



uMNAnalysis::~uMNAnalysis()
{
  //std::cout << "uMNAnalysis::~uMNAnalysis" << std::endl;
}


void uMNAnalysis::beginJob()
{
  // initialize variables
  _treeVars.iEntry = 0;
  
  
  // TFileService for output ntuples
  if( !fs )
  {
    throw edm::Exception(edm::errors::Configuration, "TFile Service is not registered in cfg file");
  }
  
  
  // output ntuple
  _tree = fs->make<TTree>(Form("ntu"),Form("ntu"));  
  _tree -> SetAutoSave(10000000000);
  _tree -> SetAutoFlush(1000000);
  
  _tree -> Branch("iEntry", &_treeVars.iEntry, "iEntry/I");
  _tree -> Branch("EvN",    &_treeVars.EvN,       "EvN/I");
  _tree -> Branch("OrN",    &_treeVars.OrN,       "OrN/I");
  _tree -> Branch("BcN",    &_treeVars.BcN,       "BcN/I");
  _tree -> Branch("LsN",    &_treeVars.LsN,       "LsN/I");
  _tree -> Branch("uMNio_eventType", &_treeVars.uMNio_eventType, "uMNio_eventType/I");
  _tree -> Branch("uMNio_laserType", &_treeVars.uMNio_laserType, "uMNio_laserType/I");
  // _tree -> Branch("crate",   &_treeVars.crate,    "crate/I");
  // _tree -> Branch("slot",    &_treeVars.slot,      "slot/I");
  // _tree -> Branch("fiber",   &_treeVars.fiber,    "fiber/I");
  // _tree -> Branch("channel", &_treeVars.channel,"channel/I");
  // _tree -> Branch("ieta",    &_treeVars.ieta,      "ieta/I");
  // _tree -> Branch("iphi",    &_treeVars.iphi,      "iphi/I");
  // _tree -> Branch("depth",   &_treeVars.depth,    "depth/I");
  
  
  // output plots
  _h1_uMNQIE_ADC_1 = fs->make<TH1F>("h1_uMNQIE_ADC_1","",256,-0.5,255.5);
  _h1_uMNQIE_ADC_ped_1 = fs->make<TH1F>("h1_uMNQIE_ADC_ped_1","",256,-0.5,255.5);
  _h1_uMNQIE_ADC_sig_1 = fs->make<TH1F>("h1_uMNQIE_ADC_sig_1","",512,-0.5,511.5);
  _h1_uMNQIE_ADC_max_1 = fs->make<TH1F>("h1_uMNQIE_ADC_max_1","",256,-0.5,255.5);
  _h1_uMNQIE_ADC_2 = fs->make<TH1F>("h1_uMNQIE_ADC_2","",256,-0.5,255.5);
  _h1_uMNQIE_ADC_ped_2 = fs->make<TH1F>("h1_uMNQIE_ADC_ped_2","",256,-0.5,255.5);
  _h1_uMNQIE_ADC_sig_2 = fs->make<TH1F>("h1_uMNQIE_ADC_sig_2","",512,-0.5,511.5);
  _h1_uMNQIE_ADC_max_2 = fs->make<TH1F>("h1_uMNQIE_ADC_max_2","",256,-0.5,255.5);
  
  _h1_uMNQIE_fC_1 = fs->make<TH1F>("h1_uMNQIE_fC_1","",20000,0.,50000.);
  _h1_uMNQIE_fC_ped_1 = fs->make<TH1F>("h1_uMNQIE_fC_ped_1","",20000,0.,50000.);
  _h1_uMNQIE_fC_sig_1 = fs->make<TH1F>("h1_uMNQIE_fC_sig_1","",20000,0.,100000.);
  _h1_uMNQIE_fC_max_1 = fs->make<TH1F>("h1_uMNQIE_fC_max_1","",20000,0.,50000.);
  _h1_uMNQIE_fC_2 = fs->make<TH1F>("h1_uMNQIE_fC_2","",20000,0,50000.);
  _h1_uMNQIE_fC_ped_2 = fs->make<TH1F>("h1_uMNQIE_fC_ped_2","",20000,0.,50000);
  _h1_uMNQIE_fC_sig_2 = fs->make<TH1F>("h1_uMNQIE_fC_sig_2","",20000,0.,100000);
  _h1_uMNQIE_fC_max_2 = fs->make<TH1F>("h1_uMNQIE_fC_max_2","",20000,0.,50000);
  
  _p1_uMNQIE_shape_ADC_1 = fs->make<TProfile>("p1_uMNQIE_shape_ADC_1","",10,-0.5,9.5);
  _p1_uMNQIE_shape_ADC_2 = fs->make<TProfile>("p1_uMNQIE_shape_ADC_2","",10,-0.5,9.5);
  _p1_uMNQIE_shapePedSub_ADC_1 = fs->make<TProfile>("p1_uMNQIE_shapePedSub_ADC_1","",10,-0.5,9.5);
  _p1_uMNQIE_shapePedSub_ADC_2 = fs->make<TProfile>("p1_uMNQIE_shapePedSub_ADC_2","",10,-0.5,9.5);
  
  _p1_uMNQIE_shape_fC_1 = fs->make<TProfile>("p1_uMNQIE_shape_fC_1","",10,-0.5,9.5);
  _p1_uMNQIE_shape_fC_2 = fs->make<TProfile>("p1_uMNQIE_shape_fC_2","",10,-0.5,9.5);  
  _p1_uMNQIE_shapePedSub_fC_1 = fs->make<TProfile>("p1_uMNQIE_shapePedSub_fC_1","",10,-0.5,9.5);
  _p1_uMNQIE_shapePedSub_fC_2 = fs->make<TProfile>("p1_uMNQIE_shapePedSub_fC_2","",10,-0.5,9.5);  
}



void uMNAnalysis::endJob()
{}


void uMNAnalysis::analyze(const edm::Event& ev, const edm::EventSetup& es)
{
  // std::cout << "uMNAnalysis::analyze" << std::endl;
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
  edm::Handle<HcalUMNioDigi> cumnio;
  edm::Handle<QIE10DigiCollection> cqie10;
  
  if( !ev.getByToken(_tokuMNio,cumnio) )
    std::cout << "Collection HcalUMNioDigi isn't available " + _taguMNio.label() + " " + _taguMNio.instance() << std::endl;
  if( !ev.getByToken(_tokuMNQIE,cqie10) )
    std::cout << "Collection HcalUMNQIEDigi isn't available " + _taguMNQIE.label() + " " + _taguMNQIE.instance() << std::endl;
  
  
  //--------------
  // pedestal mask
  
  int mask = 0x0;
  if( cumnio->valueUserWord(0) ==  3 ) mask = 0x3E0; // HB-HE-HPD
  if( cumnio->valueUserWord(0) ==  4 ) mask = 0x01F; // HO
  if( cumnio->valueUserWord(0) ==  5 ) mask = 0x3F0; // HF-PMT
  if( cumnio->valueUserWord(0) ==  7 ) mask = 0x3C0; // HEP-Mega
  if( cumnio->valueUserWord(0) ==  8 ) mask = 0x007; // HEM-Mega
  if( cumnio->valueUserWord(0) ==  9 ) mask = 0x0FF; // HBP-Mega
  if( cumnio->valueUserWord(0) == 10 ) mask = 0x07F; // HBM-Mega
  if( cumnio->valueUserWord(0) == 12 ) mask = 0x0FF; // CRF
  
  
  //----------------
  // uMNio variables
  _treeVars.uMNio_eventType = cumnio -> eventType();
  _treeVars.uMNio_laserType = cumnio -> valueUserWord(0);
  
  
  //-----------------
  // uMNQIE variables
  for(uint32_t i = 0; i < cqie10->size(); ++i)
  {
    //if( _treeVars.iEntry < 100 ) continue;
    
    QIE10DataFrame frame = static_cast<QIE10DataFrame>((*cqie10)[i]); 
    HcalDetId did = frame.detid();
    int depth = did.depth();
    //std::cout << "did: " << did << std::endl;
    
    float max_ADC_1 = -1.;
    float max_fC_1 = -1.;
    float max_ADC_2 = -1.;
    float max_fC_2 = -1.;
    float pedMean_ADC_1 = 3.832;
    float pedMean_ADC_2 = 4.085;
    float pedMean_fC_1 = 13.44;
    float pedMean_fC_2 = 14.29;
    float sig_ADC_1 = 0.;
    float sig_ADC_2 = 0.;
    float sig_fC_1 = 0.;
    float sig_fC_2 = 0.;
    //iterate thru all TS and fill
    for(int j = 0; j < frame.samples(); ++j)
    {      
      if( depth == 5 )
      {
        if( frame[j].adc() > max_ADC_1 ) max_ADC_1 = frame[j].adc();
        if( new_adc2fC[frame[j].adc()] > max_fC_1 ) max_fC_1 = new_adc2fC[frame[j].adc()];
        
        sig_ADC_1 += ( frame[j].adc()-pedMean_ADC_1 );
        sig_fC_1  += ( new_adc2fC[frame[j].adc()]-pedMean_fC_1 );
        
        _p1_uMNQIE_shape_ADC_1 -> Fill( j,frame[j].adc() );
        _p1_uMNQIE_shapePedSub_ADC_1 -> Fill( j,frame[j].adc()-pedMean_ADC_1 );
        _p1_uMNQIE_shape_fC_1 -> Fill( j,new_adc2fC[frame[j].adc()] );
        _p1_uMNQIE_shapePedSub_fC_1 -> Fill( j,new_adc2fC[frame[j].adc()]-pedMean_fC_1 );
        
        if( (mask>>j) & 0x1 )
        {
          _h1_uMNQIE_ADC_ped_1 -> Fill( frame[j].adc() );
          _h1_uMNQIE_fC_ped_1 -> Fill( new_adc2fC[frame[j].adc()] );
        }
        
        _h1_uMNQIE_ADC_1 -> Fill( frame[j].adc()  );
        _h1_uMNQIE_fC_1 -> Fill( new_adc2fC[frame[j].adc()] );
      }
      else if( depth == 6 )
      {
        if( frame[j].adc() > max_ADC_2 ) max_ADC_2 = frame[j].adc();
        if( new_adc2fC[frame[j].adc()] > max_fC_2 ) max_fC_2 = new_adc2fC[frame[j].adc()];
        
        sig_ADC_2 += ( frame[j].adc()-pedMean_ADC_2 );
        sig_fC_2  += ( new_adc2fC[frame[j].adc()]-pedMean_fC_2 );
        
        _p1_uMNQIE_shape_ADC_2 -> Fill( j,frame[j].adc() );
        _p1_uMNQIE_shapePedSub_ADC_2 -> Fill( j,frame[j].adc()-pedMean_ADC_2 );
        _p1_uMNQIE_shape_fC_2 -> Fill( j,new_adc2fC[frame[j].adc()] );
        _p1_uMNQIE_shapePedSub_fC_2 -> Fill( j,new_adc2fC[frame[j].adc()]-pedMean_fC_2 );
        
        if( (mask>>j) & 0x1 )
        {
          _h1_uMNQIE_ADC_ped_2 -> Fill( frame[j].adc() );
          _h1_uMNQIE_fC_ped_2 -> Fill( new_adc2fC[frame[j].adc()] );
        }
        
        _h1_uMNQIE_ADC_2 -> Fill( frame[j].adc()  );
        _h1_uMNQIE_fC_2 -> Fill( new_adc2fC[frame[j].adc()] );
      }
    }
    
    if( depth == 5 )
    {
      _h1_uMNQIE_ADC_sig_1 -> Fill( sig_ADC_1  );
      _h1_uMNQIE_fC_sig_1 -> Fill( sig_fC_1 );
      
      _h1_uMNQIE_ADC_max_1 -> Fill( max_ADC_1  );
      _h1_uMNQIE_fC_max_1 -> Fill( max_fC_1 );
    }
    if( depth == 6 )
    {
      _h1_uMNQIE_ADC_sig_2 -> Fill( sig_ADC_2  );
      _h1_uMNQIE_fC_sig_2 -> Fill( sig_fC_2 );
      
      _h1_uMNQIE_ADC_max_2 -> Fill( max_ADC_2  );
      _h1_uMNQIE_fC_max_2 -> Fill( max_fC_2 );
    }
    
  }
  
  
  _tree -> Fill();
  _treeVars.iEntry += 1;
}

DEFINE_FWK_MODULE(uMNAnalysis);
