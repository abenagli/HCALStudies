#include "HCALStudies/TPDigi/interface/MIPAnalysis.h"



MIPAnalysis::MIPAnalysis(edm::ParameterSet const& ps)
{
  //std::cout << "MIPAnalysis::MIPAnalysis" << std::endl;
  
  _tagQIE8 = ps.getUntrackedParameter<edm::InputTag>("tagQIE8",edm::InputTag("hcalDigis"));
  _tokQIE8 = consumes<HBHEDigiCollection>(_tagQIE8);
  
  _tagQIE11 = ps.getUntrackedParameter<edm::InputTag>("tagQIE11",edm::InputTag("hcalDigis"));
  _tokQIE11 = consumes<QIE11DigiCollection>(_tagQIE11);
  
  _tagMuons = ps.getUntrackedParameter<edm::InputTag>("tagMuons",edm::InputTag("cosmicMuonsEndCapsOnly"));
  _tokMuons = consumes<reco::TrackCollection>(_tagMuons);
  
  _tagTriggerResults = ps.getUntrackedParameter<edm::InputTag>("tagTriggerResults",edm::InputTag("TriggerResults"));
  _tokTriggerResults = consumes<edm::TriggerResults>(_tagTriggerResults);
}



MIPAnalysis::~MIPAnalysis()
{
  //std::cout << "MIPAnalysis::~MIPAnalysis" << std::endl;
}


void MIPAnalysis::beginJob()
{
  // TFileService for output ntuples
  if( !fs )
  {
    throw edm::Exception(edm::errors::Configuration, "TFile Service is not registered in cfg file");
  }
  
  
  h2_muon_innerPos_y_vs_x = fs->make<TH2F>("h2_muon_innerPos_y_vs_x","",2000,-2000.,2000.,2000,-2000.,2000.);
  h2_muon_outerPos_y_vs_x = fs->make<TH2F>("h2_muon_outerPos_y_vs_x","",2000,-2000.,2000.,2000,-2000.,2000.);
  h2_muon_mipSignal_innerPos_y_vs_x = fs->make<TH2F>("h2_muon_mipSignal_innerPos_y_vs_x","",2000,-2000.,2000.,2000,-2000.,2000.);
  h2_muon_mipSignal_outerPos_y_vs_x = fs->make<TH2F>("h2_muon_mipSignal_outerPos_y_vs_x","",2000,-2000.,2000.,2000,-2000.,2000.);
  
  
  for(int depth = 1; depth <= 4; ++depth)
  {
    _p2_maxTS_channel_iphi_vs_ieta[depth] = fs->make<TProfile2D>(Form("p2_maxTS_channel_iphi_vs_ieta_depth%d",depth),"",83,-41.5,41.5,73,-0.5,72.5);
    _p2_timing_channel_iphi_vs_ieta[depth] = fs->make<TProfile2D>(Form("p2_timing_channel_iphi_vs_ieta_depth%d",depth),"",83,-41.5,41.5,73,-0.5,72.5);
        
    _p2_ADC_ped_channel_iphi_vs_ieta[depth] = fs->make<TProfile2D>(Form("p2_ADC_ped_channel_iphi_vs_ieta_depth%d",depth),"",83,-41.5,41.5,73,-0.5,72.5);
    _p2_ADC_sig_channel_iphi_vs_ieta[depth] = fs->make<TProfile2D>(Form("p2_ADC_sig_channel_iphi_vs_ieta_depth%d",depth),"",83,-41.5,41.5,73,-0.5,72.5);
    _p2_ADC_max_channel_iphi_vs_ieta[depth] = fs->make<TProfile2D>(Form("p2_ADC_max_channel_iphi_vs_ieta_depth%d",depth),"",83,-41.5,41.5,73,-0.5,72.5);
    
    _p2_fC_ped_channel_iphi_vs_ieta[depth] = fs->make<TProfile2D>(Form("p2_fC_ped_channel_iphi_vs_ieta_depth%d",depth),"",83,-41.5,41.5,73,-0.5,72.5);
    _p2_fC_sig_channel_iphi_vs_ieta[depth] = fs->make<TProfile2D>(Form("p2_fC_sig_channel_iphi_vs_ieta_depth%d",depth),"",83,-41.5,41.5,73,-0.5,72.5);
    _p2_fC_max_channel_iphi_vs_ieta[depth] = fs->make<TProfile2D>(Form("p2_fC_max_channel_iphi_vs_ieta_depth%d",depth),"",83,-41.5,41.5,73,-0.5,72.5);
  }
  _p2_ADC_ped_RBX_iphi_vs_ieta = fs->make<TProfile2D>(Form("p2_ADC_ped_RBX_iphi_vs_ieta"),"",83,-41.5,41.5,73,-0.5,72.5);
  _p2_ADC_sig_RBX_iphi_vs_ieta = fs->make<TProfile2D>(Form("p2_ADC_sig_RBX_iphi_vs_ieta"),"",83,-41.5,41.5,73,-0.5,72.5);
  _p2_ADC_max_RBX_iphi_vs_ieta = fs->make<TProfile2D>(Form("p2_ADC_max_RBX_iphi_vs_ieta"),"",83,-41.5,41.5,73,-0.5,72.5);
  
  _p2_fC_ped_RBX_iphi_vs_ieta = fs->make<TProfile2D>(Form("p2_fC_ped_RBX_iphi_vs_ieta"),"",83,-41.5,41.5,73,-0.5,72.5);
  _p2_fC_sig_RBX_iphi_vs_ieta = fs->make<TProfile2D>(Form("p2_fC_sig_RBX_iphi_vs_ieta"),"",83,-41.5,41.5,73,-0.5,72.5);
  _p2_fC_max_RBX_iphi_vs_ieta = fs->make<TProfile2D>(Form("p2_fC_max_RBX_iphi_vs_ieta"),"",83,-41.5,41.5,73,-0.5,72.5);
  
  subDir_perChannel = fs -> mkdir( "perChannel" );
  subDir_perRBX = fs -> mkdir( "perRBX" );
  
  
  nGoodEvents_qie11 = 0;
}



