#ifndef RawAnalysis_h
#define RawAnalysis_h

/**
 *file: RawAnalysis.h
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
#include "DataFormats/FEDRawData/interface/FEDRawDataCollection.h"
#include "EventFilter/HcalRawToDigi/interface/AMC13Header.h"
#include "EventFilter/HcalRawToDigi/interface/HcalUHTRData.h"



class RawAnalysis : public edm::EDAnalyzer
{
public:
  //! ctor
  RawAnalysis(edm::ParameterSet const& ps);
  
  //! dtor
  ~RawAnalysis();
  
private:
  virtual void beginJob();
  virtual void analyze(const edm::Event& ev, const edm::EventSetup& es);
  virtual void endJob();
  
  edm::InputTag _tagFEDs;
  edm::EDGetTokenT<FEDRawDataCollection> _tokFEDs;
  
  int _iEntry;
};

#endif
