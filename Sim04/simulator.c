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
        localPtr->pid = newNode->pid;
        localPtr->state = newNode->state;
        localPtr->totalTime = newNode->totalTime;
        
        localPtr->nextProcess = NULL;
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
Name: accessMemBlock
Process: runs through the linked list to see if their is a memory block that can be access
         for the requested memory
Function Input/Parameter: pointer to the head of linkedList, int begMemAdfress, int memory offset
                          current process that is running
Function Output/parameters: none
Function Output/Returned: true if access granted, false if failed
Device Input/file: none
Device Output/device: none
dependencies: none
*/
bool accessMemBlock(MemoryManager **headPtr, int begAddress, int memOffset, int currentProcessID)
{
    // check to see if system memory has initialize
    if (headPtr == NULL || *headPtr == NULL)
    {
        // linked list is empty auto failure
        return false;
    }

    // initialize variables
    MemoryManager* wrkingRef = *headPtr;
    int endMemReqAccess = begAddress + memOffset;
    int endAddressExistingMemBlock;

    // loop through linked list starting with the first memory block
    while (wrkingRef != NULL)
    {
        // get the end address of the wrkingRef
        endAddressExistingMemBlock = wrkingRef->allocatedSize 
                + wrkingRef->begAddress;
        
        // test to see if the requested memory block is inside the
        // correct process
        if( wrkingRef->pid == currentProcessID )
        {
            // check to see if the requested memory falls
            // within the allocated block
            if(begAddress >= wrkingRef->begAddress 
                && endMemReqAccess <= endAddressExistingMemBlock)
            {
                // access successful
                return true;
            }

        }

        // iterate to next in linked list
        wrkingRef = wrkingRef->nextMemoryLoc;

    }
    

    // access failed
    return false;
}


/*
Name: allocateMemBlock
Process: takes in head of memory manager, beginning address of memory, and offsets
    to see if the memory block can be allocated in the memory manager
Function Input/Parameter: pointer to the head of linkedList, int beginning address, int offset memory
Function Output/parameters: none
Function Output/Returned: true if allocated, false if segmentation fault
Device Input/file: none
Device Output/device: none
dependencies:
*/
bool allocateMemBlock(MemoryManager **headPtr, int newBegAddress, int memOffset, int currentPid)
{
    // check to see if system memory has initialize
    if (headPtr == NULL || *headPtr == NULL)
    {
        return false;
    }

    // initialize variables
    MemoryManager* head = *headPtr;
    MemoryManager* wrkingRef = *headPtr;
    MemoryManager* newMemBlock = NULL;
    int endAddressExistingMemBlock;
    int endAddressNewMemBlock = newBegAddress + memOffset;
    int totalSysMem = wrkingRef->allocatedSize;
    int usedMem = wrkingRef->begAddress;

    // check if their is enough memory inside the system
        // and not out of bounds
    if( memOffset > ( totalSysMem - usedMem ) 
        || ( (memOffset + newBegAddress) > totalSysMem ))
    {
        return false;
    }

    // set the memory block to the next in the system
    wrkingRef = wrkingRef->nextMemoryLoc;

    // loop to check for overlapping blocks
    while (wrkingRef != NULL)
    {
        // set current blocks end address using allocatedsize and beg address
        endAddressExistingMemBlock = wrkingRef->allocatedSize + wrkingRef->begAddress;

        // check to see if there is any overlap
        if(!(endAddressNewMemBlock <= wrkingRef->begAddress ) 
            || (newBegAddress >=endAddressExistingMemBlock ) )
        {
            return false;
        }
        
        wrkingRef = wrkingRef->nextMemoryLoc;

    }

    // allocate new memory block
    newMemBlock = (MemoryManager*)malloc(sizeof(MemoryManager));

    // check to make sure the block was successfully allocated
    if(newMemBlock == NULL)
    {
        return false;
    }
    
    // initialize variables inside new memory block
    newMemBlock->begAddress = newBegAddress;
    newMemBlock->allocatedSize = memOffset;
    newMemBlock->pid = currentPid;
    newMemBlock->nextMemoryLoc = NULL;
    
    // loop through the next empty spot in linked list
    head->begAddress += memOffset;
    wrkingRef = head;
    while (wrkingRef->nextMemoryLoc !=NULL)
    {
        wrkingRef = wrkingRef->nextMemoryLoc;
    }
    wrkingRef->nextMemoryLoc = newMemBlock;
    
    // return successful allocation
    return true;
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
Name: checkInterrupts
Process: checks to if there is an inturrupt waiting in the inturrupt
         linked list
Function Input/Parameter:InterruptManager *wrkingIntrrptMngr, int masterTime, char* destStr,
  StringNode* logMessageHead, char* displayMessage, bool displayToMonitor, bool displayToLog
Function Output/parameters: none
Function Output/Returned: bool of interrupt or not
Device Input/file: none
Device Output/device: none
dependencies: pthread_mutex_lock, 
*/
bool checkInterrupts(InterruptManager *wrkingIntrrptMngr, int masterTime, char* destStr,
                    StringNode* logMessageHead, char* displayMessage, bool displayToMonitor, bool displayToLog)
 {
    // locks the mutex just incase multiple access to data
    pthread_mutex_lock(&wrkingIntrrptMngr->mut_lock);
    
    // get the linkedlist from the interrupt manager
    InteruptLinkedListNode *wrkingIntrrptLL = wrkingIntrrptMngr->interuptLL;
    
    // used for displaying if inturrupt happens
    char timeAsString[STD_STR_LEN];
    
    // make sure that the current process is not null
    if(wrkingIntrrptLL->currentProcess != NULL)
    {
    
    	// lap time to be displayed
        masterTime = accessTimer(LAP_TIMER, timeAsString);

	// test to see if the operation was an in or an out
        if( compareString (wrkingIntrrptLL->currentProcess->currentOpType->inOutArg, "in" ) == STR_EQ )
        {
            sprintf(displayMessage, "\n%s, Interrupt by Process %d %s input opertion\n\n", timeAsString, 
                                                wrkingIntrrptLL->currentProcess->pid, wrkingIntrrptLL->currentProcess->currentOpType->command);
        }
        else
        {
            sprintf(displayMessage, "\n%s, Interrupt by Process %d %s output opertion\n\n", timeAsString, 
                                                wrkingIntrrptLL->currentProcess->pid, wrkingIntrrptLL->currentProcess->currentOpType->command);
        }
        
        // display to user
        logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

        masterTime = accessTimer(LAP_TIMER, timeAsString);

	// change state back to ready
        changeProcessState(wrkingIntrrptLL->currentProcess, READY, masterTime, destStr);

        logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);
        
        // remove from inturrpt linked list
        wrkingIntrrptMngr->interuptLL = wrkingIntrrptLL->next;
        free(wrkingIntrrptLL);

	// unlock mutex
        pthread_mutex_unlock(&wrkingIntrrptMngr->mut_lock);
        return true;
    }

    // unlock mutex
    pthread_mutex_unlock(&wrkingIntrrptMngr->mut_lock);
    return false;
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
        sprintf(destStr, "\n 0.000000, OS: Process %d set to %s from %s state",
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
    ProcessControlBlock *newNodePtr = NULL;
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
            if( newNodePtr == NULL){
		return NULL;
	    }

   

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
            free(newNodePtr);
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
                if(newNodePtr != NULL){
                	newNodePtr->totalTime += wrkingRef->intArg2 * config->ioCycleRate;
                }
            }
            
        }

        // set wkgingRef to next node in OpCode
        wrkingRef = wrkingRef->nextNode;
    }
    // end loop
    

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
    if( displayToMonitor)
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
        
        if (newNode == NULL)
        {
        	return localHeadPtr;
        }

        copyString(newNode->messageStr, displayString);

        // add displayString to the StringLinkedList
            // function: insertStringNode        
        localHeadPtr = insertStringNode(localHeadPtr, newNode);


        return localHeadPtr;
    }
    return localHeadPtr;

}

