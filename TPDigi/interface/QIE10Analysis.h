#ifndef QIE10Analysis_h
#define QIE10Analysis_h

/**
 *file: QIE10Analysis.h
 *Author: Andrea Benaglia
 *Description: analyze raw digis
 */

#include <iostream>

#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TProfile2D.h"

#include "HCALStudies/TPDigi/interface/tpdigi.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/HcalDigi/interface/HcalDigiCollections.h"
#include "DataFormats/HcalDigi/interface/HcalUnpackerReport.h"



void FillTreeVars(treeVars& tv);



class QIE10Analysis : public edm::EDAnalyzer
{
public:
  //! ctor
  QIE10Analysis(edm::ParameterSet const& ps);
  
  //! dtor
  ~QIE10Analysis();
  
private:
  virtual void beginJob();
  virtual void analyze(const edm::Event& ev, const edm::EventSetup& es);
  virtual void endJob();
  
  edm::InputTag _tagQIE10;
  edm::InputTag _tagQIE8;
  edm::EDGetTokenT<QIE10DigiCollection> _tokQIE10;
  edm::EDGetTokenT<HFDigiCollection> _tokQIE8;
  
  int _iEntry;
  
  TTree* _tree;
  treeVars _treeVars;
  
  std::vector<HcalDetId*> _DAIds;
  
  
  TProfile2D* _p2_ADC_10_depth1_phi_vs_eta;
  TProfile2D* _p2_ADC_10_depth2_phi_vs_eta;
  TProfile2D* _p2_ADC_10_depth3_phi_vs_eta;
  TProfile2D* _p2_ADC_10_depth4_phi_vs_eta;
  
  TH1F* _h1_ADC_10;
  TH1F* _h1_ADC_8;
  TH1F* _h1_maxADC_10;
  TH1F* _h1_maxADC_8;
  TH1F* _h1_LETDC;
  TH2F* _h2_LETDC_vs_ADC;
  
  TH2F* _h2_ADCSumCorr_10_vs_8;
  TH2F* _h2_ADCCorr_10_vs_8;
  TH2F* _h2_ADCCorr_10_vs_10;
  TH2F* _h2_LETDCCorr_10_vs_10;
  
  TProfile* _p1_shape_10;
  TProfile* _p1_shape_8;
  
  std::map<int,TH1F*> _h1_ADC_10_TS;
  std::map<int,TH1F*> _h1_ADC_8_TS;
  std::map<int,TH1F*> _h1_LETDC_TS;
  std::map<int,TH2F*> _h2_LETDC_vs_ADC_TS;
  
  std::map<int,TH2F*> _h2_ADCCorr_10_vs_8_TS;
  std::map<int,TH2F*> _h2_ADCCorr_10_vs_10_TS;
  std::map<int,TH2F*> _h2_LETDCCorr_10_vs_10_TS;
  
  
  std::map<uint32_t,TH1F*> _h1_ADC_10_channel;
  std::map<uint32_t,TH1F*> _h1_ADC_8_channel;
  std::map<uint32_t,TH1F*> _h1_maxADC_10_channel;
  std::map<uint32_t,TH1F*> _h1_maxADC_8_channel;
  std::map<uint32_t,TH1F*> _h1_LETDC_channel;
  std::map<uint32_t,TH2F*> _h2_LETDC_vs_ADC_channel;
  
  std::map<uint32_t,TH2F*> _h2_ADCSumCorr_10_vs_8_channel;
  std::map<uint32_t,TH2F*> _h2_ADCCorr_10_vs_8_channel;
  std::map<uint32_t,TH2F*> _h2_ADCCorr_10_vs_10_channel;
  std::map<uint32_t,TH2F*> _h2_LETDCCorr_10_vs_10_channel;
  
  std::map<uint32_t,TProfile*> _p1_shape_10_channel;
  std::map<uint32_t,TProfile*> _p1_shape_8_channel;
  
  std::map<uint32_t,std::map<int,TH1F*> > _h1_ADC_10_channel_TS;
  std::map<uint32_t,std::map<int,TH1F*> > _h1_ADC_8_channel_TS;
  std::map<uint32_t,std::map<int,TH1F*> > _h1_LETDC_channel_TS;
  std::map<uint32_t,std::map<int,TH2F*> > _h2_LETDC_vs_ADC_channel_TS;
  
  std::map<uint32_t,std::map<int,TH2F*> > _h2_ADCCorr_10_vs_8_channel_TS;
  std::map<uint32_t,std::map<int,TH2F*> > _h2_ADCCorr_10_vs_10_channel_TS;
  std::map<uint32_t,std::map<int,TH2F*> > _h2_LETDCCorr_10_vs_10_channel_TS;
};

#endif
