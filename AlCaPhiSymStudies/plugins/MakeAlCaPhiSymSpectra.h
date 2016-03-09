#ifndef MakeAlCaPhiSymSpectra_h
#define MakeAlCaPhiSymSpectra_h

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

#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "Calibration/Tools/interface/EcalRingCalibrationTools.h"

class MakeAlCaPhiSymSpectra :  public edm::EDAnalyzer
{

 public:

  /// Constructor
  MakeAlCaPhiSymSpectra( const edm::ParameterSet& iConfig );
  
  /// Destructor
  ~MakeAlCaPhiSymSpectra();

 private:

  virtual void beginJob() ;
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob() ;

  // ----------additional functions------------------
  // nothing

  // ----------member data ---------------------------
  edm::EDGetTokenT<EcalRecHitCollection> recHitCollection_EB_;
  edm::EDGetTokenT<EcalRecHitCollection> recHitCollection_EE_;

  const int EB_rings = 85;
  const int EE_rings = 39;

  float ebCut_ADC = 13.;
  float eeCut_ADC = 20.;

  float enMin;
  float enMax;
  float calMin;
  float calMax;

  int nBins;
  int naiveId_;

  std::map<int,std::map<int,std::map<int,TH1F*> > > EBmap;
  std::map<int,std::map<int,std::map<int,TH1F*> > > EEmap_energy;
  std::map<int,std::map<int,std::map<int,TH1F*> > > EEmap_calib;
 
  // ------------- HISTOGRAMS ------------------------------------
  TH1F* h_nEvents;

  TH2F* h2_hitOccupancy_EB_highCut1;
  TH2F* h2_hitOccupancy_EB_highCut2;
  TH2F* h2_hitOccupancy_EEM_highCut1;
  TH2F* h2_hitOccupancy_EEM_highCut2;
  TH2F* h2_hitOccupancy_EEP_highCut1;
  TH2F* h2_hitOccupancy_EEP_highCut2;

  TH2F* h2_calib_EB;
  TH2F* h2_LC_EB;
  TH2F* h2_IC_EB;
  TH2F* h2_calib_EEM;
  TH2F* h2_LC_EEM;
  TH2F* h2_IC_EEM;
  TH2F* h2_calib_EEP;
  TH2F* h2_LC_EEP;
  TH2F* h2_IC_EEP;

  TH1F* EEM_ix66_iy26_eSpectrum;
  TH1F* EEM_ix54_iy25_eSpectrum;
  TH1F* EEM_ix100_iy57_eSpectrum;
  
  std::vector<TH1F*> EBM_eSpectrum_histos;
  std::vector<TH1F*> EBP_eSpectrum_histos;
  std::vector<TH1F*> EEM_eSpectrum_histos;
  std::vector<TH1F*> EEP_eSpectrum_histos;

  std::vector<TH1F*> EBM_etSpectrum_histos;
  std::vector<TH1F*> EBP_etSpectrum_histos;
  std::vector<TH1F*> EEM_etSpectrum_histos;
  std::vector<TH1F*> EEP_etSpectrum_histos;

  std::vector<TH1F*> EBM_calibration_histos;
  std::vector<TH1F*> EBP_calibration_histos;
  std::vector<TH1F*> EEM_calibration_histos;
  std::vector<TH1F*> EEP_calibration_histos;

};

#endif
