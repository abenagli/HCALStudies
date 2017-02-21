#ifndef BXFilter_h
#define BXFilter_h
// -*- C++ -*-
//
// Package:    BXFilter
// Class:      BXFilter
// 
/**\class BXFilter BXFilter.cc filter/BXFilter/src/BXFilter.cc
Description: Filter to select HCAL abort gap events
Implementation:
<Notes on implementation>
*/
//
// Original Author:  Martin Grunewald
//         Created:  Tue Jan 22 13:55:00 CET 2008
//
//


// include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/global/EDFilter.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"

#include "DataFormats/FEDRawData/interface/FEDRawDataCollection.h"

#include <string>

//
// class declaration
//

class BXFilter : public edm::global::EDFilter<> {
public:
  explicit BXFilter(const edm::ParameterSet&);
  virtual ~BXFilter();
  static void fillDescriptions(edm::ConfigurationDescriptions & descriptions);
  
private:
  virtual bool filter(edm::StreamID, edm::Event &, const edm::EventSetup &) const override;
  
  // ----------member data ---------------------------

  /// accept the event if its BX is within BXmin and BXmax
  const int BXmin_;
  const int BXmax_;
  /// if invert_=true, invert that event accept decision
  const bool invert_;
};

#endif
