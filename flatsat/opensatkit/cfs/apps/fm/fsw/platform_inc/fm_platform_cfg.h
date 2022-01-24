/*
** $Id: fm_platform_cfg.h 1.6.1.2 2017/01/23 21:53:19EST sstrege Exp  $
**
**  Copyright (c) 2007-2014 United States Government as represented by the 
**  Administrator of the National Aeronautics and Space Administration. 
**  All Other Rights Reserved.  
**
**  This software was created at NASA's Goddard Space Flight Center.
**  This software is governed by the NASA Open Source Agreement and may be 
**  used, distributed and modified only pursuant to the terms of that 
**  agreement.
**
** Title: File Manager (FM) Platform Configuration Header File
**
** Purpose: Specification for the CFS FM application constants
**          that can be configured form one platform to another
**
** Author: Susanne L. Strege, Code 582 NASA GSFC
**
** Notes:
**
** References:
**    Flight Software Branch C Coding Standard Version 1.0a
**
*/

#ifndef _fm_platform_cfg_h_
#define _fm_platform_cfg_h_


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM platform configuration parameters - application definitions  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/** \fmcfg File Manager Application Name
**
**  \par Description:
**       This definition must match the name used at startup by the cFE
**       Executive Services when creating the FM application.  Note that
**       application names are also an argument to certain cFE commands.
**       For example, the application name is needed to access tables
**       via cFE Table Services commands.
**
**  \par Limits:
**       FM requires that this name be defined, but otherwise places
**       no limits on the definition.  Refer to CFE Executive Services
**       for specific information on limits related to application names.
*/
#define FM_APP_NAME                     "FM"


/** \fmcfg File Manager Command Pipe Name
**
**  \par Description:
**       This definition is the name used at startup when creating a cFE
**       Software Bus command pipe for the FM application.
**
**  \par Limits:
**       FM requires that this name be defined, but otherwise places
**       no limits on the definition.  Refer to CFE Software Bus Services
**       for specific information on limits related to pipe names.
*/
#define FM_APP_PIPE_NAME                "FM_CMD_PIPE"


/** \fmcfg File Manager Command Pipe Depth
**
**  \par Description:
**       This definition sets the total number of packets that may queue
**       in the FM command pipe.  The limit for individual message types
**       in the queue is the default cFE Software Bus subscription limit
**       of four.
**
**  \par Limits:
**       The FM application limits this value to be no less than 4 and
**       no greater than 20 packets at any one time in the command pipe.
*/
#define FM_APP_PIPE_DEPTH               10


/** \sccfg Mission specific version number for FM application
**  
**  \par Description:
**       An application version number consists of four parts:
**       major version number, minor version number, revision
**       number and mission specific revision number. The mission
**       specific revision number is defined here and the other
**       parts are defined in "fm_version.h".
**
**  \par Limits:
**       Must be defined as a numeric value that is greater than
**       or equal to zero.
*/
#define FM_MISSION_REV      0


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM platform configuration parameters - output file definitions  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/** \fmcfg Default Directory List Output Filename
**
**  \par Description:
**       This definition is the default output filename used by the Get
**       Directory List to File command handler when the output filename
**       is not provided.  The default filename is used whenever the
**       commanded output filename is the empty string.
**
**  \par Limits:
**       The FM application does not place a limit on this configuration
**       parameter, however the symbol must be defined and the name will
**       be subject to the same verification tests as a commanded output
**       filename.  Set this parameter to the empty string if no default
**       filename is desired.
*/
#define FM_DIR_LIST_FILE_DEFNAME        "/cf/fm_dirlist.out"  //dcm


/** \fmcfg Maximum Directory List Output File Entries
**
**  \par Description:
**       This definition sets the upper limit for the number of directory
**       entries that may be written to a Directory List output file.
**       Directory List files are variable length, based on the number of
**       directory entries actually written to the file.  There may zero
**       entries written to the file if the directory is empty.  For most
**       environments, this definition will play no role at all, as it
**       will be set to a number much larger than the count of files that
**       will ever exist in any directory at one time.
**
**  \par Limits:
**       The FM application limits this value to be no less than 100 and
**       no greater than 10000.
*/
#define FM_DIR_LIST_FILE_ENTRIES        3000


