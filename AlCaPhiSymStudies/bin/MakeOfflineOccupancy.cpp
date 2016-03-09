// C++ code for offline thresholds studies
// to compile: c++ -o MakeOfflineOccupancy `root-config --cflags --glibs` MakeOfflineOccupancy.cpp TEndcapRings.cc
// to run: ./MakeOfflineOccupancy Occupancy_output_PU50bx25.root 

using namespace std;

#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <map>
#include <vector>
#include "TFile.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TF1.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TMath.h"
#include "TH2F.h"
#include "TROOT.h"
#include "TTree.h"
#include "TStyle.h"
#include "TMultiGraph.h"
#include "TPaveStats.h"

#include "TEndcapRings.h"

void drawPlot(TH2F* h, std::string Title, float Zmin, float Zmax);

int main(int argc, char** argv)
{

  //lists of input files
  char* inputFILE = argv[1];
  
  std::string inputFile = std::string(inputFILE);
  
  std::cout << "inputFile  = " << inputFile << std::endl;

  TH2F* h2_calibTime_EB_highCut1 = new TH2F ("h2_calibTime_EB_highCut1","h2_calibTime_EB_highCut1",170,-85,85,180,0,45);
  TH2F* h2_calibTime_EB_highCut2 = new TH2F ("h2_calibTime_EB_highCut2","h2_calibTime_EB_highCut2",170,-85,85,180,0,45);
  TH2F* h2_calibTime_EEM_highCut1 = new TH2F ("h2_calibTime_EEM_highCut1","h2_calibTime_EEM_highCut1",39,0,39,180,0,45);
  TH2F* h2_calibTime_EEM_highCut2 = new TH2F ("h2_calibTime_EEM_highCut2","h2_calibTime_EEM_highCut2",39,0,39,180,0,45);
  TH2F* h2_calibTime_EEP_highCut1 = new TH2F ("h2_calibTime_EEP_highCut1","h2_calibTime_EEP_highCut1",39,0,39,180,0,45);
  TH2F* h2_calibTime_EEP_highCut2 = new TH2F ("h2_calibTime_EEP_highCut2","h2_calibTime_EEP_highCut2",39,0,39,180,0,45);
  
  TFile* inFile = TFile::Open(inputFile.c_str());

  TH1F* h_nEvents = (TH1F*)inFile->Get("makeAlCaPhiSymSpectra/h_nEvents");
  TH2F* h2_hitOccupancy_EB_highCut1 = (TH2F*)inFile->Get("makeAlCaPhiSymSpectra/h2_hitOccupancy_EB_highCut1");
  TH2F* h2_hitOccupancy_EB_highCut2 = (TH2F*)inFile->Get("makeAlCaPhiSymSpectra/h2_hitOccupancy_EB_highCut2");
  TH2F* h2_hitOccupancy_EEM_highCut1 = (TH2F*)inFile->Get("makeAlCaPhiSymSpectra/h2_hitOccupancy_EEM_highCut1");
  TH2F* h2_hitOccupancy_EEM_highCut2 = (TH2F*)inFile->Get("makeAlCaPhiSymSpectra/h2_hitOccupancy_EEM_highCut2");
  TH2F* h2_hitOccupancy_EEP_highCut1 = (TH2F*)inFile->Get("makeAlCaPhiSymSpectra/h2_hitOccupancy_EEP_highCut1");
  TH2F* h2_hitOccupancy_EEP_highCut2 = (TH2F*)inFile->Get("makeAlCaPhiSymSpectra/h2_hitOccupancy_EEP_highCut2");

  int Nevents = h_nEvents->GetBinContent(2);

  h2_hitOccupancy_EB_highCut1->Scale(1./float(Nevents));
  h2_hitOccupancy_EB_highCut2->Scale(1./float(Nevents));
  h2_hitOccupancy_EEM_highCut1->Scale(1./float(Nevents));
  h2_hitOccupancy_EEM_highCut2->Scale(1./float(Nevents));
  h2_hitOccupancy_EEP_highCut1->Scale(1./float(Nevents));
  h2_hitOccupancy_EEP_highCut2->Scale(1./float(Nevents));

  TEndcapRings *eRings = new TEndcapRings(); 

  for(int ii=1; ii<=h2_hitOccupancy_EB_highCut1->GetNbinsY(); ii++)
      for(int jj=1; jj<=h2_hitOccupancy_EB_highCut1->GetNbinsX(); jj++)
      {
          float time = 30000/(h2_hitOccupancy_EB_highCut1->GetBinContent(jj,ii)*1500*3600);
          h2_calibTime_EB_highCut1->Fill(h2_calibTime_EB_highCut1->GetXaxis()->GetBinCenter(ii),time);

          time = 30000/(h2_hitOccupancy_EB_highCut2->GetBinContent(jj,ii)*1500*3600);
          h2_calibTime_EB_highCut2->Fill(h2_calibTime_EB_highCut2->GetXaxis()->GetBinCenter(ii),time);
      }

  for(int ii=1; ii<=h2_hitOccupancy_EEM_highCut1->GetNbinsX(); ii++)
      for(int jj=1; jj<=h2_hitOccupancy_EEM_highCut1->GetNbinsY(); jj++)
      {
          float iring = eRings->GetEndcapRing(ii,jj,-1); 
          float time = 30000/(h2_hitOccupancy_EEM_highCut1->GetBinContent(ii,jj)*1500*3600);
          h2_calibTime_EEM_highCut1->Fill(iring,time);           
          time = 30000/(h2_hitOccupancy_EEM_highCut2->GetBinContent(ii,jj)*1500*3600);
          h2_calibTime_EEM_highCut2->Fill(iring,time);

          iring = eRings->GetEndcapRing(ii,jj,1); 
          time = 30000/(h2_hitOccupancy_EEP_highCut1->GetBinContent(ii,jj)*1500*3600);
          h2_calibTime_EEP_highCut1->Fill(iring,time);           
          time = 30000/(h2_hitOccupancy_EEP_highCut2->GetBinContent(ii,jj)*1500*3600);
          h2_calibTime_EEP_highCut2->Fill(iring,time);
        
      }

  drawPlot(h2_calibTime_EB_highCut1,std::string("calibTime_EB_highCut1_PU50bx25_30kHitsperCrl_1500Hz"),0,30); 
  //drawPlot(h2_calibTime_EB_highCut2,std::string("calibTime_EB_highCut2_PU50bx25"),0,60); 
  drawPlot(h2_calibTime_EEM_highCut1,std::string("calibTime_EEM_highCut1_PU50bx25_30kHitsperCrl_1500Hz"),0,80); 
  //drawPlot(h2_calibTime_EEM_highCut2,std::string("calibTime_EEM_highCut2_PU50bx25"),0,60);     
  drawPlot(h2_calibTime_EEP_highCut1,std::string("calibTime_EEP_highCut1_PU50bx25_30kHitsperCrl_1500Hz"),0,80); 
  //drawPlot(h2_calibTime_EEP_highCut2,std::string("calibTime_EEP_highCut2_PU50bx25"),0,60);      
}

void drawPlot(TH2F* h, std::string Title, float Zmin, float Zmax) 
{
    
  gStyle -> SetOptFit (00111);
  gStyle -> SetOptStat ("");
  gStyle -> SetStatX (.90);
  gStyle -> SetStatY (.90);
  gStyle -> SetStatW (.15);
  gStyle->SetPalette(55);
     
  h -> SetTitle(Title.c_str());
  h -> GetXaxis() -> SetLabelSize(0.04);
  h -> GetYaxis() -> SetLabelSize(0.04);
  h -> GetXaxis() -> SetTitleSize(0.05);
  h -> GetYaxis() -> SetTitleSize(0.05);
  h -> GetYaxis() -> SetTitleOffset(1.);
 
  h -> GetXaxis() -> SetTitle("iRing");
  h -> GetYaxis() -> SetTitle("Time (h)");

  h -> GetZaxis() -> SetRangeUser(Zmin,Zmax);
  h->SetContour(10000);

  TCanvas* c1 = new TCanvas("c1","c1");
  c1 -> cd();
  h->Draw("colz");
  c1 -> Print((Title+".png").c_str(),"png");
  c1 -> Print((Title+".pdf").c_str(),"pdf");
    
  delete c1;

}

