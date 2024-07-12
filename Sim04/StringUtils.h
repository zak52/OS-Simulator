// Preprocessor directive 
#ifndef STRING_UTILS_H
#define STRING_UTILS_H

// header fiels
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "StandardConstants.h"
#include "datatypes.h"

// prototypes
int compareString( const char *oneStr, const char *otherStr );
void concatenateString( char *destStr, const char *sourceStr );
void copyString( char *destStr, const char *sourceStr );
bool getStringToLineEnd( FILE *instream, char *capturedString );
int findSubString( const char *testStr, const char *searchSubStr );
bool getStringConstrained( 
                           FILE *inStream, 
                           bool clearLeadingNonPrintable,
                           bool clearLeadingSpace,
                           bool stopAtNonPrintable,
                           char delimiter,
                           char *capturedString
                        );
bool getStringToDelimiter(
                          FILE *inStream,
                          char delimiter,
                          char *capturedString
                          );
int getStringLength( const char *testStr );
void getSubString( char *destStr, const char *sourceStr,
                        int startIndex, int endIndex );
void setStrToLowerCase( char *destStr, const char *sourceStr );
char toLowerCase( char testChar );
StringNode *insertStringNode( StringNode* headPtr, StringNode *newNode );


#endif // STRINGS_UTILS_H