/** \fmcfg Directory List Output File Header Sub-Type
**
**  \par Description:
**       This definition sets the cFE File Header sub-type value for FM
**       Directory List data files.  The value may be used to differentiate
**       FM Directory List files from other data files.
**
**  \par Limits:
**       The FM application places no limits on this unsigned 32 bit value.
*/
#define FM_DIR_LIST_FILE_SUBTYPE        12345


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM platform configuration parameters - TLM packet definitions   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/** \fmcfg Directory List Telemetry Packet Entry Count
**
**  \par Description:
**       This definition sets the number of directory entries contained
**       in the Directory List telemetry packet.  The command handler will
**       read directory entries until reaching the index of the start entry
**       (set via command argument) and then continue to read
**       directory entries and populate the telemtry packet until there are
**       either no more unread directory entries or until the telemetry
**       packet is full.
**
**  \par Limits:
**       The FM application limits this value to be no less than 10 and
**       and no greater than 100. The number of directory entries in the
**       telemetry packet will in large part determine the packet size.
*/
#define FM_DIR_LIST_PKT_ENTRIES         10


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM platform configuration parameters - child task definitions   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/** \fmcfg Child Task File I/O Control Settings
**
**  \par Description:
**       These definitions control the amount of file data that the FM child task
**       will process before giving up the CPU to allow other tasks time to run.
**
**       FM_CHILD_FILE_BLOCK_SIZE defines the size of each block of file data that
**       the FM child task will read or write.  This value also defines the size
**       of the FM child task I/O buffer that exists in global memory.
**
**       FM_CHILD_FILE_LOOP_COUNT defines the number of file data blocks that may
**       be processed before the FM child task sleeps (gives up the CPU).
**
**       FM_CHILD_FILE_SLEEP_MS defines the length of time (in milli-secs) before
**       the FM child task wakes (re-acquires the CPU).  Note that many platforms
**       will limit the precision of this value.
**
**       Thus the combination of the 3 values control CPU use by the FM child task.
**       Using a smaller block size minimizes the amount of RAM used by the file
**       I/O buffer, but at the expense of file efficiency.  Adjust each of the
**       values such that the combination is appropriate for the target platform.
**
**       For example, if the block size is 2048 and the loop count is 16 and the
**       sleep time is 20, then while processing a 1 Mbyte file there will be
**       32 sleep cycles of 20ms each, for a total task delay of 0.64 seconds.
**
**  \par Limits:
**       FM_CHILD_FILE_BLOCK_SIZE: The FM application limits this value to be no
**       less than 256 bytes and no greater than 32KB.
**
**       FM_CHILD_FILE_LOOP_COUNT: The FM application limits this value to be
**       non-zero.  There is no upper limit - a very large number effectively
**       means that the FM child task will not surrender the CPU to other lower
**       priority tasks.
**
**       FM_CHILD_FILE_SLEEP_MS: The FM application limits this value to be no
**       no greater than 100 ms.  The value zero generally means a very short
**       task delay - refer to the target platform documentation for specifics.
*/
#define FM_CHILD_FILE_BLOCK_SIZE        2048
#define FM_CHILD_FILE_LOOP_COUNT        16
#define FM_CHILD_FILE_SLEEP_MS          20

/** \fmcfg Child file stat sleep
**
**  \par Description:
**       OS_stat is a CPU intensive call. FM uses the OS_stat call to query a 
**       file’s size, date, and mode when setting up directory listings. 
**       Querying a large number of files and/or files large in size when 
**       processing directory listing commands can cause FM to hog the CPU. To
**       mitigate this, options to sleep a configurable number of milliseconds 
**       between calls to OS_stat for a configurable number of files
**       in a directory listing is provided. A large sleep cycle will not hang the CPU
**       but it may take a long time for directory listing to complete. A shorter
**       sleep cycle will speed up the directory listing commands but may cause
**       FM to hog the CPU.
**       
**       FM_CHILD_STAT_SLEEP_MS: The number of milliseconds to sleep each
**       cycle. One cycle is FM_CHILD_STAT_SLEEP_FILECOUNT.
**       
**       FM_CHILD_STAT_SLEEP_FILECOUNT: The number of files to process (OS_stat) before 
**       sleeping FM_CHILD_STAT_SLEEP_MS.
**       Works in tandem with FM_CHILD_STAT_SLEEP_MS to reduce CPU hogging
**       while allowing slightly more customization to balance time the operator is waiting to
**       get data back from a directory listing versus FM hogging the CPU with calls to OS_stat
**        
**       In short:
**       High SLEEP_MS means less CPU hogging by FM but a longer time to process a dir listing command
**       Low SLEEP_MS means more potential CPU hogging by FM but shorter time to process a dir listing command
**       High FILECOUNT means more potential CPU hogging by FM but a shorter time to process a dir listing command
**       Low FILECOUNT means less CPU hogging by FM but longer time to process a dir listing command
**  \par Limits:
**       The default is zero unless the mission needs require them to be changed.
**
*/
#define FM_CHILD_STAT_SLEEP_MS 0
#define FM_CHILD_STAT_SLEEP_FILECOUNT 0

/** \fmcfg Child Task Command Queue Entry Count
**
**  \par Description:
**       This definition sets the array depth for the command arguments queue in
**       the FM main task to FM child task handshake interface.  The value sets
**       the upper limit for the number of commands that can be waiting in the
**       queue to be processed by the low priority FM child task.  A multi-entry
**       command queue prevents the occasional slow command from being rejected
**       because the child task has not yet completed the previous slow command.
**
**  \par Limits:
**       The FM application limits this value to be no less than 1 and no greater
**       than 10.  There must be at least one because this is the method for
**       passing command arguments from the parent to the child task.  The upper
**       limit is arbitrary.
*/
#define FM_CHILD_QUEUE_DEPTH            3


