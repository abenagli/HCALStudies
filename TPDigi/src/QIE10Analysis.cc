#include "HCALStudies/TPDigi/interface/QIE10Analysis.h"



void FillTreeVars(treeVars& tv)
{
}



QIE10Analysis::QIE10Analysis(edm::ParameterSet const& ps)
{
  //std::cout << "QIE10Analysis::QIE10Analysis" << std::endl;
  
  _tagQIE10 = ps.getUntrackedParameter<edm::InputTag>("tagQIE10",edm::InputTag("hfDigis"));
  _tagQIE8  = ps.getUntrackedParameter<edm::InputTag>("tagQIE8", edm::InputTag("hfDigis"));
  
  _tokQIE10 = consumes<QIE10DigiCollection>(_tagQIE10);
  _tokQIE8  = consumes<HFDigiCollection>(_tagQIE8);
}



QIE10Analysis::~QIE10Analysis()
{
  //std::cout << "QIE10Analysis::~QIE10Analysis" << std::endl;
}


void QIE10Analysis::beginJob()
{
  // initialize variables
  _treeVars.iEntry = 0;
  
  
  // TFileService for output ntuples
  edm::Service<TFileService> fs;
  if( !fs )
  {
    throw edm::Exception(edm::errors::Configuration, "TFile Service is not registered in cfg file");
  }
  
  
  // output ntuple
  _tree = fs->make<TTree>(Form("ntu"),Form("ntu"));  
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
  
  
  //---------------------------
  // create a Dual Anode filter
  for(int depth = 3; depth <= 4; ++depth)
    for(int ieta = 29; ieta <= 41; ++ieta)
    {
      HcalDetId* did = new HcalDetId(HcalForward,ieta,39,depth);
      _DAIds.push_back(did);
    }
  
  
  // output plots
  _p2_ADC_10_depth1_phi_vs_eta = fs->make<TProfile2D>("p2_ADC_10_depth1_phi_vs_eta","",83,-41.5,41.5,73,-0.5,72.5);
  _p2_ADC_10_depth2_phi_vs_eta = fs->make<TProfile2D>("p2_ADC_10_depth2_phi_vs_eta","",83,-41.5,41.5,73,-0.5,72.5);
  _p2_ADC_10_depth3_phi_vs_eta = fs->make<TProfile2D>("p2_ADC_10_depth3_phi_vs_eta","",83,-41.5,41.5,73,-0.5,72.5);
  _p2_ADC_10_depth4_phi_vs_eta = fs->make<TProfile2D>("p2_ADC_10_depth4_phi_vs_eta","",83,-41.5,41.5,73,-0.5,72.5);
  
  _h1_ADC_10 = fs->make<TH1F>("h1_ADC_10","",256,-0.5,255.5);
  _h1_ADC_8 = fs->make<TH1F>("h1_ADC_8","",256,-0.5,255.5);
  _h1_maxADC_10 = fs->make<TH1F>("h1_maxADC_10","",256,-0.5,255.5);
  _h1_maxADC_8 = fs->make<TH1F>("h1_maxADC_8","",256,-0.5,255.5);
  _h1_LETDC = fs->make<TH1F>("h1_LETDC","",64,-0.5,63.5);
  _h2_LETDC_vs_ADC = fs->make<TH2F>("h2_LETDC_vs_ADC","",256,-0.5,255.5,64,-0.5,63.5);
  
  _h2_ADCSumCorr_10_vs_8  = fs->make<TH2F>("h2_ADCSumCorr_10_vs_8", "",128,-0.5,127.5,256,-0.5,255.5);
  _h2_ADCCorr_10_vs_8  = fs->make<TH2F>("h2_ADCCorr_10_vs_8", "",128,-0.5,127.5,256,-0.5,255.5);
  _h2_ADCCorr_10_vs_10 = fs->make<TH2F>("h2_ADCCorr_10_vs_10","",256,-0.5,255.5,256,-0.5,255.5);
  _h2_LETDCCorr_10_vs_10 = fs->make<TH2F>("h2_LETDCCorr_10_vs_10","",64,-0.5,63.5,64,-0.5,63.5);
  
  _p1_shape_10 = fs->make<TProfile>("p1_shape_10","",10,-0.5,9.5);
  _p1_shape_8 = fs->make<TProfile>("p1_shape_8","",10,-0.5,9.5);
  
  for(int TSIt = 0; TSIt < 10; ++TSIt)
  {
    _h1_ADC_10_TS[TSIt] = fs->make<TH1F>(Form("h1_ADC_10_TS%02d",TSIt),"",256,-0.5,255.5);
    _h1_ADC_8_TS[TSIt] = fs->make<TH1F>(Form("h1_ADC_8_TS%02d",TSIt),"",256,-0.5,255.5);
    _h1_LETDC_TS[TSIt] = fs->make<TH1F>(Form("h1_LETDC_TS%02d",TSIt),"",64,-0.5,63.5);
    _h2_LETDC_vs_ADC_TS[TSIt] = fs->make<TH2F>(Form("h2_LETDC_vs_ADC_TS%02d",TSIt),"",256,-0.5,255.5,64,-0.5,63.5);
    
    _h2_ADCCorr_10_vs_8_TS[TSIt]  = fs->make<TH2F>(Form("h2_ADCCorr_10_vs_8_TS%02d",TSIt), "",128,-0.5,127.5,256,-0.5,255.5);
    _h2_ADCCorr_10_vs_10_TS[TSIt] = fs->make<TH2F>(Form("h2_ADCCorr_10_vs_10_TS%02d",TSIt),"",256,-0.5,255.5,256,-0.5,255.5);
    _h2_LETDCCorr_10_vs_10_TS[TSIt] = fs->make<TH2F>(Form("h2_LETDCCorr_10_vs_10_TS%02d",TSIt),"",64,-0.5,63.5,64,-0.5,63.5);  
  }
  
  
  for(unsigned int it = 0; it < _DAIds.size(); ++it)
  {
    HcalDetId* did = _DAIds.at(it);
    std::string label(Form("%02d-%02d_%01d",did->ieta(),did->iphi(),did->depth()));
    std::cout << "creating rawId " << did->rawId() << "   for   ieta = " << did->ieta() << "   iphi = " << did->iphi() << "   depth = " << did->depth() << std::endl;
    
    _h1_ADC_10_channel[did->rawId()] = fs->make<TH1F>(Form("h1_ADC_10_%s",label.c_str()),"",256,-0.5,255.5);
    _h1_ADC_8_channel[did->rawId()] = fs->make<TH1F>(Form("h1_ADC_8_%s",label.c_str()),"",256,-0.5,255.5);
    _h1_maxADC_10_channel[did->rawId()] = fs->make<TH1F>(Form("h1_maxADC_10_%s",label.c_str()),"",256,-0.5,255.5);
    _h1_maxADC_8_channel[did->rawId()] = fs->make<TH1F>(Form("h1_maxADC_8_%s",label.c_str()),"",256,-0.5,255.5);
    _h1_LETDC_channel[did->rawId()] = fs->make<TH1F>(Form("h1_LETDC_%s",label.c_str()),"",64,-0.5,63.5);
    _h2_LETDC_vs_ADC_channel[did->rawId()] = fs->make<TH2F>(Form("h2_LETDC_vs_ADC_%s",label.c_str()),"",256,-0.5,255.5,64,-0.5,63.5);
    
    _h2_ADCSumCorr_10_vs_8_channel[did->rawId()]  = fs->make<TH2F>(Form("h2_ADCSumCorr_10_vs_8_%s",label.c_str()), "",128,-0.5,127.5,256,-0.5,255.5);
    _h2_ADCCorr_10_vs_8_channel[did->rawId()]  = fs->make<TH2F>(Form("h2_ADCCorr_10_vs_8_%s",label.c_str()), "",128,-0.5,127.5,256,-0.5,255.5);
    _h2_ADCCorr_10_vs_10_channel[did->rawId()] = fs->make<TH2F>(Form("h2_ADCCorr_10_vs_10_%s",label.c_str()),"",256,-0.5,255.5,256,-0.5,255.5);
    _h2_LETDCCorr_10_vs_10_channel[did->rawId()] = fs->make<TH2F>(Form("h2_LETDCCorr_10_vs_10_%s",label.c_str()),"",64,-0.5,63.5,64,-0.5,63.5);
    
    _p1_shape_10_channel[did->rawId()] = fs->make<TProfile>(Form("p1_shape_10_%s",label.c_str()),"",10,-0.5,9.5);
    _p1_shape_8_channel[did->rawId()] = fs->make<TProfile>(Form("p1_shape_8_%s",label.c_str()),"",10,-0.5,9.5);
    
    for(int TSIt = 0; TSIt < 10; ++TSIt)
    {
      _h1_ADC_10_channel_TS[did->rawId()][TSIt] = fs->make<TH1F>(Form("h1_ADC_10_%s_TS%02d",label.c_str(),TSIt),"",256,-0.5,255.5);
      _h1_ADC_8_channel_TS[did->rawId()][TSIt] = fs->make<TH1F>(Form("h1_ADC_8_%s_TS%02d",label.c_str(),TSIt),"",256,-0.5,255.5);
      _h1_LETDC_channel_TS[did->rawId()][TSIt] = fs->make<TH1F>(Form("h1_LETDC_%s_TS%02d",label.c_str(),TSIt),"",64,-0.5,63.5);
      _h2_LETDC_vs_ADC_channel_TS[did->rawId()][TSIt] = fs->make<TH2F>(Form("h2_LETDC_vs_ADC_%s_TS%02d",label.c_str(),TSIt),"",256,-0.5,255.5,64,-0.5,63.5);
      
      _h2_ADCCorr_10_vs_8_channel_TS[did->rawId()][TSIt]  = fs->make<TH2F>(Form("h2_ADCCorr_10_vs_8_%s_TS%02d",label.c_str(),TSIt), "",128,-0.5,127.5,256,-0.5,255.5);
      _h2_ADCCorr_10_vs_10_channel_TS[did->rawId()][TSIt] = fs->make<TH2F>(Form("h2_ADCCorr_10_vs_10_%s_TS%02d",label.c_str(),TSIt),"",256,-0.5,255.5,256,-0.5,255.5);
      _h2_LETDCCorr_10_vs_10_channel_TS[did->rawId()][TSIt] = fs->make<TH2F>(Form("h2_LETDCCorr_10_vs_10_%s_TS%02d",label.c_str(),TSIt),"",64,-0.5,63.5,64,-0.5,63.5);  
    }  
  }
}



