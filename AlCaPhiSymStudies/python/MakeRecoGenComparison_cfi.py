import FWCore.ParameterSet.Config as cms

makeRecoGenComparison = cms.EDAnalyzer("MakeRecoGenComparison",
    pcaloHits_EB                          = cms.InputTag("cfWriter","g4SimHitsEcalHitsEB"),
    pcaloHits_EE                          = cms.InputTag("cfWriter","g4SimHitsEcalHitsEE"),
    uncalibrecHitCollection_noMultifit_EB = cms.InputTag("ecalUncalibRecHit","EcalUncalibRecHitsEB"),
    uncalibrecHitCollection_noMultifit_EE = cms.InputTag("ecalUncalibRecHit","EcalUncalibRecHitsEE"),
    uncalibrecHitCollection_EB            = cms.InputTag("ecalMultiFitUncalibRecHit","EcalUncalibRecHitsEB"),
    uncalibrecHitCollection_EE            = cms.InputTag("ecalMultiFitUncalibRecHit","EcalUncalibRecHitsEE"),
    recHitCollection_noMultifit_EB        = cms.InputTag("ecalRechitNoMultifit","EcalRecHitsEB"),
    recHitCollection_noMultifit_EE        = cms.InputTag("ecalRechitNoMultifit","EcalRecHitsEE"),
    recHitCollection_EB                   = cms.InputTag("ecalRechitMultifit","EcalMultifitRecHitsEB"),
    recHitCollection_EE                   = cms.InputTag("ecalRechitMultifit","EcalMultifitRecHitsEE"),
)
