// header files
#include <stdio.h>
#include "datatypes.h"
#include "StringUtils.h"
#include "metadataops.h"
#include "simulator.h"
#include "configops.h"

// Program constants
typedef enum { MIN_NUM_ARGS = 3, LAST_FOUR_LETTERS = 4 } PRGRM_CONSTANTS;

// command line struct for storing command line switch settings
typedef struct CmdLineDataStruct
    {
        bool programRunFlag;
        bool configDisplayFlag;
        bool mdDisplayFlag;
        bool runSimFlag;

        char fileName[ STD_STR_LEN ];
    } CmdLineData;


// function prototypes
void showCommandLineFormat(void);
bool processCmdLine( int numArgs, char **strVector, CmdLineData *clDataPtr);
void clearCmdLineStruct( CmdLineData *clDataPtr );
