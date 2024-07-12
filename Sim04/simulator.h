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
bool accessMemBlock(MemoryManager **headPtr, int begAddress, int memOffset, int currentProcessID);
ProcessControlBlock *addPCBNode(ProcessControlBlock *localPtr, ProcessControlBlock *newNode);
bool allocateMemBlock(MemoryManager **headPtr, int newBegAddress, int memOffset, int currentPid);
bool checkForTerminated( ProcessControlBlock *localHeadPtr );
bool checkInterrupts(InterruptManager *wrkingIntrrptMngr, int masterTime, char* destStr,
                    StringNode* logMessageHead, char* displayMessage, bool displayToMonitor, bool displayToLog);
void changeProcessState(ProcessControlBlock *wrkingRef,
                        ProcessState newState, double masterTime, char* destStr );
ProcessControlBlock *createPCBLinkedList(OpCodeType *metaData, ConfigDataType *config);
ProcessControlBlock *clearPCBLinkedList(ProcessControlBlock *localPtr );
StringNode* displayToType(StringNode* localHeadPtr, char *displayString, bool displayToMonitor, bool displayToLog );
bool freeMemBlock( MemoryManager** headMemManagerPTR, int currentPID );
ProcessControlBlock* getCurrentPCB(char *cpuSchedString, ProcessControlBlock *localPtr, ProcessControlBlock *curentProcess);
void initializeInterruptManager(InterruptManager *wrkingRef);
MemoryManager* intializeSysAvailableMemory(int availableMemSize );
bool makeLogFile(char* fileName, StringNode* headPtr );
void runSim( ConfigDataType *configPtr, OpCodeType *metaDataMstrPtr );
void startTimerThread(int runTime);
void startIOThread(ProcessControlBlock *wrkingProcess, int runTime,
         InterruptManager *wrkingIntrrptMnger);
void * runTimerThread(void *runTime);
void *runIOOperation(void *arguments);

