import sys,os
import math
import ROOT
from ROOT import *
from DataFormats.FWLite import Events, Handle

gSystem.Load("libFWCoreFWLite.so")
AutoLibraryLoader.enable()

file1=['output_PU20_25ns_noMultifit_noEtCut.root']
file2=['output_PU20_25ns_noEtCut.root']

h1 = TH1F("h1","ScEt",120,0.,40.)
h1.SetStats(0)
h2 = TH1F("h2","ScEt",120,0.,40.)
h2.SetStats(0)

eff1=0.
nEgammas=0
nEgammas_sc7=0

for i in xrange(len(file1)):

  events = Events (file1[i])
  print file1[i]

  for event in events:

      handle = Handle('std::vector<reco::RecoEcalCandidate>')
      event.getByLabel('hltEgammaCandidatesUnseeded','',handle);
      egammas = handle.product()
      
      for egamma in egammas:    
          nEgammas=nEgammas+1
          if egamma.et() > 7.:
              nEgammas_sc7=nEgammas_sc7+1
          h1.Fill(egamma.et())

eff1=float(nEgammas_sc7)/float(nEgammas)
eff_err1=math.sqrt( float(eff1)*(1-float(eff1))/float(nEgammas) )*100.
eff1=eff1*100.

eff2=0.
nEgammas=0
nEgammas_sc7=0

for i in xrange(len(file2)):

  events = Events (file2[i])
  print file2[i]

  for event in events:

      handle = Handle('std::vector<reco::RecoEcalCandidate>')
      event.getByLabel('hltEgammaCandidatesUnseeded','',handle);
      egammas = handle.product()

      for egamma in egammas:    
          nEgammas=nEgammas+1
          if egamma.et() > 7.:
              nEgammas_sc7=nEgammas_sc7+1
          h2.Fill(egamma.et()) 

eff2=float(nEgammas_sc7)/float(nEgammas)
eff_err2=math.sqrt( float(eff2)*(1-float(eff2))/float(nEgammas) )*100.
eff2=eff2*100.

h1.Scale(1./h1.Integral())
h2.Scale(1./h2.Integral())

c1 = TCanvas()

h1.GetXaxis().SetTitle("pt (GeV)")
#h1.GetYaxis().SetRangeUser(0.99,1.2)

h1.SetMarkerColor(kBlack)
h1.SetLineColor(kBlack)
h1.SetMarkerStyle(20)
h1.SetMarkerSize(0.5)
h2.SetMarkerColor(kRed)
h2.SetLineColor(kRed)
h2.SetMarkerStyle(20)
h2.SetMarkerSize(0.5)

h1.Draw("")
h2.Draw("same")

legend = TLegend(0.62, 0.82, 0.89, 0.94);
legend.SetFillColor(kWhite);
legend.SetFillStyle(1000);
legend.SetLineWidth(0);
legend.SetLineColor(kWhite);
legend.SetTextFont(42);
legend.SetTextSize(0.03);
legend.AddEntry(h1,"PU20_25ns_noMultifit_noEtCut","L");
legend.AddEntry(h2,"PU20_25ns_noEtCut","L");
legend.Draw("same");

latex1 = TLatex(0.62, 0.78, "eff_SC7 = "+str("%.2f" % eff1)+"#pm"+str("%.2f" % eff_err1)+" %");
latex1.SetNDC();
latex1.SetTextColor(kBlack);
latex1.SetTextSize(0.04);
latex1.Draw("same");

latex2 = TLatex(0.62, 0.73, "eff_SC7 = "+str("%.2f" % eff2)+"#pm"+str("%.2f" % eff_err2)+" %");
latex2.SetNDC();
latex2.SetTextColor(kRed);
latex2.SetTextSize(0.04);
latex2.Draw("same");

c1.Update()
c1.Print("ScEt_PU20_25ns.pdf");
c1.Print("ScEt_PU20_25ns.png");


