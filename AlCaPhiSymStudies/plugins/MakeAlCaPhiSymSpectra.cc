#include "MakeAlCaPhiSymSpectra.h"
#include "HLTStudies/AlCaPhiSymStudies/interface/TEndcapRings.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Common/interface/EventBase.h"
#include "DataFormats/Provenance/interface/EventAuxiliary.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "SimDataFormats/CaloHit/interface/PCaloHit.h"
#include "SimDataFormats/CaloHit/interface/PCaloHitContainer.h"
#include "SimDataFormats/CrossingFrame/interface/MixCollection.h"
#include "SimDataFormats/CrossingFrame/interface/CrossingFramePlaybackInfoExtended.h"
#include "SimDataFormats/CrossingFrame/interface/CrossingFrame.h"
#include "SimDataFormats/CrossingFrame/interface/PCrossingFrame.h"

#include "CalibCalorimetry/EcalLaserCorrection/interface/EcalLaserDbService.h"
#include "CalibCalorimetry/EcalLaserCorrection/interface/EcalLaserDbRecord.h"

#include "CondFormats/EcalObjects/interface/EcalIntercalibConstantsMC.h"
#include "CondFormats/DataRecord/interface/EcalIntercalibConstantsMCRcd.h"

#include "CondFormats/EcalObjects/interface/EcalADCToGeVConstant.h"
#include "CondFormats/DataRecord/interface/EcalADCToGeVConstantRcd.h"

#include "CondFormats/EcalObjects/interface/EcalChannelStatus.h"
#include "CondFormats/DataRecord/interface/EcalChannelStatusRcd.h"
#include "CondFormats/EcalObjects/interface/EcalChannelStatusCode.h"

#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/CaloEventSetup/interface/CaloTopologyRecord.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "Geometry/EcalAlgo/interface/EcalBarrelGeometry.h"
#include "Geometry/EcalAlgo/interface/EcalEndcapGeometry.h"

#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"

#include "Calibration/Tools/interface/EcalRingCalibrationTools.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <map>

MakeAlCaPhiSymSpectra::MakeAlCaPhiSymSpectra(const edm::ParameterSet& ps){

  recHitCollection_EB_ = consumes<EcalRecHitCollection>(ps.getParameter<edm::InputTag>("recHitCollection_EB"));
  recHitCollection_EE_ = consumes<EcalRecHitCollection>(ps.getParameter<edm::InputTag>("recHitCollection_EE"));

  enMin = 0.;
  enMax = 100.;
  calMin = 0.;
  calMax = 50.;

  nBins = 100000;  
  naiveId_ = 0;

  edm::Service<TFileService> fs;

  h_nEvents = fs->make<TH1F>("h_nEvents","h_nEvents",3,-1,2);

  h2_hitOccupancy_EB_highCut1 = fs->make<TH2F>("h2_hitOccupancy_EB_highCut1","h2_hitOccupancy_EB_highCut1",360,0,360,170,-85,85);
  h2_hitOccupancy_EB_highCut2 = fs->make<TH2F>("h2_hitOccupancy_EB_highCut2","h2_hitOccupancy_EB_highCut2",360,0,360,170,-85,85);
  h2_hitOccupancy_EEM_highCut1 = fs->make<TH2F>("h2_hitOccupancy_EEM_highCut1","h2_hitOccupancy_EEM_highCut1",100,1,101,100,1,101);
  h2_hitOccupancy_EEM_highCut2 = fs->make<TH2F>("h2_hitOccupancy_EEM_highCut2","h2_hitOccupancy_EEM_highCut2",100,1,101,100,1,101);
  h2_hitOccupancy_EEP_highCut1 = fs->make<TH2F>("h2_hitOccupancy_EEP_highCut1","h2_hitOccupancy_EEP_highCut1",100,1,101,100,1,101);
  h2_hitOccupancy_EEP_highCut2 = fs->make<TH2F>("h2_hitOccupancy_EEP_highCut2","h2_hitOccupancy_EEP_highCut2",100,1,101,100,1,101);

  EBM_eSpectrum_histos.resize(EB_rings);
  EBP_eSpectrum_histos.resize(EB_rings);
  EEM_eSpectrum_histos.resize(EE_rings);
  EEP_eSpectrum_histos.resize(EE_rings);

  EBM_etSpectrum_histos.resize(EB_rings);
  EBP_etSpectrum_histos.resize(EB_rings);
  EEM_etSpectrum_histos.resize(EE_rings);
  EEP_etSpectrum_histos.resize(EE_rings);

  std::ostringstream t;
  std::ostringstream title;
  for (int i=0; i<EB_rings; i++) { //EB

    t << "EBM_eSpectrum_" << i+1;
    EBM_eSpectrum_histos[i] = fs->make<TH1F>(t.str().c_str(),t.str().c_str(),nBins,enMin,enMax); 
    t.str("");
    t << "EBM_etSpectrum_" << i+1;
    EBM_etSpectrum_histos[i] = fs->make<TH1F>(t.str().c_str(),t.str().c_str(),nBins,enMin,enMax); 
    t.str("");

    t << "EBP_eSpectrum_" << i+1;
    EBP_eSpectrum_histos[i] = fs->make<TH1F>(t.str().c_str(),t.str().c_str(),nBins,enMin,enMax); 
    t.str("");
    t << "EBP_etSpectrum_" << i+1;
    EBP_etSpectrum_histos[i] = fs->make<TH1F>(t.str().c_str(),t.str().c_str(),nBins,enMin,enMax); 
    t.str("");
  }

  for (int i=0; i<EE_rings; i++) { //EE
    t << "EEM_eSpectrum_" << i+1;
    EEM_eSpectrum_histos[i] = fs->make<TH1F>(t.str().c_str(),t.str().c_str(),nBins,enMin,enMax); 
    t.str("");
    t << "EEM_etSpectrum_" << i+1;
    EEM_etSpectrum_histos[i] = fs->make<TH1F>(t.str().c_str(),t.str().c_str(),nBins,enMin,enMax); 
    t.str("");
    
    t << "EEP_eSpectrum_" << i+1;
    EEP_eSpectrum_histos[i] = fs->make<TH1F>(t.str().c_str(),t.str().c_str(),nBins,enMin,enMax); 
    t.str("");
    t << "EEP_etSpectrum_" << i+1;
    EEP_etSpectrum_histos[i] = fs->make<TH1F>(t.str().c_str(),t.str().c_str(),nBins,enMin,enMax); 
    t.str("");
    
  }
   
  
}

