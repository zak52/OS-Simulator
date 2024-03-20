#include "StringUtils.h"

/*
Name: compareString
Process: compares two strings with the following results:
    if left string less than right string, returns less than zero
    if left string greater than right string, return greater than zero
    if left string equals right string, returns zero
    - equals test includes length
Function Input/Parameters: left and right strings (char*)
Function Output/Parameters: none
Function Output/Return: result as specified
Device I/O: none
Dependencies: getStringLength
*/
int compareString(const char *oneStr, const char *otherStr)
{
    //Initialize Function
    int diff, index = 0;

    //loop to end of shortest string, with overrun protection
    while(oneStr[index] != NULL_CHAR && otherStr[index] != NULL_CHAR && index < MAX_STR_LEN)
    {
        //get difference in characters
        diff = oneStr[index] - otherStr[index];

        //check for difference
        if(diff != 0)
        {
            //return difference if not zero
            return diff;
        }

        //increment index
        index++;
    }

    //return difference in length, if any
    return getStringLength(oneStr) - getStringLength(otherStr);
}

/*
Name: concatenateString
Process: appends one string onto another
Function Input/Parameters: source string (char*)
Function Output/Parameters: destination string (char*)
Function Output/Return: none
Device I/O: none
Dependencies: getStringLength
*/
void concatenateString(char *destStr, const char *sourceStr)
{
    //Initialize Method
        //Set destination index
        int destIndex = getStringLength(destStr);

        //get source string length
        int sourceStrLen = getStringLength(sourceStr);

        //create temp string pointer
        char *tempStr;

        //other variables
        int sourceIndex = 0;

    //copy source string in case of aliasing
    tempStr = (char*) malloc(sizeof(sourceStrLen + 1));
    copyString(tempStr, sourceStr);

    //loop to end of source string
    while(tempStr[sourceIndex] != NULL_CHAR && destIndex < MAX_STR_LEN)
    {
        //assign characters to end of destination string
        destStr[destIndex] = tempStr[sourceIndex];

        //update indicies
        destIndex++;
        sourceIndex++;

        //set temporary end of destination string
        destStr[destIndex] = NULL_CHAR;
    }

    //release memory used for temp string
    free(tempStr);
}

/*
Name: copyString
Process: copies one string into another, overwriting data
    in the destination string.
Function Input/Parameters: source string (char*)
Function Output/Parameters: destination string (char*)
Function Output/Return: none
Device I/O: none
Dependencies: getStringLength
*/
void copyString(char *destStr, const char *sourceStr)
{
    //Initialize Method
    int index = 0;

    //check for source and destination not the same (aliasing, compare pointers)
    if(destStr != sourceStr)
    {
        //Loop to end of source string
        while(sourceStr[index] != NULL_CHAR && index < MAX_STR_LEN)
        {
            //assign characters to end of dest string
            destStr[index] = sourceStr[index];

            //update index
            index++;

            //set temp end of dest string
            destStr[index] = NULL_CHAR;
        }
    }
}




/*
Name: findSubString
Process: linear search for fiven substring within another string
Function Input/Parameters: source test string, source search string (char*)
Function Output/Parameters: none
Function Output/Return: index of found substring, or SUBSTRING_NOT_FOUND constant if not found.
Device I/O: none
Dependencies: getStringLength
*/
int findSubString(const char *testStr, const char *searchSubStr)
{
    //Initialize Method
        //initialize test string length
        int testStrLen = getStringLength(testStr);

        //Initialize master index - location of sub string start point
        int masterIndex = 0;

        //Initialize Other Variables
        int searchIndex, internalIndex;

    //loop accross test string
    while(masterIndex < testStrLen)
    {
        //Set internal loop index to current test string index
        internalIndex = masterIndex;

        //set internal search index to zero
        searchIndex = 0;

        //loop to end of test string
        //while test string/sub string characters are the same
        while(internalIndex <= testStrLen && testStr[internalIndex] == searchSubStr[searchIndex])
        {
            //increment test string, substring indicies
            internalIndex++;
            searchIndex++;

            //check for end of substring (search completed)
            if(searchSubStr[searchIndex] == NULL_CHAR)
            {
                //return beginning location of sub string
                return masterIndex;
            }
        }

        //Increment current beginnin location index
        masterIndex++;
    }

    //assume tests have failed at this point, return SUBSTRING_NOT_FOUND
    return SUBSTRING_NOT_FOUND;
}

