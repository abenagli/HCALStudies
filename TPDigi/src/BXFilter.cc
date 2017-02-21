#include "HCALStudies/TPDigi/interface/BXFilter.h"



//
// constructors and destructor
//
BXFilter::BXFilter(const edm::ParameterSet& config) :
  //now do what ever initialization is needed
  BXmin_( config.getParameter<int>("BXmin") ),
  BXmax_( config.getParameter<int>("BXmax") ),
  invert_( config.getParameter<bool>("invert") )
{
}


BXFilter::~BXFilter()
{
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)
}


void
BXFilter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<int>("BXmin",0);
  desc.add<int>("BXmax",3653);
  desc.add<bool>("invert",false);
  descriptions.add("bxFilter",desc);
}
//
// member functions
//

// ------------ method called on each new Event  ------------
bool
BXFilter::filter(edm::StreamID, edm::Event& iEvent, const edm::EventSetup& iSetup) const
{
  using namespace edm;
  
  if (iEvent.isRealData()) {
    bool accept(false);
    if( iEvent.bunchCrossing() >= BXmin_ && iEvent.bunchCrossing() <= BXmax_)
      accept = true;
    return accept;
  } else {
    return true;
  }
}

DEFINE_FWK_MODULE(BXFilter);
