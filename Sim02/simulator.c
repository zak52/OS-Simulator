// header files
#include "simulator.h"



ProcessControlBlock *addPCBNode(ProcessControlBlock *localPtr, ProcessControlBlock *newNode)
{
    // check to see if localPtr is null
    if ( localPtr == NULL )
    {
        // access memory for localPtr
            // function: malloc
        localPtr = ( ProcessControlBlock *)  malloc( sizeof(
            ProcessControlBlock
        ));

        // assign from newNode to localPtr
            // fucntion: copyString
        localPtr->currentOpType = newNode->currentOpType;
        localPtr->nextProcess = NULL;
        localPtr->pid = newNode->pid;
        localPtr->state = newNode->state;
        localPtr->totalTime = newNode->totalTime;

        // return localPtr
        return localPtr;
    }
    // assume end of list not found
    // set localPtr to next node in list
    localPtr->nextProcess = addPCBNode( localPtr->nextProcess, newNode );

    // return localPtr
    return localPtr;
}


/*
Name: checkForTerminated
Process: runs through linked list to see if all PCBs are terminated
Function Input/Parameter: pointer to the head of linkedList
Function Output/parameters: none
Function Output/Returned: true if terminated, false otherwise
Device Input/file: none
Device Output/device: none
dependencies:
*/
bool checkForTerminated( ProcessControlBlock *localHeadPtr )
{

    bool returnBoolean = false;
    ProcessControlBlock *wrkingRef = localHeadPtr;
    while( wrkingRef != NULL )
    {
        if ( wrkingRef->state == TERMINATED )
        {
            returnBoolean = true;
            
        }
        else
        {
            return false;
        }
        wrkingRef = wrkingRef->nextProcess;
    }
    return returnBoolean;
}

/*
Name: changeProcessState
Process: takes in a ptr to a pcb and new state then sets pcb to that state
Function Input/Parameter:ProcessControlBlock *wrkingRef,
                        ProcessState newState, double masterTime,
                        char* destStr
Function Output/parameters: destStr - returns the str that has the message of the change state
                            wrkref - pointer to the updated wrkref
Function Output/Returned: none
Device Input/file: none
Device Output/device: none
dependencies:
*/
void changeProcessState(ProcessControlBlock *wrkingRef,
                        ProcessState newState, double masterTime,
                        char* destStr )
{
    // intialize vairables:
        // old process state   
    ProcessState oldState = wrkingRef->state;
    char oldStateString[STD_STR_LEN];
    char newStateString[STD_STR_LEN];
    char timeString[STD_STR_LEN];
    
    // make the exisiting state into a string for display
    switch(oldState)
    {
        case NEW:
            copyString( oldStateString, "NEW" );
            break;

        case READY:
            copyString( oldStateString, "READY" );
            break;

        case RUNNING:
            copyString( oldStateString, "RUNNING" );
            break;

        case WAITING:
            copyString( oldStateString, "WAITING" );
            break;

        case TERMINATED:
            copyString( oldStateString, "TERMINATED" );
    }

    // make the new state into a printable string
    switch (newState)
    {
        case NEW:
            copyString( newStateString, "NEW" );
            break;

        case READY:
            copyString( newStateString, "READY" );
            break;

        case RUNNING:
            copyString( newStateString, "RUNNING" );
            break;

        case WAITING:
            copyString( newStateString, "WAITING" );
            break;

        case TERMINATED:
            copyString( newStateString, "TERMINATED" );
            
    }

    // change the exisiting state inside the process to the new one
    wrkingRef->state = newState;

    if (masterTime != 0.0 )
    {
        // access the timer to get time of change
        masterTime = accessTimer(LAP_TIMER, timeString);

        // set message to destination message
        sprintf(destStr, "%s, OS: Process %d set to %s from %s state \n", timeString,
        wrkingRef->pid, newStateString, oldStateString);
    }
    else
    {
        // set message to destination message
        sprintf(destStr, " 0.000000, OS: Process %d set to %s from %s state \n",
        wrkingRef->pid, newStateString, oldStateString);
    }
}

