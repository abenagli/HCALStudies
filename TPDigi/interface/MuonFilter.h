#ifndef MuonFilter_h
#define MuonFilter_h

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

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"



//
// class declaration
//

class MuonFilter : public edm::EDFilter
{
public:
  explicit MuonFilter(const edm::ParameterSet&);
  ~MuonFilter();
  
private:
  virtual bool filter(edm::Event &, const edm::EventSetup &);
  
  // ----------member data ---------------------------
  edm::InputTag _tagMuons;
  edm::EDGetTokenT<reco::TrackCollection> _tokMuons;
};

#endif