/*
Name: freeMemBlock
Process: takes in a PTR to head of memory manager linked list, and PID
frees all the memory assigned to that process
Function Input/Parameter: pointer to the head of linkedList, PID with memory we are clearing
Function Output/parameters: none
Function Output/Returned: true if worked, false otherwise
Device Input/file: none
Device Output/device: none
dependencies:
*/
bool freeMemBlock( MemoryManager** headMemManagerPTR, int currentPID )
{
    // initialize variables
    MemoryManager* wrkingRef = *headMemManagerPTR;
    MemoryManager* headptr = *headMemManagerPTR;
    MemoryManager* prevWrkingRef = NULL;
    MemoryManager* memBlockToFree = NULL;

    // loop through link list to find all with the current PID
    while( wrkingRef != NULL )
    {
        // check to see if the PID matches
        if ( wrkingRef->pid == currentPID )
        {
            memBlockToFree = wrkingRef;
            
            // check to see if there was a block before it thats not the head
            if(prevWrkingRef != NULL )
            {
                // rearrange the linked list to cut out this memory block
                prevWrkingRef->nextMemoryLoc = wrkingRef->nextMemoryLoc;
                headptr->begAddress = headptr->begAddress - wrkingRef->allocatedSize;
            }
            // otherwise; the wrkingRef is the head and needs to be cleared
            else
            {
                *headMemManagerPTR = wrkingRef->nextMemoryLoc;
            }

            // iterate to the next block
            wrkingRef = wrkingRef->nextMemoryLoc;
            
            // free the memory
            free(memBlockToFree);

        }
        else
        {
            // assign the prevwrkingRef iterate to the next block
            prevWrkingRef = wrkingRef;
            wrkingRef = wrkingRef->nextMemoryLoc;
        }

    }

    return true;

}

