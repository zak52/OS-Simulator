// header files
#include "StringUtils.h"
#include "StandardConstants.h"
#include "datatypes.h"
#include "simtimer.h"
#include "metadataops.h"
#include "StringUtils.h"
#include <pthread.h>
#include "configops.h"


// protoypes
ProcessControlBlock *addPCBNode(ProcessControlBlock *localPtr, ProcessControlBlock *newNode);
bool checkForTerminated( ProcessControlBlock *localHeadPtr );
void changeProcessState(ProcessControlBlock *wrkingRef,
                        ProcessState newState, double masterTime, char* destStr );
ProcessControlBlock *createPCBLinkedList(OpCodeType *metaData, ConfigDataType *config);
ProcessControlBlock *clearPCBLinkedList(ProcessControlBlock *localPtr );
StringNode* displayToType(StringNode* localHeadPtr, char *displayString, bool displayToMonitor, bool displayToLog );
ProcessControlBlock* getCurrentPCB(char *cpuSchedString, ProcessControlBlock *localPtr);
bool makeLogFile(char* fileName, StringNode* headPtr );
void runSim( ConfigDataType *configPtr, OpCodeType *metaDataMstrPtr );
void startTimerThread(int runTime);
void * runTimerThread(void *runTime);
