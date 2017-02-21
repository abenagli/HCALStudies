#include "HCALStudies/TPDigi/interface/MuonFilter.h"



//
// constructors and destructor
//
MuonFilter::MuonFilter(const edm::ParameterSet& config)
{
  _tagMuons = config.getUntrackedParameter<edm::InputTag>("tagMuons",edm::InputTag("cosmicMuonsEndCapsOnly"));
  _tokMuons = consumes<reco::TrackCollection>(_tagMuons);
}


MuonFilter::~MuonFilter()
{
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)
}


//
// member functions
//

// ------------ method called on each new Event  ------------
bool MuonFilter::filter(edm::Event& ev, const edm::EventSetup& es)
{
  edm::Handle<reco::TrackCollection> cmuons;
  
  if( !ev.getByToken(_tokMuons,cmuons) )
  {
    std::cout << "Collection reco::TrackCollection isn't available " + _tagMuons.label() + " " + _tagMuons.instance() << std::endl;  
    return false;
  }
  
  
  //----------------
  // muons variables
  for(uint32_t i = 0; i < cmuons->size(); ++i)
  {
    std::cout << "muon!!!" << std::endl;
    reco::Track muon = static_cast<reco::Track>((*cmuons)[i]);
    const math::XYZPoint outerPos = muon.outerPosition();
    if( outerPos.z() > 0. ) return true;
  }
  
  return false;
}

DEFINE_FWK_MODULE(MuonFilter);