/*
Name: createPCBLinkedList
Process: takes in the pointer to the metadata struct and config struct and makes a linked
         list of pcb nodes
Function Input/Parameter: OpCodeType *metaData, ConfigDataType *config
Function Output/parameters: none
Function Output/Returned: head ptr to the PCB linked list
Device Input/file: none
Device Output/device: none
dependencies:
*/
ProcessControlBlock *createPCBLinkedList(OpCodeType *metaData, ConfigDataType *config)
{
    // initialize variables
    ProcessControlBlock *localHeadPtr = NULL;
    OpCodeType *wrkingRef = metaData;
    ProcessControlBlock *newNodePtr;
    int pidAssignment = 0;

    // loop through metadata opcodes
    while (wrkingRef != NULL)
    {


        // check for app and start command
            // function: compareString
        if( compareString(wrkingRef->command, "app" ) == STR_EQ 
            && compareString(wrkingRef->strArg1, "start" ) == STR_EQ )
        {
            
            // allocate memory for the temp data structure
                // function: malloc
            newNodePtr = (ProcessControlBlock *)malloc( sizeof( ProcessControlBlock ) );

   

            // inialize new node with pid, state, total time and currentOpCode assigned to next node, nextNode = NULL
            newNodePtr->currentOpType =wrkingRef->nextNode;
            newNodePtr->nextProcess = NULL;
            newNodePtr->pid = pidAssignment;
            newNodePtr->state = NEW;
            newNodePtr->totalTime=0;
                
        }
        // else if check for the end of process
            // function compareString
        else if( compareString( wrkingRef->command, "app" ) == STR_EQ 
                && compareString( wrkingRef->strArg1, "end" ) == STR_EQ )
        {
            if( newNodePtr != NULL )
            {
            // increment pidAssignment
            pidAssignment += 1;
            // add newNode
            localHeadPtr = addPCBNode(localHeadPtr, newNodePtr);
            }
            else {
                
                free(newNodePtr);
                printf( "Error creating new Node" );
                return NULL;

            }
        }
        // else assigning programs
        else
        {
            if( compareString( wrkingRef->command, "cpu" ) == STR_EQ )
            {
            // check to see if cpu command
                // function compareString

                // assign the totalProccessTime intArg2 * procCycleRates
                newNodePtr->totalTime += wrkingRef->intArg2 * config->procCycleRate;
            }
            else if ( compareString( wrkingRef->command, "mem" ) != STR_EQ )
            {
            // else check if not memory command, then i/o command
                // function: compareString

                // assign the totalProcessTime intArg2 * ioCycleRates
                newNodePtr->totalTime += wrkingRef->intArg2 * config->ioCycleRate;
            }
            
        }

        // set wkgingRef to next node in OpCode
        wrkingRef = wrkingRef->nextNode;
    }
    // end loop
    
    // free any memory
    free(newNodePtr);

    // return head of linkedList
    return localHeadPtr;
}

/*
Name: clearPCBLinkedList
Process: clears the PCBLinkedList at the end of its use so there is no memory leakage
Function Input/Parameter: ProcessControlBlock pointing to head of linked list
Function Output/parameters: none
Function Output/Returned: empty pointer to an empyt PCB linkedList
Device Input/file: none
Device Output/device: none
dependencies: free, clearPCBLinkedList
*/
ProcessControlBlock *clearPCBLinkedList(ProcessControlBlock *localPtr )
{
    // check if local pointer is set to NULL (list is empty)
    if( localPtr != NULL )
    {
        // call itself for recursive functionaility
        clearPCBLinkedList(localPtr->nextProcess);

        // release mememory
            // function: free
        free(localPtr);

        // set localPtr to NULL
        localPtr = NULL;
    }

    // return NULL at end. 
    return NULL;
}

