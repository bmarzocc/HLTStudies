import FWCore.ParameterSet.Config as cms

makeAlCaPhiSymSpectra = cms.EDAnalyzer("MakeAlCaPhiSymSpectra",
    recHitCollection_EB                   = cms.InputTag("ecalRecHit","EcalRecHitsEB"),
    recHitCollection_EE                   = cms.InputTag("ecalRecHit","EcalRecHitsEE"),
)
