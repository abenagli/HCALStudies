#ifndef TPAnalysis_h
#define TPAnalysis_h

/**
 *file: TPAnalysis.h
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



bool isAllZero(const HcalTriggerPrimitiveDigi* tp1);
bool matching(const HcalTriggerPrimitiveDigi* tp1, const HcalTriggerPrimitiveDigi* tp2);
void FillTreeVars(treeVars& tv, coord& aCoord,
                  const HcalTriggerPrimitiveDigi* tp1 = NULL, const HcalTriggerPrimitiveDigi* tp2 = NULL,
                  const HcalTriggerPrimitiveDigi* etp1 = NULL, const HcalTriggerPrimitiveDigi* etp2 = NULL,
                  const HcalTriggerPrimitiveDigi* l1tp = NULL);
void PrintTP(const HcalTriggerPrimitiveDigi* tp = NULL);



class TPAnalysis : public edm::EDAnalyzer
{
public:
  //! ctor
  TPAnalysis(edm::ParameterSet const& ps);
  
  //! dtor
  ~TPAnalysis();
  
private:
  virtual void beginJob();
  virtual void analyze(const edm::Event& ev, const edm::EventSetup& es);
  virtual void endJob();
  
  edm::InputTag _tagData_VME;
  edm::InputTag _tagData_uTCA;
  edm::InputTag _tagEmul_VME;
  edm::InputTag _tagEmul_uTCA;
  edm::InputTag _tagL1;
  edm::InputTag _tagReport_VME;
  edm::InputTag _tagReport_uTCA;
  
  edm::EDGetTokenT<HcalTrigPrimDigiCollection> _tokData_VME;
  edm::EDGetTokenT<HcalTrigPrimDigiCollection> _tokData_uTCA;
  edm::EDGetTokenT<HcalTrigPrimDigiCollection> _tokEmul_VME;
  edm::EDGetTokenT<HcalTrigPrimDigiCollection> _tokEmul_uTCA;
  edm::EDGetTokenT<HcalTrigPrimDigiCollection> _tokL1;
  edm::EDGetTokenT<HcalUnpackerReport> _tokReport_VME;
  edm::EDGetTokenT<HcalUnpackerReport> _tokReport_uTCA;
  
  HcalElectronicsMap const* _emap;
  
  hcaldqm::filter::HashFilter _filter_VME;
  hcaldqm::filter::HashFilter _filter_uTCA;
  hcaldqm::electronicsmap::ElectronicsMap _ehashmap_VME;
  hcaldqm::electronicsmap::ElectronicsMap _ehashmap_uTCA;
  
  int _iEntry;
  
  std::map<std::string,TTree*> _trees;
  treeVars _treeVars;
  
  TProfile2D* _p2_Et_phi_vs_eta;
  
  std::map<int,TH2F*> _h2_FG_phi_vs_eta;
  std::map<int,TH2F*> _h2_FG_phi_vs_eta_cut;
  std::map<int,TProfile2D*> _p2_FG_phi_vs_eta;
  
  std::map<int,TH2F*> _h2_FG_phi_vs_eta_emul;
  std::map<int,TH2F*> _h2_FG_phi_vs_eta_cut_emul;
  std::map<int,TProfile2D*> _p2_FG_phi_vs_eta_emul;
};

#endif