/*
Name: displayToType
Process: takes in the String linked list, a display string and to booleans to 
         dictate if the code is being display to the monitor, log, or both
         if to monitor: prints to screen
         if to log: stores in linked list
         if both: does both
Function Input/Parameter: StringNode* localHeadPtr - pointer to local string linked list
                          char *displayString - pointer to string being displayed or stored
                          bool displayToMonitor, bool displayToLog
Function Output/parameters: none
Function Output/Returned: StringNode pointer to be as the new head for the String linkedlist
Device Input/file: none
Device Output/device: none
dependencies: printf, copyString, malloc, free
*/
StringNode *displayToType(StringNode* localHeadPtr, char *displayString, bool displayToMonitor, bool displayToLog )
{
    // check to see if displayToMonitor is true
    if( displayToMonitor )
    {
        // display message to user in terminal
            // fucntion: printf
        printf("%s", displayString);
    }
    // check to see if displayToLog
    if( displayToLog )
    {
        // iniatlize varibles as needed
        StringNode *newNode;

        newNode = ( StringNode * ) malloc( sizeof( StringNode ) );

        copyString(newNode->messageStr, displayString);

        // add displayString to the StringLinkedList
            // function: insertStringNode        
        localHeadPtr = insertStringNode(localHeadPtr, newNode);

        // free newNode
        free(newNode);

        return localHeadPtr;
    }

    return localHeadPtr;

}

/*
Name: makeLogFile
Process: take in a linked list of strings and the specified file name and 
         dumps the linked list to the file all at one time
Function Input/Parameter: char* filename - string file name
                          StringNode* headPtr - localptr to the head of linked list
Function Output/parameters: none
Function Output/Returned: true on successful input, false on unsuccessful input
Device Input/file: none
Device Output/device: none
dependencies: fopen, fclose, fprintf
*/
bool makeLogFile(char* fileName,  StringNode* headPtr )
{
    // iniatilize variables
        // create file pointer
    FILE *logFile;
    char message[MAX_STR_LEN];  
        // set wrkingref for stringNode linkedlist
    StringNode* wrkingRef = headPtr;

    // open file for writing
        // function: fopen
    logFile = fopen(fileName, "w");

    // check to see if file was open properly
    if ( logFile == NULL || headPtr == NULL )
    {
        printf("file print error. \n");
        fprintf(logFile, "%s",  message);
        // return false if failed
        return false;
    }
    // loop through linked list until NULL
    while( wrkingRef != NULL )
    {
        //function: fprintf
        fprintf(logFile, "%s",  wrkingRef->messageStr);
        wrkingRef = wrkingRef->nextNode;
    }
    // end loop

    // close file
        // fucntion: fclose
    fclose(logFile);

    return true;
    
}

