#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <iterator>
#include <algorithm>

#include "TSystem.h"
#include "TFile.h"
#include "TTree.h"
#include "TH2F.h"

#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"
#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/FWLite/interface/InputSource.h"
#include "DataFormats/FWLite/interface/OutputFiles.h"
#include "PhysicsTools/FWLite/interface/TFileService.h"

#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"

#include "PhysicsTools/Utilities/macros/setTDRStyle.C"

using namespace std;

//**********MAIN**************************************************************************
int main( int argc, char *argv[] )
{
    setTDRStyle();
    gStyle->SetPadTickX( 1 );
    gStyle->SetPadTickY( 1 );
    gStyle->SetOptTitle( 0 );
    gStyle->SetOptFit( 1 );

    gSystem->Load( "libFWCoreFWLite" );
    AutoLibraryLoader::enable();

    if( argc < 2 ) {
        cout << "Usage : " << argv[0] << " [parameters.py]" << endl;
        return 0;
    }
    if( !edm::readPSetsFrom( argv[1] )->existsAs<edm::ParameterSet>( "process" ) ) {
        cout << " ERROR: ParametersSet 'process' is missing in your configuration file"
             << endl;
        return 0;
    }

    //-----get the python configuration-----
    const edm::ParameterSet &process = edm::readPSetsFrom( argv[1] )->getParameter<edm::ParameterSet>( "process" );
    const edm::ParameterSet &filesOpt = process.getParameter<edm::ParameterSet>( "ioFiles" );

    //---io files option---
    vector<string> filesList = filesOpt.getParameter<vector<string> >("inputFiles"); 
    std::cout << filesOpt.getParameter<string>("outputFile").c_str() << std::endl;
    TFile* outFile = new TFile(filesOpt.getParameter<string>("outputFile").c_str(), "recreate");
    
    TH2F* h2_hitOccupancy_EB = new TH2F("h2_hitOccupancy_EB","h2_hitOccupancy_EB",360,0,360,170,-85,85);
    TH2F* h2_hitOccupancy_EEP = new TH2F("h2_hitOccupancy_EEP","h2_hitOccupancy_EEP",100,1,101,100,1,101);
    TH2F* h2_hitOccupancy_EEM = new TH2F("h2_hitOccupancy_EEM","h2_hitOccupancy_EEM",100,1,101,100,1,101);
    TH1F* h_nEvents = new TH1F("h_nEvents","h_nEvents",3,-1,2);

    int iEvent=0;
    for(unsigned int iFile=0; iFile<filesList.size(); iFile++)
    {
        TFile* inFile = TFile::Open(filesList.at(iFile).c_str());
	std::cout << " >>> " << filesList.at(iFile) << std::endl;

        //---FWLite interfaces---
        fwlite::Event event(inFile);
        fwlite::Handle<EcalRecHitCollection> ebRechitHandle;
        fwlite::Handle<EcalRecHitCollection> eeRechitHandle;

        //---events loop---
        for(event.toBegin(); !event.atEnd(); ++event)
        {
            iEvent++;        
    
            //--get EBrechits--
            ebRechitHandle.getByLabel(event,"hltAlCaPhiSymUncalibrator","phiSymEcalRecHitsEB","TEST");  
            ebRechitHandle.product(); 
         
            for(EcalRecHitCollection::const_iterator rechitItr = ebRechitHandle->begin () ;rechitItr != ebRechitHandle->end () ;++rechitItr)
            {
                EBDetId id_crystal(rechitItr->id());
                h2_hitOccupancy_EB->Fill(id_crystal.iphi(),id_crystal.ieta());  
            }

            //--get EErechits--
            eeRechitHandle.getByLabel(event,"hltAlCaPhiSymUncalibrator","phiSymEcalRecHitsEE","TEST");  
            eeRechitHandle.product(); 
         
            for(EcalRecHitCollection::const_iterator rechitItr = eeRechitHandle->begin () ;rechitItr != eeRechitHandle->end () ;++rechitItr)
            {
                EEDetId id_crystal(rechitItr->id());
                 
                if(id_crystal.zside() > 0)
                   h2_hitOccupancy_EEP->Fill(id_crystal.ix(),id_crystal.iy());
                if(id_crystal.zside() < 0)
                   h2_hitOccupancy_EEM->Fill(id_crystal.ix(),id_crystal.iy());
            }
        }
    }  
    
    std::cout << "nEvents = " << iEvent << std::endl;
    
    h_nEvents->SetBinContent(h_nEvents->FindBin(0),iEvent);
    
    outFile->cd();

    h2_hitOccupancy_EB->Write();
    h2_hitOccupancy_EEP->Write();
    h2_hitOccupancy_EEM->Write();
    h_nEvents->Write();

    outFile->Close();
}

