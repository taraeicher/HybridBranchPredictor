This branch predictor combines a perceptron predictor with a Gshare predictor. It uses
2K perceptrons with 18 weights each and a global history table of 17 bits. The Gshare
predictor is of size 128K with each entry containing a 2-bit predictor. A choice table
of 16K 2-bit saturating counters is used to choose between the perceptron prediction
and the Gshare prediction for a particular branch. Both the Gshare and perceptron predictor
are updated on each branch outcome. The choice table, perceptron table, and Gshare table
are all indexed by the last 10 bits of the global history register XORed with the last 10 bits
of the PC address.

This branch predictor is meant to be run with the simulator available at
https://drive.google.com/open?id=0B5TPQrtyq9_kRmxmak1ESFF4cXc
. To install the simulator, run the following commands:

	tar -xzvf bpc6421AU15.tar.gz
	cd bpc6421AU15
	cd sim
	make ./predictor ../traces/SHORT-INT-1.cbp4.gz
	
After installing the simulator, overwrite the existing predictor.cc and predictor.h files
with the CC and H files in this repository. Run using either runall.pl or doit.sh in the 
scripts directory. You can view your results using the getdata.pl script.

This predictor achieves the following results with respect to misprediction rate:

LONG-SPEC2K6-00		6.417

LONG-SPEC2K6-01		7.382

LONG-SPEC2K6-02		8.793

LONG-SPEC2K6-03		6.689

LONG-SPEC2K6-04		11.426

LONG-SPEC2K6-05		5.992

LONG-SPEC2K6-06		5.758

LONG-SPEC2K6-07		16.262

LONG-SPEC2K6-08		3.562

LONG-SPEC2K6-09		6.549

SHORT-FP-1			4.613

SHORT-FP-2			1.157

SHORT-FP-3			0.525

SHORT-INT-1			9.531

SHORT-INT-2			9.101

SHORT-INT-3			12.738

SHORT-MM-1			9.719

SHORT-MM-2			11.077

SHORT-MM-3			5.047

SHORT-SERV-1		3.766

SHORT-SERV-2		3.643

SHORT-SERV-3		6.598

AMEAN				7.107