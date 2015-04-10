#!/usr/bin/python
import string, sys, os, getopt, subprocess, time, shutil
import math
import numpy

def usage():
    print "Usage: computeEfficiency.py --dir=[dir]"
    
try:
     opts, args = getopt.getopt(sys.argv[1:], "d:", ["dir="])

except getopt.GetoptError:
     #* print help information and exit:*
     usage()
     sys.exit(2)

for opt, arg in opts:
    
     if opt in ("--dir"):
        dir = arg

print "Dir = ",dir

file = dir+"inputFiles.txt"

numTotal = 0
numPass = 0

with open(file) as data:
    for line in data:
      fileEfftmp = dir+line
      fileEff = fileEfftmp.strip('\n')
      isline = 0
      numline = 0
      numlineL1 = 0
      with open(fileEff) as dataEff:
          partsL1ZeroBias=[]
          for lineEff in dataEff:
             if "Module Summary" in lineEff:
                isline = 1
             if isline == 1:
                if "TrigReport" and "hltL1sL1ZeroBias" in lineEff:
                   numlineL1 = numlineL1+1
                   if numlineL1 == 1:
                      partstmp = lineEff.split(" ")
                      for s in partstmp:
                        if s != '' and s != 'TrigReport' and s != 'hltL1sL1ZeroBias\n': 
                          partsL1ZeroBias.append(int(s))     
                if "TrigReport" and "hltActivityEtFilter" in lineEff:
                   numline = numline+1
                   if numline == 1:
                      partstmp = lineEff.split(" ")
                      parts=[]
                      for s in partstmp:
                        if s != '' and s != 'TrigReport' and s != 'hltActivityEtFilter\n':
                          parts.append(int(s))     
                      #print float(parts[2])/float(parts[1])
                      numTotal = numTotal+int(parts[1])
                      numPass = numPass+int(parts[2])
                      if partsL1ZeroBias[1] != parts[1]:
                         print "Different hltL1sL1ZeroBias!!"

efficiency = float(numPass)/float(numTotal)
print "Passed = ",float(numPass)
print "Total  = ",float(numTotal)
print "Eff    = ",efficiency*100,"+/-",math.sqrt( float(efficiency)*(1-float(efficiency))/float(numTotal) )*100,"%"

