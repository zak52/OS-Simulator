// header files
#include "configops.h"

// functions:





/*
Name: clearConfigData
Process: frees dynamically allocated config data structure
         if it has not already been freed
Function Input/Parameter: pointer to config data structure (ConfigDataType *)
Function Output/parameters: none
Function Output/Returned: NULL (ConfigDataType *)
Device Input/file: none
Device Output/device: none
dependencies: TBD
*/
ConfigDataType *clearConfigData( ConfigDataType *configData)
{
    // check that config data pointer is not null
    if( configData != NULL )
        {
        // free data structure memory
            // function: free
            free( configData );
        // set config pointer to NULL
            configData = NULL;
        }
    // set config data pointer to null
    return NULL;
}

/*
Name: configCodeToString
Process: utility function converts configuration code numbers
         to the string they represent
Function Input/Parameter: configuration code (int)
Function Output/parameters: resulting output string (char *)
Function Output/Returned: none
Device Input/file: none
Device Output/device: none
dependencies: copyString
*/
void configCodeToString( int code, char *outString )
    {
    // Define Array with eight items, and show (10) lengths
    char displayString[ 8 ][ 10 ] = { "SJF-N", "SRTF-P", "FCFS-P", 
                                      "RR-P", "FCFS-N", "Monitor",
                                      "File", "Both"};
    // copyString to return parameter
        // function: copyString
    copyString( outString, displayString[ code ] );
    }

/*
Name: displayConfigData
Process: screen dump/display of all config data
Function Input/Parameter: pointer to config data struture (ConfigDataType *)
Function Output/parameters: none
Function Output/Returned: none
Device Input/file: none
Device Output/device: displayed as specified
dependencies: configCodeToString, printf
*/
void displayConfigData( ConfigDataType *configData)
    {
    // initialize function/variables
    char displayString[ STD_STR_LEN ];
    // print lines of display for all member values
        // function: printf, codeToString (translates coded items)
    printf( "Config File Display\n" );
    printf( "-------------------\n" );
    printf( "Version                : %3.2f\n", configData->version );
    printf( "Program file name      : %s\n", configData->metaDataFileName );
    configCodeToString( configData->cpuSchedCode, displayString );
    printf( "CPU schedule selection : %s\n", displayString );
    printf( "Quantum time           : %d\n", configData->quantumCycles );
    printf( "Memory Display         : " );
    if( configData->memDisplay )
        {
            printf( "On\n" );
        }
    else
        {
            printf( "Off\n" );
        }
    printf( "Memory Available      : %d\n", configData->memAvailable );
    printf( "Process cycle rate    : %d\n", configData->procCycleRate );
    printf( "I/O cycle rate        : %d\n", configData->ioCycleRate );
    configCodeToString( configData->logToCode, displayString);
    printf( "Log to selection      : %s\n", displayString);
    printf( "Log file name         : %s\n\n", configData->logToFileName);
    }