void MIPAnalysis::endJob()
{}


void MIPAnalysis::analyze(const edm::Event& ev, const edm::EventSetup& es)
{
  // std::cout << "MIPAnalysis::analyze" << std::endl;
  // std::cout << "------------------------------" << std::endl;
  // std::cout << ">>> run: "   << ev.id().run() << std::endl;
  // std::cout << ">>> LS:  "   << ev.luminosityBlock() << std::endl;
  // std::cout << ">>> BX: "    << ev.bunchCrossing() << std::endl;
  // std::cout << ">>> event: " << ev.id().event() << std::endl;
  // std::cout << "------------------------------" << std::endl;
  
  
  //----------------
  // get collections
  edm::Handle<reco::TrackCollection> cmuons;
  edm::Handle<HBHEDigiCollection> cqie8;
  edm::Handle<QIE11DigiCollection> cqie11;
  edm::Handle<edm::TriggerResults> ctriggerresults;
  
  if( !ev.getByToken(_tokMuons,cmuons) )
    std::cout << "Collection reco::TrackCollection isn't available " + _tagMuons.label() + " " + _tagMuons.instance() << std::endl;
  
  if( !ev.getByToken(_tokQIE8,cqie8) )
    std::cout << "Collection HBHEDigiCollection isn't available " + _tagQIE8.label() + " " + _tagQIE8.instance() << std::endl;
  
  if( !ev.getByToken(_tokQIE11,cqie11) )
    std::cout << "Collection QIE11DigiCollection isn't available " + _tagQIE11.label() + " " + _tagQIE11.instance() << std::endl;  
  
  if( !ev.getByToken(_tokTriggerResults,ctriggerresults) )
    std::cout << "Collection TriggerResults isn't available " + _tagTriggerResults.label() + " " + _tagTriggerResults.instance() << std::endl;
  
  
  //----------------
  // muons variables
  reco::Track muon;
  for(uint32_t i = 0; i < cmuons->size(); ++i)
  {
    muon = static_cast<reco::Track>((*cmuons)[i]);
    
    const unsigned int n_TriggerResults( ctriggerresults.product()->size() );
    const edm::TriggerNames& trigNames = ev.triggerNames(*ctriggerresults);

    for ( unsigned int itrig( 0 ); itrig < n_TriggerResults; ++itrig )
    {
      if ( ctriggerresults.product()->accept( itrig ) )
      {
        std::cout << "path " << trigNames.triggerName(itrig);
        std::cout << ", module index " << ctriggerresults.product()->index( itrig );
        std::cout << ", state (Ready = 0, Pass = 1, Fail = 2, Exception = 3) " << ctriggerresults.product()->state( itrig );
        std::cout << ", accept " << ctriggerresults.product()->accept( itrig );
        std::cout << std::endl;
      }
    }
    
    const math::XYZPoint innerPos = muon.innerPosition();
    const math::XYZPoint outerPos = muon.outerPosition();
    
    std::cout << ">>> muon found:   eta: " << muon.eta() << "   phi: " << muon.phi() << "   innerPos: " << innerPos << "   outerPos: " << outerPos << std::endl;
    
    h2_muon_innerPos_y_vs_x -> Fill( innerPos.x(), innerPos.y() );
    h2_muon_outerPos_y_vs_x -> Fill( outerPos.x(), outerPos.y() );
  }
  
  
  //---------------
  // qie8 variables
  for(uint32_t i = 0; i < cqie8->size(); ++i)
  {
    HBHEDataFrame frame = static_cast<HBHEDataFrame>((*cqie8)[i]);
    
    HcalDetId did = frame.id();
    //std::cout << "did: " << did << "   " << did.subdet() << std::endl;
    
    if( did.subdet() != 2 ) continue;
    if( did.zside() < 0 ) continue;
    
    std::string label_channel = std::string(Form("ieta%d_iphi%d_depth%d",did.ieta(),did.iphi(),did.depth()));
    
    std::string label_RBX = "";
    if( did.subdet() == HcalBarrel  ) label_RBX += "HB";
    if( did.subdet() == HcalEndcap  ) label_RBX += "HE";
    if( did.subdet() == HcalOuter   ) label_RBX += "HO";
    if( did.subdet() == HcalForward ) label_RBX += "HF";
    if( did.ieta() > 0 ) label_RBX += "P";
    if( did.ieta() < 0 ) label_RBX += "M";
    label_RBX += std::string(Form("%02d",GetRBXId(did.subdet(),did.iphi())));
    
    //std::cout << "label_channel: " << label_channel << "   label_RBX: " << label_RBX << std::endl;
    
    if( _h1_ADC_channel[label_channel] == NULL )
    {
      perChannelList.push_back( std::make_tuple(did.ieta(),did.iphi(),did.depth(),label_channel,label_RBX) );
      
      _h1_ADC_channel[label_channel] = subDir_perChannel.make<TH1F>(Form("h1_ADC_%s",label_channel.c_str()),"",256,-0.5,255.5);
      _h1_ADC_ped_channel[label_channel] = subDir_perChannel.make<TH1F>(Form("h1_ADC_ped_%s",label_channel.c_str()),"",256,-0.5,255.5);
      _h1_ADC_sig_channel[label_channel] = subDir_perChannel.make<TH1F>(Form("h1_ADC_sig_%s",label_channel.c_str()),"",512,-0.5,511.5);
      _h1_ADC_max_channel[label_channel] = subDir_perChannel.make<TH1F>(Form("h1_ADC_max_%s",label_channel.c_str()),"",256,-0.5,255.5);
      
      _h1_fC_channel[label_channel] = subDir_perChannel.make<TH1F>(Form("h1_fC_%s",label_channel.c_str()),"",20000,0.,50000.);
      _h1_fC_ped_channel[label_channel] = subDir_perChannel.make<TH1F>(Form("h1_fC_ped_%s",label_channel.c_str()),"",20000,0.,50000.);
      _h1_fC_sig_channel[label_channel] = subDir_perChannel.make<TH1F>(Form("h1_fC_sig_%s",label_channel.c_str()),"",20000,0.,100000.);
      _h1_fC_max_channel[label_channel] = subDir_perChannel.make<TH1F>(Form("h1_fC_max_%s",label_channel.c_str()),"",20000,0.,50000.);
      
      _p1_ADC_shape_channel[label_channel] = subDir_perChannel.make<TProfile>(Form("p1_ADC_shape_%s",label_channel.c_str()),"",10,-0.5,9.5);
      _p1_ADC_shapePedSub_channel[label_channel] = subDir_perChannel.make<TProfile>(Form("p1_ADC_shapePedSub_%s",label_channel.c_str()),"",10,-0.5,9.5);
      _p1_fC_shape_channel[label_channel] = subDir_perChannel.make<TProfile>(Form("p1_fC_shape_%s",label_channel.c_str()),"",10,-0.5,9.5);
      _p1_fC_shapePedSub_channel[label_channel] = subDir_perChannel.make<TProfile>(Form("p1_fC_shapePedSub_%s",label_channel.c_str()),"",10,-0.5,9.5);
    }
    
    if( _h1_ADC_RBX[label_RBX] == NULL )
    {
      _h1_ADC_RBX[label_RBX] = subDir_perRBX.make<TH1F>(Form("h1_ADC_%s",label_RBX.c_str()),"",256,-0.5,255.5);
      _h1_ADC_ped_RBX[label_RBX] = subDir_perRBX.make<TH1F>(Form("h1_ADC_ped_%s",label_RBX.c_str()),"",256,-0.5,255.5);
      _h1_ADC_sig_RBX[label_RBX] = subDir_perRBX.make<TH1F>(Form("h1_ADC_sig_%s",label_RBX.c_str()),"",512,-0.5,511.5);
      _h1_ADC_max_RBX[label_RBX] = subDir_perRBX.make<TH1F>(Form("h1_ADC_max_%s",label_RBX.c_str()),"",256,-0.5,255.5);
      
      _h1_fC_RBX[label_RBX] = subDir_perRBX.make<TH1F>(Form("h1_fC_%s",label_RBX.c_str()),"",20000,0.,50000.);
      _h1_fC_ped_RBX[label_RBX] = subDir_perRBX.make<TH1F>(Form("h1_fC_ped_%s",label_RBX.c_str()),"",20000,0.,50000.);
      _h1_fC_sig_RBX[label_RBX] = subDir_perRBX.make<TH1F>(Form("h1_fC_sig_%s",label_RBX.c_str()),"",20000,0.,100000.);
      _h1_fC_max_RBX[label_RBX] = subDir_perRBX.make<TH1F>(Form("h1_fC_max_%s",label_RBX.c_str()),"",20000,0.,50000.);
      
      _p1_ADC_shape_RBX[label_RBX] = subDir_perRBX.make<TProfile>(Form("p1_ADC_shape_%s",label_RBX.c_str()),"",10,-0.5,9.5);
      _p1_ADC_shapePedSub_RBX[label_RBX] = subDir_perRBX.make<TProfile>(Form("p1_ADC_shapePedSub_%s",label_RBX.c_str()),"",10,-0.5,9.5);
      _p1_fC_shape_RBX[label_RBX] = subDir_perRBX.make<TProfile>(Form("p1_fC_shape_%s",label_RBX.c_str()),"",10,-0.5,9.5);
      _p1_fC_shapePedSub_RBX[label_RBX] = subDir_perRBX.make<TProfile>(Form("p1_fC_shapePedSub_%s",label_RBX.c_str()),"",10,-0.5,9.5);
      
      for(int jj = 0; jj < 10; ++jj)
      {
        _p1_ADC_shape_RBX_i[label_RBX][jj] = subDir_perRBX.make<TProfile>(Form("p1_ADC_shape_%s_%d",label_RBX.c_str(),jj),"",10,-0.5,9.5);
        _p1_ADC_shapePedSub_RBX_i[label_RBX][jj] = subDir_perRBX.make<TProfile>(Form("p1_ADC_shapePedSub_%s_%d",label_RBX.c_str(),jj),"",10,-0.5,9.5);
        _p1_fC_shape_RBX_i[label_RBX][jj] = subDir_perRBX.make<TProfile>(Form("p1_fC_shape_%s_%d",label_RBX.c_str(),jj),"",10,-0.5,9.5);
        _p1_fC_shapePedSub_RBX_i[label_RBX][jj] = subDir_perRBX.make<TProfile>(Form("p1_fC_shapePedSub_%s_%d",label_RBX.c_str(),jj),"",10,-0.5,9.5);        
      }
    }
    
    int max_TS = -1;
    float max_ADC = -1.;
    float max_fC = -1.;
    float ped_ADC = 0.;
    float ped_fC = 0.;
    float sig_ADC = 0.;
    float sig_fC = 0.;
    
    //iterate thru all TS and fill
    for(int j = 0; j < 10; ++j)
    {      
      if( frame[j].adc() > max_ADC ) { max_ADC = frame[j].adc(); max_TS = j; }
      if( frame[j].nominal_fC() > max_fC ) max_fC = frame[j].nominal_fC();
      
      if( j < 2 )
      {
        ped_ADC += ( frame[j].adc() );
        ped_fC  += ( frame[j].nominal_fC() );
      }
    }
    ped_ADC /= 2.;
    ped_fC /= 2.;
    
    
    // basic selection
    if( ped_ADC > 50. ) continue;
    //if( max_ADC < 20. ) continue;
    
    // if( max_ADC > 20 )
    // {
    //   std::cout << did << std::endl;
    //   std::cout << "ADC: " ;
    //   for(int j = 0; j < 10; ++j)
    //     std::cout << frame[j].adc() << " ";
    //   std::cout << std::endl;
    //   std::cout << "fC: " ;
    //   for(int j = 0; j < 10; ++j)
    //     std::cout << frame[j].nominal_fC() << " ";
    //   std::cout << std::endl;
    // }
    
    _p2_maxTS_channel_iphi_vs_ieta[did.depth()] -> Fill( did.ieta(),did.iphi(),max_TS );
    
    
    //iterate thru all TS and fill
    for(int j = 0; j < 10; ++j)
    {      
      
      if( (j >= (max_TS - 1)) && (j <= max_TS + 2) )
      {
        sig_ADC += ( frame[j].adc()-ped_ADC );
        sig_fC  += ( frame[j].nominal_fC()-ped_fC );
      }
      
      _p1_ADC_shape_channel[label_channel] -> Fill( j,frame[j].adc() );
      _p1_ADC_shapePedSub_channel[label_channel] -> Fill( j,frame[j].adc()-ped_ADC );
      _p1_fC_shape_channel[label_channel] -> Fill( j,frame[j].nominal_fC() );
      _p1_fC_shapePedSub_channel[label_channel] -> Fill( j,frame[j].nominal_fC()-ped_fC );
      
      _h1_ADC_channel[label_channel] -> Fill( frame[j].adc()  );
      _h1_fC_channel[label_channel] -> Fill( frame[j].nominal_fC() );
      
      _p1_ADC_shape_RBX[label_RBX] -> Fill( j,frame[j].adc() );
      _p1_ADC_shapePedSub_RBX[label_RBX] -> Fill( j,frame[j].adc()-ped_ADC );
      _p1_fC_shape_RBX[label_RBX] -> Fill( j,frame[j].nominal_fC() );
      _p1_fC_shapePedSub_RBX[label_RBX] -> Fill( j,frame[j].nominal_fC()-ped_fC );
      
      _h1_ADC_RBX[label_RBX] -> Fill( frame[j].adc()  );
      _h1_fC_RBX[label_RBX] -> Fill( frame[j].nominal_fC() );
    }
    
    _h1_ADC_ped_channel[label_channel] -> Fill( ped_ADC );
    _h1_fC_ped_channel[label_channel] -> Fill( ped_fC );
    _h1_ADC_sig_channel[label_channel] -> Fill( sig_ADC  );
    _h1_fC_sig_channel[label_channel] -> Fill( sig_fC );
    _h1_ADC_max_channel[label_channel] -> Fill( max_ADC  );
    _h1_fC_max_channel[label_channel] -> Fill( max_fC );
    
    _h1_ADC_ped_RBX[label_RBX] -> Fill( ped_ADC );
    _h1_fC_ped_RBX[label_RBX] -> Fill( ped_fC );
    _h1_ADC_sig_RBX[label_RBX] -> Fill( sig_ADC  );
    _h1_fC_sig_RBX[label_RBX] -> Fill( sig_fC );
    _h1_ADC_max_RBX[label_RBX] -> Fill( max_ADC  );
    _h1_fC_max_RBX[label_RBX] -> Fill( max_fC );
  }
  
  
  //---------------
  // qie11 variables
  for(uint32_t i = 0; i < cqie11->size(); ++i)
  {
    QIE11DataFrame frame = static_cast<QIE11DataFrame>((*cqie11)[i]);
    
    HcalDetId did = frame.id();
    //std::cout << "did: " << did << "   " << did.subdet() << std::endl;
    
    if( did.subdet() != 2 ) continue;
    if( did.zside() < 0 ) continue;
    
    std::string label_channel = std::string(Form("ieta%d_iphi%d_depth%d",did.ieta(),did.iphi(),did.depth()));
    
    std::string label_RBX = "";
    if( did.subdet() == HcalBarrel  ) label_RBX += "HB";
    if( did.subdet() == HcalEndcap  ) label_RBX += "HE";
    if( did.subdet() == HcalOuter   ) label_RBX += "HO";
    if( did.subdet() == HcalForward ) label_RBX += "HF";
    if( did.ieta() > 0 ) label_RBX += "P";
    if( did.ieta() < 0 ) label_RBX += "M";
    label_RBX += std::string(Form("%02d",GetRBXId(did.subdet(),did.iphi())));
    
    //std::cout << "label_channel: " << label_channel << "   label_RBX: " << label_RBX << std::endl;
    
    if( _h1_ADC_channel[label_channel] == NULL )
    {
      perChannelList.push_back( std::make_tuple(did.ieta(),did.iphi(),did.depth(),label_channel,label_RBX) );
      
      _h1_ADC_channel[label_channel] = subDir_perChannel.make<TH1F>(Form("h1_ADC_%s",label_channel.c_str()),"",256,-0.5,255.5);
      _h1_ADC_ped_channel[label_channel] = subDir_perChannel.make<TH1F>(Form("h1_ADC_ped_%s",label_channel.c_str()),"",256,-0.5,255.5);
      _h1_ADC_sig_channel[label_channel] = subDir_perChannel.make<TH1F>(Form("h1_ADC_sig_%s",label_channel.c_str()),"",512,-0.5,511.5);
      _h1_ADC_max_channel[label_channel] = subDir_perChannel.make<TH1F>(Form("h1_ADC_max_%s",label_channel.c_str()),"",256,-0.5,255.5);
      
      _h1_fC_channel[label_channel] = subDir_perChannel.make<TH1F>(Form("h1_fC_%s",label_channel.c_str()),"",20000,0.,50000.);
      _h1_fC_ped_channel[label_channel] = subDir_perChannel.make<TH1F>(Form("h1_fC_ped_%s",label_channel.c_str()),"",20000,0.,50000.);
      _h1_fC_sig_channel[label_channel] = subDir_perChannel.make<TH1F>(Form("h1_fC_sig_%s",label_channel.c_str()),"",20000,0.,100000.);
      _h1_fC_max_channel[label_channel] = subDir_perChannel.make<TH1F>(Form("h1_fC_max_%s",label_channel.c_str()),"",20000,0.,50000.);
      
      _p1_ADC_shape_channel[label_channel] = subDir_perChannel.make<TProfile>(Form("p1_ADC_shape_%s",label_channel.c_str()),"",10,-0.5,9.5);
      _p1_ADC_shapePedSub_channel[label_channel] = subDir_perChannel.make<TProfile>(Form("p1_ADC_shapePedSub_%s",label_channel.c_str()),"",10,-0.5,9.5);
      _p1_fC_shape_channel[label_channel] = subDir_perChannel.make<TProfile>(Form("p1_fC_shape_%s",label_channel.c_str()),"",10,-0.5,9.5);
      _p1_fC_shapePedSub_channel[label_channel] = subDir_perChannel.make<TProfile>(Form("p1_fC_shapePedSub_%s",label_channel.c_str()),"",10,-0.5,9.5);
    }
    
    if( _h1_ADC_RBX[label_RBX] == NULL )
    {
      _h1_ADC_RBX[label_RBX] = subDir_perRBX.make<TH1F>(Form("h1_ADC_%s",label_RBX.c_str()),"",256,-0.5,255.5);
      _h1_ADC_ped_RBX[label_RBX] = subDir_perRBX.make<TH1F>(Form("h1_ADC_ped_%s",label_RBX.c_str()),"",256,-0.5,255.5);
      _h1_ADC_sig_RBX[label_RBX] = subDir_perRBX.make<TH1F>(Form("h1_ADC_sig_%s",label_RBX.c_str()),"",512,-0.5,511.5);
      _h1_ADC_max_RBX[label_RBX] = subDir_perRBX.make<TH1F>(Form("h1_ADC_max_%s",label_RBX.c_str()),"",256,-0.5,255.5);
      
      _h1_fC_RBX[label_RBX] = subDir_perRBX.make<TH1F>(Form("h1_fC_%s",label_RBX.c_str()),"",20000,0.,50000.);
      _h1_fC_ped_RBX[label_RBX] = subDir_perRBX.make<TH1F>(Form("h1_fC_ped_%s",label_RBX.c_str()),"",20000,0.,50000.);
      _h1_fC_sig_RBX[label_RBX] = subDir_perRBX.make<TH1F>(Form("h1_fC_sig_%s",label_RBX.c_str()),"",20000,0.,100000.);
      _h1_fC_max_RBX[label_RBX] = subDir_perRBX.make<TH1F>(Form("h1_fC_max_%s",label_RBX.c_str()),"",20000,0.,50000.);
      
      _p1_ADC_shape_RBX[label_RBX] = subDir_perRBX.make<TProfile>(Form("p1_ADC_shape_%s",label_RBX.c_str()),"",10,-0.5,9.5);
      _p1_ADC_shapePedSub_RBX[label_RBX] = subDir_perRBX.make<TProfile>(Form("p1_ADC_shapePedSub_%s",label_RBX.c_str()),"",10,-0.5,9.5);
      _p1_fC_shape_RBX[label_RBX] = subDir_perRBX.make<TProfile>(Form("p1_fC_shape_%s",label_RBX.c_str()),"",10,-0.5,9.5);
      _p1_fC_shapePedSub_RBX[label_RBX] = subDir_perRBX.make<TProfile>(Form("p1_fC_shapePedSub_%s",label_RBX.c_str()),"",10,-0.5,9.5);
    }
    
    int max_TS = -1;
    float max_ADC = -1.;
    float max_fC = -1.;
    float ped_ADC = 0.;
    float ped_fC = 0.;
    float sig_ADC = 0.;
    float sig_fC = 0.;
    
    //iterate thru all TS and fill
    for(int j = 0; j < 10; ++j)
    {      
      if( j > 0 )
      {
        if( frame[j].adc() > max_ADC ) { max_ADC = frame[j].adc(); max_TS = j; }
        if( new_adc2fC[frame[j].adc()] > max_fC ) max_fC = new_adc2fC[frame[j].adc()];
      }
      
      if( j < 3 && j > 0 )
      {
        ped_ADC += ( frame[j].adc() );
        ped_fC  += ( new_adc2fC[frame[j].adc()] );
      }
    }
    ped_ADC /= 2.;
    ped_fC /= 2.;
    
    
    // basic selection
    // if( ped_ADC > 50. ) continue;
    // if( max_ADC < 20. ) continue;
    if( max_ADC == 255 ) continue;
    if( max_ADC < 70 ) continue;
    
    std::cout << did << std::endl;
    std::cout << "ADC: " ;
    for(int j = 0; j < 10; ++j)
      std::cout << frame[j].adc() << " ";
    std::cout << std::endl;
    std::cout << "fC: " ;
    for(int j = 0; j < 10; ++j)
      std::cout << new_adc2fC[frame[j].adc()] << " ";
    std::cout << std::endl;
    
    h2_muon_mipSignal_innerPos_y_vs_x -> Fill( muon.innerPosition().x(), muon.innerPosition().y() );
    h2_muon_mipSignal_outerPos_y_vs_x -> Fill( muon.outerPosition().x(), muon.outerPosition().y() );
    
    _p2_maxTS_channel_iphi_vs_ieta[did.depth()] -> Fill( did.ieta(),did.iphi(),max_TS );
    
    
    //iterate thru all TS and fill
    for(int j = 0; j < 10; ++j)
    {      
      
      if( (j >= (max_TS - 1)) && (j <= max_TS + 2) )
      {
        sig_ADC += ( frame[j].adc()-ped_ADC );
        sig_fC  += ( new_adc2fC[frame[j].adc()]-ped_fC );
      }
      
      _p1_ADC_shape_channel[label_channel] -> Fill( j,frame[j].adc() );
      _p1_ADC_shapePedSub_channel[label_channel] -> Fill( j,frame[j].adc()-ped_ADC );
      _p1_fC_shape_channel[label_channel] -> Fill( j,new_adc2fC[frame[j].adc()] );
      _p1_fC_shapePedSub_channel[label_channel] -> Fill( j,new_adc2fC[frame[j].adc()]-ped_fC );
      
      _h1_ADC_channel[label_channel] -> Fill( frame[j].adc()  );
      _h1_fC_channel[label_channel] -> Fill( new_adc2fC[frame[j].adc()] );
      
      _p1_ADC_shape_RBX[label_RBX] -> Fill( j,frame[j].adc() );
      _p1_ADC_shapePedSub_RBX[label_RBX] -> Fill( j,frame[j].adc()-ped_ADC );
      _p1_fC_shape_RBX[label_RBX] -> Fill( j,new_adc2fC[frame[j].adc()] );
      _p1_fC_shapePedSub_RBX[label_RBX] -> Fill( j,new_adc2fC[frame[j].adc()]-ped_fC );
      
      if( nGoodEvents_qie11 < 10 )
      {
        std::cout << "nGoodEvents_qie11: " << nGoodEvents_qie11 << std::endl;
        _p1_ADC_shape_RBX_i[label_RBX][nGoodEvents_qie11] -> Fill( j,frame[j].adc() );
        _p1_ADC_shapePedSub_RBX_i[label_RBX][nGoodEvents_qie11] -> Fill( j,frame[j].adc()-ped_ADC );
        _p1_fC_shape_RBX_i[label_RBX][nGoodEvents_qie11] -> Fill( j,new_adc2fC[frame[j].adc()] );
        _p1_fC_shapePedSub_RBX_i[label_RBX][nGoodEvents_qie11] -> Fill( j,new_adc2fC[frame[j].adc()]-ped_fC );
      }
      
      _h1_ADC_RBX[label_RBX] -> Fill( frame[j].adc()  );
      _h1_fC_RBX[label_RBX] -> Fill( new_adc2fC[frame[j].adc()] );
    }
    ++nGoodEvents_qie11;
    
    _h1_ADC_ped_channel[label_channel] -> Fill( ped_ADC );
    _h1_fC_ped_channel[label_channel] -> Fill( ped_fC );
    _h1_ADC_sig_channel[label_channel] -> Fill( sig_ADC  );
    _h1_fC_sig_channel[label_channel] -> Fill( sig_fC );
    _h1_ADC_max_channel[label_channel] -> Fill( max_ADC  );
    _h1_fC_max_channel[label_channel] -> Fill( max_fC );
    
    _h1_ADC_ped_RBX[label_RBX] -> Fill( ped_ADC );
    _h1_fC_ped_RBX[label_RBX] -> Fill( ped_fC );
    _h1_ADC_sig_RBX[label_RBX] -> Fill( sig_ADC  );
    _h1_fC_sig_RBX[label_RBX] -> Fill( sig_fC );
    _h1_ADC_max_RBX[label_RBX] -> Fill( max_ADC  );
    _h1_fC_max_RBX[label_RBX] -> Fill( max_fC );
  }
  
  
  // fill per channel maps
  for(unsigned int it = 0; it < perChannelList.size(); ++it)
  {
    std::tuple<int,int,int,std::string,std::string> val = perChannelList.at(it);
    int ieta = std::get<0>(val);
    int iphi = std::get<1>(val);
    int depth = std::get<2>(val);
    std::string label_channel = std::get<3>(val);
    std::string label_RBX = std::get<4>(val);
    
    if( _h1_ADC_ped_channel[label_channel]->GetEntries() < 30. ) continue;
    
    _p2_ADC_ped_channel_iphi_vs_ieta[depth] -> Fill( ieta,iphi,_h1_ADC_ped_channel[label_channel]->GetMean() );
    _p2_ADC_sig_channel_iphi_vs_ieta[depth] -> Fill( ieta,iphi,_h1_ADC_sig_channel[label_channel]->GetMean() );
    _p2_ADC_max_channel_iphi_vs_ieta[depth] -> Fill( ieta,iphi,_h1_ADC_max_channel[label_channel]->GetMean() );
    
    _p2_fC_ped_channel_iphi_vs_ieta[depth] -> Fill( ieta,iphi,_h1_fC_ped_channel[label_channel]->GetMean() );
    _p2_fC_sig_channel_iphi_vs_ieta[depth] -> Fill( ieta,iphi,_h1_fC_sig_channel[label_channel]->GetMean() );
    _p2_fC_max_channel_iphi_vs_ieta[depth] -> Fill( ieta,iphi,_h1_fC_max_channel[label_channel]->GetMean() );
    
    _p2_ADC_ped_RBX_iphi_vs_ieta -> Fill( ieta,iphi,_h1_ADC_ped_RBX[label_RBX]->GetMean() );
    _p2_ADC_sig_RBX_iphi_vs_ieta -> Fill( ieta,iphi,_h1_ADC_sig_RBX[label_RBX]->GetMean() );
    _p2_ADC_max_RBX_iphi_vs_ieta -> Fill( ieta,iphi,_h1_ADC_max_RBX[label_RBX]->GetMean() );
    
    _p2_fC_ped_RBX_iphi_vs_ieta -> Fill( ieta,iphi,_h1_fC_ped_RBX[label_RBX]->GetMean() );
    _p2_fC_sig_RBX_iphi_vs_ieta -> Fill( ieta,iphi,_h1_fC_sig_RBX[label_RBX]->GetMean() );
    _p2_fC_max_RBX_iphi_vs_ieta -> Fill( ieta,iphi,_h1_fC_max_RBX[label_RBX]->GetMean() );
  }
}

DEFINE_FWK_MODULE(MIPAnalysis);
