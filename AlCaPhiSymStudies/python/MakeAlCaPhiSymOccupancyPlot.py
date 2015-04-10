import FWCore.ParameterSet.Config as cms

process = cms.PSet()

process.ioFiles = cms.PSet(

    outputFile = cms.string('/afs/cern.ch/work/b/bmarzocc/HLTStudies/CMSSW_7_4_0_pre9/src/HLTStudies/AlCaPhiSymStudies/output.root'),

    inputFiles = cms.vstring(
        "root://eoscms.cern.ch//store/group/dpg_ecal/alca_ecalcalib/bmarzocc/AlcaPhiStream/OccupancyStudies/Neutrino_Pt-2to20_gun/crab_Alca_EcalPhiSym_PU20_25ns_eb3_ee5/150409_214223/0000/outputALCAPHISYM_99.root",
    )
)
