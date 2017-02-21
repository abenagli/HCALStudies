#ifndef TPMismatchFilter_h
#define TPMismatchFilter_h

// include files
#include "HCALStudies/TPDigi/interface/TPAnalysis.h"
#include "HCALStudies/TPDigi/interface/tpdigi.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "DataFormats/Common/interface/Handle.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"

#include "DataFormats/FEDRawData/interface/FEDRawDataCollection.h"

#include "DataFormats/HcalDigi/interface/HcalDigiCollections.h"
#include "CondFormats/HcalObjects/interface/HcalElectronicsMap.h"
#include "CalibFormats/HcalObjects/interface/HcalDbService.h"
#include "CalibFormats/HcalObjects/interface/HcalDbRecord.h"
#include "DQM/HcalCommon/interface/ElectronicsMap.h"
#include "DQM/HcalCommon/interface/HashFilter.h"
#include "DQM/HcalCommon/interface/Utilities.h"

#include <string>

//
// class declaration
//

class TPMismatchFilter : public edm::EDFilter
{
public:
  explicit TPMismatchFilter(const edm::ParameterSet&);
  ~TPMismatchFilter();
  
private:
  virtual bool filter(edm::Event &, const edm::EventSetup &);
  
  // ----------member data ---------------------------
  edm::InputTag _tagData_VME;
  edm::InputTag _tagData_uTCA;
  
  edm::EDGetTokenT<HcalTrigPrimDigiCollection> _tokData_VME;
  edm::EDGetTokenT<HcalTrigPrimDigiCollection> _tokData_uTCA;
  
  hcaldqm::filter::HashFilter _filter_VME;
  hcaldqm::filter::HashFilter _filter_uTCA;
  hcaldqm::electronicsmap::ElectronicsMap _ehashmap_VME;
  hcaldqm::electronicsmap::ElectronicsMap _ehashmap_uTCA;
  
  HcalElectronicsMap const* _emap;
};

#endif