/*
Name: initializeInterruptManager
Process: takes in empty inturruptmanager and initializes it
Function Input/Parameter: InterruptManager *wrkingRef
Function Output/parameters: none
Function Output/Returned: none
Device Input/file: none
Device Output/device: none
dependencies: malloc
*/
void initializeInterruptManager(InterruptManager *wrkingRef) {
    
    // create the interrupt Linked list set the head to null
    wrkingRef->interuptLL = NULL;

    if( wrkingRef->interuptLL == NULL )
    {
        wrkingRef->interuptLL = ( InteruptLinkedListNode *)  malloc( 
            sizeof(InteruptLinkedListNode));

        wrkingRef->interuptLL->currentProcess = NULL;
        wrkingRef->interuptLL->next = NULL;

    }
    
    // initilize the the mutex lock
    pthread_mutex_init(&wrkingRef->mut_lock, NULL);
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
Name: runioOperation
Process: takes in a void * arguments, mostly passing in an IO argument struct
runs the timer in the background until finished and once finished adds to inturrupt linked list
Function Input/Parameter:
Function Output/parameters: none
Function Output/Returned: none
Device Input/file: none
Device Output/device: none
dependencies: runTimer, pthread_mutex_unlock, pthread_mutex_lock,
              malloc
*/
void *runIOOperation(void *arguments) {

    // change arguments back to IO arguments to be used
    IOThreadArguments *args = (IOThreadArguments *)arguments;

    int *timePtr = &args->runTime;

    int time = *timePtr;
    
    // run the timer in background for the IO operation
    runTimer(time);  

    // use the mutex inside the inturrpt manager to protect the data inside the current process
    pthread_mutex_lock(&args->intrrptMnger->mut_lock);
    
    // once completed create a new node to be added to the interupt linked list
    InteruptLinkedListNode *newIntrrpt = malloc(sizeof(InteruptLinkedListNode));
    newIntrrpt->currentProcess = args->currentProcess;
    newIntrrpt->next = args->intrrptMnger->interuptLL;
    args->intrrptMnger->interuptLL = newIntrrpt;
    
    // unlock mutex so data is no longer protected
    pthread_mutex_unlock(&args->intrrptMnger->mut_lock);

    // free the allocated args to protect data
    free(args);

    return NULL;
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
ProcessControlBlock* getCurrentPCB(char *cpuSchedString, ProcessControlBlock *localPtr, ProcessControlBlock *curentProcess)
{
    // iniatilize variables
        ProcessControlBlock *wrkingRef = NULL;
        ProcessControlBlock *shortestRemainingNode = NULL;

    // check for configcode code to know what sched job to do
    if(compareString(cpuSchedString, "FCFS-N") == STR_EQ && !checkForTerminated(localPtr))
    {
        wrkingRef = localPtr;
        while(wrkingRef->state == TERMINATED )
        {
            wrkingRef = wrkingRef->nextProcess;
        }
        return wrkingRef;
    }
    else if(compareString(cpuSchedString, "FCFS-P")  == STR_EQ && !checkForTerminated(localPtr))
    {
    	// set wrkingref to head of linked list 
		wrkingRef = localPtr;
	
	// loop through till you find a process that isnt block or terminated
		while (wrkingRef != NULL && (wrkingRef->state == TERMINATED || wrkingRef->state == WAITING))
		{
		    wrkingRef = wrkingRef->nextProcess;
		}

	// if wrkingref == null then return the head of link list
	if(wrkingRef == NULL )
	{
	     return localPtr;
	}
	
	
	return wrkingRef;  //

    }
    else if(compareString(cpuSchedString, "SJF-N")  == STR_EQ && !checkForTerminated(localPtr))
    {
        wrkingRef = localPtr;

        
        while(wrkingRef != NULL )
        {
            if (wrkingRef->state != TERMINATED && 
                    (shortestRemainingNode == NULL || wrkingRef->totalTime < shortestRemainingNode->totalTime))
            {
                shortestRemainingNode = wrkingRef;
            }
            wrkingRef = wrkingRef->nextProcess;
        }
        return shortestRemainingNode;
    }
    else if(compareString(cpuSchedString, "RR-P")  == STR_EQ && !checkForTerminated(localPtr))
    {
        // make sure the currentprocess isn't null; if null return local ptr
        if(curentProcess == NULL && localPtr->state != TERMINATED )
        {
            return localPtr;
        }

        // check to see if the next process is null or not
        if(curentProcess->nextProcess != NULL )
        {
            // set wrkingref to the next process
            wrkingRef = curentProcess->nextProcess;
        }
        // otherwise set the process to the localptr head
        else
        {
            wrkingRef = localPtr;
        }

        int startingPID = wrkingRef->pid;

        // using a do loop to check last make sure the startingPID isn't the same
        // as the wrkref pid
        do
        {
            // check to see if the current wrkingref is in the ready state
            if( wrkingRef->state == READY )
            {
                // return wrkingref
                return wrkingRef;
            }
            // move to the next process; check to see if at end of list
            if( wrkingRef->nextProcess != NULL )
            {
                wrkingRef = wrkingRef->nextProcess;
            }
            else
            {
                wrkingRef = localPtr;
            }

        } while ( startingPID != wrkingRef->pid );
        
        // returns NULL if no process is available; CPU IDLE.
        return curentProcess;

    }
    else if (compareString(cpuSchedString, "SRTF-P")  == STR_EQ && !checkForTerminated(localPtr))
    {
    	    // set wrkingref to head
	    wrkingRef = localPtr;
	    // set shortest to null
	    shortestRemainingNode = NULL;
	    
	    // loop through to end of linked list
	    while (wrkingRef != NULL)
	    {
	    // make sure that wrkingref isnt terminated or blocked
		if ((wrkingRef->state != TERMINATED && wrkingRef->state != WAITING) && 
		    (shortestRemainingNode == NULL || wrkingRef->totalTime < shortestRemainingNode->totalTime))
		{
		    shortestRemainingNode = wrkingRef;
		}
		wrkingRef = wrkingRef->nextProcess;
	    }

	    // if null just return head
	    if (shortestRemainingNode == NULL)
	    {
		return localPtr; 
	    }

	    // return shortest remaining node
	    return shortestRemainingNode;

    }
    else
    {
        return NULL;
    }

    return wrkingRef;

}

/*
Name: intializeSysAvailableMemory
Process: takes in the available memory size and creates a base memory for the system
Function Input/Parameter: integer of available memory size
Function Output/parameters: none
Function Output/Returned: pointer to the memory manager
Device Input/file: none
Device Output/device: none
dependencies: malloc
*/
MemoryManager* intializeSysAvailableMemory(int availableMemSize )
{
    // allocated memory for a memory manager head of linked list
    MemoryManager* memMgrHead = malloc(sizeof(MemoryManager));

    // check to see if head is null
    if( memMgrHead != NULL )
    {
        memMgrHead->allocatedSize = availableMemSize;
        memMgrHead->begAddress = 0;
        memMgrHead->pid = -1;
        memMgrHead->nextMemoryLoc = NULL;
    }
    // can't create more system memory, memory already initilized
    else
    {
        // free created memory manager
        free(memMgrHead);
    }

    // return head of memory manager linked list
    return memMgrHead;
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
            MemoryManager *MemoryManagerHead = NULL;

        // set display to monitor, set display to log
            bool displayToLog = false, displayToMonitor = false, displayMemMngmnt = configPtr->memDisplay;

        // set the init variables for displaying
            char displayMessage[MAX_STR_LEN];
            char timeAsString[STD_STR_LEN];
            double masterTime = 0.0;
            char displayType[MAX_STR_LEN];
            configCodeToString( configPtr->logToCode, displayType);
        
        // get the current scheduling code/variables
            char cpuSchedString[MAX_STR_LEN];
            StringNode* logMessageHead = NULL;
            configCodeToString(configPtr->cpuSchedCode , cpuSchedString);
            int quantum = configPtr->quantumCycles;
            bool CPUIDLE = false;
            int previousProcessNumber = -1;
            bool premeptive = false;
            int cycleTime = 0;
            int wrkingQuatum = 0;
       
        // set the memory information
            int maxMem = 0;

        // find out if the system is premeptive
        if(compareString(cpuSchedString, "SRTF-P") == STR_EQ || compareString(cpuSchedString, "FCFS-P") == STR_EQ 
                        || compareString(cpuSchedString, "RR-P") == STR_EQ)
        {
            premeptive = true;
        }

        // set up the interrupt manager
            // function: initializeInterruptManager;
            InterruptManager intrrptMngr;
        
        initializeInterruptManager(&intrrptMngr);

        
    

    // display log message to user
        // check to see if memory is being displayed
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
        copyString(displayMessage, " 0.000000, OS: Simulator Start\n\n");
        displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

        masterTime = accessTimer(ZERO_TIMER, timeAsString);

        // create PCB queue: first in first out operation
            // function createPCBQueue
        localPCBHead = createPCBLinkedList(metaDataMstrPtr, configPtr);

        ProcessControlBlock *wrkingRef = localPCBHead;
        // loop through PCB queue wrking ref
        while( wrkingRef != NULL )
        {
            // set PCBs in queue to READY state
                // changeProcessState
            masterTime = accessTimer(LAP_TIMER, timeAsString);

            changeProcessState(wrkingRef, READY, masterTime, displayMessage);

            logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

            wrkingRef = wrkingRef->nextProcess;
        }
        // end loop

        // free wrkingref
        free( wrkingRef );

        // initializeMemory
            // function intializeSysAvailableMemory
        maxMem = configPtr->memAvailable;
        MemoryManagerHead = intializeSysAvailableMemory( maxMem );

        // check to see if displaying memory
        if( displayMemMngmnt )
        {
            // add formating for memory management
                // function : displayType, copyString
            copyString(displayMessage, "\n--------------------------------------------------\n");

            // display
            logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

            // function : displayType, copyString
            copyString(displayMessage, "After memory initialization\n");

            // display
            logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

            sprintf( displayMessage, "%d [ Open, P#: x, 0-0 ] %d", 
            MemoryManagerHead->begAddress, MemoryManagerHead->allocatedSize - 1 );

            // display
            logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

            // add formating for memory management
                // function : displayType, copyString
            copyString(displayMessage, "\n--------------------------------------------------\n");

            // display
            logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

        }

        currentProcess = getCurrentPCB(cpuSchedString, localPCBHead, currentProcess);

        // set wrking OpCode
        OpCodeType *wrkingOpCode = currentProcess->currentOpType;

        // log process of time
        masterTime = accessTimer(LAP_TIMER, timeAsString);
        sprintf( displayMessage, "\n%s, OS: Process %d selected with %d ms remaining\n", timeAsString,
            currentProcess->pid, currentProcess->totalTime );
        logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);


        // set from waiting to running
        changeProcessState(currentProcess, RUNNING, masterTime, displayMessage);

        // display change to preferred place
        logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);



        // loop through process queue
        while(!checkForTerminated(localPCBHead))
        {
            
            // if app end set process to terinate and display or log to user
                // function: changeProcessState, displayType
            if( compareString( wrkingOpCode->command, "app" ) == 
                STR_EQ && compareString( wrkingOpCode->strArg1, "end" ) == STR_EQ)
            {

                masterTime = accessTimer(LAP_TIMER, timeAsString);

                sprintf(displayMessage, "%s, OS: Process %d ended\n", timeAsString, currentProcess->pid);
                logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                // free any memory associated with the process
                freeMemBlock( &MemoryManagerHead, currentProcess->pid );

                MemoryManager* wrkingMemRef = MemoryManagerHead->nextMemoryLoc;
                                
                if( displayMemMngmnt )
                {
                    // add formating for memory management
                        // function : displayType, copyString
                    copyString(displayMessage, "\n--------------------------------------------------\n");

                    // display
                    logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                    // function : displayType, copyString
                    sprintf(displayMessage, "After clear process %d success\n", currentProcess->pid);
                                    
                    // display
                    logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                    while( wrkingMemRef != NULL )
                    {

                        sprintf( displayMessage, "%d [ Used, P#: %d, %d-%d ] %d\n", 
                                0, currentProcess->pid, wrkingMemRef->begAddress,
                                (wrkingMemRef->allocatedSize + wrkingMemRef->begAddress - 1), wrkingMemRef->allocatedSize );

                        // display
                        logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                        wrkingMemRef = wrkingMemRef->nextMemoryLoc;

                    }

                    sprintf( displayMessage, "%d [ Open, P#: x, 0-0 ] %d", 
                    MemoryManagerHead->begAddress, MemoryManagerHead->allocatedSize - 1 );

                    // display
                    logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                    // add formating for memory management
                        // function : displayType, copyString
                    copyString(displayMessage, "\n--------------------------------------------------\n\n");

                    // display
                    logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);
                }



                // display end of process
                changeProcessState(currentProcess, TERMINATED, masterTime, displayMessage);

                // display change to preferred place
                logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                // set process to terminated
                    // function: changeProcessState
		  while((currentProcess->state == WAITING || currentProcess->state == TERMINATED) && !checkForTerminated(localPCBHead))
                    {
                        // need to add two things to get current PCB, so it starts from the most current process
            
                        // set current process to the next process in the queue
                        currentProcess = getCurrentPCB(cpuSchedString, localPCBHead, currentProcess);

                        // if process is null then tell user that cpu is idling; and set cpu idle flag
                        // to not display multiple times
                        if( (currentProcess->state == WAITING || currentProcess->state == TERMINATED) && !CPUIDLE && !checkForTerminated(localPCBHead))
                        {

                            masterTime = accessTimer(LAP_TIMER, timeAsString);

                            // display memory allocation request
                            sprintf(displayMessage, "\n%s, OS: CPU Idle, all active processes blocked\n", timeAsString);
                                
                            logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                            CPUIDLE = true;
                        }

                        (checkInterrupts( &intrrptMngr, masterTime, displayMessage,
                            logMessageHead, displayMessage, displayToMonitor, displayToLog));

                    }

                    // check to see if CPU was idling process should be picked at the point
                    if( CPUIDLE )
                    {
                        CPUIDLE = false;
                        
                    }

                    if( currentProcess != NULL && currentProcess->state != TERMINATED)
                    {

                            sprintf(displayMessage, "\n%s, OS: Process %d selected with %d ms remaining\n",
                            timeAsString, currentProcess->pid, currentProcess->totalTime);
                            logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                            changeProcessState(currentProcess, RUNNING, masterTime, displayMessage);
                            logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);
                            wrkingQuatum = 0;
                    }
                

                if(currentProcess != NULL  && currentProcess->state != TERMINATED)
                {
                    // if current process is at end of queue
                        // change states to running and dislay to user or fuke
                            // function: changeProcessStatem displayType
                    masterTime = accessTimer( LAP_TIMER, timeAsString );

                    sprintf( displayMessage, "\n%s, OS: Process %d selected with %d ms remaining \n",
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


                            // check to see if premetive
                            if ( premeptive )
                            {

                                // need to set current process to blocked
                                changeProcessState(currentProcess, WAITING, masterTime, displayMessage);

                                logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                                // start io thread
                                    // function: startIOThread
                                startIOThread(currentProcess, (wrkingOpCode->intArg2*configPtr->ioCycleRate), 
                                    &intrrptMngr);
                                    
                                // handle time remaining in process
                                currentProcess->totalTime = currentProcess->totalTime - (wrkingOpCode->intArg2*configPtr->ioCycleRate);

                                wrkingOpCode->intArg2 = 0;

                            }
                            else
                            {
                                // start a timer thread to for I/O operation
                                    // function: startTimeThread
                                startTimerThread(wrkingOpCode->intArg2 * configPtr->ioCycleRate);
                                    // subtrack the time remaining by the time thats run and update the PCB

                                // get end time
                                    // function: accessTimer
                                masterTime = accessTimer(LAP_TIMER, timeAsString);


                                sprintf(displayMessage, "%s, OS: Process %d %s input opertion end\n", timeAsString, currentProcess->pid, 
                                currentProcess->currentOpType->strArg1);

                                // display end time to preferred place
                                logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                                wrkingOpCode->intArg2 = 0;

                            }

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

                            // check to see if premetive
                            if ( premeptive )
                            {
                            
                                // need to set current process to blocked
                                changeProcessState(currentProcess, WAITING, masterTime, displayMessage);

                                logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                                // start io thread
                                    // function: startIOThread
                                startIOThread(currentProcess, (wrkingOpCode->intArg2*configPtr->ioCycleRate), 
                                    &intrrptMngr);
                                    
                                // handle time remaining in process
                                currentProcess->totalTime = currentProcess->totalTime - (wrkingOpCode->intArg2*configPtr->ioCycleRate);

                                wrkingOpCode->intArg2 = 0;
                                
                            }
                            else
                            {
                                // run timer I/O 
                                    // startTimeThread
                                startTimerThread(wrkingOpCode->intArg2 * configPtr->ioCycleRate);

                                // get end time
                                masterTime = accessTimer(LAP_TIMER, timeAsString);

                                // display to preferred place
                                sprintf(displayMessage, "%s, OS: Process %d %s output operation end\n", timeAsString, 
                                        currentProcess->pid, currentProcess->currentOpType->strArg1);
                                
                                logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                                wrkingOpCode->intArg2 = 0;

                            }
                        }
                    }
                    // check if cpu command
                    else if( compareString( wrkingOpCode->command, "cpu" ) == STR_EQ )
                    {
                        

                        // check to see if premeptive:
                        if( premeptive )
                        {
                            
                            // see of quatum is less than number of cycles being run by the op code
                            if( quantum < wrkingOpCode->intArg2)
                            {
                                cycleTime = quantum;
                            }
                            else
                            {
                                cycleTime = wrkingOpCode->intArg2;
                            }
                            
                            masterTime = accessTimer(LAP_TIMER, timeAsString);

                            // display to preferred place
                            sprintf(displayMessage, "%s, OS: Process %d %s proccess operation start\n", timeAsString, 
                                currentProcess->pid, currentProcess->currentOpType->command);
                                logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);


                            // have loop for the quatumn and loop thru until end of cycles
                            while( wrkingQuatum < cycleTime && !(checkInterrupts( &intrrptMngr, masterTime, displayMessage,
                            logMessageHead, displayMessage, displayToMonitor, displayToLog)))
                            {
                                    
                                    // running one cycle at a time

                                    // check for interrupts, if interrupts occur leave the loop

                                // create a timer thread for cpu operation
                                    // starttimerthread
                                startTimerThread(1 * configPtr->procCycleRate);
                                
                                // handle cycles left in opcode
                                wrkingOpCode->intArg2 = wrkingOpCode->intArg2 - 1;  
                                wrkingQuatum = wrkingQuatum + 1;
                                
                                // handle time remaining in process
                                currentProcess->totalTime = currentProcess->totalTime - (1 * configPtr->procCycleRate);

                            }
                            if( wrkingQuatum == configPtr->quantumCycles )
                                {
                                    // get ending time
                                    masterTime = accessTimer(LAP_TIMER, timeAsString);
                                
                                    // display to preferred place
                                    sprintf(displayMessage, "%s, OS: Process %d quantum time out, %s proccess operation end\n", timeAsString, 
                                        currentProcess->pid, currentProcess->currentOpType->command);

                                    logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);
                                }
                                else
                                {
                                
                                    // display to preferred place
                                    sprintf(displayMessage, "%s, OS: Process %d %s proccess operation end\n", timeAsString, 
                                        currentProcess->pid, currentProcess->currentOpType->command);
                                    // display end time to user
                                    logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                                }
  
                        }
                        else
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

                            wrkingOpCode->intArg2 = 0;
                        }
                    }
                    // assume memory command
                    else
                    {

                        // check to see what kind of memory request it is
                        if( compareString (wrkingOpCode->strArg1, "allocate" ) == STR_EQ )
                        {
                            // get starter time
                            masterTime = accessTimer(LAP_TIMER, timeAsString);

                            // display memory allocation request
                            sprintf(displayMessage, "%s, OS: Process %d %s allocate request (%d, %d)\n", timeAsString, 
                                        currentProcess->pid, currentProcess->currentOpType->command,
                                        wrkingOpCode->intArg2, wrkingOpCode->intArg3);
                            
                            logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                            // test to to see if allocation was successfull
                            if(allocateMemBlock(&MemoryManagerHead, wrkingOpCode->intArg2, wrkingOpCode->intArg3, currentProcess->pid))
                            {
                                // set working memory to next in linked list so not the head of linked list
                                MemoryManager* wrkingMemRef = MemoryManagerHead->nextMemoryLoc;
                                
                                // loop through until it finds the next null pointptr in memory
                                while(wrkingMemRef->nextMemoryLoc != NULL)
                                {
                                    // set wrkingref to next in linked list
                                    wrkingMemRef= wrkingMemRef->nextMemoryLoc;
                                }
                                // if config file is set to display memory
                                if( displayMemMngmnt )
                                {
                                    // add formating for memory management
                                        // function : displayType, copyString
                                    copyString(displayMessage, "\n--------------------------------------------------\n");

                                    // display
                                    logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                                    // function : displayType, copyString
                                    copyString(displayMessage, "After Allocate Successful\n");

                                    // display
                                    logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                                    // while the wrking memory is not null to display the memory actions
                                    while( wrkingMemRef != NULL )
                                    {
                                        // display memory to display message
                                            // fucntion: sprintf
                                        sprintf( displayMessage, "%d [ Used, P#: %d, %d-%d ] %d\n", 
                                        0, currentProcess->pid, wrkingMemRef->begAddress,
                                        (wrkingMemRef->allocatedSize + wrkingMemRef->begAddress - 1), wrkingMemRef->allocatedSize );

                                        // display to user based on where to display
                                            // function displaytotype
                                        logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                                        // set the next point in linked list
                                       wrkingMemRef = wrkingMemRef->nextMemoryLoc;

                                    }

                                    // display the final message
                                        // function sprintf
                                    sprintf( displayMessage, "%d [ Open, P#: x, 0-0 ] %d", 
                                    MemoryManagerHead->begAddress, MemoryManagerHead->allocatedSize - 1 );

                                    // display
                                    logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                                    // add formating for memory management
                                        // function : displayType, copyString
                                    copyString(displayMessage, "\n--------------------------------------------------\n\n");

                                    // display
                                    logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                                }

                                // get the master time for the simulator
                                masterTime = accessTimer(LAP_TIMER, timeAsString);

                                // display memory allocation request
                                sprintf(displayMessage, "%s, OS: Process %d successfull %s allocate request\n", timeAsString, 
                                            currentProcess->pid, currentProcess->currentOpType->command);
                            
                                // set the linked list to the head of the linked list
                                logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                            }
                            // allocation failed
                            else
                            {
                                // set away from the head
                                MemoryManager* wrkingMemRef = MemoryManagerHead->nextMemoryLoc;
                                
                                // check to see if display memory is active
                                if( displayMemMngmnt )
                                {
                                    // add formating for memory management
                                        // function : displayType, copyString
                                    copyString(displayMessage, "\n--------------------------------------------------\n");

                                    // display
                                    logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                                    // function : displayType, copyString
                                    copyString(displayMessage, "After Allocate overlap failure\n");
                                    
                                    // display
                                    logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                                    // check for end of function
                                    while( wrkingMemRef != NULL )
                                    {
                                        // copy to displayMessage for formatting
                                        sprintf( displayMessage, "%d [ Used, P#: %d, %d-%d ] %d\n", 
                                        0, currentProcess->pid, wrkingMemRef->begAddress,
                                        (wrkingMemRef->allocatedSize + wrkingMemRef->begAddress - 1), wrkingMemRef->allocatedSize );

                                        // display
                                        logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                                       wrkingMemRef = wrkingMemRef->nextMemoryLoc;

                                    }

                                    sprintf( displayMessage, "%d [ Open, P#: x, 0-0 ] %d", 
                                    MemoryManagerHead->begAddress, MemoryManagerHead->allocatedSize - 1 );

                                    // display
                                    logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                                    // add formating for memory management
                                        // function : displayType, copyString
                                    copyString(displayMessage, "\n--------------------------------------------------\n\n");

                                    // display
                                    logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);
                                }

                                masterTime = accessTimer(LAP_TIMER, timeAsString);

                                // display memory allocation request failed
                                sprintf(displayMessage, "%s, OS: Process %d failed %s allocate request\n", timeAsString, 
                                            currentProcess->pid, currentProcess->currentOpType->command);
                            
                                // update log message linked list
                                logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                                // loop through to the end of the op codes because of failure
                                while(!(compareString( wrkingOpCode->nextNode->command, "app" ) == 
                                    STR_EQ && compareString( wrkingOpCode->nextNode->strArg1, "end" ) == STR_EQ))
                                    {
                                        wrkingOpCode = wrkingOpCode->nextNode;
                                    }

                                // lap time for simulator
                                masterTime = accessTimer(LAP_TIMER, timeAsString);

                                // display memory allocation request
                                sprintf(displayMessage, "\n%s, OS: Segmentation Fault, Process %d\n", timeAsString, 
                                            currentProcess->pid);

                                // update log message linked list
                                logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);


                            }
                        }
                        // check to see if it is access prexisiting memory
                        else if (compareString (wrkingOpCode->strArg1, "access" ) == STR_EQ )
                        {
                            // get starter time
                            masterTime = accessTimer(LAP_TIMER, timeAsString);

                            // display memory allocation request
                            sprintf(displayMessage, "%s, OS: Process %d %s memory access request (%d, %d)\n", timeAsString, 
                                        currentProcess->pid, currentProcess->currentOpType->command,
                                        wrkingOpCode->intArg2, wrkingOpCode->intArg3);

                            // update log message linked list
                            logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);
                            
                            // test to see if access was successful
                            if( accessMemBlock(&MemoryManagerHead, wrkingOpCode->intArg2,
                             wrkingOpCode->intArg3, currentProcess->pid))
                             {
                                // test to not the head of the linked list, 2nd element
                                MemoryManager* wrkingMemRef = MemoryManagerHead->nextMemoryLoc;
                                
                                // test to see if displaying memory
                                if( displayMemMngmnt )
                                {
                                    // add formating for memory management
                                        // function : displayType, copyString
                                    copyString(displayMessage, "\n--------------------------------------------------\n");

                                    // display
                                    logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                                    // function : displayType, copyString
                                    copyString(displayMessage, "After access success\n");
                                    
                                    // display
                                    logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                                    // loop through to display all memory in linked list memorymanager
                                    while( wrkingMemRef != NULL )
                                    {

                                        // update display message
                                        sprintf( displayMessage, "%d [ Used, P#: %d, %d-%d ] %d\n", 
                                        0, currentProcess->pid, wrkingMemRef->begAddress,
                                        (wrkingMemRef->allocatedSize + wrkingMemRef->begAddress - 1), wrkingMemRef->allocatedSize );

                                        // display / update log string linked list
                                        logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                                        // iterate the linked list
                                        wrkingMemRef = wrkingMemRef->nextMemoryLoc;

                                    }

                                    // display the final portion of the meory
                                    sprintf( displayMessage, "%d [ Open, P#: x, 0-0 ] %d", 
                                    MemoryManagerHead->begAddress, MemoryManagerHead->allocatedSize - 1 );

                                    // display
                                    logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                                    // add formating for memory management
                                        // function : displayType, copyString
                                    copyString(displayMessage, "\n--------------------------------------------------\n\n");

                                    // display
                                    logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);
                                }

                                // lap the master timer
                                masterTime = accessTimer(LAP_TIMER, timeAsString);

                                // display memory allocation request
                                sprintf(displayMessage, "%s, OS: Process %d successful %s access request\n", timeAsString, 
                                            currentProcess->pid, currentProcess->currentOpType->command);
                            
                                logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                             }
                             // otherwise; access is a failure
                             else
                             {
                                // assign wrkingMemRef to loop through
                                MemoryManager* wrkingMemRef = MemoryManagerHead->nextMemoryLoc;
                                
                                // check if displaying memory managment
                                if( displayMemMngmnt )
                                {
                                    // add formating for memory management
                                        // function : displayType, copyString
                                    copyString(displayMessage, "\n--------------------------------------------------\n");

                                    // display
                                    logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                                    // function : displayType, copyString
                                    copyString(displayMessage, "After access failure\n");
                                    
                                    // display
                                    logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                                    // display through the memory linked list
                                    while( wrkingMemRef != NULL )
                                    {

                                        sprintf( displayMessage, "%d [ Used, P#: %d, %d-%d ] %d\n", 
                                        0, currentProcess->pid, wrkingMemRef->begAddress,
                                        (wrkingMemRef->allocatedSize + wrkingMemRef->begAddress - 1), wrkingMemRef->allocatedSize );

                                        // display
                                        logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                                       wrkingMemRef = wrkingMemRef->nextMemoryLoc;

                                    }

                                    sprintf( displayMessage, "%d [ Open, P#: x, 0-0 ] %d", 
                                    MemoryManagerHead->begAddress, MemoryManagerHead->allocatedSize - 1 );

                                    // display
                                    logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                                    // add formating for memory management
                                        // function : displayType, copyString
                                    copyString(displayMessage, "\n--------------------------------------------------\n\n");

                                    // display
                                    logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);
                                }

                                masterTime = accessTimer(LAP_TIMER, timeAsString);

                                // display memory allocation request
                                sprintf(displayMessage, "%s, OS: Process %d failed %s access request\n", timeAsString, 
                                            currentProcess->pid, currentProcess->currentOpType->command);
                            
                                logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                                // loop through to the end of the op codes because of failure
                                while(!(compareString( wrkingOpCode->nextNode->command, "app" ) == 
                                    STR_EQ && compareString( wrkingOpCode->nextNode->strArg1, "end" ) == STR_EQ))
                                    {
                                        wrkingOpCode = wrkingOpCode->nextNode;
                                    }

                                masterTime = accessTimer(LAP_TIMER, timeAsString);

                                // display memory allocation request
                                sprintf(displayMessage, "\n%s, OS: Segmentation Fault, Process %d\n", timeAsString, 
                                            currentProcess->pid);
                            
                                logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                             }
                        }
                        

                        
                    }
                
                // check to see if cycle count is 0

                if( wrkingOpCode->intArg2 == 0 || compareString( wrkingOpCode->command, "mem" ) == STR_EQ )
                {
                    // set currentOpType to the next node
                    currentProcess->currentOpType = wrkingOpCode->nextNode;
                }


            }


            
            if( currentProcess != NULL )
            {  

                // if current process has been set to blocked or if quantum has been tripped
                

                if( currentProcess->state == WAITING || wrkingQuatum == configPtr->quantumCycles)
                {
                    if( currentProcess != NULL && currentProcess->state != WAITING )
                    {

                            masterTime = accessTimer( LAP_TIMER, timeAsString );

                            changeProcessState(currentProcess, READY, masterTime, displayMessage);

                                                // display change to preferred place
                            logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                            previousProcessNumber = currentProcess->pid;
                    }
                    // using a do loop, check to see if currentprocess is equal to null;
                    do
                    {
                        // need to add two things to get current PCB, so it starts from the most current process
            
                        // set current process to the next process in the queue
                        currentProcess = getCurrentPCB(cpuSchedString, localPCBHead, currentProcess);

                        // if process is null then tell user that cpu is idling; and set cpu idle flag
                        // to not display multiple times
                        if( (currentProcess->state == WAITING || currentProcess->state == TERMINATED) && !CPUIDLE)
                        {

                            masterTime = accessTimer(LAP_TIMER, timeAsString);

                            // display memory allocation request
                            sprintf(displayMessage, "\n%s, OS: CPU Idle, all active processes blocked\n", timeAsString);
                                
                            logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                            CPUIDLE = true;
                        }

                        (checkInterrupts( &intrrptMngr, masterTime, displayMessage,
                            logMessageHead, displayMessage, displayToMonitor, displayToLog));

                    } while((currentProcess->state == WAITING) && !checkForTerminated(localPCBHead));

                    // check to see if CPU was idling process should be picked at the point
                    if( CPUIDLE )
                    {
                        CPUIDLE = false;
                        
                        masterTime = accessTimer(LAP_TIMER, timeAsString);

                        // display memory allocation request
                        sprintf(displayMessage, "\n%s, OS: CPU idle interuppt, end idle\n", timeAsString);
                                
                        logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);
                    }

                    if( currentProcess != NULL )
                    {

                            sprintf(displayMessage, "\n%s, OS: Process %d selected with %d ms remaining\n",
                            timeAsString, currentProcess->pid, currentProcess->totalTime);
                            logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                            changeProcessState(currentProcess, RUNNING, masterTime, displayMessage);
                            logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);
                            wrkingQuatum = 0;
                    }

                }
            }

            if( currentProcess != NULL )
            {
                wrkingOpCode = currentProcess->currentOpType;
            }
        }
        // end loop

        // free any memory associated with the process
        freeMemBlock( &MemoryManagerHead, MemoryManagerHead->pid );
         
        if( displayMemMngmnt )
        {
            // add formating for memory management
                // function : displayType, copyString
            copyString(displayMessage, "\n--------------------------------------------------\n");

            // display
            logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

            // function : displayType, sprintf
            sprintf(displayMessage, "After clear all process success\n");               

            // display
            logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

                        // function : displayType, sprintf
            sprintf(displayMessage, "No Memory configured");               

            // display
            logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);

            // add formating for memory management
                // function : displayType, copyString
            copyString(displayMessage, "\n--------------------------------------------------\n");

            // display
            logMessageHead = displayToType(logMessageHead, displayMessage, displayToMonitor, displayToLog);
        }

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
Name: startIOThread
Process: takes in the current process that is going to have IO run on them and interrupt
         manager and creates a detached thread; because it wont be joined with main program
Function Input/Parameter: processcontrolblock pntr runtime and wrkingintrrptmngrt
Function Output/parameters: none
Function Output/Returned: none
Device Input/file: none
Device Output/device: none
dependencies: pthread_attr_init, pthread_create, pthread_attr_destroy
*/
void startIOThread(ProcessControlBlock *wrkingProcess, int runTime,
         InterruptManager *wrkingIntrrptMnger) 
{
    // initialize thread and arguments
    pthread_t thread;
    pthread_attr_t attr;
    
    // Allocate memory for thread arguments
    IOThreadArguments *args = malloc(sizeof(IOThreadArguments));

    // fill out the arguments that are going to be passed to the void * function
    args->currentProcess = wrkingProcess;
    args->runTime = runTime;
    args->intrrptMnger = wrkingIntrrptMnger;

    // set the attributes and set the state of thread to detached so it ends after finishing running
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    
    // create thread like normal and then destroy
    pthread_create(&thread, &attr, runIOOperation, args);
    pthread_attr_destroy(&attr);
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

