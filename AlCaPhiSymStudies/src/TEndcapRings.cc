#include "HLTStudies/AlCaPhiSymStudies/interface/TEndcapRings.h"



//-----
// ctor

TEndcapRings::TEndcapRings()
{
  // initialization
  for(int ix = 1; ix <= 100; ++ix)
    for(int iy = 1; iy <= 100; ++iy)
      for(int iz = 0; iz <= 1; ++iz)
        iEndcapRing[ix][iy][iz] = -1;
  
  FILE *fRing;
  fRing = fopen("eerings.dat","r");
  //fRing = fopen("./eerings.dat","r");
  std::cout << "Inizializing endcap geometry from: eerings.dat" << std::endl;
  int ix,iy,iz,ir;
  while(fscanf(fRing,"(%d,%d,%d) %d \n",&ix,&iy,&iz,&ir) !=EOF )
  {
    if( iz < 0 ) iz = 0; 
    iEndcapRing[ix][iy][iz] = ir;
  }

  std::cout << "eering.dat read correctly" << std::endl; 
  return;
}

//-----
// dtor

TEndcapRings::~TEndcapRings()
{
  return;
}



//--------
// methods

int TEndcapRings::GetEndcapRing(int ix, int iy, int iz)
{
  int iSide = iz; 
  if( iSide < 0 ) iSide = 0; 
  return iEndcapRing[ix][iy][iSide];
}

int TEndcapRings::GetEndcapIeta(int ix, int iy, int iz)
{
  int iSide = iz; 
  if( iSide < 0 ) iSide = 0;
  int iEtaOffset = 86*iz; 
  int iEta = iEtaOffset + iz*iEndcapRing[ix][iy][iSide];
  
  return iEta;
}

int TEndcapRings::GetEndcapIphi(int ix, int iy, int iz)
{
  double iX = ix-50.5;
  double iY = iy-50.5;
  int iPhi = -1;
  
  if( iX > 0 && iY > 0 ) iPhi =  int(       10 + TMath::ATan(iY/iX)*360. / (2.*TMath::Pi()) ) % 360 + 1;
  if( iX < 0 && iY > 0 ) iPhi =  int( 180 + 10 + TMath::ATan(iY/iX)*360. / (2.*TMath::Pi()) ) % 360 + 1;
  if( iX < 0 && iY < 0 ) iPhi =  int( 180 + 10 + TMath::ATan(iY/iX)*360. / (2.*TMath::Pi()) ) % 360 + 1;
  if( iX > 0 && iY < 0 ) iPhi =  int( 360 + 10 + TMath::ATan(iY/iX)*360. / (2.*TMath::Pi()) ) % 360 + 1;
  
  return iPhi;
}

float TEndcapRings::GetEtaFromIRing(const int& iRing)
{
  if( (iRing >= 1) && (iRing <= 85) )
    return ( 0.0174*(iRing-0.5) );
  if( (iRing >= -85) && (iRing <= -1) )
    return ( 0.0174*(iRing+0.5) );
  if( iRing >= 86 )
    return ( 1.47135 + 0.0281398*(iRing-86) - 0.00059926*pow(iRing-86,2) + 2.23632e-05*pow(iRing-86,3) );
  if( iRing <= -86 )
    return -1 * ( 1.47135 + 0.0281398*(fabs(iRing)-86) - 0.00059926*pow(fabs(iRing)-86,2) + 2.23632e-05*pow(fabs(iRing)-86,3) );
  return -1.;
}
