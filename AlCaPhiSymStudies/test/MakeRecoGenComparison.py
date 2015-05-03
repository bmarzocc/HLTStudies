import FWCore.ParameterSet.Config as cms
from Configuration.AlCa.GlobalTag import GlobalTag

process = cms.Process("MakeRecoGenComparison")

# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(1000)
process.MessageLogger.cerr = cms.untracked.PSet(threshold = cms.untracked.string("ERROR"))

# Geometry
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
process.load('Configuration.Geometry.GeometryExtended2015Reco_cff')
process.load("Geometry.CaloEventSetup.CaloTopology_cfi")
process.load("Geometry.CaloEventSetup.CaloGeometry_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")

process.load("HLTStudies.AlCaPhiSymStudies.MakeRecoGenComparison_cfi")

process.GlobalTag.globaltag = "MCRUN2_74_V5"
                                                                      
process.source = cms.Source("PoolSource",
#    skipEvents = cms.untracked.uint32(0),                       
    fileNames = cms.untracked.vstring(
    '/store/group/dpg_ecal/alca_ecalcalib/bmarzocc/Mixing_RECO/outputMixing_RECO_95.root'
    )
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.TFileService = cms.Service("TFileService",
    fileName = cms.string('OUTPUTFILENAME.root')
)

process.p = cms.Path(
      process.makeRecoGenComparison
    )


