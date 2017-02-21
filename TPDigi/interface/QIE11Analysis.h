#ifndef QIE11Analysis_h
#define QIE11Analysis_h

/**
 *file: QIE11Analysis.h
 *Author: Andrea Benaglia
 *Description: analyze raw digis
 */

#include <iostream>

#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"

#include "HCALStudies/TPDigi/interface/tpdigi.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/Common/interface/DataFrame.h"
#include "DataFormats/HcalDigi/interface/HcalDigiCollections.h"
#include "DataFormats/HcalDigi/interface/HcalUnpackerReport.h"
#include "DQM/HcalCommon/interface/HashFilter.h"
#include "DQM/HcalCommon/interface/ElectronicsMap.h"



class QIE11Analysis : public edm::EDAnalyzer
{
public:
  //! ctor
  QIE11Analysis(edm::ParameterSet const& ps);
  
  //! dtor
  ~QIE11Analysis();
  
private:
  virtual void beginJob();
  virtual void analyze(const edm::Event& ev, const edm::EventSetup& es);
  virtual void endJob();
  
  edm::InputTag _tagQIE11;
  edm::EDGetTokenT<QIE11DigiCollection> _tokQIE11;
  
  hcaldqm::filter::HashFilter _filter_C36;
  HcalElectronicsMap const* _emap;
  hcaldqm::electronicsmap::ElectronicsMap _ehashmap;
  
  edm::Service<TFileService> _fs;

  int _iEntry;
  
  TTree* _tree;
  treeVars _treeVars;
  
  TH1F* _h1_ADC_11;
  TH1F* _h1_fC_11;
  TH1F* _h1_maxADC_11;
  TH1F* _h1_maxfC_11;
  TH1F* _h1_LETDC;
  TH2F* _h2_LETDC_vs_ADC;
  TH2F* _h2_LETDC_vs_fC;
  TProfile* _p1_shapeADC_11;
  TProfile* _p1_shapefC_11;
  
  std::map<int,TH1F*> _h1_ADC_11_TS;
  std::map<int,TH1F*> _h1_fC_11_TS;
  std::map<int,TH1F*> _h1_LETDC_TS;
  std::map<int,TH2F*> _h2_LETDC_vs_ADC_TS;
  std::map<int,TH2F*> _h2_LETDC_vs_fC_TS;
  
  std::map<std::string,TH1F*> _h1_ADC_11_channel;
  std::map<std::string,TH1F*> _h1_fC_11_channel;
  std::map<std::string,TH1F*> _h1_maxADC_11_channel;
  std::map<std::string,TH1F*> _h1_maxfC_11_channel;
  std::map<std::string,TH1F*> _h1_LETDC_channel;
  std::map<std::string,TH2F*> _h2_LETDC_vs_ADC_channel;
  std::map<std::string,TH2F*> _h2_LETDC_vs_fC_channel;
  std::map<std::string,TProfile*> _p1_shapeADC_11_channel;
  std::map<std::string,TProfile*> _p1_shapefC_11_channel;
  
  std::map<std::string,std::map<int,TH1F*> > _h1_ADC_11_channel_TS;
  std::map<std::string,std::map<int,TH1F*> > _h1_fC_11_channel_TS;
  std::map<std::string,std::map<int,TH1F*> > _h1_LETDC_channel_TS;
  std::map<std::string,std::map<int,TH2F*> > _h2_LETDC_vs_ADC_channel_TS;
  std::map<std::string,std::map<int,TH2F*> > _h2_LETDC_vs_fC_channel_TS;
};

#endif
