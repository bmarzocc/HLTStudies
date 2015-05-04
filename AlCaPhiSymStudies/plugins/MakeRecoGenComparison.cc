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
  p_ERatio_vs_bx_EB            = fs->make<TProfile>("p_ERatio_vs_bx_EB","p_ERatio_vs_bx_EB",18,-13,5,-10000000.,10000000.);
  p_ERatio_vs_bx_EE            = fs->make<TProfile>("p_ERatio_vs_bx_EE","p_ERatio_vs_bx_EE",18,-13,5,-10000000.,10000000.);
  h_nEvents                    = fs->make<TH1F>("h_nEvents","h_nEvents",3,-1,2);
  for(int ii = -12; ii < 4;ii++){
    std::ostringstream t1;
    std::ostringstream t2;
    std::ostringstream t3;
    std::ostringstream t4;
    t1 << "p_ERatio_vs_Energy_EB_noMultifit_" << ii;
    p_ERatio_vs_Energy_EB_noMultifit[ii] = fs->make<TProfile>(t1.str().c_str(),t1.str().c_str(),200,0.,2.,-10000000.,10000000);
    t2 << "p_ERatio_vs_Energy_EB_" << ii;
    p_ERatio_vs_Energy_EB[ii] = fs->make<TProfile>(t2.str().c_str(),t2.str().c_str(),200,0.,2.,-10000000.,10000000);
    t3 << "p_ERatio_vs_Energy_EE_noMultifit_" << ii;
    p_ERatio_vs_Energy_EE_noMultifit[ii] = fs->make<TProfile>(t3.str().c_str(),t3.str().c_str(),200,0.,2.,-10000000.,10000000);
    t4 << "p_ERatio_vs_Energy_EE_" << ii;
    p_ERatio_vs_Energy_EE[ii] = fs->make<TProfile>(t4.str().c_str(),t4.str().c_str(),200,0.,2.,-10000000.,10000000);
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

  //rechitsEB_noMultifit
  edm::Handle<EcalRecHitCollection> recHitsEE_noMultifit;
  ev.getByLabel( recHitCollection_noMultifit_EE_, recHitsEE_noMultifit );
  const EcalRecHitCollection* theEndcapEcalRecHits_noMultifit = recHitsEE_noMultifit.product () ;
  if ( ! recHitsEE_noMultifit.isValid() ) {
    std::cerr << "EcalValidation::analyze --> recHitsEE_noMultifit not found" << std::endl; 
  }


  //rechitsEB_noMultifit
  edm::Handle<EcalRecHitCollection> recHitsEB;
  ev.getByLabel( recHitCollection_EB_, recHitsEB );
  const EcalRecHitCollection* theBarrelEcalRecHits = recHitsEB.product () ;
  if ( ! recHitsEB.isValid() ) {
    std::cerr << "EcalValidation::analyze --> recHitsEB not found" << std::endl; 
  }

  //rechitsEE_noMultifit
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
    if(EEid.zside() > 0) sumE_EE[(*pcaloHitItrEE)->eventId().bunchCrossing()][EEid.ix()][EEid.iy()][1] = sumE_EE[(*pcaloHitItrEE)->eventId().bunchCrossing()][EEid.ix()][EEid.iy()][1] + (*pcaloHitItrEE)->energy();
    else if(EEid.zside() < 0) sumE_EE[(*pcaloHitItrEE)->eventId().bunchCrossing()][EEid.ix()][EEid.iy()][-1] = sumE_EE[(*pcaloHitItrEE)->eventId().bunchCrossing()][EEid.ix()][EEid.iy()][-1] + (*pcaloHitItrEE)->energy();

    float seedLaserCorrection = theLaser->getLaserCorrection(EEid, evtTimeStamp);
    if(EEid.zside() > 0) mapLaserCorr[EEid.ix()][EEid.iy()][1] = seedLaserCorrection;
    else if(EEid.zside() < 0) mapLaserCorr[EEid.ix()][EEid.iy()][-1] = seedLaserCorrection;
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
                 
            //std::cout << bx << " " << npe << " " << npeSmeared << std::endl;
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
        } 

  //Fill ratio plots
  EcalRecHitCollection::const_iterator rechitItrEB_noMultifit;
  for(rechitItrEB_noMultifit = theBarrelEcalRecHits_noMultifit->begin () ;rechitItrEB_noMultifit != theBarrelEcalRecHits_noMultifit->end () ;++rechitItrEB_noMultifit)
  {
    EBDetId EBid(rechitItrEB_noMultifit->id());
    float calib = rechitItrEB_noMultifit->energy()/(*theBarrelEcalUncalibRecHits_noMultifit->find(rechitItrEB_noMultifit->id())).amplitude();
    float energy = rechitItrEB_noMultifit->energy();
    float ratio = 1.;       
    if((*theBarrelEcalUncalibRecHits_noMultifit->find(rechitItrEB_noMultifit->id())).amplitude() == 0) continue;          
    for(std::map<int,std::map<int, std::map<int, std::map<int,float> > > >::iterator it=sumESmeared_EB.begin(); it!=sumESmeared_EB.end(); ++it)             
    {   
      if(it->first != 0) energy = calib*(*theBarrelEcalUncalibRecHits_noMultifit->find(rechitItrEB_noMultifit->id())).outOfTimeAmplitude(it->first);
      ratio = energy/sumESmeared_EB[it->first][EBid.ieta()][EBid.iphi()][0];
      if(sumESmeared_EB[it->first][EBid.ieta()][EBid.iphi()][0] == 0) continue;
      p_ERatio_vs_Energy_EB_noMultifit[it->first]->Fill(sumESmeared_EB[it->first][EBid.ieta()][EBid.iphi()][0],ratio);
      p_ERatio_vs_bx_EB_noMultifit->Fill(it->first,ratio);
      /*if(sumESmeared_EB[it->first][EBid.ieta()][EBid.iphi()][0] < 0.02 && it->first == 0) 
         std::cout << EBid.ieta() << " " << EBid.iphi() << " " << sumESmeared_EB[it->first][EBid.ieta()][EBid.iphi()][0] << " " << energy << std::endl;*/
    }

  }

  EcalRecHitCollection::const_iterator rechitItrEE_noMultifit;
  for(rechitItrEE_noMultifit = theEndcapEcalRecHits_noMultifit->begin () ;rechitItrEE_noMultifit != theEndcapEcalRecHits_noMultifit->end () ;++rechitItrEE_noMultifit)
  {
    EEDetId EEid(rechitItrEE_noMultifit->id());
    float calib = rechitItrEE_noMultifit->energy()/(*theEndcapEcalUncalibRecHits_noMultifit->find(rechitItrEE_noMultifit->id())).amplitude();
    float energy = rechitItrEE_noMultifit->energy();
    float ratio = 1.;       
    if((*theEndcapEcalUncalibRecHits_noMultifit->find(rechitItrEE_noMultifit->id())).amplitude() == 0) continue;          
    for(std::map<int,std::map<int, std::map<int, std::map<int,float> > > >::iterator it=sumESmeared_EE.begin(); it!=sumESmeared_EE.end(); ++it)             
    {   
      if(it->first != 0) energy = calib*(*theEndcapEcalUncalibRecHits_noMultifit->find(rechitItrEE_noMultifit->id())).outOfTimeAmplitude(it->first);
      if(EEid.zside() > 0){
         if(sumESmeared_EE[it->first][EEid.ix()][EEid.iy()][1] == 0) continue;
         ratio = energy/sumESmeared_EE[it->first][EEid.ix()][EEid.iy()][1];
         p_ERatio_vs_bx_EE_noMultifit->Fill(it->first,ratio);
         p_ERatio_vs_Energy_EE_noMultifit[it->first]->Fill(sumESmeared_EE[it->first][EEid.ix()][EEid.iy()][1],ratio);
      }
      else if(EEid.zside() < 0){
         if(sumESmeared_EE[it->first][EEid.ix()][EEid.iy()][-1] == 0) continue;
         ratio = energy/sumESmeared_EE[it->first][EEid.ix()][EEid.iy()][-1];
         p_ERatio_vs_bx_EE_noMultifit->Fill(it->first,ratio);
         p_ERatio_vs_Energy_EE_noMultifit[it->first]->Fill(sumESmeared_EE[it->first][EEid.ix()][EEid.iy()][-1],ratio);
      } 
    }

  }

  EcalRecHitCollection::const_iterator rechitItrEB;
  for(rechitItrEB = theBarrelEcalRecHits->begin () ;rechitItrEB != theBarrelEcalRecHits->end () ;++rechitItrEB)
  {
    EBDetId EBid(rechitItrEB->id());
    float calib = rechitItrEB->energy()/(*theBarrelEcalUncalibRecHits->find(rechitItrEB->id())).amplitude();
    float energy = rechitItrEB->energy();
    float ratio = 1.;       
    if((*theBarrelEcalUncalibRecHits->find(rechitItrEB->id())).amplitude() == 0) continue;          
    for(std::map<int,std::map<int, std::map<int, std::map<int,float> > > >::iterator it=sumESmeared_EB.begin(); it!=sumESmeared_EB.end(); ++it)             
    {   
      if(it->first != 0) energy = calib*(*theBarrelEcalUncalibRecHits->find(rechitItrEB->id())).outOfTimeAmplitude(it->first);
      ratio = energy/sumESmeared_EB[it->first][EBid.ieta()][EBid.iphi()][0];
      if(sumESmeared_EB[it->first][EBid.ieta()][EBid.iphi()][0] == 0) continue;
      p_ERatio_vs_Energy_EB[it->first]->Fill(sumESmeared_EB[it->first][EBid.ieta()][EBid.iphi()][0],ratio);
      p_ERatio_vs_bx_EB->Fill(it->first,ratio);
    }

  }

  EcalRecHitCollection::const_iterator rechitItrEE;
  for(rechitItrEE = theEndcapEcalRecHits->begin () ;rechitItrEE != theEndcapEcalRecHits->end () ;++rechitItrEE)
  {
    EEDetId EEid(rechitItrEE->id());
    float calib = rechitItrEE->energy()/(*theEndcapEcalUncalibRecHits->find(rechitItrEE->id())).amplitude();
    float energy = rechitItrEE->energy();
    float ratio = 1.;       
    if((*theEndcapEcalUncalibRecHits->find(rechitItrEE->id())).amplitude() == 0) continue;          
    for(std::map<int,std::map<int, std::map<int, std::map<int,float> > > >::iterator it=sumESmeared_EE.begin(); it!=sumESmeared_EE.end(); ++it)             
    {   
      if(it->first != 0) energy = calib*(*theEndcapEcalUncalibRecHits->find(rechitItrEE->id())).outOfTimeAmplitude(it->first);
      if(EEid.zside() > 0){
         if(sumESmeared_EE[it->first][EEid.ix()][EEid.iy()][1] == 0) continue;
         ratio = energy/sumESmeared_EE[it->first][EEid.ix()][EEid.iy()][1];
         p_ERatio_vs_bx_EE->Fill(it->first,ratio);
         p_ERatio_vs_Energy_EE[it->first]->Fill(sumESmeared_EE[it->first][EEid.ix()][EEid.iy()][1],ratio);
      }
      else if(EEid.zside() < 0){
         if(sumESmeared_EE[it->first][EEid.ix()][EEid.iy()][-1] == 0) continue;
         ratio = energy/sumESmeared_EE[it->first][EEid.ix()][EEid.iy()][-1];
         p_ERatio_vs_bx_EE->Fill(it->first,ratio);
         p_ERatio_vs_Energy_EE[it->first]->Fill(sumESmeared_EE[it->first][EEid.ix()][EEid.iy()][-1],ratio);
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

}

// ----------additional functions-------------------
float MakeRecoGenComparison::doPhotonStatistics(float& npe){

  //smearing
  TRandom Poisson(0);
  int npeSmeared = Poisson.Poisson(int(npe));
  return float(npeSmeared);

}

//define this as a plug-in
DEFINE_FWK_MODULE(MakeRecoGenComparison);