/*
Name: getCurrentPCB
Process: checks the scheduling job; then dictates what the next PCB that should be up
         dictated by the scheduling
Function Input/Parameter: int code - with the scheduling job
                          ProcessControlBlock pointing to the head of the PCB linked list
Function Output/parameters: none
Function Output/Returned: return the currentPCB next in the scheduler
Device Input/file: none
Device Output/device: none
dependencies: configCodeToString, compareString
*/
ProcessControlBlock* getCurrentPCB(int code, ProcessControlBlock *localPtr)
{
    // iniatilize variables
    char configCodeStr[STD_STR_LEN];
    ProcessControlBlock *wrkingRef = NULL;
    configCodeToString(code, configCodeStr);
    
    // check for configcode code to know what sched job to do
    if(compareString(configCodeStr, "FCSF-N") && !checkForTerminated(localPtr))
    {
        wrkingRef = localPtr;
        while(wrkingRef->state == TERMINATED )
        {
            wrkingRef = wrkingRef->nextProcess;
        }
        return wrkingRef;
    }
    else if(compareString(configCodeStr, "FCSF-P") && !checkForTerminated(localPtr))
    {
        wrkingRef = localPtr;
        while(wrkingRef->state == TERMINATED )
        {
            wrkingRef = wrkingRef->nextProcess;
        }
        return wrkingRef;

    }
    else if(compareString(configCodeStr, "SJF-N") && !checkForTerminated(localPtr))
    {
        wrkingRef = localPtr;
        while(wrkingRef->state == TERMINATED )
        {
            wrkingRef = wrkingRef->nextProcess;
        }
        return wrkingRef;
    }
    else if(compareString(configCodeStr, "RR-P") && !checkForTerminated(localPtr))
    {
        wrkingRef = localPtr;
        while(wrkingRef->state == TERMINATED )
        {
            wrkingRef = wrkingRef->nextProcess;
        }
        return wrkingRef;

    }
    else if (compareString(configCodeStr, "SRTF-P") && !checkForTerminated(localPtr))
    {
        wrkingRef = localPtr;
        while(wrkingRef->state == TERMINATED )
        {
            wrkingRef = wrkingRef->nextProcess;
        }
        return wrkingRef;

    }
    else
    {
        return NULL;
    }

    return wrkingRef;

}



