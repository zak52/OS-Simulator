// protect from multiple compiling
#ifndef DATATYPES_H
#define DATATYPES_H

#include <stdio.h>
#include <stdbool.h>
#include "StandardConstants.h"

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



#endif