void QIE10Analysis::endJob()
{}


void QIE10Analysis::analyze(const edm::Event& ev, const edm::EventSetup& es)
{
  // std::cout << "QIE10Analysis::analyze" << std::endl;
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
  edm::Handle<QIE10DigiCollection> cqie10;
  edm::Handle<HFDigiCollection> cqie8;

  if( !ev.getByToken(_tokQIE10,cqie10) )
    std::cout << "Collection QIE10DigiCollection isn't available " + _tagQIE10.label() + " " + _tagQIE10.instance() << std::endl;
  if( !ev.getByToken(_tokQIE8,cqie8) )
    std::cout << "Collection QIE8DigiCollection isn't available " + _tagQIE8.label() + " " + _tagQIE8.instance() << std::endl;
  
  
  //------------
  // QIE10 plots
  std::map<uint32_t,QIE10DataFrame> mqie10;
  for(uint32_t i = 0; i < cqie10->size(); ++i)
  {
    QIE10DataFrame frame = static_cast<QIE10DataFrame>((*cqie10)[i]);
    HcalDetId did = frame.detid();
    
    //iterate thru all TS and fill
    float maxADC = -999999.;
    std::map<uint32_t,float> maxADC_channel;
    for( int j = 0; j < frame.samples(); ++j)
    {
      if( frame[j].adc() > maxADC ) maxADC = frame[j].adc();
      
      if( did.depth() == 1 ) _p2_ADC_10_depth1_phi_vs_eta -> Fill(did.ieta(),did.iphi(),frame[j].adc());
      if( did.depth() == 2 ) _p2_ADC_10_depth2_phi_vs_eta -> Fill(did.ieta(),did.iphi(),frame[j].adc());
      if( did.depth() == 3 ) _p2_ADC_10_depth3_phi_vs_eta -> Fill(did.ieta(),did.iphi(),frame[j].adc());
      if( did.depth() == 4 ) _p2_ADC_10_depth4_phi_vs_eta -> Fill(did.ieta(),did.iphi(),frame[j].adc());
      
      _h1_ADC_10 -> Fill( frame[j].adc() );
      _h1_LETDC -> Fill( frame[j].le_tdc() );
      _h2_LETDC_vs_ADC -> Fill( frame[j].adc(),frame[j].le_tdc() );

      _h1_ADC_10_TS[j] -> Fill( frame[j].adc() );
      _h1_LETDC_TS[j] -> Fill( frame[j].le_tdc() );
      _h2_LETDC_vs_ADC_TS[j] -> Fill( frame[j].adc(),frame[j].le_tdc() );
      
      if( _h1_ADC_10_channel[did.rawId()] == NULL ) continue;
      
      if( frame[j].adc() > maxADC_channel[did.rawId()] ) maxADC_channel[did.rawId()] = frame[j].adc();
      
      _h1_ADC_10_channel[did.rawId()] -> Fill( frame[j].adc() );
      _h1_LETDC_channel[did.rawId()] -> Fill( frame[j].le_tdc() );
      _h2_LETDC_vs_ADC_channel[did.rawId()] -> Fill( frame[j].adc(),frame[j].le_tdc() );
      
      _h1_ADC_10_channel_TS[did.rawId()][j] -> Fill( frame[j].adc() );
      _h1_LETDC_channel_TS[did.rawId()][j] -> Fill( frame[j].le_tdc() );
      _h2_LETDC_vs_ADC_channel_TS[did.rawId()][j] -> Fill( frame[j].adc(),frame[j].le_tdc() );
    }
    
    _h1_maxADC_10 -> Fill( maxADC );
    if( maxADC > 0. )
      for( int j = 0; j < frame.samples(); ++j)
        _p1_shape_10 -> Fill( j,frame[j].adc() );
    for(std::map<uint32_t,float>::const_iterator mapIt = maxADC_channel.begin(); mapIt != maxADC_channel.end(); ++mapIt)
    {
      _h1_maxADC_10_channel[mapIt->first] -> Fill( mapIt->second );    
      if( mapIt->second > 0. )
        for( int j = 0; j < frame.samples(); ++j)
          _p1_shape_10_channel[mapIt->first] -> Fill( j,frame[j].adc() );
    }
    
    //Correlation histograms
    mqie10[did.rawId()] = frame;
  }
  
  
  /*
  //-------------
  // collect QIE8
  std::map<uint32_t, HFDataFrame> mqie8;
  for(HFDigiCollection::const_iterator it = cqie8->begin(); it != cqie8->end(); ++it)
  {
    //we only need iphi 39
    if( !(it->id().iphi()==39 && it->id().ieta()>0) )
      continue;
    
    mqie8[it->id().rawId()] = *it;
  }
  
  
  //------------------
  // correlation plots
  for(std::map<uint32_t,QIE10DataFrame>::const_iterator it = mqie10.begin(); it != mqie10.end(); ++it)
  {
    uint32_t rawid = it->first;
    
     //1 is QIE10 
    HcalDetId did1 = HcalDetId(rawid);
    QIE10DataFrame frame1_10 = it->second;
    
    // qie10-qie10 channels
    if( (did1.ieta()==30 || did1.ieta()==34) && 
        (did1.depth()==2 || did1.depth()==4) )
    {
      if (did1.depth()==4) continue; // skip depth4, they are treated for depth2
      
      //2 is QIE10
      HcalDetId did2 = HcalDetId(HcalForward,did1.ieta(),did1.iphi(),did1.depth()+2);
      QIE10DataFrame frame2_10 = mqie10[did2.rawId()];
      
      //fill
      for(int j = 0; j < frame1_10.samples(); ++j)
      {
        _h2_ADCCorr_10_vs_10 -> Fill( frame2_10[j].adc(),frame1_10[j].adc() );
        _h2_ADCCorr_10_vs_10_TS[j] -> Fill( frame2_10[j].adc(),frame1_10[j].adc() );
        
        _h2_LETDCCorr_10_vs_10 -> Fill( frame2_10[j].le_tdc(),frame1_10[j].le_tdc() );
        _h2_LETDCCorr_10_vs_10_TS[j] -> Fill( frame2_10[j].le_tdc(),frame1_10[j].le_tdc() );
        
        _h2_ADCCorr_10_vs_10_channel[did2.rawId()] -> Fill( frame2_10[j].adc(),frame1_10[j].adc() );
        _h2_ADCCorr_10_vs_10_channel_TS[did2.rawId()][j] -> Fill( frame2_10[j].adc(),frame1_10[j].adc() );
        
        _h2_LETDCCorr_10_vs_10_channel[did2.rawId()] -> Fill( frame2_10[j].le_tdc(),frame1_10[j].le_tdc() );
        _h2_LETDCCorr_10_vs_10_channel_TS[did2.rawId()][j] -> Fill( frame2_10[j].le_tdc(),frame1_10[j].le_tdc() );
      }
    }
    else // qie8-qie10 channels 
    {
      //2 is QIE8
      int newdepth = -1;
      if( did1.depth() == 3 ) newdepth = 1;
      if( did1.depth() == 4 ) newdepth = 2;
      HcalDetId did2 = HcalDetId(HcalForward,did1.ieta(),did1.iphi(),newdepth);
      HFDataFrame frame2_8 = mqie8[did2.rawId()];
      
      //fill
      float ADCSum_8 = 0.;
      float ADCSum_10 = 0.;
      float maxADC = -999999.;
      std::map<uint32_t,float> maxADC_channel;
      for(int j=0; j<frame1_10.samples()-1; ++j)
      {
        if( j==3 || j==4 || j==5 )
        {
          ADCSum_8  += frame2_8[j+1].adc();
          ADCSum_10 += frame1_10[j].adc();
        }
        
        if( frame2_8[j+1].adc() > maxADC ) maxADC = frame2_8[j+1].adc();
        if( frame2_8[j+1].adc() > maxADC_channel[did1.rawId()] ) maxADC_channel[did1.rawId()] = frame2_8[j+1].adc();
        
        
        _h1_ADC_8 -> Fill( frame2_8[j+1].adc() );      
        _h1_ADC_8_TS[j] -> Fill( frame2_8[j+1].adc() );
        _h1_ADC_8_channel[did1.rawId()] -> Fill( frame2_8[j+1].adc() );
        _h1_ADC_8_channel_TS[did1.rawId()][j] -> Fill( frame2_8[j+1].adc() );
        
        _h2_ADCCorr_10_vs_8 -> Fill( frame2_8[j+1].adc(),frame1_10[j].adc() );
        _h2_ADCCorr_10_vs_8_TS[j] -> Fill( frame2_8[j+1].adc(),frame1_10[j].adc() );
        
        _h2_ADCCorr_10_vs_8_channel[did1.rawId()] -> Fill( frame2_8[j+1].adc(),frame1_10[j].adc() );
        _h2_ADCCorr_10_vs_8_channel_TS[did1.rawId()][j] -> Fill( frame2_8[j+1].adc(),frame1_10[j].adc() );
      }
      
      _h1_maxADC_8 -> Fill( maxADC );
      if( maxADC > 0. )
        for(int j=0; j<frame1_10.samples(); ++j)
          _p1_shape_8 -> Fill( j,frame2_8[j].adc() );
      for(std::map<uint32_t,float>::const_iterator mapIt = maxADC_channel.begin(); mapIt != maxADC_channel.end(); ++mapIt)
      {
        _h1_maxADC_8_channel[mapIt->first] -> Fill( mapIt->second );    
        if( mapIt->second > 0. )
          for(int j=0; j<frame1_10.samples(); ++j)          
            _p1_shape_8_channel[mapIt->first] -> Fill( j,frame2_8[j].adc() );            
      }
    
      ADCSum_8 /= 3.;
      ADCSum_10 /= 3.;
      _h2_ADCSumCorr_10_vs_8 -> Fill( ADCSum_8,ADCSum_10 );
      _h2_ADCSumCorr_10_vs_8_channel[did1.rawId()] -> Fill( ADCSum_8,ADCSum_10 );
    }
  }
  */
  
  _treeVars.iEntry += 1;
}

DEFINE_FWK_MODULE(QIE10Analysis);