/*
Name: getConfigData
Process: driver function for captures configuration data from a config file
Function Input/Parameter: file name (const char*)
Function Output/parameters: pointer to config data pointer (ConfigDataType **),
                            end/result state message pointer (char *)
Function Output/Returned: Boolean result of data access operation (bool)
Device Input/file: config data uploaded
Device Output/device: none
dependencies: copySting, fopen, getStringToDelimiter, compareString,
              fclose, malloc, free, stripTrailingSpaces, getDataLineCode,
              fscanf, setStrToLowerCase, valueInRange, getCpuSchedCode,
              getLogToCode
*/
bool getConfigData(const char *fileName, ConfigDataType **configData, char *endStateMsg) {
    // Initialize function/variables

    // Set constant number of data lines
    const int NUM_DATA_LINES = 10;

    // Set read-only constant
    const char READ_ONLY_FLAG[] = "r";

    // Create a pointer for data input
    ConfigDataType *tempData;

    // Declare other variables
    FILE *fileAccessPtr;
    char dataBuffer[MAX_STR_LEN];
    int intData, dataLineCode, lineCtr = 0;
    double doubleData;

    // Set endStateMsg to success
    char testMsg[] = "Configuration file upload successful";
    copyString(endStateMsg, testMsg);

    // Initialize config data pointer in case of return error
    *configData = NULL;

    // Open file
    fileAccessPtr = fopen(fileName, READ_ONLY_FLAG);

    // Check for file open failure
    if (fileAccessPtr == NULL) {
        // Set end state message to config file access error
        char testMsg[] = "Configuration file access error";
        copyString(endStateMsg, testMsg);

        // Return file access error
        return false;
    }

    // Get the first line, check for failure
    if (!getStringToDelimiter(fileAccessPtr, COLON, dataBuffer) || compareString(dataBuffer, "Start Simulator Configuration File") != STR_EQ) {
        // Close file access
        fclose(fileAccessPtr);

        // Set end state message to corrupt leader line error
        char testMsg[] = "Corrupt configuration leader line error";
        copyString(endStateMsg, testMsg);

        // Return corrupt file data
        return false;
    }

    // Create a temporary pointer to the configuration data structure
    tempData = (ConfigDataType *)malloc(sizeof(ConfigDataType));

    // Loop to read config data items
    while (lineCtr < NUM_DATA_LINES) {
        // Get the line leader, check for failure
        if (!getStringToDelimiter(fileAccessPtr, COLON, dataBuffer)) {
            // Free temp struct memory
            free(tempData);

            // Close file access
            fclose(fileAccessPtr);

            // Set end state message to line capture failure
            char testMsg[] = "Configuration data line capture error";
            copyString(endStateMsg, testMsg);

            // Return incomplete file error
            return false;
        }

        // Strip trailing spaces of config leader lines
        stripTrailingSpaces(dataBuffer);

        // Find correct data line code number from string
        dataLineCode = getDataLineCode(dataBuffer);

        // Check for data line found
        if (dataLineCode != CFG_CORRUPT_PROMPT_ERR) {
            // Get data value

            // Check for version number (double value)
            if (dataLineCode == CFG_VERSION_CODE) {
                // Get version number
                fscanf(fileAccessPtr, "%lf", &doubleData);
            }

            // Otherwise, check for metaData, file names,
            // CPU scheduling names, or memory display setting (strings)
            else if (dataLineCode == CFG_MD_FILE_NAME_CODE ||
                     dataLineCode == CFG_LOG_FILE_NAME_CODE ||
                     dataLineCode == CFG_CPU_SCHED_CODE ||
                     dataLineCode == CFG_LOG_TO_CODE ||
                     dataLineCode == CFG_MEM_DISPLAY_CODE) {
                // Get string input
                fscanf(fileAccessPtr, "%s", dataBuffer);
                
                // Set string to lower case for testing in valueInRange
                setStrToLowerCase(dataBuffer, dataBuffer);
            }

            // Otherwise, assume integer data
            else {
                // Get integer input
                fscanf(fileAccessPtr, "%d", &intData);
            }

            // Check for data value in range
            if (valueInRange(dataLineCode, intData, doubleData, dataBuffer)) {
                // Assign to data pointer depending on config item
                // (all config line possibilities)
                switch (dataLineCode) {
                    case CFG_VERSION_CODE:
                        tempData->version = doubleData;
                        break;

                    case CFG_MD_FILE_NAME_CODE:
                        copyString(tempData->metaDataFileName, dataBuffer);
                        break;

                    case CFG_CPU_SCHED_CODE:
                        tempData->cpuSchedCode = getCpuSchedCode(dataBuffer);
                        break;

                    case CFG_QUANT_CYCLES_CODE:
                        tempData->quantumCycles = intData;
                        break;

                    case CFG_MEM_DISPLAY_CODE:
                        tempData->memDisplay = compareString(dataBuffer, "on") == 0;
                        break;

                    case CFG_MEM_AVAILABLE_CODE:
                        tempData->memAvailable = intData;
                        break;

                    case CFG_PROC_CYCLES_CODE:
                        tempData->procCycleRate = intData;
                        break;

                    case CFG_IO_CYCLES_CODE:
                        tempData->ioCycleRate = intData;
                        break;

                    case CFG_LOG_TO_CODE:
                        tempData->logToCode = getLogToCode(dataBuffer);
                        break;

                    case CFG_LOG_FILE_NAME_CODE:
                        copyString(tempData->logToFileName, dataBuffer);
                        break;
                }
            }

            // Otherwise, assume data value not in range
            else {
                // Free temp struct memory
                free(tempData);

                // Close file access
                fclose(fileAccessPtr);

                // Set end state message to configuration data out of range
                char testMsg[] = "Configuration item out of range";
                copyString(endStateMsg, testMsg);

                // Return data out of range
                return false;
            }
        } else {
            // Free temp struct memory
            free(tempData);

            // Close file access
            fclose(fileAccessPtr);

            // Set end state message to configuration corrupt prompt error
            char testMsg[] = "Corrupted configuration prompt";
            copyString(endStateMsg, testMsg);

            // Return corrupt config file code
            return false;
        }

        // Increment line counter
        lineCtr++;
    }

    // Acquire end of sim config string
    if (!getStringToDelimiter(fileAccessPtr, '.', dataBuffer) ||
        compareString(dataBuffer, "End Simulator Configuration File") != STR_EQ) {
        // Free temp struct memory
        free(tempData);

        // Close file access
        fclose(fileAccessPtr);

        // Set end state message to corrupt configuration end line
        char testMsg[] = "Configuration end line capture error";
        copyString(endStateMsg, testMsg);

        // Return corrupt file data
        return false;
    }

    // Test for "file only" output so memory diagnostics do not display
    tempData->memDisplay = tempData->memDisplay && tempData->logToCode != LOGTO_FILE_CODE;

    // Assign temporary pointer to parameter return pointer
    *configData = tempData;

    // Close file access
    fclose(fileAccessPtr);

    // Return no error code
    return true;
}