/*
Name: getStringConstrained
Process: captures a string from the input stream with various constraints
Function Input/Parameters: clears leading non printable (Boolean),
                            clears leading space (Boolean),
                            stops at non printable (Boolean),
                            stops at specified delimeter (char)
                            Note: consumes delimeter
Function Output/Parameters: string returned (char*)
Function Output/Return: success of operation (Boolean)
Device I/O: none
Dependencies: fgetc
*/
bool getStringConstrained(FILE *inStream, bool clearLeadingNonPrintable, bool clearLeadingSpace,
    bool stopAtNonPrintable, char delimeter, char *capturedString)
{
    //Initialize Method
    int intChar = EOF, index = 0;

    //Initialize output string
    capturedString[index] = NULL_CHAR;

    //capture first value in stream
    intChar = fgetc(inStream);


    //loop to clear non printable or space, if indicated
    while((intChar != EOF) && ((clearLeadingNonPrintable && intChar < (int)SPACE_CHAR) || (clearLeadingSpace && intChar == (int)SPACE_CHAR)))
    {
        //get next character
        intChar = fgetc(inStream);
    }

    //check for end of file found
    if(intChar == EOF)
    {
        //return failed operation
        return false;
    }

    //loop to capture input
    while(

        //continues if not at end of file and max string length not reached
        (intChar != EOF && index < MAX_STR_LEN - 1)

        //AND
        //continues if not printable flag set and characters are printable
        //     OR continues if not printable flag not set
        && ((stopAtNonPrintable && intChar >= (int)SPACE_CHAR) || (!stopAtNonPrintable))

        //AND
        //continues if specified delimeter is not found
        && (intChar!= (char)delimeter))
    {
        //place character in array element
        capturedString[index] = (char)intChar;

        //increment array index
        index++;

        //set next element to null char
        capturedString[index] = NULL_CHAR;

        //get next character as integer
        intChar = fgetc(inStream);
    }

    //return successful operation
    return true;
}

/*
Name: getStringLength
Process: finds the length of a string by counting
    characters up to the NULL_CHAR character
Function Input/Parameters: c-style string (char*)
Function Output/Parameters: none
Function Output/Return: length of string
Device I/O: none
Dependencies: none
*/
int getStringLength(const char *testStr)
{
    //Initialize Method
    int index = 0;

    //loop to end of string, protect from overflow
    while(index < STD_STR_LEN && testStr[index] != NULL_CHAR)
    {
        index++;
    }

    return index;
}

/*
Name: getStringToDelimeter
Process: captures a string from the input stream to a
    specified delimeter; Note: consumes delimeter.
Function Input/Parameters: input stream (FILE*), delimeter (char)
Function Output/Parameters: string returned (char*)
Function Output/Return: success of operation (Boolean)
Device I/O: none
Dependencies: getStringConstrained
*/
bool getStringToDelimiter(FILE *inStream, char delimeter, char *capturedString)
{
    //call engine function with delimeter
    return getStringConstrained(
        inStream,            //file stream pointer
        true,                //clears leading non printable characters
        true,                //Boolean clearLeadingSpace
        true,                //stops at non printable
        delimeter,            //stops at delimeter
        capturedString        //returns string
        );
}

/*
Name: getStringToLineEnd
Process: captures a string from the input stream
    to the end of the line.
Function Input/Parameters: input stream (FILE*)
Function Output/Parameters: string returned (char*)
Function Output/Return: success of operation (Boolean)
Device I/O: none
Dependencies: getStringConstrained
*/
bool getStringToLineEnd(FILE *inStream, char *capturedString)
{
    //call engine function with specified constraints
    return getStringConstrained(
        inStream,            //file stream pointer
        true,                //clears leading non printable characters
        true,                //vool clearLeadingSpace
        true,                //stops at non printable
        NON_PRINTABLE_CHAR,    //non printable delimeter
        capturedString        //returns string
        );
}

