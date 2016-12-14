#ifndef _PREDICTOR_H_
#define _PREDICTOR_H_

#include "utils.h"
#include "tracer.h"
#include <bitset>
#include <stdint.h>

#define PERC_TABLE_SIZE 2048 
#define GSHARE_TABLE_SIZE 131072
#define CHOICE_TABLE_SIZE 16384
#define HIST_LEN   17 // Optimal value as determined by paper

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

class PREDICTOR{

  // The state is defined for perceptron predictor.

 private:

  bitset<HIST_LEN>  ghr;           // global history register
  bitset<2> choiceTable[CHOICE_TABLE_SIZE]; // saturating counters
  int8_t perceptronTable[PERC_TABLE_SIZE][HIST_LEN + 1]; //table of perceptrons
  UINT32 perceptronSteps; // number of training steps for each perceptron
  UINT32  *pht;          // pattern history table
  
 public:

  // The interface to the four functions below CAN NOT be changed

  PREDICTOR(void);
  bool    GetPrediction(UINT32 PC);  
  void    UpdatePredictor(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget);
  void    TrackOtherInst(UINT32 PC, OpType opType, UINT32 branchTarget);

  // Contestants can define their own functions below

  UINT32 HashPC(UINT32 PC, UINT32 numPerceptronEntries);
  bool    GetGsharePrediction(UINT32 PC); 
  void    UpdateGsharePredictor(UINT32 PC, bool resolveDir);
  bool    GetPerceptronPrediction(UINT32 PC); 
  void    UpdatePerceptronPredictor(UINT32 PC, bool resolveDir, bool predDir);
};



/***********************************************************/
#endif