/*
Name: getpuSchedCode
Process: converts cpu schedule string to code (all scheduling possibilities)
Function Input/Parameters: Lower case code string (const char *)
Function Output Parameters: none
Function Output/Returned: cpu schedule code (ConfigDataCodes)
Device Input/device: none
Device Output/device: none
Dependencies: compareString
*/
ConfigDataCodes getCpuSchedCode( const char *lowerCaseCodeStr )
    {
    // initialize function/variables
        
        // set default return to FCFS-N
        ConfigDataCodes returnVal = CPU_SCHED_FCFS_N_CODE;

    // check for SJF-N
        // function: compareString
    if(compareString( lowerCaseCodeStr, "sjf-n" ) == STR_EQ )
        {
        //set SJF-N code
            returnVal = CPU_SCHED_SJF_N_CODE;
        }
    // check for SRTF-P
    if( compareString( lowerCaseCodeStr, "srtf-p" ) == STR_EQ )
        {
        // function: compareString

        // set SRTF-P code
            returnVal = CPU_SCHED_SRTF_P_CODE;
        }

    // check for FCFS-P
        // function: compareString
    if( compareString( lowerCaseCodeStr, "fcfs-p" ) == STR_EQ )
        {
            // set FCFS-P
            returnVal = CPU_SCHED_FCFS_P_CODE;
        }
    
    // check for RR-P
        // function: compareString
    if( compareString( lowerCaseCodeStr, "rr-p" ) == STR_EQ )
        {
            // set RR-P code
            returnVal = CPU_SCHED_RR_P_CODE;
        }

    // returned the selected value
    return returnVal;
    }
/*
Name: getDataLineCode
Process: converts leader line string to configuration code value
         (all config file leader lines)
Function Input/Parameter: config leader line string (const char *)
Function Output/parameters: none
Function Output/Returned: configuration code value (ConfigCodeMessages)
Device Input/file: none
Device Output/device: none
dependencies: compareString
*/
ConfigCodeMessages getDataLineCode( const char *dataBuffer )
    {
        // return appropriate code depending on prompt string provided
            // function: compareString
        if( compareString( dataBuffer, "Version/Phase" ) == STR_EQ )
            {
                return CFG_VERSION_CODE;
            }
        if( compareString( dataBuffer, "File Path" ) == STR_EQ )
            {
                return CFG_MD_FILE_NAME_CODE;
            }
        if( compareString( dataBuffer, "CPU Scheduling Code" ) == STR_EQ )
            {
                return CFG_CPU_SCHED_CODE;
            }
        if( compareString( dataBuffer, "Quantum Time (cycles)" ) == STR_EQ )
            {
                return CFG_QUANT_CYCLES_CODE;
            }
        if( compareString( dataBuffer, "Memory Display (On/Off)" ) == STR_EQ )
            {
                return CFG_MEM_DISPLAY_CODE;
            }
        if( compareString( dataBuffer, "Memory Available (KB)" ) == STR_EQ )
            {
                return CFG_MEM_AVAILABLE_CODE;
            }
        if( compareString( dataBuffer, "Processor Cycle Time (msec)" ) == STR_EQ )
            {
                return CFG_PROC_CYCLES_CODE;
            }
        if( compareString( dataBuffer, "I/O Cycle Time (msec)" ) == STR_EQ )
            {
                return CFG_IO_CYCLES_CODE;
            }
        if( compareString( dataBuffer, "Log To" ) == STR_EQ )
            {
                return CFG_LOG_TO_CODE;
            }
        if( compareString( dataBuffer, "Log File Path" ) == STR_EQ)
        {
            return CFG_LOG_FILE_NAME_CODE;
        }

        // return corrupt leader line error code
        return CFG_CORRUPT_PROMPT_ERR;
    }