/*
Name: getSubString
Process: captures string within larger string between
    two inclusive indicies. returns empty string if either
    index is out of range; assumes enough memory in destination string.
Function Input/Parameters: source string (char*), start and end indicies (int)
Function Output/Parameters: destination string (char*)
Function Output/Return: none
Device I/O: none
Dependencies: getStringLength, malloc, copyString, free
*/
void getSubString(char *destStr, const char *sourceStr, int startIndex, int endIndex)
{
    //Initialize Method
        //set length of source string
        int sourceLength = getStringLength(sourceStr);

        //initialize the destination index to zero
        int destIndex = 0;

        //initialize source index to start index
        int sourceIndex = startIndex;

        //create pointer for temp string
        char *tempStr;

    //check for indicies within limits
    if(startIndex >= 0 && startIndex <= endIndex && endIndex < sourceLength)
    {
        //create temp string
        tempStr = (char*) malloc((sourceLength + 1));
        copyString(tempStr, sourceStr);

        //loop accross requsted substring (indicies)
        while(sourceIndex <= endIndex)
        {
            //assign source charr to destination element
            destStr[destIndex] = tempStr[sourceIndex];

            //increment indicies
            destIndex++;
            sourceIndex++;

            //set temp end of dest string
            destStr[destIndex] = NULL_CHAR;
        }

        //return memory for temporary string
        free(tempStr);
    }
}

/*
Name: setStrToLowerCase
Process: iterates through string, sets any upper case letter to
    lower case; no effect on other letters.
Function Input/Parameters: source string (char*)
Function Output/Parameters: destination string (char*)
Function Output/Return: none
Device I/O: none
Dependencies: toLowerCase
*/
void setStrToLowerCase(char *destStr, const char *sourceStr)
{
    // initialize function/variables

        // get source string length
            // function: getStringLength
        int sourceStrLen = getStringLength( sourceStr );

        // create temporary string pointer
        char *tempStr;

        // create other variables
        int index = 0;

    // copy source string in case of aliasing
        // function: malloc, copyString
    tempStr = (char *)malloc( sourceStrLen + 1 );
    copyString( tempStr, sourceStr );

    // loop across source string
    while( tempStr[ index ] != NULL_CHAR && index < MAX_STR_LEN )
        {
        // set individual character to lower case as needed,
        // assign to destination string
        destStr[ index ] = toLowerCase( tempStr[ index ] );

        // update index
        index++;

        // set temporary end of destination string
        destStr[ index ] = NULL_CHAR;
        }
    // end loop

    // release memory used for temp string
        // function: free
    free( tempStr );
}
/*
Name: toLowerCase
Process: if character is upper case, sets it to lower case;
    otherwise returns character unchanged.
Function Input/Parameters: test character (char)
Function Output/Parameters: none
Function Output/Return: character to set to lower case, if appropriate
Device I/O: none
Dependencies: none
*/
char toLowerCase(char testChar)
{
    //check for upper case letter
    if(testChar >= 'A' && testChar <= 'Z')
    {
        //convert to lower case
        return testChar - 'A' + 'a';
    }

    //otherwise, assume no upper case letter, return unchanged char
    return testChar;
}



/*
Name: insertStringNode
Process: if character is upper case, sets it to lower case;
    otherwise returns character unchanged.
Function Input/Parameters: test character (char)
Function Output/Parameters: none
Function Output/Return: character to set to lower case, if appropriate
Device I/O: none
Dependencies: none
*/
StringNode *insertStringNode(StringNode *headPtr, StringNode *newNode)
{
    if ( headPtr == NULL )
    {
        headPtr = (StringNode *) malloc( sizeof( StringNode ) );

        copyString(headPtr->messageStr, newNode->messageStr);
        
        headPtr->nextNode = NULL;

        return headPtr;
    }

    headPtr->nextNode = insertStringNode( headPtr->nextNode, newNode );

    return headPtr;

}


