// protect from multiple compiling
#ifndef DATATYPES_H
#define DATATYPES_H

#include <stdio.h>
#include <stdbool.h>
#include "StandardConstants.h"
#include <pthread.h>

// global constants

typedef enum { CMD_STR_LEN = 5,
               IO_ARG_STR_LEN = 5,
               STR_SRG_LEN = 15 } OpCodeArrayCapacity;


// config data structure
typedef struct ConfigDataType
{
    double version;
    char metaDataFileName[ LARGE_STR_LEN ];
    int cpuSchedCode;
    int quantumCycles;
    bool memDisplay;
    int memAvailable;
    int procCycleRate;
    int ioCycleRate;
    int logToCode;
    char logToFileName[ LARGE_STR_LEN ];
} ConfigDataType;



typedef struct OpCodeType
{
    int pid; // pid, added when PCB is created
    char command[ CMD_STR_LEN ]; // three letter command quantity
    char inOutArg[ IO_ARG_STR_LEN ]; // for device in/out
    char strArg1[ 15 ]; // arg 1 descriptor, up to 12 chars
    int intArg2; // cycles or memory, assumes 4 byte
    int intArg3; // memory, assumes 4 byte int
    // also non/premption indicator
    double opEndTime; // size of time string returned from
    int accessTimer;
    struct OpCodeType *nextNode; // pointer to next node as needed
} OpCodeType;

// different states of the process
typedef enum ProcessState
{
    NEW,
    READY,
    RUNNING,
    WAITING,
    TERMINATED
} ProcessState;

// process control block structure
typedef struct ProcessControlBlock
{
    int pid; // Process ID, unique Identifier for each process
    ProcessState state; // currecnt state of the process
    int totalTime; // total execution time of the process
    OpCodeType *currentOpType;
    struct ProcessControlBlock *nextProcess; // pointer to the next process for efficient management
} ProcessControlBlock;

// string node for string linked list
typedef struct StringNode
{
    char messageStr[MAX_STR_LEN];
    struct StringNode* nextNode;
} StringNode;


// memory manager struct for memory management
typedef struct MemoryManager 
{
    int begAddress;
    int allocatedSize;
    int pid;
    struct MemoryManager *nextMemoryLoc;

} MemoryManager;

// finishing list
typedef struct InteruptLinkedListNode
{
    ProcessControlBlock *currentProcess; // pointer to the current PCB
    struct InteruptLinkedListNode *next; // pointer to next in linked list, head will always be null
} InteruptLinkedListNode;

// actual manager for interrupts locks
// contains the linked list for completed blocked
typedef struct InterruptManager
{
    InteruptLinkedListNode *interuptLL; // add to queue when finished running
    pthread_mutex_t mut_lock; // mutex lock to protect data
} InterruptManager;

// arguments contianed for 
typedef struct IOThreadArguments
{
    ProcessControlBlock *currentProcess; // pointer to current process
    int runTime;
    InterruptManager *intrrptMnger;
} IOThreadArguments;

#endif