/*
Name: runSim
Process: primary simulator driver
Function Input/Parameter: configuration data (ConfigDataType *),
                          metadata (OpCodeType *)
Function Output/parameters: none
Function Output/Returned: none
Device Input/file: none
Device Output/device: none
dependencies: printf, createPCBLinkedList, displayType, 
accessTimer, 
*/
void runSim( ConfigDataType *configPtr, OpCodeType *metaDataMstrPtr )
{
    
    // Initalize Variables
        //printf("\n   Running Simulator:   \n");
        // printf("==========================\n");
        // set current process to null
        ProcessControlBlock *currentProcess  = NULL;
        ProcessControlBlock *localPCBHead = NULL;
        // set display to monitor, set display to log
        bool displayToLog = false, displayToMonitor = false;
        char displayMessage[MAX_STR_LEN];
        char timeAsString[STD_STR_LEN];
        double masterTime = 0.0;
        char displayType[MAX_STR_LEN];
        configCodeToString( configPtr->logToCode, displayType);
        char cpuSchedString[MAX_STR_LEN];
        StringNode* logMessageHead = NULL;
        configCodeToString(configPtr->cpuSchedCode , cpuSchedString);
    

    // display log message to user

        // if sim displaying to both
        if( compareString (displayType, "Both" ) == STR_EQ )
        {
            displayToLog = true;
            displayToMonitor = true;
        // if only displaying to monitor
        } else if(compareString (displayType, "Monitor" ) == STR_EQ)
        {
            displayToLog = false;
            displayToMonitor = true;
        // if only displaying to log
        } else if( compareString (displayType, "File" ) == STR_EQ )
        {
            displayToLog = true;
            displayToMonitor = false;

            // display to user if only display to log
            // printf
            printf( "\nSimulator only displaying to log, Please wait ...\n" );
        }

        // display header for file and simiultor running:
            // displayToType
            logMessageHead = displayToType(logMessageHead, "\nRunning Simulator with Specified configuration and Metadata:\n", displayToMonitor, displayToLog);
            logMessageHead = displayToType(logMessageHead, "=============================================================\n\n", displayToMonitor, displayToLog);
            

        // set master time
        //masterTime = accessTimer(ZERO_TIMER, timeAsString);
        // display start if simulator:
            // function : displayType, copyString
        copyString(displayMessage, " 0.000000, OS: Simulator Start\n");
        displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

        // create PCB queue: first in first out operation
            // function createPCBQueue
        localPCBHead = createPCBLinkedList(metaDataMstrPtr, configPtr);

        ProcessControlBlock *wrkingRef = localPCBHead;
        // loop through PCB queue wrking ref
        while( wrkingRef != NULL )
        {
            // set PCBs in queue to READY state
                // changeProcessState
            changeProcessState(wrkingRef, READY, masterTime, displayMessage);
            logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);
            wrkingRef = wrkingRef->nextProcess;
        }
        // end loop

        // free wrkingref
        free( wrkingRef );

        currentProcess = getCurrentPCB(configPtr->cpuSchedCode, localPCBHead);

        // set wrking OpCode
        OpCodeType *wrkingOpCode = currentProcess->currentOpType;

        // log process of time
        sprintf( displayMessage, "\n 0.000000, OS: Process %d selected with %d ms remaining\n", 
            currentProcess->pid, currentProcess->totalTime );
        logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

        masterTime = accessTimer(ZERO_TIMER, timeAsString);

        // set from waiting to running
        changeProcessState(currentProcess, RUNNING, masterTime, displayMessage);

        // display change to preferred place
        logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);



        // loop through process queue
        while(!checkForTerminated(localPCBHead) && currentProcess != NULL)
        {

            // if app end set process to terinate and display or log to user
                // function: changeProcessState, displayType
            if( compareString( wrkingOpCode->command, "app" ) == 
                STR_EQ && compareString( wrkingOpCode->strArg1, "end" ) == STR_EQ)
            {
                // display end of process
                changeProcessState(currentProcess, TERMINATED, masterTime, displayMessage);

                // display change to preferred place
                logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                masterTime = accessTimer(LAP_TIMER, timeAsString);
                sprintf(displayMessage, "%s, OS: Process %d ended\n\n", timeAsString, currentProcess->pid);
                logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);
                // set process to terminated
                    // function: changeProcessState

                // set current process to the next process in the queue
                currentProcess = getCurrentPCB(configPtr->cpuSchedCode, localPCBHead);
                if(currentProcess != NULL)
                {
                    // if current process is at end of queue
                        // change states to running and dislay to user or fuke
                            // function: changeProcessStatem displayType
                    masterTime = accessTimer( LAP_TIMER, timeAsString );
                    sprintf( displayMessage, "%s, OS: Process %d selected with %d ms remaining \n",
                        timeAsString, currentProcess->pid, currentProcess->totalTime );
                    logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                    // set from waiting to running
                    changeProcessState(currentProcess, RUNNING, masterTime, displayMessage);

                    // display change to preferred place
                    logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);
                }
            }
            // else run the op code type
            else
            {
                // function: Program
                // check for dev command
                    // function: compareString
                if(compareString( wrkingOpCode->command, "dev" ) == STR_EQ )
                {
                    // check for input
                        // function: compareString
                    if( compareString (wrkingOpCode->inOutArg, "in" ) == STR_EQ )
                    {
                        // get start time
                            // function: accessTimer
                        masterTime = accessTimer(LAP_TIMER, timeAsString);
                        
                        sprintf(displayMessage, "%s, OS: Process %d %s input opertion start\n", timeAsString, currentProcess->pid, 
                        currentProcess->currentOpType->strArg1);

                        // display end time to preferred place
                        logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);


                        // start a timer thread to for I/O operation
                            // function: startTimeThread
                        startTimerThread(wrkingOpCode->intArg2 * configPtr->ioCycleRate);

                        // get end time
                            // function: accessTimer
                        masterTime = accessTimer(LAP_TIMER, timeAsString);


                        sprintf(displayMessage, "%s, OS: Process %d %s input opertion end\n", timeAsString, currentProcess->pid, 
                        currentProcess->currentOpType->strArg1);

                        // display end time to preferred place
                        logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                    }
                    else
                    {
                        // else assume output

                            // set starter time
                                // function: accessTimer
                            masterTime = accessTimer(LAP_TIMER, timeAsString);

                            // display to preferred place
                            sprintf(displayMessage, "%s, OS: Process %d %s output operation start\n", timeAsString, 
                                    currentProcess->pid, currentProcess->currentOpType->strArg1);
                            
                            logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                            // run timer I/O 
                                // startTimeThread
                            startTimerThread(wrkingOpCode->intArg2 * configPtr->ioCycleRate);

                            // get end time
                            masterTime = accessTimer(LAP_TIMER, timeAsString);

                            // display to preferred place
                            sprintf(displayMessage, "%s, OS: Process %d %s output operation end\n", timeAsString, 
                                    currentProcess->pid, currentProcess->currentOpType->strArg1);
                            
                            logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);


                        }
                    }
                    // check if cpu command
                    else if( compareString( wrkingOpCode->command, "cpu" ) == STR_EQ )
                    {
                        // get starting time
                        masterTime = accessTimer(LAP_TIMER, timeAsString);

                        // display to preferred place
                        sprintf(displayMessage, "%s, OS: Process %d %s proccess operation start\n", timeAsString, 
                                    currentProcess->pid, currentProcess->currentOpType->command);
                        logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                        // create a timer thread for cpu operation
                            // starttimerthread
                        startTimerThread(wrkingOpCode->intArg2 * configPtr->procCycleRate);

                        // get ending time
                        masterTime = accessTimer(LAP_TIMER, timeAsString);

                        // display to preferred place
                        sprintf(displayMessage, "%s, OS: Process %d %s proccess operation end\n", timeAsString, 
                                    currentProcess->pid, currentProcess->currentOpType->command);

                        // display end time to user
                        logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                    }
                    // assume memory command
                    else
                    {
                        // get starter time
                        masterTime = accessTimer(LAP_TIMER, timeAsString);

                        // display memory allocation request
                        sprintf(displayMessage, "%s, OS: Process %d %s memory allocate request (%d, %d)\n", timeAsString, 
                                    currentProcess->pid, currentProcess->currentOpType->command,
                                    wrkingOpCode->intArg2, wrkingOpCode->intArg3);
                        logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                        
                    }

                // set currentOpType to the next node
                currentProcess->currentOpType = wrkingOpCode->nextNode;
            }

            if( currentProcess != NULL )
            {
                wrkingOpCode = currentProcess->currentOpType;
            }
        }
        // end loop

        // check if displaying to log
        if (displayToLog)
        {
            // function: makeLogFile
            makeLogFile(configPtr->logToFileName, logMessageHead);
        }

        // display end of program
            // function: displayToType, copyString
            copyString( displayMessage, "\nEnd Of Simulator\n\n");
            logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

        // free queue memory
            // function: freePcbQueue
            currentProcess = clearPCBLinkedList(currentProcess);
}

