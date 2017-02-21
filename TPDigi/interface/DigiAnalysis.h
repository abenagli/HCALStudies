#ifndef DigiAnalysis_h
#define DigiAnalysis_h

/**
 *file: DigiAnalysis.h
 *Author: Andrea Benaglia
 *Description: analyze raw digis
 */

#include <iostream>

#include "TTree.h"

#include "HCALStudies/TPDigi/interface/tpdigi.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/HcalDigi/interface/HcalDigiCollections.h"
#include "DataFormats/HcalDigi/interface/HcalUnpackerReport.h"




bool isAllZero(const HFDataFrame* df1);
bool matching(const HFDataFrame* df1, const HFDataFrame* df2);
void FillTreeVars(treeVars& tv, coord& aCoord,
                  const HFDataFrame* df1 = NULL, const HFDataFrame* df2 = NULL);



class DigiAnalysis : public edm::EDAnalyzer
{
public:
  //! ctor
  DigiAnalysis(edm::ParameterSet const& ps);
  
  //! dtor
  ~DigiAnalysis();
  
private:
  virtual void beginJob();
  virtual void analyze(const edm::Event& ev, const edm::EventSetup& es);
  virtual void endJob();
  
  edm::InputTag _tagHF_VME;
  edm::InputTag _tagHF_uTCA;
  edm::InputTag _tagHFReport_VME;
  edm::InputTag _tagHFReport_uTCA;
  edm::InputTag _tagHFReport;
  edm::InputTag _tagHOReport;
  edm::EDGetTokenT<HFDigiCollection> _tokHF_VME;
  edm::EDGetTokenT<HFDigiCollection> _tokHF_uTCA;
  edm::EDGetTokenT<HcalUnpackerReport> _tokHFReport_VME;
  edm::EDGetTokenT<HcalUnpackerReport> _tokHFReport_uTCA;
  edm::EDGetTokenT<HcalUnpackerReport> _tokHFReport;
  edm::EDGetTokenT<HcalUnpackerReport> _tokHOReport;
  
  int _iEntry;
  
  std::map<std::string,TTree*> _trees;
  treeVars _treeVars;
};

#endif
