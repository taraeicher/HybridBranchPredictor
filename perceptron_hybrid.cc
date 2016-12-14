#include "predictor.h"
#include <bitset>
#include <stdint.h>


#define PERC_TABLE_SIZE 2048 
#define GSHARE_TABLE_SIZE 131072
#define CHOICE_TABLE_SIZE 16384
#define THRESHOLD 46 // Floor(1.93 * HIST_LEN + 14) as specified in paper
#define HIST_LEN   17 // Optimal value as determined by paper
#define PHT_CTR_MAX  3
#define PHT_CTR_INIT 2

/////////////// STORAGE BUDGET JUSTIFICATION ////////////////
// Total storage budget: 64KB = 524288 bits
// Total bits in history register:  17 bits
// Total Gshare table storage: 262144 bits
// Total number of weights per perceptron:  17 + 1 = 18
// Total bits to store weight: 1 + log(46) = 1 + 5 = 6
// Total number of perceptrons: 2048
// Total perceptron table size = 2048 * 18 * 6 = 221184 bits
// Total choice table size: 16384 * 2 = 32768 bits
// Total size = History register size + Gshare table size +
//				Perceptron table size + Choice table size
//              + Perceptron steps counter size = 524287 bits
/////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

PREDICTOR::PREDICTOR(void){

  // Given the history length and table size, construct the ghr
  // and perceptron and gshare PHT tables. Initialize the choice table.

  ghr              = bitset<HIST_LEN>();
  perceptronSteps = 0;
  
  // Initialize the choice table.
  for(UINT32 iii=0; iii < CHOICE_TABLE_SIZE; iii++){
    choiceTable[iii] = bitset<2>();
  }

  // Initialize the PHT.
  pht = new UINT32[GSHARE_TABLE_SIZE];

  for(UINT32 ii=0; ii< GSHARE_TABLE_SIZE; ii++){
    pht[ii]=PHT_CTR_INIT; 
  }

  // Initialize each entry in the perceptron table to a value of
  // zero. Initialize number of steps executed for each perceptron to zero.

  for(UINT32 i=0; i < PERC_TABLE_SIZE; i++){
    for(UINT32 j=0; j < HIST_LEN; j++){
	  perceptronTable[i][j] = 0;
    }
  }
}


/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

bool   PREDICTOR::GetPrediction(UINT32 PC){

    // If the highest bit in the saturating counter is 1, choose
	// gshare. Otherwise, choose the perceptron predictor.
	UINT32 counterIndex = HashPC(PC, CHOICE_TABLE_SIZE);
	if (choiceTable[counterIndex][1] == 1){
		return GetGsharePrediction(PC);
	}
	else{
		return GetPerceptronPrediction(PC);
	}
}


/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

