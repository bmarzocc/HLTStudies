#!/usr/bin/perl

# ----------------------------------------------------------------------------
#      MAIN PROGRAM
# ----------------------------------------------------------------------------

use Env;

#PG lettura dei parametri da cfg file
#PG --------------------------------
print "reading ".$ARGV[0]."\n" ;

open (USERCONFIG,$ARGV[0]) ;

while (<USERCONFIG>)
{
    chomp; 
    s/#.*//;                # no comments
    s/^\s+//;               # no leading white
    s/\s+$//;               # no trailing white
#    next unless length;     # anything left?
    my ($var, $value) = split(/\s*=\s*/, $_, 2);
    $User_Preferences{$var} = $value;
}

$BASEDir          = $User_Preferences{"BASEDir"};
$LISTOFSamples    = $User_Preferences{"LISTOFSamples"} ;
$EXEName          = $User_Preferences{"EXEName"} ;
$JOBCfgTemplate   = $User_Preferences{"JOBCfgTemplate"} ;
$OUTPUTSAVEPath   = $User_Preferences{"OUTPUTSAVEPath"} ;
$OUTPUTFILEName   = $User_Preferences{"OUTPUTFILEName"} ;
$JOBModulo        = $User_Preferences{"JOBModulo"} ;
$QUEUE            = $User_Preferences{"QUEUE"};
$JOBdir           = $User_Preferences{"JOBdir"};


print "BASEDir = "          .$BASEDir."\n" ;
print "LISTOFSamples = "    .$LISTOFSamples."\n" ;
print "EXEName = "          .$EXEName."\n" ;
print "JOBCfgTemplate = "   .$JOBCfgTemplate."\n" ;
print "OUTPUTSAVEPath = "   .$OUTPUTSAVEPath."\n" ;
print "OUTPUTFILEName = "   .$OUTPUTFILEName."\n" ;
print "JOBModulo = "        .$JOBModulo."\n" ;
print "QUEUE  = "           .$QUEUE."\n" ;
print "JOBdir  = "          .$JOBdir."\n\n" ;






$sampleJobListFile = "./lancia.sh";
open(SAMPLEJOBLISTFILE, ">", $sampleJobListFile);


#####################################################
# PG prepare the array containing the root files list
#####################################################


