#ifndef MIPAnalysis_h
#define MIPAnalysis_h

/**
 *file: MIPAnalysis.h
 *Author: Andrea Benaglia
 *Description: analyze raw digis
 */

#include <iostream>
#include <string>
#include <map>
#include <tuple>

#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TProfile2D.h"

#include "HCALStudies/TPDigi/interface/umnvars.h"
#include "HCALStudies/TPDigi/interface/Utils.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "CommonTools/Utils/interface/TFileDirectory.h"
#include "DataFormats/HcalDigi/interface/HcalDigiCollections.h"
#include "DataFormats/HcalDigi/interface/HcalUMNioDigi.h"
#include "DataFormats/HcalDigi/interface/HcalCalibDataFrame.h"
#include "DataFormats/HcalDetId/interface/HcalSubdetector.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/Common/interface/TriggerResults.h"



class MIPAnalysis : public edm::EDAnalyzer
{
public:
  //! ctor
  MIPAnalysis(edm::ParameterSet const& ps);
  
  //! dtor
  ~MIPAnalysis();
  
private:
  virtual void beginJob();
  virtual void analyze(const edm::Event& ev, const edm::EventSetup& es);
  virtual void endJob();
  
  edm::InputTag _tagQIE8;
  edm::EDGetTokenT<HBHEDigiCollection> _tokQIE8;
  
  edm::InputTag _tagQIE11;
  edm::EDGetTokenT<QIE11DigiCollection> _tokQIE11;
  
  edm::InputTag _tagMuons;
  edm::EDGetTokenT<reco::TrackCollection> _tokMuons;
  
  edm::InputTag _tagTriggerResults;
  edm::EDGetTokenT<edm::TriggerResults> _tokTriggerResults;
  
  edm::Service<TFileService> fs;
  TFileDirectory subDir_perChannel;
  TFileDirectory subDir_perRBX;
  std::vector<std::tuple<int,int,int,std::string,std::string> > perChannelList;
  std::vector<std::string> perRBXList;
  
  int _iEntry;
  
 
  TH2F* h2_muon_innerPos_y_vs_x;
  TH2F* h2_muon_outerPos_y_vs_x;
  TH2F* h2_muon_mipSignal_innerPos_y_vs_x;
  TH2F* h2_muon_mipSignal_outerPos_y_vs_x;
  
  std::map<int,TProfile2D*> _p2_maxTS_channel_iphi_vs_ieta;
  std::map<int,TProfile2D*> _p2_timing_channel_iphi_vs_ieta;
  
  std::map<int,TProfile2D*> _p2_ADC_ped_channel_iphi_vs_ieta;
  std::map<int,TProfile2D*> _p2_ADC_sig_channel_iphi_vs_ieta;
  std::map<int,TProfile2D*> _p2_ADC_max_channel_iphi_vs_ieta;
  TProfile2D* _p2_ADC_ped_RBX_iphi_vs_ieta;
  TProfile2D* _p2_ADC_sig_RBX_iphi_vs_ieta;
  TProfile2D* _p2_ADC_max_RBX_iphi_vs_ieta;
  
  std::map<int,TProfile2D*> _p2_fC_ped_channel_iphi_vs_ieta;
  std::map<int,TProfile2D*> _p2_fC_sig_channel_iphi_vs_ieta;
  std::map<int,TProfile2D*> _p2_fC_max_channel_iphi_vs_ieta;
  TProfile2D* _p2_fC_ped_RBX_iphi_vs_ieta;
  TProfile2D* _p2_fC_sig_RBX_iphi_vs_ieta;
  TProfile2D* _p2_fC_max_RBX_iphi_vs_ieta;
  
  std::map<std::string,TH1F*> _h1_ADC_channel;
  std::map<std::string,TH1F*> _h1_ADC_ped_channel;
  std::map<std::string,TH1F*> _h1_ADC_sig_channel;
  std::map<std::string,TH1F*> _h1_ADC_max_channel;
  std::map<std::string,TH1F*> _h1_ADC_RBX;
  std::map<std::string,TH1F*> _h1_ADC_ped_RBX;
  std::map<std::string,TH1F*> _h1_ADC_sig_RBX;
  std::map<std::string,TH1F*> _h1_ADC_max_RBX;
  
  std::map<std::string,TH1F*> _h1_fC_channel;
  std::map<std::string,TH1F*> _h1_fC_ped_channel;
  std::map<std::string,TH1F*> _h1_fC_sig_channel;
  std::map<std::string,TH1F*> _h1_fC_max_channel;
  std::map<std::string,TH1F*> _h1_fC_RBX;
  std::map<std::string,TH1F*> _h1_fC_ped_RBX;
  std::map<std::string,TH1F*> _h1_fC_sig_RBX;
  std::map<std::string,TH1F*> _h1_fC_max_RBX;
  
  std::map<std::string,TProfile*> _p1_ADC_shape_channel;
  std::map<std::string,TProfile*> _p1_ADC_shapePedSub_channel;
  std::map<std::string,TProfile*> _p1_ADC_shape_RBX;
  std::map<std::string,TProfile*> _p1_ADC_shapePedSub_RBX;
  
  std::map<std::string,TProfile*> _p1_fC_shape_channel;
  std::map<std::string,TProfile*> _p1_fC_shapePedSub_channel;
  std::map<std::string,TProfile*> _p1_fC_shape_RBX;
  std::map<std::string,TProfile*> _p1_fC_shapePedSub_RBX;
  
  std::map<std::string,std::map<int,TProfile*> > _p1_ADC_shape_RBX_i;
  std::map<std::string,std::map<int,TProfile*> > _p1_ADC_shapePedSub_RBX_i;
  std::map<std::string,std::map<int,TProfile*> > _p1_fC_shape_RBX_i;
  std::map<std::string,std::map<int,TProfile*> > _p1_fC_shapePedSub_RBX_i;
  
  
  int nGoodEvents_qie11;
};

#endif
