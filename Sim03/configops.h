// header files
#include "StringUtils.h"
#include "StandardConstants.h"
#include "datatypes.h"


typedef enum { CFG_FILE_ACCESS_ERR,
               CFG_CORRUPT_DESCRIPTOR_ERR,
               CFG_DATA_OUT_OF_RANGE_ERR,
               CFG_CORRUPT_PROMPT_ERR,
               CFG_VERSION_CODE,
               CFG_MD_FILE_NAME_CODE,
               CFG_CPU_SCHED_CODE,
               CFG_QUANT_CYCLES_CODE,
               CFG_MEM_DISPLAY_CODE,
               CFG_MEM_AVAILABLE_CODE,
               CFG_PROC_CYCLES_CODE,
               CFG_IO_CYCLES_CODE,
               CFG_LOG_TO_CODE,
               CFG_LOG_FILE_NAME_CODE }  ConfigCodeMessages;


typedef enum { CPU_SCHED_SJF_N_CODE,
               CPU_SCHED_SRTF_P_CODE,
               CPU_SCHED_FCFS_P_CODE,
               CPU_SCHED_RR_P_CODE,
               CPU_SCHED_FCFS_N_CODE,
               LOGTO_MONITOR_CODE,
               LOGTO_FILE_CODE,
               LOGTO_BOTH_CODE,
               NON_PREEMPTIVE_CODE,
               PREEMPTIVE_CODE } ConfigDataCodes;


// prototypes:
ConfigDataType *clearConfigData( ConfigDataType *configData);
void displayConfigData( ConfigDataType *configData);
bool getConfigData( const char *fileName,
                                ConfigDataType **configData, char *endStateMsg );
void configCodeToString( int code, char *outString );
ConfigCodeMessages getDataLineCode( const char *dataBuffer );
void stripTrailingSpaces( char *str ); 
ConfigDataCodes getCpuSchedCode( const char *lowerCaseCodeStr );
bool valueInRange( int lineCode, int intVal, 
                            double doubleVal, const char *lowerCaseStringVal );
ConfigCodeMessages getLogToCode( const char *lowerCaseLogToStr );
