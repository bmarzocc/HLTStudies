#include "MakeRecoGenComparison.h"

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

#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/CaloEventSetup/interface/CaloTopologyRecord.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "Geometry/EcalAlgo/interface/EcalBarrelGeometry.h"
#include "Geometry/EcalAlgo/interface/EcalEndcapGeometry.h"

#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"

#include <fstream>
#include <iostream>
#include <memory>

MakeRecoGenComparison::MakeRecoGenComparison(const edm::ParameterSet& ps){

  pcaloHits_EB_                          = ps.getParameter<edm::InputTag>("pcaloHits_EB");
  pcaloHits_EE_                          = ps.getParameter<edm::InputTag>("pcaloHits_EE");
  uncalibrecHitCollection_noMultifit_EB_ = ps.getParameter<edm::InputTag>("uncalibrecHitCollection_noMultifit_EB");
  uncalibrecHitCollection_noMultifit_EE_ = ps.getParameter<edm::InputTag>("uncalibrecHitCollection_noMultifit_EE");
  uncalibrecHitCollection_EB_            = ps.getParameter<edm::InputTag>("uncalibrecHitCollection_EB");
  uncalibrecHitCollection_EE_            = ps.getParameter<edm::InputTag>("uncalibrecHitCollection_EE");
  recHitCollection_noMultifit_EB_        = ps.getParameter<edm::InputTag>("recHitCollection_noMultifit_EB");
  recHitCollection_noMultifit_EE_        = ps.getParameter<edm::InputTag>("recHitCollection_noMultifit_EE");
  recHitCollection_EB_                   = ps.getParameter<edm::InputTag>("recHitCollection_EB");
  recHitCollection_EE_                   = ps.getParameter<edm::InputTag>("recHitCollection_EE");

  naiveId_ = 0;

  mapLaserCorr.clear();
  sumE_EB.clear(); 
  sumE_EE.clear(); 
  sumESmeared_EB.clear(); 
  sumESmeared_EE.clear(); 

  edm::Service<TFileService> fs;
  p_ERatio_vs_bx_EB_noMultifit = fs->make<TProfile>("p_ERatio_vs_bx_noMultifit_EB","p_ERatio_vs_bx_noMultifit_EB",18,-13,5,-10000000.,10000000.);
  p_ERatio_vs_bx_EE_noMultifit = fs->make<TProfile>("p_ERatio_vs_bx_noMultifit_EE","p_ERatio_vs_bx_noMultifit_EE",18,-13,5,-10000000.,10000000.);
  p_ERatio_vs_bx_EB            = fs->make<TProfile>("p_ERatio_vs_bx_EB","p_ERatio_vs_bx_EB",10,-6,4,-10000000.,10000000.);
  p_ERatio_vs_bx_EE            = fs->make<TProfile>("p_ERatio_vs_bx_EE","p_ERatio_vs_bx_EE",10,-6,4,-10000000.,10000000.);
  h2_Occupancy_EB              = fs->make<TH2F>("h2_Occupancy_EB","h2_Occupancy_EB",360,0,360,170,-85,85);
  h2_Occupancy_EEP             = fs->make<TH2F>("h2_Occupancy_EEP","h2_Occupancy_EEP",100,1,101,100,1,101);
  h2_Occupancy_EEM             = fs->make<TH2F>("h2_Occupancy_EEM","h2_Occupancy_EEM",100,1,101,100,1,101);
  h_PCaloHits_Energy_EB        = fs->make<TH1F>("h_PCaloHits_Energy_EB","h_PCaloHits_Energy_EB",600,0.,6.);
  h_PCaloHits_Energy_EE        = fs->make<TH1F>("h_PCaloHits_Energy_EE","h_PCaloHits_Energy_EE",600,0.,6.);
  h_Energy_EB_noMultifit       = fs->make<TH1F>("h_Energy_EB_noMultifit","h_Energy_EB_noMultifit",900,-2.,6.);
  h_Energy_EE_noMultifit       = fs->make<TH1F>("h_Energy_EE_noMultifit","h_Energy_EE_noMultifit",1000,-4,6.);
  h_Energy_EB                  = fs->make<TH1F>("h_Energy_EB","h_Energy_EB",600,0.,6.);
  h_Energy_EE                  = fs->make<TH1F>("h_Energy_EE","h_Energy_EE",600,0.,6.);
  h_nEvents                    = fs->make<TH1F>("h_nEvents","h_nEvents",3,-1,2);

  for(int ii = -5; ii < 4;ii++){
    std::ostringstream t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18;

    if(ii == 0) t1 << "p_ERatio_vs_Energy_EB_noMultifit_bx" << ii;
    if(ii == 0) p_ERatio_vs_Energy_EB_noMultifit[ii] = fs->make<TProfile>(t1.str().c_str(),t1.str().c_str(),400,0.,4.,-10000000.,10000000);
    t2 << "p_ERatio_vs_Energy_EB_bx" << ii;
    p_ERatio_vs_Energy_EB[ii] = fs->make<TProfile>(t2.str().c_str(),t2.str().c_str(),400,0.,4.,-10000000.,10000000);
    if(ii == 0) t3 << "p_ERatio_vs_Energy_EE_noMultifit_bx" << ii;
    if(ii == 0) p_ERatio_vs_Energy_EE_noMultifit[ii] = fs->make<TProfile>(t3.str().c_str(),t3.str().c_str(),400,0.,4.,-10000000.,10000000);
    t4 << "p_ERatio_vs_Energy_EE_bx" << ii;
    p_ERatio_vs_Energy_EE[ii] = fs->make<TProfile>(t4.str().c_str(),t4.str().c_str(),400,0.,4.,-10000000.,10000000);

    if(ii == 0) t5 << "p_RecoEnergy_vs_Energy_EB_noMultifit_bx" << ii;
    if(ii == 0) p_RecoEnergy_vs_Energy_EB_noMultifit[ii] = fs->make<TProfile>(t5.str().c_str(),t5.str().c_str(),400,0.,4.,-10000000.,10000000);
    t6 << "p_RecoEnergy_vs_Energy_EB_bx" << ii;
    p_RecoEnergy_vs_Energy_EB[ii] = fs->make<TProfile>(t6.str().c_str(),t6.str().c_str(),400,0.,4.,-10000000.,10000000);
    if(ii == 0) t7 << "p_RecoEnergy_vs_Energy_EE_noMultifit_bx" << ii;
    if(ii == 0) p_RecoEnergy_vs_Energy_EE_noMultifit[ii] = fs->make<TProfile>(t7.str().c_str(),t7.str().c_str(),400,0.,4.,-10000000.,10000000);
    t8 << "p_RecoEnergy_vs_Energy_EE_bx" << ii;
    p_RecoEnergy_vs_Energy_EE[ii] = fs->make<TProfile>(t8.str().c_str(),t8.str().c_str(),400,0.,4.,-10000000.,10000000);

    if(ii == 0) t9 << "h2_RecoEnergy_vs_Energy_EB_noMultifit_bx" << ii;
    if(ii == 0) h2_RecoEnergy_vs_Energy_EB_noMultifit[ii] = fs->make<TH2F>(t9.str().c_str(),t9.str().c_str(),400,0.,4.,500,-1.,4.);
    t10 << "h2_RecoEnergy_vs_Energy_EB_bx" << ii;
    h2_RecoEnergy_vs_Energy_EB[ii] = fs->make<TH2F>(t10.str().c_str(),t10.str().c_str(),400,0.,4.,400,0.,4.);
    if(ii == 0) t11 << "h2_RecoEnergy_vs_Energy_EE_noMultifit_bx" << ii;
    if(ii == 0) h2_RecoEnergy_vs_Energy_EE_noMultifit[ii] = fs->make<TH2F>(t11.str().c_str(),t11.str().c_str(),400,0.,4.,600,-1.,4.);
    t12 << "h2_RecoEnergy_vs_Energy_EE_bx" << ii;
    h2_RecoEnergy_vs_Energy_EE[ii] = fs->make<TH2F>(t12.str().c_str(),t12.str().c_str(),400,0.,4.,400,0.,4.);

    t13 << "h_PCaloHits_vs_bx_EB_bx" << ii;
    h_PCaloHits_vs_bx_EB[ii] = fs->make<TH1F>(t13.str().c_str(),t13.str().c_str(),600,0.,6.);
    t14 << "h_PCaloHits_vs_bx_EE_bx" << ii;
    h_PCaloHits_vs_bx_EE[ii] = fs->make<TH1F>(t14.str().c_str(),t14.str().c_str(),600,0.,6.);
    if(ii == 0) t15 << "h_RecoEnergy_vs_bx_EB_noMultifit_bx" << ii;
    if(ii == 0) h_RecoEnergy_vs_bx_EB_noMultifit[ii] = fs->make<TH1F>(t15.str().c_str(),t15.str().c_str(),600,0.,6.);
    if(ii == 0) t16 << "h_RecoEnergy_vs_bx_EE_noMultifit_bx" << ii;
    if(ii == 0) h_RecoEnergy_vs_bx_EE_noMultifit[ii] = fs->make<TH1F>(t16.str().c_str(),t16.str().c_str(),600,0.,6.);
    t17 << "h_RecoEnergy_vs_bx_EB_bx" << ii;
    h_RecoEnergy_vs_bx_EB[ii] = fs->make<TH1F>(t17.str().c_str(),t17.str().c_str(),600,0.,6.);
    t18 << "h_RecoEnergy_vs_bx_EE_bx" << ii;
    h_RecoEnergy_vs_bx_EE[ii] = fs->make<TH1F>(t18.str().c_str(),t18.str().c_str(),600,0.,6.);

  }

}

