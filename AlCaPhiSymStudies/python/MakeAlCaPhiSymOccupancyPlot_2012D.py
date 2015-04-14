import FWCore.ParameterSet.Config as cms

process = cms.PSet()

process.ioFiles = cms.PSet(

    outputFile = cms.string('/afs/cern.ch/work/b/bmarzocc/HLTStudies/CMSSW_5_3_23/src/HLTStudies/AlCaPhiSymStudies/output.root'),

    inputFiles = cms.vstring(
        "root://eoscms.cern.ch//store/group/dpg_ecal/alca_ecalcalib/bmarzocc/AlcaPhiStream/ZeroBiasParked/crab_Alca_EcalPhiSym_2012D/150413_210820/0000/outputALCAPHISYM_45.root",
    )
)