/*
Name: startTimerThread
Process: takes int runTime and creates a thread for that amount of time
Function Input/Parameter: int runtime
Function Output/parameters: none
Function Output/Returned: none
Device Input/file: none
Device Output/device: none
dependencies: pthread_attr_init, pthread_create, pthread_join
*/
void startTimerThread(int runTime)
{
    // initialize variables
    pthread_t thread;
    pthread_attr_t threadAttr;
    int time = runTime, *timePtr;
    void *threadParameter;

    // iniatilize attribute
    pthread_attr_init(&threadAttr);

    // make a void star point for pthread_create
    timePtr = &time;
    threadParameter = (void *) timePtr;

    // create thread
    pthread_create(&thread, &threadAttr, runTimerThread,
     threadParameter);

    // exit thread
    pthread_join(thread, NULL);
}

/*
Name: Runtimer thread
Process: takes in void start runtime and runs a timer thread for that amount of time
Function Input/Parameter: void * runTime pointer
Function Output/parameters: none
Function Output/Returned: none
Device Input/file: none
Device Output/device: none
dependencies: runTimer, pthread_exit
*/
void * runTimerThread(void *runTime)
{
    // initialize variables
        // cast variable from void point to a int pointer
    int *timePtr = (int *) runTime;
    int time = *timePtr;

    runTimer(time);

    pthread_exit(0);

}