MakeRecoGenComparison::~MakeRecoGenComparison()
{
        // do anything here that needs to be done at desctruction time
        // (e.g. close files, deallocate resources etc.)
}

// ------------ method called once each job just before starting event loop  ------------

void MakeRecoGenComparison::beginJob()
{
}

// ------------ method called to for each event  ------------
void MakeRecoGenComparison::analyze(const edm::Event& ev, const edm::EventSetup& iSetup)
{
  naiveId_++;
  std::cout << "Event = " << naiveId_ << std::endl;
  //DataBase LaserCorrection
  edm::ESHandle<EcalLaserDbService> theLaser;
  iSetup.get<EcalLaserDbRecord>().get(theLaser);

  /*// calo geometry
  edm::ESHandle<CaloGeometry> pGeometry;
  iSetup.get<CaloGeometryRecord>().get(pGeometry);
  const CaloGeometry *geometry = pGeometry.product();*/

  //pcalohitsEB
  edm::Handle<PCrossingFrame<PCaloHit> > pcaloHitsEB;
  ev.getByLabel( pcaloHits_EB_, pcaloHitsEB );
  std::vector<const PCaloHit*> thePcaloHitsEB = pcaloHitsEB.product()->getPileups();
  if ( ! pcaloHitsEB.isValid() ) {
    std::cerr << "EcalValidation::analyze --> pcaloHitsEB not found" << std::endl; 
  }    

  //pcalohitsEE
  edm::Handle<PCrossingFrame<PCaloHit> > pcaloHitsEE;
  ev.getByLabel( pcaloHits_EE_, pcaloHitsEE );
  std::vector<const PCaloHit*> thePcaloHitsEE = pcaloHitsEE.product()->getPileups();
  if ( ! pcaloHitsEE.isValid() ) {
    std::cerr << "EcalValidation::analyze --> pcaloHitsEE not found" << std::endl; 
  }   

  //uncalibrechitsEB_noMultifit
  edm::Handle<EcalUncalibratedRecHitCollection> uncalibrecHitsEB_noMultifit;
  ev.getByLabel( uncalibrecHitCollection_noMultifit_EB_, uncalibrecHitsEB_noMultifit );
  const EcalUncalibratedRecHitCollection* theBarrelEcalUncalibRecHits_noMultifit = uncalibrecHitsEB_noMultifit.product () ;
  if ( ! uncalibrecHitsEB_noMultifit.isValid() ) {
    std::cerr << "EcalValidation::analyze --> uncalibrecHitsEB_noMultifit not found" << std::endl; 
  }

  //uncalibrechitsEE_noMultifit
  edm::Handle<EcalUncalibratedRecHitCollection> uncalibrecHitsEE_noMultifit;
  ev.getByLabel( uncalibrecHitCollection_noMultifit_EE_, uncalibrecHitsEE_noMultifit );
  const EcalUncalibratedRecHitCollection* theEndcapEcalUncalibRecHits_noMultifit = uncalibrecHitsEE_noMultifit.product () ;
  if ( ! uncalibrecHitsEE_noMultifit.isValid() ) {
    std::cerr << "EcalValidation::analyze --> uncalibrecHitsEE_noMultifit not found" << std::endl; 
  }

  //uncalibrechitsEB
  edm::Handle<EcalUncalibratedRecHitCollection> uncalibrecHitsEB;
  ev.getByLabel( uncalibrecHitCollection_EB_, uncalibrecHitsEB );
  const EcalUncalibratedRecHitCollection* theBarrelEcalUncalibRecHits = uncalibrecHitsEB.product () ;
  if ( ! uncalibrecHitsEB.isValid() ) {
    std::cerr << "EcalValidation::analyze --> uncalibrecHitsEB not found" << std::endl; 
  }

  //uncalibrechitsEE
  edm::Handle<EcalUncalibratedRecHitCollection> uncalibrecHitsEE;
  ev.getByLabel( uncalibrecHitCollection_EE_, uncalibrecHitsEE );
  const EcalUncalibratedRecHitCollection* theEndcapEcalUncalibRecHits = uncalibrecHitsEE.product () ;
  if ( ! uncalibrecHitsEE.isValid() ) {
    std::cerr << "EcalValidation::analyze --> uncalibrecHitsEE not found" << std::endl; 
  }

  //rechitsEB_noMultifit
  edm::Handle<EcalRecHitCollection> recHitsEB_noMultifit;
  ev.getByLabel( recHitCollection_noMultifit_EB_, recHitsEB_noMultifit );
  const EcalRecHitCollection* theBarrelEcalRecHits_noMultifit = recHitsEB_noMultifit.product () ;
  if ( ! recHitsEB_noMultifit.isValid() ) {
    std::cerr << "EcalValidation::analyze --> recHitsEB_noMultifit not found" << std::endl; 
  }

  //rechitsEE_noMultifit
  edm::Handle<EcalRecHitCollection> recHitsEE_noMultifit;
  ev.getByLabel( recHitCollection_noMultifit_EE_, recHitsEE_noMultifit );
  const EcalRecHitCollection* theEndcapEcalRecHits_noMultifit = recHitsEE_noMultifit.product () ;
  if ( ! recHitsEE_noMultifit.isValid() ) {
    std::cerr << "EcalValidation::analyze --> recHitsEE_noMultifit not found" << std::endl; 
  }


  //rechitsEB
  edm::Handle<EcalRecHitCollection> recHitsEB;
  ev.getByLabel( recHitCollection_EB_, recHitsEB );
  const EcalRecHitCollection* theBarrelEcalRecHits = recHitsEB.product () ;
  if ( ! recHitsEB.isValid() ) {
    std::cerr << "EcalValidation::analyze --> recHitsEB not found" << std::endl; 
  }

  //rechitsEE
  edm::Handle<EcalRecHitCollection> recHitsEE;
  ev.getByLabel( recHitCollection_EE_, recHitsEE );
  const EcalRecHitCollection* theEndcapEcalRecHits = recHitsEE.product () ;
  if ( ! recHitsEE.isValid() ) {
    std::cerr << "EcalValidation::analyze --> recHitsEE not found" << std::endl; 
  }

  //evtTime = 0;
  const edm::Timestamp& evtTimeStamp = edm::Timestamp(0);

  //Fill pcalohits sumE
  int ipcalo=0;
  std::vector<const PCaloHit*>::const_iterator pcaloHitItrEB;
  for(pcaloHitItrEB = thePcaloHitsEB.begin () ;pcaloHitItrEB != thePcaloHitsEB.end () ;++pcaloHitItrEB)
  {
    ipcalo++;
    //std::cout<< "CaloHit "<< ipcalo+1<<" "<<  (*pcaloHitItrEB)->time() << " " << (*pcaloHitItrEB)->energy() << " from EncodedEventId: "<<(*pcaloHitItrEB)->eventId().bunchCrossing()<<" " <<(*pcaloHitItrEB)->eventId().event() <<std::endl;

    EBDetId EBid((*pcaloHitItrEB)->id());
    sumE_EB[(*pcaloHitItrEB)->eventId().bunchCrossing()][EBid.ieta()][EBid.iphi()][0] = sumE_EB[(*pcaloHitItrEB)->eventId().bunchCrossing()][EBid.ieta()][EBid.iphi()][0] + (*pcaloHitItrEB)->energy();
    
    float seedLaserCorrection = theLaser->getLaserCorrection(EBid, evtTimeStamp);
    mapLaserCorr[EBid.ieta()][EBid.iphi()][0] = seedLaserCorrection;
  }
 
  ipcalo=0;
  std::vector<const PCaloHit*>::const_iterator pcaloHitItrEE;
  for(pcaloHitItrEE = thePcaloHitsEE.begin () ;pcaloHitItrEE != thePcaloHitsEE.end () ;++pcaloHitItrEE)
  {
    ipcalo++;
    //if(ipcalo+1%1000 == 0) std::cout<< "CaloHit "<< ipcalo+1<<"/"<<  (*pcaloHitItr)->time() << " " << (*pcaloHitItr)->energy() << " from EncodedEventId: "<<(*pcaloHitItr)->eventId().bunchCrossing()<<" " <<(*pcaloHitItr)->eventId().event() <<std::endl;

    EEDetId EEid((*pcaloHitItrEE)->id());
    sumE_EE[(*pcaloHitItrEE)->eventId().bunchCrossing()][EEid.ix()][EEid.iy()][EEid.zside()] = sumE_EE[(*pcaloHitItrEE)->eventId().bunchCrossing()][EEid.ix()][EEid.iy()][EEid.zside()] + (*pcaloHitItrEE)->energy();
   
    float seedLaserCorrection = theLaser->getLaserCorrection(EEid, evtTimeStamp);
    mapLaserCorr[EEid.ix()][EEid.iy()][EEid.zside()] = seedLaserCorrection;

  }

  //doPhotoStatistics
  for(std::map<int,std::map<int, std::map<int, std::map<int,float> > > >::iterator it1=sumE_EB.begin(); it1!=sumE_EB.end(); ++it1)
    for(std::map<int,std::map<int, std::map<int,float> > >::iterator it2=it1->second.begin(); it2!=it1->second.end(); ++it2)
      for(std::map<int,std::map<int,float> >::iterator it3=it2->second.begin(); it3!=it2->second.end(); ++it3)
        for(std::map<int,float>::iterator it4=it3->second.begin(); it4!=it3->second.end(); ++it4){ 
            int bx = it1->first;
            int ieta = it2->first;
            int iphi = it3->first;
            int iz = it4->first; 
            if(iz != 0) continue;
            float energy = it4->second;   
            float npe = energy/mapLaserCorr[ieta][iphi][iz]*simHitToPhotoelectronsEB;
            float npeSmeared = doPhotonStatistics(npe);
            //float npeSmeared = npe;
            float energySmeared = npeSmeared/simHitToPhotoelectronsEB*mapLaserCorr[ieta][iphi][iz];   
            sumESmeared_EB[bx][ieta][iphi][iz] = energySmeared;

            if(energySmeared != 0){
               h_PCaloHits_Energy_EB->Fill(energySmeared);
               if(bx < -5 || bx > 3) continue;
               h_PCaloHits_vs_bx_EB[bx]->Fill(energySmeared);
            }

            if(sumE_EB[0][ieta][iphi][iz] != 0 && sumE_EB[bx][ieta][iphi][iz] >0.1) h2_Occupancy_EB->Fill(iphi,ieta);
        } 
  
  for(std::map<int,std::map<int, std::map<int, std::map<int,float> > > >::iterator it1=sumE_EE.begin(); it1!=sumE_EE.end(); ++it1)
    for(std::map<int,std::map<int, std::map<int,float> > >::iterator it2=it1->second.begin(); it2!=it1->second.end(); ++it2)
      for(std::map<int,std::map<int,float> >::iterator it3=it2->second.begin(); it3!=it2->second.end(); ++it3)
        for(std::map<int,float>::iterator it4=it3->second.begin(); it4!=it3->second.end(); ++it4){ 
            int bx = it1->first;
            int ix = it2->first;
            int iy = it3->first;
            int iz = it4->first; 
            if(iz == 0) continue;
            float energy = it4->second;   
            float npe = energy/mapLaserCorr[ix][iy][iz]*simHitToPhotoelectronsEE;
            float npeSmeared = doPhotonStatistics(npe);
            //float npeSmeared = npe;
            float energySmeared = npeSmeared/simHitToPhotoelectronsEE*mapLaserCorr[ix][iy][iz];   
            sumESmeared_EE[bx][ix][iy][iz] = energySmeared;

            if(energySmeared != 0){
               h_PCaloHits_Energy_EE->Fill(energySmeared);
               if(bx < -5 || bx > 3) continue;
               h_PCaloHits_vs_bx_EE[bx]->Fill(energySmeared);
            }

            if(sumE_EE[0][ix][iy][iz] != 0 && sumE_EE[bx][ix][iy][iz] >0.1){
              if(iz > 0) h2_Occupancy_EEP->Fill(ix,iy); 
              if(iz < 0) h2_Occupancy_EEM->Fill(ix,iy);
            } 
        } 

  //Fill the plots

  EcalRecHitCollection::const_iterator rechitItrEB_noMultifit;
  for(rechitItrEB_noMultifit = theBarrelEcalRecHits_noMultifit->begin () ;rechitItrEB_noMultifit != theBarrelEcalRecHits_noMultifit->end () ;++rechitItrEB_noMultifit)
  {
    EBDetId EBid(rechitItrEB_noMultifit->id());      
    if((*theBarrelEcalUncalibRecHits_noMultifit->find(rechitItrEB_noMultifit->id())).amplitude() == 0) continue;          
    for(std::map<int,std::map<int, std::map<int, std::map<int,float> > > >::iterator it=sumESmeared_EB.begin(); it!=sumESmeared_EB.end(); ++it)             
    {   
      if(it->first != 0) continue;
      float calib = rechitItrEB_noMultifit->energy()/(*theBarrelEcalUncalibRecHits_noMultifit->find(rechitItrEB_noMultifit->id())).amplitude();
      float energy = 0.;
      float ratio = 1.;  
      if(it->first != 0) energy = calib*(*theBarrelEcalUncalibRecHits_noMultifit->find(rechitItrEB_noMultifit->id())).outOfTimeAmplitude(it->first+5);
      else energy = rechitItrEB_noMultifit->energy();
      if(sumESmeared_EB[it->first][EBid.ieta()][EBid.iphi()][0] == 0) continue;
      ratio = energy/sumESmeared_EB[it->first][EBid.ieta()][EBid.iphi()][0];
      p_ERatio_vs_Energy_EB_noMultifit[it->first]->Fill(sumESmeared_EB[it->first][EBid.ieta()][EBid.iphi()][0],ratio);
      p_ERatio_vs_bx_EB_noMultifit->Fill(it->first,ratio);
      p_RecoEnergy_vs_Energy_EB_noMultifit[it->first]->Fill(sumESmeared_EB[it->first][EBid.ieta()][EBid.iphi()][0],energy);
      h2_RecoEnergy_vs_Energy_EB_noMultifit[it->first]->Fill(sumESmeared_EB[it->first][EBid.ieta()][EBid.iphi()][0],energy);
      if(energy != 0.){
         h_Energy_EB_noMultifit->Fill(energy);
         h_RecoEnergy_vs_bx_EB_noMultifit[it->first]->Fill(energy);
      } 
    }

  }

  EcalRecHitCollection::const_iterator rechitItrEE_noMultifit;
  for(rechitItrEE_noMultifit = theEndcapEcalRecHits_noMultifit->begin () ;rechitItrEE_noMultifit != theEndcapEcalRecHits_noMultifit->end () ;++rechitItrEE_noMultifit)
  {
    EEDetId EEid(rechitItrEE_noMultifit->id());      
    if((*theEndcapEcalUncalibRecHits_noMultifit->find(rechitItrEE_noMultifit->id())).amplitude() == 0) continue;          
    for(std::map<int,std::map<int, std::map<int, std::map<int,float> > > >::iterator it=sumESmeared_EE.begin(); it!=sumESmeared_EE.end(); ++it)             
    {   
      if(it->first != 0) continue;
      float calib = rechitItrEE_noMultifit->energy()/(*theEndcapEcalUncalibRecHits_noMultifit->find(rechitItrEE_noMultifit->id())).amplitude();
      float energy = 0.;
      float ratio = 1.; 
      if(it->first != 0) energy = calib*(*theEndcapEcalUncalibRecHits_noMultifit->find(rechitItrEE_noMultifit->id())).outOfTimeAmplitude(it->first+5);
      else energy = rechitItrEE_noMultifit->energy();
      if(sumESmeared_EE[it->first][EEid.ix()][EEid.iy()][EEid.zside()] == 0) continue;
      ratio = energy/sumESmeared_EE[it->first][EEid.ix()][EEid.iy()][EEid.zside()];
      p_ERatio_vs_bx_EE_noMultifit->Fill(it->first,ratio);
      p_ERatio_vs_Energy_EE_noMultifit[it->first]->Fill(sumESmeared_EE[it->first][EEid.ix()][EEid.iy()][EEid.zside()],ratio);
      p_RecoEnergy_vs_Energy_EE_noMultifit[it->first]->Fill(sumESmeared_EE[it->first][EEid.ix()][EEid.iy()][EEid.zside()],energy);
      h2_RecoEnergy_vs_Energy_EE_noMultifit[it->first]->Fill(sumESmeared_EE[it->first][EEid.ix()][EEid.iy()][EEid.zside()],energy);
      if(energy != 0.){
         h_Energy_EE_noMultifit->Fill(energy);
         h_RecoEnergy_vs_bx_EE_noMultifit[it->first]->Fill(energy);
      } 
    }

  }

  EcalRecHitCollection::const_iterator rechitItrEB;
  for(rechitItrEB = theBarrelEcalRecHits->begin () ;rechitItrEB != theBarrelEcalRecHits->end () ;++rechitItrEB)
  {
    EBDetId EBid(rechitItrEB->id());      
    if((*theBarrelEcalUncalibRecHits->find(rechitItrEB->id())).amplitude() == 0) continue;          
    for(std::map<int,std::map<int, std::map<int, std::map<int,float> > > >::iterator it=sumESmeared_EB.begin(); it!=sumESmeared_EB.end(); ++it)             
    {   
      if(it->first < -5 || it->first > 3) continue;
      float energy = 0.;
      float ratio = 1.; 
      float calib = rechitItrEB->energy()/(*theBarrelEcalUncalibRecHits->find(rechitItrEB->id())).amplitude();
      if(it->first != 0) energy = calib*(*theBarrelEcalUncalibRecHits->find(rechitItrEB->id())).outOfTimeAmplitude(it->first+5);
      else energy = rechitItrEB->energy();
      if(sumESmeared_EB[it->first][EBid.ieta()][EBid.iphi()][0] == 0) continue;
      ratio = energy/sumESmeared_EB[it->first][EBid.ieta()][EBid.iphi()][0];
      p_ERatio_vs_Energy_EB[it->first]->Fill(sumESmeared_EB[it->first][EBid.ieta()][EBid.iphi()][0],ratio);
      p_ERatio_vs_bx_EB->Fill(it->first,ratio);
      p_RecoEnergy_vs_Energy_EB[it->first]->Fill(sumESmeared_EB[it->first][EBid.ieta()][EBid.iphi()][0],energy);
      h2_RecoEnergy_vs_Energy_EB[it->first]->Fill(sumESmeared_EB[it->first][EBid.ieta()][EBid.iphi()][0],energy);
      if(energy != 0.){
         h_Energy_EB->Fill(energy);
         h_RecoEnergy_vs_bx_EB[it->first]->Fill(energy);
      } 
    }

  }

  EcalRecHitCollection::const_iterator rechitItrEE;
  for(rechitItrEE = theEndcapEcalRecHits->begin () ;rechitItrEE != theEndcapEcalRecHits->end () ;++rechitItrEE)
  {
    EEDetId EEid(rechitItrEE->id());    
    if((*theEndcapEcalUncalibRecHits->find(rechitItrEE->id())).amplitude() == 0) continue;          
    for(std::map<int,std::map<int, std::map<int, std::map<int,float> > > >::iterator it=sumESmeared_EE.begin(); it!=sumESmeared_EE.end(); ++it)             
    {   
      if(it->first < -5 || it->first > 3) continue;
      float energy = 0.;
      float ratio = 1.;   
      float calib = rechitItrEE->energy()/(*theEndcapEcalUncalibRecHits->find(rechitItrEE->id())).amplitude();
      if(it->first != 0) energy = calib*(*theEndcapEcalUncalibRecHits->find(rechitItrEE->id())).outOfTimeAmplitude(it->first+5);
      else energy = rechitItrEE->energy();
      if(sumESmeared_EE[it->first][EEid.ix()][EEid.iy()][EEid.zside()] == 0) continue;
      ratio = energy/sumESmeared_EE[it->first][EEid.ix()][EEid.iy()][EEid.zside()];
      p_ERatio_vs_bx_EE->Fill(it->first,ratio);
      p_ERatio_vs_Energy_EE[it->first]->Fill(sumESmeared_EE[it->first][EEid.ix()][EEid.iy()][EEid.zside()],ratio);
      p_RecoEnergy_vs_Energy_EE[it->first]->Fill(sumESmeared_EE[it->first][EEid.ix()][EEid.iy()][EEid.zside()],energy);
      h2_RecoEnergy_vs_Energy_EE[it->first]->Fill(sumESmeared_EE[it->first][EEid.ix()][EEid.iy()][EEid.zside()],energy);
      if(energy != 0.){
         h_Energy_EE->Fill(energy);
         h_RecoEnergy_vs_bx_EE[it->first]->Fill(energy);
      } 
    }

  }

  mapLaserCorr.clear();
  sumE_EB.clear(); 
  sumE_EE.clear(); 
  sumESmeared_EB.clear(); 
  sumESmeared_EE.clear(); 
  
}

// ------------ method called once each job just after ending the event loop  ------------
void MakeRecoGenComparison::endJob()
{

  h_nEvents->SetBinContent(h_nEvents->FindBin(0),naiveId_); 
  /*h2_Occupancy_EB->Scale(1./naiveId_);
  h2_Occupancy_EEP->Scale(1./naiveId_);
  h2_Occupancy_EEM->Scale(1./naiveId_);*/

}

// ----------additional functions-------------------
float MakeRecoGenComparison::doPhotonStatistics(float& npe){

  //smearing
  TRandom Poisson(0);
  int npeSmeared = Poisson.Poisson(npe);
  return float(npeSmeared);

}

//define this as a plug-in
DEFINE_FWK_MODULE(MakeRecoGenComparison);