/*
Name: getLogToCode
Process: converts leader line string to configuration code value
         (all config file leader lines)
Function Input/Parameter: config leader line string (const char *)
Function Output/parameters: none
Function Output/Returned: configuration code value (ConfigCodeMessages)
Device Input/file: none
Device Output/device: none
dependencies: compareString
*/
ConfigCodeMessages getLogToCode( const char *lowerCaseLogToStr )
    {
        // initialize function/variables

            // set default to log to monitor
        ConfigDataCodes returnVal = LOGTO_MONITOR_CODE;

        // check for BOTH
            // function: compareString
        if( compareString( lowerCaseLogToStr, "both" ) == STR_EQ )
            {
            // set return value to both code
            returnVal = LOGTO_BOTH_CODE;
            }

        // check for FILE
            // function: compareString
        if( compareString( lowerCaseLogToStr, "file" ) == STR_EQ )
            {
            // set return value to file code
            returnVal = LOGTO_FILE_CODE;
            }

        // return selected code        
        return (ConfigCodeMessages)returnVal;
    }

/*
Name: stripTrailingSpaces
Process: removes trailing spaces from input config leader Lines
Function Input/Parameters: config leader line string (char *)
Function Output/Parameters: updated config leader Line string (char #)
Function Output/Returned: none
Device Input/device: none
Device Output/device: none
Dependencies: getStringLength
*/
void stripTrailingSpaces( char *str )
    {
    // initialize index to length of string - 1 to get highest array index
    int index = getStringLength( str ) - 1;

    // loop while space is found at end of string
    while( str[ index ] == SPACE_CHAR )
        {
        // set element to NULL_CHAR
        str[ index ] = NULL_CHAR;

        // decrement Index
        index--;
        }
    // end loop from end of string
    } 

/*
Name: valueInRange
Process: checks for config data lalues in range, including string values
(all config data values)
Function Input/Parameters: line code number for specific config value (int),
integer value, as needed (int), double value, as needed (double), string value, as needed (const char *)
Function Output/Parameters: none
Function Output/Returned: Boolean result of range test (bool)
Device Input/device: none
Device Output/device: none
Dependencies: compareString
*/
bool valueInRange( int lineCode, int intVal, 
                            double doubleVal, const char *lowerCaseStringVal )
    {
        // initialize function/variables

            // set result to true all tests are to find false
        bool result = true;

        // use line code to identifu promt line
        switch( lineCode )
            {
            // for version code
            case CFG_VERSION_CODE:

                // check if limits of verison code are exceeded
                if( doubleVal < 0.00 || doubleVal > 10.00)
                {
                    // set Boolean result to false
                    result = false;
                }

                // break
                break;

            // for cpu scheduling code
            case CFG_CPU_SCHED_CODE:

                // check for not finding one of the scheduling strings
                    // function compareString
                if( compareString( lowerCaseStringVal, "fcfs-n" ) != STR_EQ
                    && compareString( lowerCaseStringVal, "sjf-n" ) != STR_EQ
                    && compareString( lowerCaseStringVal, "srtf-p" ) != STR_EQ
                    && compareString( lowerCaseStringVal, "fcfs-p" ) != STR_EQ
                    && compareString( lowerCaseStringVal, "rr-p" ) != STR_EQ )
                    {
                    // set Boolean result to false
                    result = false;
                    }

                // break
                break;

            // for quantum cycles
            case CFG_QUANT_CYCLES_CODE:

                // check for quantum cycles limits exceeded
                if( intVal < 0 || intVal > 100 )
                    {
                    // set Boolean result to false
                    result = false;
                    }

                // break
                break;

            // for memory display
            case CFG_MEM_DISPLAY_CODE:

                // check for not finding either "on" or "off"
                    // function: compareString
                if( compareString( lowerCaseStringVal, "on" ) != STR_EQ 
                    && compareString( lowerCaseStringVal, "off" ) != STR_EQ )
                    {
                    // set Boolean result to false
                    result = false;

                    }

                // break
                break;

            // for memory available
            case CFG_MEM_AVAILABLE_CODE:

                // check for available memory limits exceeded
                if( intVal < 1024 || intVal > 102400 )
                {
                    // set Boolean result to false
                    result = false;
                }

                // break
                break;

            // check for process cycles
            case CFG_PROC_CYCLES_CODE:

                // check for process cycles limits exceeded
                if( intVal < 1 || intVal > 100 )
                    {
                    // set boolean result to false
                    result = false;
                    }

                // break
                break;

            // check for I/O cycles
            case CFG_IO_CYCLES_CODE:

                // check for I/O cycles limits exceeded
                if( intVal < 1 || intVal > 1000 )
                    {
                        // set Boolean result to false
                        result = false;
                    }

                // break
                break;

            // check for log to operation
            case CFG_LOG_TO_CODE:

                // check for not finding one of the log to strings
                    // function: compareString
                if( compareString( lowerCaseStringVal, "both" ) != STR_EQ
                    && compareString( lowerCaseStringVal, "monitor" ) != STR_EQ
                    && compareString( lowerCaseStringVal, "file" ) != STR_EQ )
                    {
                    // set boolean result to false
                    result = false;
                    }
                // break
                break;
            }

        // return result of limits analysis
        return result;
    }
