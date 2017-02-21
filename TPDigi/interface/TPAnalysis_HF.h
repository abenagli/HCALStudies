#ifndef TPAnalysis_HF_h
#define TPAnalysis_HF_h

/**
 *file: TPAnalysis_HF.h
 *Author: Andrea Benaglia
 *Description: analyze raw digis
 */

#include <iostream>

#include "TTree.h"

#include "HCALStudies/TPDigi/interface/tpdigi.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/HcalDigi/interface/HcalDigiCollections.h"
#include "CondFormats/HcalObjects/interface/HcalElectronicsMap.h"
#include "CalibFormats/HcalObjects/interface/HcalDbService.h"
#include "CalibFormats/HcalObjects/interface/HcalDbRecord.h"
#include "DQM/HcalCommon/interface/ElectronicsMap.h"
#include "DQM/HcalCommon/interface/HashFilter.h"
#include "DQM/HcalCommon/interface/Utilities.h"



class TPAnalysis_HF : public edm::EDAnalyzer
{
public:
  //! ctor
  TPAnalysis_HF(edm::ParameterSet const& ps);
  
  //! dtor
  ~TPAnalysis_HF();
  
private:
  virtual void beginJob();
  virtual void analyze(const edm::Event& ev, const edm::EventSetup& es);
  virtual void endJob();
  
  edm::InputTag _tagData_uTCA;
  edm::InputTag _tagData_emul;
  edm::InputTag _tagL1;
  edm::InputTag _tagReport_uTCA;
  edm::InputTag _tagReport_emul;
  
  edm::EDGetTokenT<HcalTrigPrimDigiCollection> _tokData_uTCA;
  edm::EDGetTokenT<HcalTrigPrimDigiCollection> _tokData_emul;
  edm::EDGetTokenT<HcalTrigPrimDigiCollection> _tokL1;
  edm::EDGetTokenT<HcalUnpackerReport> _tokReport_uTCA;
  edm::EDGetTokenT<HcalUnpackerReport> _tokReport_emul;
  
  HcalElectronicsMap const* _emap;
  
  hcaldqm::filter::HashFilter _filter_uTCA;
  hcaldqm::electronicsmap::ElectronicsMap _ehashmap_uTCA;
  
  int _iEntry;
  
  std::map<std::string,TTree*> _trees;
  treeVars _treeVars;
};

#endif
