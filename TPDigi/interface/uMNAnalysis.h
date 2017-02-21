#ifndef uMNAnalysis_h
#define uMNAnalysis_h

/**
 *file: uMNAnalysis.h
 *Author: Andrea Benaglia
 *Description: analyze raw digis
 */

#include <iostream>
#include <string>
#include <map>

#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"

#include "HCALStudies/TPDigi/interface/umnvars.h"
#include "HCALStudies/TPDigi/interface/Utils.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/HcalDigi/interface/HcalDigiCollections.h"
#include "DataFormats/HcalDigi/interface/HcalUMNioDigi.h"
#include "DataFormats/HcalDigi/interface/HcalCalibDataFrame.h"


class uMNAnalysis : public edm::EDAnalyzer
{
public:
  //! ctor
  uMNAnalysis(edm::ParameterSet const& ps);
  
  //! dtor
  ~uMNAnalysis();
  
private:
  virtual void beginJob();
  virtual void analyze(const edm::Event& ev, const edm::EventSetup& es);
  virtual void endJob();
  
  edm::InputTag _taguMNio;
  edm::InputTag _taguMNQIE;
  edm::EDGetTokenT<HcalUMNioDigi> _tokuMNio;
  edm::EDGetTokenT<QIE10DigiCollection> _tokuMNQIE;
  
  edm::Service<TFileService> fs;
  
  int _iEntry;
  
  TTree* _tree;
  uMNTreeVars _treeVars;
  
  TH1F* _h1_uMNQIE_ADC_1;
  TH1F* _h1_uMNQIE_ADC_ped_1;
  TH1F* _h1_uMNQIE_ADC_sig_1;
  TH1F* _h1_uMNQIE_ADC_max_1;
  TH1F* _h1_uMNQIE_ADC_2;
  TH1F* _h1_uMNQIE_ADC_ped_2;
  TH1F* _h1_uMNQIE_ADC_sig_2;
  TH1F* _h1_uMNQIE_ADC_max_2;
  
  TH1F* _h1_uMNQIE_fC_1;
  TH1F* _h1_uMNQIE_fC_ped_1;
  TH1F* _h1_uMNQIE_fC_sig_1;
  TH1F* _h1_uMNQIE_fC_max_1;
  TH1F* _h1_uMNQIE_fC_2;
  TH1F* _h1_uMNQIE_fC_ped_2;
  TH1F* _h1_uMNQIE_fC_sig_2;
  TH1F* _h1_uMNQIE_fC_max_2;
  
  TProfile* _p1_uMNQIE_shape_ADC_1;
  TProfile* _p1_uMNQIE_shapePedSub_ADC_1;
  TProfile* _p1_uMNQIE_shape_ADC_2;
  TProfile* _p1_uMNQIE_shapePedSub_ADC_2;
  
  TProfile* _p1_uMNQIE_shape_fC_1;
  TProfile* _p1_uMNQIE_shapePedSub_fC_1;
  TProfile* _p1_uMNQIE_shape_fC_2;
  TProfile* _p1_uMNQIE_shapePedSub_fC_2;
};

#endif
