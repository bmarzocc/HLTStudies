#ifndef MakeRecoGenComparison_h
#define MakeRecoGenComparison_h

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
 #include <fstream>
#include <iterator>
#include <algorithm>

#include "TSystem.h"
#include "TFile.h"
#include "TTree.h"
#include "TH2F.h"
#include "TRandom.h"
#include "TProfile.h"

// Framework
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ProducerBase.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"


#include "PhysicsTools/Utilities/macros/setTDRStyle.C"

class MakeRecoGenComparison :  public edm::EDAnalyzer
{

 public:

  /// Constructor
  MakeRecoGenComparison( const edm::ParameterSet& iConfig );
  
  /// Destructor
  ~MakeRecoGenComparison();

 private:

  virtual void beginJob() ;
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob() ;

  // ----------additional functions------------------
  float doPhotonStatistics(float&);

  // ----------member data ---------------------------
  edm::InputTag pcaloHits_EB_;
  edm::InputTag pcaloHits_EE_;
  edm::InputTag uncalibrecHitCollection_noMultifit_EB_;
  edm::InputTag uncalibrecHitCollection_noMultifit_EE_;
  edm::InputTag uncalibrecHitCollection_EB_;
  edm::InputTag uncalibrecHitCollection_EE_;
  edm::InputTag recHitCollection_noMultifit_EB_;
  edm::InputTag recHitCollection_noMultifit_EE_;
  edm::InputTag recHitCollection_EB_;
  edm::InputTag recHitCollection_EE_;

  const float simHitToPhotoelectronsEB = 2250.0;
  const float simHitToPhotoelectronsEE = 1800.0;
  int naiveId_;      

  std::map<int,std::map<int,std::map<int,float> > > mapLaserCorr;
  std::map<int,std::map<int,std::map<int,std::map<int,float> > > > sumE_EB; 
  std::map<int,std::map<int,std::map<int,std::map<int,float> > > > sumE_EE; 
  std::map<int,std::map<int,std::map<int,std::map<int,float> > > > sumESmeared_EB; 
  std::map<int,std::map<int,std::map<int,std::map<int,float> > > > sumESmeared_EE; 

  // ------------- HISTOGRAMS ------------------------------------
  TProfile* p_ERatio_vs_bx_EB_noMultifit;
  TProfile* p_ERatio_vs_bx_EE_noMultifit;
  TProfile* p_ERatio_vs_bx_EB;
  TProfile* p_ERatio_vs_bx_EE;
  std::map<int,TProfile* > p_ERatio_vs_Energy_EB_noMultifit;
  std::map<int,TProfile* > p_ERatio_vs_Energy_EB;
  std::map<int,TProfile* > p_ERatio_vs_Energy_EE_noMultifit;
  std::map<int,TProfile* > p_ERatio_vs_Energy_EE;
  TH1F* h_nEvents;
};

#endif
