#ifndef METADATA_OPS_H
#define METADATA_OPS_H

// header files
#include "StringUtils.h"
#include "StandardConstants.h"
#include "datatypes.h"

//Enum Declaration

typedef enum { 
    BAD_ARG_VAL = -1,
    NO_ACCESS_ERR,
    MD_FILE_ACCESS_ERR,
    MD_CORRUPT_DESCRIPTOR_ERR, // Fixed the typo here
    OPCMD_ACCESS_ERR,
    CORRUPT_OPCMD_ARG_ERR,
    CORRUPT_OPCMD_ERR,
    UNBALANCED_START_END_ERR,
    COMPLETE_OPCMD_FOUND_MSG,
    LAST_OPCMD_FOUND_MSG
} OpCodeMessages;
	

// prototypes
OpCodeType *clearMetaDataList( OpCodeType *localPtr );
void displayMetaData( const OpCodeType *localPtr );
bool getMetaData( const char *fileName,
                             OpCodeType **opCodeDataHead, char *endStateMsg );
OpCodeType *addNode( OpCodeType *localPtr, OpCodeType *newNode );
int getCommand ( char *cmd, const char *inputStr, int index);
int getNumberArg( int *number, const char *inputStr, int index );
OpCodeMessages getOpCommand ( FILE *filePtr, OpCodeType *inData );
int getStringArg( char *strArg, const char *inputStr, int index );
int updateEndCount( int count, const char *opString );
int updateStartCount( int count, const char *opString );
bool verifyFirstStringArg( const char *strArg );
bool verifyValidCommand( char *testCmd );
bool isDigit( char testChar );


#endif