open (LISTOFSamples,$LISTOFSamples) ;
while (<LISTOFSamples>)
{
    system("cd ".$BASEDir."\n");
    
    chomp($_);
    
    ($INPUTSAVEPath,$sample1) = split(" ") ;
    $subsample = substr($sample,0,1);
    if($subsample eq "#")
    {
	next;
    }
    
    print("Sample: ".$sample1."\n") ;  

    system ("rm -r ".$JOBdir." \n") ;
    system ("mkdir ".$JOBdir." \n") ;
    
  
  
    
  
  
    $LISTOFFiles = "./list_".$sample1.".txt" ;
    system ("cmsLs -R ".$INPUTSAVEPath."/".$sample1." | grep root | awk '{print \$5}' > ".$LISTOFFiles."\n") ;

    #$globalPATH = $INPUTSAVEPath."/".$sample1."/" ;
    #system ("rfdir ".$INPUTSAVEPath."/".$sample1." | grep root | awk '{print \"rfio:".$globalPATH."\"\$9}' > ".$LISTOFFiles."\n") ;
  
  
  
    $totNumber = 0;
    $jobNumber = 0;
  
    open (LISTOFFiles,$LISTOFFiles) ;
    while (<LISTOFFiles>)
    {
	++$totNumber;
    }

    $jobNumber = int($totNumber/$JOBModulo);
    if( $totNumber%$JOBModulo != 0)
    {
	$jobNumber = $jobNumber+1;
    }
    
    print "NumberOfJobs = ".$jobNumber."\n";
    
  
  
  
  
  
    ################
    # loop over jobs 
    ################
    
    for($jobIt = 1; $jobIt <= $jobNumber; ++$jobIt)
    { 
	$currDir = `pwd` ;
	chomp ($currDir) ;
    
	$jobDir = $currDir."/".$JOBdir."/JOB_".$jobIt ;
	system ("mkdir ".$jobDir." \n") ;
    
	$tempBjob = $jobDir."/bjob_".$jobIt.".sh" ;
	$command = "touch ".$tempBjob ;
	system ($command) ;
	$command = "chmod 777 ".$tempBjob ;
	system ($command) ;
    
	$tempo1 = "./tempo1" ;
	#system ("cat ".$JOBCfgTemplate."   | sed -e s%OUTPUTFILENAME%".$OUTPUTSAVEPath.$sample1."/".$OUTPUTFILEName."_".$jobIt.
	system ("cat ".$JOBCfgTemplate."   | sed -e s%OUTPUT%".$OUTPUTFILEName."_".$jobIt.
		                       "%g > ".$tempo1) ;
    
    
    
	$it = 0;
	$JOBLISTOFFiles;

	open (LISTOFFiles2,$LISTOFFiles) ;
	while (<LISTOFFiles2>)
	{
	    chomp; 
	    s/#.*//;                # no comments
	    s/^\s+//;               # no leading white
	    s/\s+$//;               # no trailing white
	    $file = $_ ;
	    
	    if( ($it >= ($jobIt - 1)*$JOBModulo) && ($it < ($jobIt)*$JOBModulo) )
	    { 
		#$JOBLISTOFFiles = "APICE".$INPUTSAVEPath."/".$sample."/".$file."APICE,";
		$JOBLISTOFFiles = $JOBLISTOFFiles."APICE"."root://eoscms.cern.ch/".$file."APICE,";
	    }
	    ++$it;
	}
	
	
	$tempo2 = "./tempo2" ;    
	system ("cat ".$tempo1." | sed -e s%LISTOFFILES%".$JOBLISTOFFiles."%g > ".$tempo2) ;
	$JOBLISTOFFiles = "" ;

	$tempo3 = "./tempo3" ;
	system ("cat ".$tempo2." | sed -e s%APICE%\\'%g > ".$tempo3) ;
    
    
	$JOBCfgFile = $jobDir."/".$EXEName ;
	system ("mv ".$tempo3." ".$JOBCfgFile) ;
	system ("rm ./tempo*") ;
    
    
    
    
    
    
    ######################
    # make job files
    ######################    
    
	open (SAMPLEJOBFILE, ">", $tempBjob) or die "Can't open file ".$tempBjob;

	$command = "#!/bin/tcsh" ;
	print SAMPLEJOBFILE $command."\n";

	$command = "cd ".$BASEDir ;
	print SAMPLEJOBFILE $command."\n";

	$command = "setenv SCRAM_ARCH slc6_amd64_gcc491" ;
	print SAMPLEJOBFILE $command."\n";
    
	$command = "eval `scramv1 ru -csh`" ;
	print SAMPLEJOBFILE $command."\n";
    
	$command = "cd -" ;
	print SAMPLEJOBFILE $command."\n";


	#$command = "cmsMkdir ".$OUTPUTSAVEPath;
	#print SAMPLEJOBFILE $command."\n";

	$command = "MakeAlCaPhiSymOccupancyPlot ".$JOBCfgFile ;
	print SAMPLEJOBFILE $command."\n";

	$command = "cp ".$OUTPUTFILEName."_".$jobIt.".root ".$OUTPUTSAVEPath;
	print SAMPLEJOBFILE $command."\n";

	
	############
	# submit job
	############
	
    $command = "bsub -cwd ".$jobDir." -q ".$QUEUE." ".$tempBjob."\n" ; 
	print SAMPLEJOBLISTFILE $command."\n";
    
    }

    system ("rm ".$LISTOFFiles) ;
}  