void  PREDICTOR::UpdatePredictor(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget){

   // Update saturating counter based on current state and outcome.
   UINT32 counterIndex = HashPC(PC, CHOICE_TABLE_SIZE);
   
   // Decrement if perceptron got it right or gshare got it wrong.
   
   if ((choiceTable[counterIndex][1] == 0 && resolveDir == predDir) || (choiceTable[counterIndex][1] == 1 && resolveDir != predDir)){
      
	  
		 if(choiceTable[counterIndex][1] == 1 && choiceTable[counterIndex][0] == 1){
			choiceTable[counterIndex].set(0,0);
		 }
		 else if(choiceTable[counterIndex][1] == 1 && choiceTable[counterIndex][0] == 0){
			choiceTable[counterIndex].set(0,1);
			choiceTable[counterIndex].set(1,0);
		 }
		 else if(choiceTable[counterIndex][1] == 0 && choiceTable[counterIndex][0] == 1){
			choiceTable[counterIndex].set(0,0);
		 }
	}
   else{
      
	  // Increment if perceptron got it wrong or gshare got it right.
	  
	  if ((choiceTable[counterIndex][1] == 1 && resolveDir == predDir) || (choiceTable[counterIndex][1] == 0 && resolveDir != predDir)){
	    if(choiceTable[counterIndex][1] == 1 && choiceTable[counterIndex][0] == 0){
			choiceTable[counterIndex].set(0,1);
		 }
		 else if(choiceTable[counterIndex][1] == 0 && choiceTable[counterIndex][0] == 1){
			choiceTable[counterIndex].set(1,1);
			choiceTable[counterIndex].set(0,0);
		 }
		 else if(choiceTable[counterIndex][1] == 0 && choiceTable[counterIndex][0] == 0){
			choiceTable[counterIndex].set(0,1);
		 }
	  }
   }
   UpdatePerceptronPredictor(PC, resolveDir, predDir);
   UpdateGsharePredictor(PC, resolveDir);
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

void    PREDICTOR::TrackOtherInst(UINT32 PC, OpType opType, UINT32 branchTarget){

  // This function is called for instructions which are not
  // conditional branches, just in case someone decides to design
  // a predictor that uses information from such instructions.
  // We expect most contestants to leave this function untouched.

  return;
}

/////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

UINT32 PREDICTOR::HashPC(UINT32 PC, UINT32 hashLimit){
	
	// Hash the PC so that it can be used as an index for the perceptron table.
	
	UINT32 PCend = PC % hashLimit;
	UINT32 ghrend = ((UINT32)ghr.to_ulong()) % hashLimit;
	return PCend ^ ghrend;
}

bool PREDICTOR::GetGsharePrediction(UINT32 PC){
	  
  UINT32 phtIndex   = HashPC(PC, GSHARE_TABLE_SIZE);
  UINT32 phtCounter = pht[phtIndex];
  
  if(phtCounter > PHT_CTR_MAX/2){
    return TAKEN; 
  }else{
    return NOT_TAKEN; 
  }
} 

void PREDICTOR::UpdateGsharePredictor(UINT32 PC, bool resolveDir){
	
  UINT32 phtIndex   = HashPC(PC, GSHARE_TABLE_SIZE);
  UINT32 phtCounter = pht[phtIndex];

  // update the PHT

  if(resolveDir == TAKEN){
    pht[phtIndex] = SatIncrement(phtCounter, PHT_CTR_MAX);
  }else{
    pht[phtIndex] = SatDecrement(phtCounter);
  }

  // update the GHR
  ghr = (ghr << 1);
  if(resolveDir == TAKEN){
    ghr.set(0, 1); 
  }
  else{
	ghr.set(0, 0);
  }

}

bool PREDICTOR::GetPerceptronPrediction(UINT32 PC){
	
  UINT32 perceptronIndex = HashPC(PC, PERC_TABLE_SIZE);
  INT32 prediction = 0;

  // Calculate prediction based on selected perceptron and global history.
  // First add the bias, then all other weights.

  prediction += perceptronTable[perceptronIndex][0]; 
  for(UINT32 i=1; i < HIST_LEN + 1; i++){
	  
    // If history bit is taken, add the weight to the prediction.
    // Else, subtract the weight.

    if(ghr[i - 1] == 1){
	  prediction += perceptronTable[perceptronIndex][i];   
    }
    else{
	  prediction -= perceptronTable[perceptronIndex][i]; 
    }
  }

  // Update perceptron steps to absolute value of the prediction.

  perceptronSteps = abs(prediction);

  // If the prediction is negative, predict not taken. If it is positive,
  // predict taken. Assume zero is positive.
 
 if(prediction >= 0){
    return TAKEN; 
  }
  else{
    return NOT_TAKEN; 
  }  
}
void PREDICTOR::UpdatePerceptronPredictor(UINT32 PC, bool resolveDir, bool predDir){
	
	UINT32 perceptronIndex = HashPC(PC, PERC_TABLE_SIZE);

  // Update the perceptron table entry only if the threshold has not been
  // reached or the predicted and true outcomes disagree. Update the bias first, then the weights.

  if(resolveDir != predDir || perceptronSteps <= THRESHOLD){
    
    // If the branch was taken, increment the bias value. Else, decrement it.

    if(resolveDir == TAKEN){
       perceptronTable[perceptronIndex][0]++;
	}
    else{
       perceptronTable[perceptronIndex][0]--;
	}
	   
	// Update the weights.

	for(UINT32 i = 1; i < HIST_LEN + 1; i++){

       // If the branch outcome matches the history bit, increment the weight value.
	   // Else, decrement the weight value.

       if((resolveDir == TAKEN && ghr[i - 1] == 1) || (resolveDir == NOT_TAKEN && ghr[i - 1] == 0)){
	      perceptronTable[perceptronIndex][i]++;
       }
	   else{
	      perceptronTable[perceptronIndex][i]--;
	   }
	}	   
  }

  // update the GHR by shifting left and setting new bit.
  ghr = (ghr << 1);
  if(resolveDir == TAKEN){
    ghr.set(0, 1); 
  }
  else{
	ghr.set(0, 0);
  }
}