MakeAlCaPhiSymSpectra::~MakeAlCaPhiSymSpectra()
{
}

// ------------ method called once each job just before starting event loop  ------------

void MakeAlCaPhiSymSpectra::beginJob()
{

}

// ------------ method called to for each event  ------------
void MakeAlCaPhiSymSpectra::analyze(const edm::Event& ev, const edm::EventSetup& iSetup)
{
  naiveId_++;
  //std::cout << "Event = " << naiveId_ << std::endl;
  
  using namespace edm;
   
  //---Channel Status
  edm::ESHandle<EcalChannelStatus> csHandle;
  iSetup.get<EcalChannelStatusRcd>().get(csHandle);
  const EcalChannelStatus& channelStatus = *csHandle; 

  //---Calo Geometry
  edm::ESHandle<CaloGeometry> pGeometry;
  iSetup.get<CaloGeometryRecord>().get(pGeometry);
  const CaloGeometry *geometry = pGeometry.product();
  
  //---iRing-geometry 
  EcalRingCalibrationTools::setCaloGeometry(pGeometry.product()); 
  EcalRingCalibrationTools CalibRing;

  static const short N_RING_BARREL = EcalRingCalibrationTools::N_RING_BARREL;
  static const short N_RING_ENDCAP = EcalRingCalibrationTools::N_RING_ENDCAP;
  
  //---rechitsEB
  edm::Handle<EcalRecHitCollection> recHitsEB;
  ev.getByToken( recHitCollection_EB_, recHitsEB );
  const EcalRecHitCollection* theBarrelEcalRecHits = recHitsEB.product () ;
  if ( ! recHitsEB.isValid() ) {
    std::cerr << "EcalValidation::analyze --> recHitsEB not found" << std::endl; 
  }
  
  //---rechitsEE
  edm::Handle<EcalRecHitCollection> recHitsEE;
  ev.getByToken( recHitCollection_EE_, recHitsEE );
  const EcalRecHitCollection* theEndcapEcalRecHits = recHitsEE.product () ;
  if ( ! recHitsEE.isValid() ) {
    std::cerr << "EcalValidation::analyze --> recHitsEE not found" << std::endl; 
  }
    
  //---EBRechits iteration
  EBRecHitCollection::const_iterator itb;
  //std::cout << ">>> start EBRechits iteration " << std::endl;
  for (itb = theBarrelEcalRecHits->begin(); itb != theBarrelEcalRecHits->end(); ++itb)
    {
      EBDetId id_crystal(itb->id());
      GlobalPoint mycell = geometry -> getPosition(DetId(itb->id()));

      int ieta = id_crystal.ieta();
      float eta = mycell.eta();

      uint16_t statusCode = 0;
      statusCode = channelStatus[itb->id().rawId()].getStatusCode();
      if (statusCode != 0) continue;
            
      float e  = itb->energy();
      float et = itb -> energy()*mycell.perp()/mycell.mag();   

      float ebCut_GeV = 1.75*10*0.04;
      float et_thr_1 = ebCut_GeV/cosh(eta) + 1.;
      float et_thr_2 = ebCut_GeV/cosh(eta) + 2.;

      //occupancy plots
      if (e > ebCut_GeV && et < et_thr_1)
	h2_hitOccupancy_EB_highCut1->Fill(id_crystal.iphi(),id_crystal.ieta());
      if (e > ebCut_GeV && et < et_thr_2)
	h2_hitOccupancy_EB_highCut2->Fill(id_crystal.iphi(),id_crystal.ieta()); 
      
      //spectra
      if (ieta < 0) { //EBM
	EBM_eSpectrum_histos[ieta+85]->Fill(e);
	EBM_etSpectrum_histos[ieta+85]->Fill(et);
      }
      else if (ieta > 0) { //EBP
	EBP_eSpectrum_histos[ieta-1]->Fill(e);
	EBP_etSpectrum_histos[ieta-1]->Fill(et);

      }
      
    }
  
  //std::cout << "EBRechits iteration finished" << std::endl;

  //---EERechits iteration
  EERecHitCollection::const_iterator ite;
  //std::cout << ">>> start EERechits iteration " << std::endl;
  for (ite = theEndcapEcalRecHits->begin(); ite != theEndcapEcalRecHits->end(); ++ite)
    {
      EEDetId id_crystal(ite->id());
      GlobalPoint mycell = geometry -> getPosition(DetId(ite->id()));
      
      float eta = mycell.eta();
      int iring = CalibRing.getRingIndex(id_crystal);
      if(id_crystal.zside()<0) iring = iring-N_RING_BARREL;
      else if(id_crystal.zside()>0) iring = iring-N_RING_BARREL-N_RING_ENDCAP/2;

      uint16_t statusCode = 0;
      statusCode = channelStatus[ite->id().rawId()].getStatusCode();
      if (statusCode != 0) continue;
     
      float e  = ite->energy();
      float et = ite -> energy()*mycell.perp()/mycell.mag(); 
      float eepCut_GeV = 0;
      if(iring < 30) eepCut_GeV = 10*(4.87*iring + 98.12)/1000.;
      else eepCut_GeV = 10*(35.42*iring - 787.79)/1000.;
      float et_thr_1 = eepCut_GeV/cosh(eta) + 1.;
      float et_thr_2 = eepCut_GeV/cosh(eta) + 2.;

      //occupancy plots
      if (id_crystal.zside() > 0) { //EEP
	if (e > eepCut_GeV && et < et_thr_1)
	  h2_hitOccupancy_EEP_highCut1->Fill(id_crystal.ix(),id_crystal.iy());
	if (e > eepCut_GeV && et < et_thr_2)
	  h2_hitOccupancy_EEP_highCut2->Fill(id_crystal.ix(),id_crystal.iy());
      }
            
      if (id_crystal.zside() < 0) { //EEM
	if (e > eepCut_GeV && et < et_thr_1)
	  h2_hitOccupancy_EEM_highCut1->Fill(id_crystal.ix(),id_crystal.iy());
	if (e > eepCut_GeV && et < et_thr_2)
	  h2_hitOccupancy_EEM_highCut2->Fill(id_crystal.ix(),id_crystal.iy());
      }
	            
      //spectra
      if (id_crystal.zside() < 0) { //EEM
	EEM_eSpectrum_histos[iring]->Fill(e);
	EEM_etSpectrum_histos[iring]->Fill(et);
      }
      else if (id_crystal.zside() > 0) { //EEP
	EEP_eSpectrum_histos[iring]->Fill(e);
	EEP_etSpectrum_histos[iring]->Fill(et);
      }
      
    }  

}
  

// ------------ method called once each job just after ending the event loop  ------------
void MakeAlCaPhiSymSpectra::endJob()
{
  h_nEvents->SetBinContent(h_nEvents->FindBin(0),naiveId_); 
}


//define this as a plug-in
DEFINE_FWK_MODULE(MakeAlCaPhiSymSpectra);
