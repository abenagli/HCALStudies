#include "HCALStudies/TPDigi/interface/RawAnalysis.h"



RawAnalysis::RawAnalysis(edm::ParameterSet const& ps)
{
  //std::cout << "RawAnalysis::RawAnalysis" << std::endl;
  
  _tagFEDs = ps.getUntrackedParameter<edm::InputTag>("tagFEDs", edm::InputTag("rawDataCollector"));
  _tokFEDs = consumes<FEDRawDataCollection>(_tagFEDs);
}



RawAnalysis::~RawAnalysis()
{
  //std::cout << "RawAnalysis::~RawAnalysis" << std::endl;
}


void RawAnalysis::beginJob()
{
  // TFileService for output ntuples
  edm::Service<TFileService> fs;
  if( !fs )
  {
    throw edm::Exception(edm::errors::Configuration, "TFile Service is not registered in cfg file");
  }
  
  _iEntry = 0;
}



void RawAnalysis::endJob()
{
}


void RawAnalysis::analyze(const edm::Event& ev, const edm::EventSetup& es)
{
  std::cout << "RawAnalysis::analyze" << std::endl;
  std::cout << "------------------------------" << std::endl;
  std::cout << ">>> run: "   << ev.id().run() << std::endl;
  std::cout << ">>> LS:  "   << ev.luminosityBlock() << std::endl;
  std::cout << ">>> Orbit:  "<< ev.orbitNumber() << std::endl;
  std::cout << ">>> BX: "    << ev.bunchCrossing() << std::endl;
  std::cout << ">>> event: " << ev.id().event() << std::endl;
  std::cout << "------------------------------" << std::endl;
  
  
  
  //----------------
  // get collections
  edm::Handle<FEDRawDataCollection> cfeds;
  
  if( !ev.getByToken(_tokFEDs, cfeds) )
    std::cout << "Collection FEDRawDataCollection isn't available "
      + _tagFEDs.label() + " " + _tagFEDs.instance() + " " + _tagFEDs.process() << std::endl;
  
  
  for(int FEDId = 1100; FEDId <= 1132; FEDId+=1)
  {
    if( FEDId > 1122 && FEDId < 1132 ) continue;
    
    FEDRawData const& feds = cfeds -> FEDData(FEDId);
    
    std::cout << "trying FED " << FEDId << std::endl;
    hcal::AMC13Header const* AMC13 = (hcal::AMC13Header const*)( feds.data() );
    if( !AMC13 ) continue;
    
    uint32_t orn = AMC13->orbitNumber();
    uint32_t bcn = AMC13->bunchId();
    uint32_t evn = AMC13->l1aNumber();
    int eventType = AMC13->CDFEventType();
    
    int nAMC = AMC13->NAMC();
    
    std::cout << "AMC13   FED: " << FEDId << "   eventType: " << eventType << std::endl;
    
    for(int AMCId = 0; AMCId < nAMC; ++AMCId)
   {
      int slot = AMC13->AMCSlot(AMCId);
      int crate = AMC13->AMCId(AMCId)&0xFF;

      HcalUHTRData uhtr(AMC13->AMCPayload(AMCId),AMC13->AMCSize(AMCId));
      
      const uint64_t* data = AMC13->AMCPayload(AMCId);
      
      uint32_t uhtr_orn = uhtr.orbitNumber();
      uint32_t uhtr_bcn = uhtr.bunchNumber();
      uint32_t uhtr_evn = uhtr.l1ANumber();
      
      bool qorn = (uhtr_orn!=orn);
      bool qbcn = (uhtr_bcn!=bcn);
      bool qevn = (uhtr_evn!=evn);
      
      std::cout << ">>> AMCId: " << AMCId << "   crate: " << ((data[1])&0xFF) << "   FW flavour: " << ((data[1]>>32)&0xFF) << "   eventType: " << ((data[1]>>40)&0xF) << "   payloadFormat: " << ((data[1]>>44)&0xF) << std::endl;
      // //if( qorn )
      // {
      //   std::cout << "FED: " << FEDId << "   AMC13_ORN: " << orn << "   AMC" << AMCId << "_ORN: " << uhtr_orn << std::endl;
      // }
      // //if( qbcn )
      // {
      //   std::cout << "FED: " << FEDId << "   AMC13_BCN: " << bcn << "   AMC" << AMCId << "_BCN: " << uhtr_bcn << std::endl;
      // }
      // //if( qevn )
      // {
      //   std::cout << "FED: " << FEDId << "   AMC13_EVN: " << evn << "   AMC" << AMCId << "_EVN: " << uhtr_evn << std::endl;
      // }
    }
  }
  
  
  // unpacking uMNio
  {
    int FEDId = 1134;
    //int FEDId = 930;
    FEDRawData const& feds = cfeds -> FEDData(FEDId);
    
    
    hcal::AMC13Header const* AMC13 = (hcal::AMC13Header const*)( feds.data() );
    if( AMC13 )
    {
      uint32_t orn = AMC13->orbitNumber();
      uint32_t bcn = AMC13->bunchId();
      uint32_t evn = AMC13->l1aNumber();
      int eventType = AMC13->CDFEventType();
      
      int nAMC = AMC13->NAMC();
      
      std::cout << "AMC13   FED: " << FEDId << "   eventType: " << eventType << std::endl;
      
      for(int AMCId = 0; AMCId < nAMC; ++AMCId)
      {
        int slot = AMC13->AMCSlot(AMCId);
        int crate = AMC13->AMCId(AMCId)&0xFF;
        if( slot != 11 ) continue;
        //if( slot != 4 ) continue;
        
        // std::cout << "unpacking FED " << FEDId << ":   crate: " << crate << "   slot: " << slot << std::endl;
        
        const uint64_t* data = AMC13->AMCPayload(AMCId);
        int length = AMC13->AMCSize(AMCId);
        
        std::cout << ">>> crate: " << ((data[1])&0xFF) << "   FW flavour: " << ((data[1]>>32)&0xFF) << "   eventType: " << ((data[1]>>40)&0xF) << "   payloadFormat: " << ((data[1]>>44)&0xF) << std::endl;
        // std::cout << ">>> Data length: " << ((data[0])&0xFFFFF) << std::endl;
        // std::cout << ">>> BcN: " << ((data[0]>>20)&0xFFF) << std::endl;
        // std::cout << ">>> EvN: " << ((data[0]>>32)&0xFFFFFF) << std::endl;
        // std::cout << ">>> CrateId: " << ((data[1])&0xFF) << std::endl;
        // std::cout << ">>> SlotId: " << ((data[1]>>8)&0xF) << std::endl;
        // std::cout << ">>> Presamples: " << ((data[1]>>12)&0xF) << std::endl;
        // std::cout << ">>> OrN (1): " << ((data[1]>>16)&0xFFFF) << std::endl;
        // std::cout << ">>> Event type: " << ((data[1]>>40)&0xF) << std::endl;
        // std::cout << ">>> Payload format: " << ((data[1]>>44)&0xF) << std::endl;
        // std::cout << ">>> FW: " << ((data[1]>>60)&0x3F) << "." << ((data[1]>>54)&0x3F) << "." << ((data[1]>>48)&0x3F) << std::endl;
        // std::cout << ">>> OrN (2): " << (data[2]&0xFFFF) << std::endl;
        // std::cout << ">>> RunNumber: " << ((data[2]>>16)&0xFFFFFFFF) << std::endl;
        // std::cout << ">>> Number of user words: " << ((data[3])&0xFF) << std::endl;
        // std::cout << ">>> User word[0] id: " << ((data[3]>>16)&0xFFFF) << std::endl;
        std::cout << ">>> User word[0] value: " << ((data[3]>>32)&0xFFFFFFFF) << std::endl;
        // std::cout << ">>> Data length: " << ((data[4])&0xFFFFF) << std::endl;
        // std::cout << ">>> EvN: " << ((data[4]>>24)&0xFF) << std::endl;
        // std::cout << ">>> CRC32: " << ((data[4]>>32)&0xFFFFFFFF) << std::endl;
       }
    }
  }
  ++_iEntry;
}

DEFINE_FWK_MODULE(RawAnalysis);