/** \fmcfg Child Task Name - cFE object name
**
**  \par Description:
**       This definition sets the FM child task object name.  The task object
**       name is required during child task creation by cFE Executive Services.
**
**  \par Limits:
**       FM requires that this name be defined, but otherwise places
**       no limits on the definition.  Refer to CFE Executive Services
**       for specific information on limits related to object names.
*/
#define FM_CHILD_TASK_NAME              "FM_CHILD_TASK"


/** \fmcfg Child Task Stack Size
**
**  \par Description:
**       This definition sets the size in bytes of the FM child task
**       stack.  It is highly recommended that this assignment be made
**       by someone familiar with the system requirements for tasks
**       running on the target platform.
**
**  \par Limits:
**       The FM application limits this value to be no less than 2048
**       and no greater than 20480.  These limits are purely arbitrary
**       and may need to be modified for specific platforms.
*/
#define FM_CHILD_TASK_STACK_SIZE        20480


/** \fmcfg Child Task Execution Priority
**
**  \par Description:
**       This definition sets the execution priority for the FM child
**       task.  It is highly recommended that this assignment be made
**       by someone familiar with the system requirements for tasks
**       running on the target platform.
**
**  \par Limits:
**       Value to be no less than 1 and no greater than 255.  
**
**  \par Priority Values:
**       Note that a small value has higher priority than a large value.
**       Thus, 100 is higher priority than 150. It is also necessary to
**       ensure that a child task has lower priority than its parent.
**       It should be clear that a child task that runs ahead of its
**       parent defeats the purpose of having a child task to run in
**       the background.
*/
#define FM_CHILD_TASK_PRIORITY          205


/** \fmcfg Child Task Semaphore Name - cFE object name
**
**  \par Description:
**       This definition sets the FM child task semaphore object name.
**       The semaphore object name is required during semaphore creation
**       by cFE Executive Services.
**
**  \par Limits:
**       FM requires that this name be defined, but otherwise places
**       no limits on the definition.  Refer to CFE Executive Services
**       for specific information on limits related to object names.
*/
#define FM_CHILD_SEM_NAME               "FM_CHILD_SEM"


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM platform configuration parameters - table definitions        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/** \fmcfg Free Space Table Name - cFE object name
**
**  \par Description:
**       Table object name is required during table creation.
**
**  \par Limits:
**       FM requires that this name be defined, but otherwise places
**       no limits on the definition.  Refer to CFE Table Services
**       for specific information on limits related to table names.
*/
#define FM_TABLE_CFE_NAME               "FreeSpace"


/** \fmcfg Free Space Table Name - filename with path
**
**  \par Description:
**       Table name with path is required to load table at startup.
**
**  \par Limits:
**       FM requires that this name be defined, but otherwise places
**       no limits on the definition.  If the named table does not
**       exist or fails validation, the table load will fail.
*/
#define FM_TABLE_DEF_NAME               "/cf/fm_freespace.tbl"    //dcm


/** \fmcfg Free Space Table Name - filename without path
**
**  \par Description:
**       Table name without path defines the output name for the table
**       file created during the table make process.
**
**  \par Limits:
**       FM requires that this name be defined, but otherwise places
**       no limits on the definition.  If the table name is not
**       valid then the make process may fail, or the table file may
**       be unloadable to the target hardware.
*/
#define FM_TABLE_FILENAME               "fm_freespace.tbl"


/** \fmcfg Free Space Table Description
**
**  \par Description:
**       Table files contain headers that include descriptive text.
**       This text will be put into the file header during the table
**       make process.
**
**  \par Limits:
**       FM requires that this name be defined, but otherwise places
**       no limits on the definition.  Refer to cFE Table Services
**       for limits related to table descriptive text.
*/
#define FM_TABLE_DEF_DESC               "FM File System Free Space Table"


/** \fmcfg Number of Free Space Table Entries
**
**  \par Description:
**       This value defines the number of entries in both the FM file system
**       free space table and the FM file system free space telemetry packet.
**       Note: this value does not define the number of file systems present
**       or supported by the CFE-OSAL, the value only defines the number of
**       file systems for which FM may be enabled to report free space data.
**
**  \par Limits:
**       FM limits this value to be not less than 1 and not greater than 16.
*/
#define FM_TABLE_ENTRY_COUNT            8


/** \fmcfg Table Data Validation Error Code
**
**  \par Description:
**       Table data is verified during the table load process.  Should
**       the validation process fail, this value will be returned by 
**       FM to cFE Table Services and displayed in an event message.
**
**  \par Limits:
**       FM requires that this value be defined, but otherwise places
**       no limits on the definition.  Refer to cFE Table Services
**       for limits related to error return values.
*/
#define FM_TABLE_VALIDATION_ERR         (0xCF000080L)


#endif /* _fm_platform_cfg_h_ */

/************************/
/*  End of File Comment */
/************************/
