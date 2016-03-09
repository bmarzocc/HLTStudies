import FWCore.ParameterSet.Config as cms
from Configuration.AlCa.GlobalTag import GlobalTag

process = cms.Process("MakeAlCaPhiSymSpectra")

# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(100)
process.MessageLogger.cerr = cms.untracked.PSet(threshold = cms.untracked.string("ERROR"))

# Geometry
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
process.load('Configuration.Geometry.GeometryExtended2015Reco_cff')
process.load("Geometry.CaloEventSetup.CaloTopology_cfi")
process.load("Geometry.CaloEventSetup.CaloGeometry_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")

process.load("HLTStudies.AlCaPhiSymStudies.MakeAlCaPhiSymSpectra_cfi")

process.GlobalTag.globaltag = "80X_mcRun2_asymptotic_v0"
                                                                      
process.source = cms.Source("PoolSource",
#    skipEvents = cms.untracked.uint32(0),                       
    fileNames = cms.untracked.vstring(
    '/store/user/bmarzocc/AlCaPhiSym_MinBias_13TeV_PU50bx25_80X_mcRun2_asymptotic_v0_RECO/outputALCAPHISYM_80X_mcRun2_asymptotic_v0_RECO_9.root'

    )
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.TFileService = cms.Service("TFileService",
    fileName = cms.string('MakeAlCaPhiSymSpectra_output.root')
)

process.p = cms.Path(
      process.makeAlCaPhiSymSpectra
    )

