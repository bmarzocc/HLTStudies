import FWCore.ParameterSet.Config as cms

process = cms.PSet()

process.ioFiles = cms.PSet(

    outputFile = cms.string('OUTPUT.root'),

    inputFiles = cms.vstring(
        [LISTOFFILES]
    )
)
