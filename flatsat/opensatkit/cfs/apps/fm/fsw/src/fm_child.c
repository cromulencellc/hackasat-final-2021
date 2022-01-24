/*
** $Id: fm_child.c 1.16.1.5 2017/01/24 23:53:32EST mdeschu Exp  $
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
** Purpose: File Manager (FM) Child task (low priority command handler)
**
** Author: Scott Walling (Microtel)
**
** Notes:
**
*/

#include "cfe.h"
#include "fm_msg.h"
#include "fm_msgdefs.h"
#include "fm_msgids.h"
#include "fm_events.h"
#include "fm_app.h"
#include "fm_child.h"
#include "fm_cmds.h"
#include "fm_cmd_utils.h"
#include "fm_perfids.h"
#include "fm_platform_cfg.h"
#include "fm_verify.h"

#include <string.h>

/************************************************************************
** OSAL Compatibility for directory name access
** New OSAL version have an access macro to get the string.  If that
** macro is defined, use it, otherwise assume "d_name" structure member.
*************************************************************************/
#ifndef OS_DIRENTRY_NAME
#define OS_DIRENTRY_NAME(x)     ((x).d_name)
#endif


#define FM_QUEUE_SEM_NAME "FM_QUEUE_SEM"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM child task -- startup initialization                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 FM_ChildInit(void)
{
    int32 TaskTextLen = 64;
    char TaskText[TaskTextLen];
    int32 Result;
    

    /* Create counting semaphore (given by parent to wake-up child) */
    Result = OS_CountSemCreate(&FM_GlobalData.ChildSemaphore, FM_CHILD_SEM_NAME, 0, 0);
    if (Result != CFE_SUCCESS)
    {
        strncpy(TaskText, "create semaphore failed", TaskTextLen);
    }
    else
    {
        /* Create mutex semaphore (protect access to ChildQueueCount) */
        Result = OS_MutSemCreate(&FM_GlobalData.ChildQueueCountSem, FM_QUEUE_SEM_NAME, 0);

        if (Result != CFE_SUCCESS)
        {
            strncpy(TaskText, "create queue count semaphore failed", TaskTextLen);
        }
        else
        {
            /* Create child task (low priority command handler) */
            Result = CFE_ES_CreateChildTask(&FM_GlobalData.ChildTaskID,
                                             FM_CHILD_TASK_NAME,
                                             FM_ChildTask, 0,
                                             FM_CHILD_TASK_STACK_SIZE,
                                             FM_CHILD_TASK_PRIORITY, 0);
            if (Result != CFE_SUCCESS)
            {
                strncpy(TaskText, "create task failed", TaskTextLen);
            }
        }
    }
    
    if (Result != CFE_SUCCESS)
    {   
        CFE_EVS_SendEvent(FM_CHILD_INIT_ERR_EID, CFE_EVS_ERROR,
           "Child Task initialization error: %s: result = %d",
            TaskText, (int)Result);
    }

    return(Result);

} /* End of FM_ChildInit() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM child task -- task entry point                               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void FM_ChildTask(void)
{
    char *TaskText = "Child Task";
    int32 Result;

    /*
    ** The child task runs until the parent dies (normal end) or
    **  until it encounters a fatal error (semaphore error, etc.)...
    */
    Result = CFE_ES_RegisterChildTask();

    if (Result != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(FM_CHILD_INIT_ERR_EID, CFE_EVS_ERROR,
           "%s initialization error: register child failed: result = %d",
            TaskText, (int)Result);
    }
    else
    {
        CFE_EVS_SendEvent(FM_CHILD_INIT_EID, CFE_EVS_INFORMATION,
           "%s initialization complete", TaskText);

        /* Child task process loop */
        FM_ChildLoop();
    }

    /* Stop the parent from invoking the child task */
    FM_GlobalData.ChildSemaphore = FM_CHILD_SEM_INVALID;

    /* This call allows cFE to clean-up system resources */
    CFE_ES_ExitChildTask();

    return;

} /* End of FM_ChildTask() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM child task -- main process loop                              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void FM_ChildLoop(void)
{
    char *TaskText = "Child Task termination error: ";
    int32 Result = CFE_SUCCESS;

    while (Result == CFE_SUCCESS)
    {
        /* Pend on the "handshake" semaphore */
        Result = OS_CountSemTake(FM_GlobalData.ChildSemaphore);

        /* Mark the period when this task is active */
        CFE_ES_PerfLogEntry(FM_CHILD_TASK_PERF_ID);

        if (Result == CFE_SUCCESS)
        {
            /* Make sure the parent/child handshake is not broken */
            if (FM_GlobalData.ChildQueueCount == 0)
            {
                FM_GlobalData.ChildCmdErrCounter++;
                CFE_EVS_SendEvent(FM_CHILD_TERM_ERR_EID, CFE_EVS_ERROR,
                                  "%s empty queue", TaskText);

                /* Set result that will terminate child task run loop */
                Result = CFE_OS_ERROR;
            }
            else if (FM_GlobalData.ChildReadIndex >= FM_CHILD_QUEUE_DEPTH)
            {
                FM_GlobalData.ChildCmdErrCounter++;
                CFE_EVS_SendEvent(FM_CHILD_TERM_ERR_EID, CFE_EVS_ERROR,
                                  "%s invalid queue index: index = %d",
                                  TaskText, (int)FM_GlobalData.ChildReadIndex);

                /* Set result that will terminate child task run loop */
                Result = CFE_OS_ERROR;
            }
            else
            {
                /* Invoke the child task command handler */
                FM_ChildProcess();
            }
        }
        else
        {
            CFE_EVS_SendEvent(FM_CHILD_TERM_ERR_EID, CFE_EVS_ERROR,
                              "%s semaphore take failed: result = %d",
                              TaskText, (int)Result);
        }

        CFE_ES_PerfLogExit(FM_CHILD_TASK_PERF_ID);
    }

    return;

} /* End of FM_ChildLoop() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM child task -- interface handshake processor                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void FM_ChildProcess(void)
{
    char *TaskText = "Child Task";
    FM_ChildQueueEntry_t *CmdArgs;

    CmdArgs = &FM_GlobalData.ChildQueue[FM_GlobalData.ChildReadIndex];

    /* Invoke the command specific handler */
    switch (CmdArgs->CommandCode)
    {
        case FM_COPY_CC:
            FM_ChildCopyCmd(CmdArgs);
            break;

        case FM_MOVE_CC:
            FM_ChildMoveCmd(CmdArgs);
            break;

        case FM_RENAME_CC:
            FM_ChildRenameCmd(CmdArgs);
            break;

        case FM_DELETE_CC:
            FM_ChildDeleteCmd(CmdArgs);
            break;

        case FM_DELETE_ALL_CC:
            FM_ChildDeleteAllCmd(CmdArgs);
            break;

        case FM_DECOMPRESS_CC:
            FM_ChildDecompressCmd(CmdArgs);
            break;

        case FM_CONCAT_CC:
            FM_ChildConcatCmd(CmdArgs);
            break;

        case FM_CREATE_DIR_CC:
            FM_ChildCreateDirCmd(CmdArgs);
            break;

        case FM_DELETE_DIR_CC:
            FM_ChildDeleteDirCmd(CmdArgs);
            break;

        case FM_GET_FILE_INFO_CC:
            FM_ChildFileInfoCmd(CmdArgs);
            break;

        case FM_GET_DIR_FILE_CC:
            FM_ChildDirListFileCmd(CmdArgs);
            break;

        case FM_GET_DIR_PKT_CC:
            FM_ChildDirListPktCmd(CmdArgs);
            break;

        case FM_DELETE_INT_CC:
            FM_ChildDeleteCmd(CmdArgs);
            break;
            
        case FM_SET_FILE_PERM_CC:
            FM_ChildSetPermissionsCmd(CmdArgs);
            break;

        default:
            FM_GlobalData.ChildCmdErrCounter++;
            CFE_EVS_SendEvent(FM_CHILD_EXE_ERR_EID, CFE_EVS_ERROR,
               "%s execution error: invalid command code: cc = %d",
                TaskText, (int)CmdArgs->CommandCode);
            break;
    }

    /* Update the handshake queue read index */
    FM_GlobalData.ChildReadIndex++;

    if (FM_GlobalData.ChildReadIndex >= FM_CHILD_QUEUE_DEPTH)
    {
        FM_GlobalData.ChildReadIndex = 0;
    }

    /* Prevent parent/child updating queue counter at same time */
    OS_MutSemTake(FM_GlobalData.ChildQueueCountSem);
    FM_GlobalData.ChildQueueCount--;
    OS_MutSemGive(FM_GlobalData.ChildQueueCountSem);

    return;

} /* End of FM_ChildProcess() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM child task command handler -- Copy File                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void FM_ChildCopyCmd(FM_ChildQueueEntry_t *CmdArgs)
{
    char *CmdText = "Copy File";
    int32 OS_Status;

    /* Report current child task activity */
    FM_GlobalData.ChildCurrentCC = CmdArgs->CommandCode;

    /* Note the order of the arguments to OS_cp (src,tgt) */
    OS_Status = OS_cp(CmdArgs->Source1, CmdArgs->Target);

    if (OS_Status != OS_SUCCESS)
    {
        FM_GlobalData.ChildCmdErrCounter++;

        /* Send command failure event (error) */
        CFE_EVS_SendEvent(FM_COPY_OS_ERR_EID, CFE_EVS_ERROR,
           "%s error: OS_cp failed: result = %d, src = %s, tgt = %s",
            CmdText, (int)OS_Status, CmdArgs->Source1, CmdArgs->Target);
    }
    else
    {
        FM_GlobalData.ChildCmdCounter++;

        /* Send command completion event (info) */
        CFE_EVS_SendEvent(FM_COPY_CMD_EID, CFE_EVS_DEBUG,
           "%s command: src = %s, tgt = %s",
            CmdText, CmdArgs->Source1, CmdArgs->Target);
    }

    /* Report previous child task activity */
    FM_GlobalData.ChildPreviousCC = CmdArgs->CommandCode;
    FM_GlobalData.ChildCurrentCC = 0;

    return;

} /* End of FM_ChildCopyCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM child task command handler -- Move File                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void FM_ChildMoveCmd(FM_ChildQueueEntry_t *CmdArgs)
{
    char *CmdText = "Move File";
    int32 OS_Status;

    /* Report current child task activity */
    FM_GlobalData.ChildCurrentCC = CmdArgs->CommandCode;

    OS_Status = OS_mv(CmdArgs->Source1, CmdArgs->Target);

    if (OS_Status != OS_SUCCESS)
    {
        FM_GlobalData.ChildCmdErrCounter++;

        /* Send command failure event (error) */
        CFE_EVS_SendEvent(FM_MOVE_OS_ERR_EID, CFE_EVS_ERROR,
           "%s error: OS_mv failed: result = %d, src = %s, tgt = %s",
            CmdText, (int)OS_Status, CmdArgs->Source1, CmdArgs->Target);
    }
    else
    {
        FM_GlobalData.ChildCmdCounter++;

        /* Send command completion event (info) */
        CFE_EVS_SendEvent(FM_MOVE_CMD_EID, CFE_EVS_DEBUG,
           "%s command: src = %s, tgt = %s",
            CmdText, CmdArgs->Source1, CmdArgs->Target);
    }

    /* Report previous child task activity */
    FM_GlobalData.ChildPreviousCC = CmdArgs->CommandCode;
    FM_GlobalData.ChildCurrentCC = 0;

    return;

} /* End of FM_ChildMoveCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM child task command handler -- Rename File                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void FM_ChildRenameCmd(FM_ChildQueueEntry_t *CmdArgs)
{
    char *CmdText = "Rename File";
    int32 OS_Status;

    /* Report current child task activity */
    FM_GlobalData.ChildCurrentCC = CmdArgs->CommandCode;

    OS_Status = OS_rename(CmdArgs->Source1, CmdArgs->Target);

    if (OS_Status != OS_SUCCESS)
    {
        FM_GlobalData.ChildCmdErrCounter++;

        /* Send command failure event (error) */
        CFE_EVS_SendEvent(FM_RENAME_OS_ERR_EID, CFE_EVS_ERROR,
           "%s error: OS_rename failed: result = %d, src = %s, tgt = %s",
            CmdText, (int)OS_Status, CmdArgs->Source1, CmdArgs->Target);
    }
    else
    {
        FM_GlobalData.ChildCmdCounter++;

        /* Send command completion event (info) */
        CFE_EVS_SendEvent(FM_RENAME_CMD_EID, CFE_EVS_DEBUG,
           "%s command: src = %s, tgt = %s",
            CmdText, CmdArgs->Source1, CmdArgs->Target);
    }

    /* Report previous child task activity */
    FM_GlobalData.ChildPreviousCC = CmdArgs->CommandCode;
    FM_GlobalData.ChildCurrentCC = 0;

    return;

} /* End of FM_ChildRenameCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM child task command handler -- Delete File                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void FM_ChildDeleteCmd(FM_ChildQueueEntry_t *CmdArgs)
{
    char *CmdText = "Delete File";
    int32 OS_Status;

    /* Report current child task activity */
    FM_GlobalData.ChildCurrentCC = CmdArgs->CommandCode;

    OS_Status = OS_remove(CmdArgs->Source1);

    if (OS_Status != OS_SUCCESS)
    {
        FM_GlobalData.ChildCmdErrCounter++;

        /* Send command failure event (error) */
        CFE_EVS_SendEvent(FM_DELETE_OS_ERR_EID, CFE_EVS_ERROR,
           "%s error: OS_remove failed: result = %d, file = %s",
            CmdText, (int)OS_Status, CmdArgs->Source1);
    }
    else
    {
        FM_GlobalData.ChildCmdCounter++;

        if (CmdArgs->CommandCode != FM_DELETE_INT_CC)
        {
            /* Send command completion event (info) */
            CFE_EVS_SendEvent(FM_DELETE_CMD_EID, CFE_EVS_DEBUG,
               "%s command: file = %s", CmdText, CmdArgs->Source1);
        }
    }

    /* Report previous child task activity */
    FM_GlobalData.ChildPreviousCC = CmdArgs->CommandCode;
    FM_GlobalData.ChildCurrentCC = 0;

    return;

} /* End of FM_ChildDeleteCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM child task command handler -- Delete All Files               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void FM_ChildDeleteAllCmd(FM_ChildQueueEntry_t *CmdArgs)
{
    char        *CmdText = "Delete All Files";
    char        *Directory;
    char        *DirWithSep;
    os_dirp_t    DirPtr;
    os_dirent_t *DirEntry;
    int32        OS_Status;
    uint32       FilenameState;
    uint32       NameLength;
    uint32       DeleteCount = 0;
    uint32       FilesNotDeletedCount = 0;
    uint32       DirectoriesSkippedCount = 0;
    char         Filename[OS_MAX_PATH_LEN];

    /* Report current child task activity */
    FM_GlobalData.ChildCurrentCC = CmdArgs->CommandCode;

    /*
    ** Command argument useage for this command:
    **
    **  CmdArgs->CommandCode = FM_DELETE_ALL_CC
    **  CmdArgs->Source1     = directory name
    **  CmdArgs->Source2     = directory name plus separator
    */
    Directory  = CmdArgs->Source1;
    DirWithSep = CmdArgs->Source2;

    /* Open directory so that we can read from it */
    DirPtr = OS_opendir(Directory);

    if (DirPtr == NULL)
    {
        FM_GlobalData.ChildCmdErrCounter++;

        /* Send command failure event (error) */
        CFE_EVS_SendEvent(FM_DELETE_ALL_OS_ERR_EID, CFE_EVS_ERROR,
           "%s error: OS_opendir failed: dir = %s",
            CmdText, Directory);
    }
    else
    {
        /* Read each directory entry and delete the files */
        while ((DirEntry = OS_readdir(DirPtr)) != NULL )
        {
            /* 
            ** Ignore the "." and ".." directory entries 
            */
            if ((strcmp(OS_DIRENTRY_NAME(*DirEntry), FM_THIS_DIRECTORY) != 0) &&
                (strcmp(OS_DIRENTRY_NAME(*DirEntry), FM_PARENT_DIRECTORY) != 0))
            {
                /* Construct full path filename */
                NameLength = strlen(DirWithSep) + strlen(OS_DIRENTRY_NAME(*DirEntry));

                if (NameLength >= OS_MAX_PATH_LEN)
                {
                   FilesNotDeletedCount++;
                }
                else
                {
                    /* Note: Directory name already has trailing "/" appended */
                    strcpy(Filename, DirWithSep);
                    strcat(Filename, OS_DIRENTRY_NAME(*DirEntry));

                    /* What kind of directory entry is this? */
                    FilenameState = FM_GetFilenameState(Filename, OS_MAX_PATH_LEN, FALSE);

                    if (FilenameState == FM_NAME_IS_INVALID)
                    {
                        FilesNotDeletedCount++;
                    }
                    else if (FilenameState == FM_NAME_IS_NOT_IN_USE)
                    {
                        /* This result is very unlikely - the */
                        /*   name existed a moment ago when   */
                        /*   the directory entry was read but */
                        /*   now the call to OS_stat() failed */
                        /*   implying that the entry is gone  */
                        FilesNotDeletedCount++;
                    }
                    else if (FilenameState == FM_NAME_IS_DIRECTORY)
                    {
                        DirectoriesSkippedCount++;
                    }
                    else if (FilenameState == FM_NAME_IS_FILE_OPEN)
                    {
                       FilesNotDeletedCount++;
                    }
                    else if (FilenameState == FM_NAME_IS_FILE_CLOSED)
                    {
                        if ((OS_Status = OS_remove(Filename)) == OS_SUCCESS)
                        {
                            /*
                            ** After deleting the file, rewind the directory
                            ** to keep the file system from getting confused
                            */
                            OS_rewinddir(DirPtr);

                            /* Increment delete count */
                            DeleteCount++;
                        }
                        else
                        {
                            FilesNotDeletedCount++;
                        }
                    }
                }
            } /* end if "." or ".." directory entries */
        } /* End while OS_readdir */

        OS_closedir(DirPtr);
       
        /* Send command completion event (info) */
        CFE_EVS_SendEvent(FM_DELETE_ALL_CMD_EID, CFE_EVS_DEBUG,
                          "%s command: deleted %d files: dir = %s",
                          CmdText, (int)DeleteCount, Directory);
        FM_GlobalData.ChildCmdCounter++;

        if ( FilesNotDeletedCount > 0 )
        {
           /* If errors occured, report generic event(s) */
           CFE_EVS_SendEvent(FM_DELETE_ALL_WARNING_EID, CFE_EVS_INFORMATION,
           "%s command: one or more files could not be deleted. Files may be open : dir = %s",
            CmdText, Directory);
           FM_GlobalData.ChildCmdWarnCounter++;
        }

        if ( DirectoriesSkippedCount > 0 )
        {
           /* If errors occured, report generic event(s) */
           CFE_EVS_SendEvent(FM_DELETE_ALL_WARNING_EID, CFE_EVS_INFORMATION,
           "%s command: one or more directories skipped : dir = %s",
            CmdText, Directory);
           FM_GlobalData.ChildCmdWarnCounter++;
        }

    } /* end if DirPtr == NULL */

    /* Report previous child task activity */
    FM_GlobalData.ChildPreviousCC = CmdArgs->CommandCode;
    FM_GlobalData.ChildCurrentCC = 0;

    return;

} /* End of FM_ChildDeleteAllCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM child task command handler -- Decompress File                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void FM_ChildDecompressCmd(FM_ChildQueueEntry_t *CmdArgs)
{
    char    *CmdText = "Decompress File";
    int32    CFE_Status;

    /* Report current child task activity */
    FM_GlobalData.ChildCurrentCC = CmdArgs->CommandCode;

    /* Decompress source file into target file */
    CFE_Status = CFE_FS_Decompress(CmdArgs->Source1, CmdArgs->Target);

    if (CFE_Status != CFE_SUCCESS)
    {
        FM_GlobalData.ChildCmdErrCounter++;

        /* Send command failure event (error) */
        CFE_EVS_SendEvent(FM_DECOM_CFE_ERR_EID, CFE_EVS_ERROR,
           "%s error: CFE_FS_Decompress failed: result = %d, src = %s, tgt = %s",
            CmdText, (int)CFE_Status, CmdArgs->Source1, CmdArgs->Target);
    }
    else
    {
        FM_GlobalData.ChildCmdCounter++;

        /* Send command completion event (info) */
        CFE_EVS_SendEvent(FM_DECOM_CMD_EID, CFE_EVS_DEBUG,
           "%s command: src = %s, tgt = %s",
            CmdText, CmdArgs->Source1, CmdArgs->Target);
    }

    /* Report previous child task activity */
    FM_GlobalData.ChildPreviousCC = CmdArgs->CommandCode;
    FM_GlobalData.ChildCurrentCC = 0;

    return;

} /* End of FM_ChildDecompressCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM child task command handler -- Concatenate Files              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void FM_ChildConcatCmd(FM_ChildQueueEntry_t *CmdArgs)
{
    char       *CmdText = "Concat Files";
    boolean     ConcatResult = FALSE;
    boolean     CopyInProgress = FALSE;
    boolean     CreatedTgtFile = FALSE;
    boolean     OpenedSource2 = FALSE;
    boolean     OpenedTgtFile = FALSE;
    int32       LoopCount;
    int32       OS_Status;
    int32       FileHandleSrc;
    int32       FileHandleTgt;
    int32       BytesRead;
    int32       BytesWritten;

    /* Report current child task activity */
    FM_GlobalData.ChildCurrentCC = CmdArgs->CommandCode;

    /* Copy source file #1 to the target file */
    OS_Status = OS_cp(CmdArgs->Source1, CmdArgs->Target);

    if (OS_Status != OS_SUCCESS)
    {
        FM_GlobalData.ChildCmdErrCounter++;

        /* Send command failure event (error) */
        CFE_EVS_SendEvent(FM_CONCAT_OS_ERR_EID, CFE_EVS_ERROR,
           "%s error: OS_cp failed: result = %d, src = %s, tgt = %s",
            CmdText, (int)OS_Status, CmdArgs->Source1, CmdArgs->Target);
    }
    else
    {
        CreatedTgtFile = TRUE;
    }

    /* Open source file #2 */
    if (CreatedTgtFile)
    {
        FileHandleSrc = OS_open(CmdArgs->Source2, OS_READ_ONLY, 0);

        if (FileHandleSrc < 0)
        {
            FM_GlobalData.ChildCmdErrCounter++;

            /* Send command failure event (error) */
            CFE_EVS_SendEvent(FM_CONCAT_OS_ERR_EID, CFE_EVS_ERROR,
               "%s error: OS_open failed: result = %d, src2 = %s",
                CmdText, (int)FileHandleSrc, CmdArgs->Source2);
        }
        else
        {
            OpenedSource2 = TRUE;
        }
    }

    /* Open target file */
    if (OpenedSource2)
    {
        FileHandleTgt = OS_open(CmdArgs->Target, OS_READ_WRITE, 0);

        if (FileHandleTgt < 0)
        {
            FM_GlobalData.ChildCmdErrCounter++;

            /* Send command failure event (error) */
            CFE_EVS_SendEvent(FM_CONCAT_OS_ERR_EID, CFE_EVS_ERROR,
               "%s error: OS_open failed: result = %d, tgt = %s",
                CmdText, (int)FileHandleTgt, CmdArgs->Target);
        }
        else
        {
            OpenedTgtFile = TRUE;
        }
    }

    /* Append source file #2 to target file */
    if (OpenedTgtFile)
    {
        /* Seek to end of target file */
        OS_lseek(FileHandleTgt, 0, OS_SEEK_END);
        CopyInProgress = TRUE;
        LoopCount = 0;

        while (CopyInProgress)
        {
            BytesRead = OS_read(FileHandleSrc, FM_GlobalData.ChildBuffer,
                                FM_CHILD_FILE_BLOCK_SIZE);

            if (BytesRead == 0)
            {
                /* Success - finished reading source file #2 */
                CopyInProgress = FALSE;
                ConcatResult = TRUE;
            }
            else if (BytesRead < 0)
            {
                CopyInProgress = FALSE;
                FM_GlobalData.ChildCmdErrCounter++;

                /* Send command failure event (error) */
                CFE_EVS_SendEvent(FM_CONCAT_OS_ERR_EID, CFE_EVS_ERROR,
                   "%s error: OS_read failed: result = %d, file = %s",
                    CmdText, (int)BytesRead, CmdArgs->Source2);
            }
            else
            {
                /* Write source file #2 to target file */
                BytesWritten = OS_write(FileHandleTgt, FM_GlobalData.ChildBuffer, BytesRead);

                if (BytesWritten != BytesRead)
                {
                    CopyInProgress = FALSE;
                    FM_GlobalData.ChildCmdErrCounter++;

                    /* Send command failure event (error) */
                    CFE_EVS_SendEvent(FM_CONCAT_OS_ERR_EID, CFE_EVS_ERROR,
                       "%s error: OS_write failed: result = %d, expected = %d",
                        CmdText, (int)BytesWritten, (int)BytesRead);
                }
            }

            /* Avoid CPU hogging */
            if (CopyInProgress)
            {
                LoopCount++;
                if (LoopCount == FM_CHILD_FILE_LOOP_COUNT)
                {
                    /* Give up the CPU */
                    CFE_ES_PerfLogExit(FM_CHILD_TASK_PERF_ID);
                    OS_TaskDelay(FM_CHILD_FILE_SLEEP_MS);
                    CFE_ES_PerfLogEntry(FM_CHILD_TASK_PERF_ID);
                    LoopCount = 0;
                }
            }
        }
    }

    if (OpenedTgtFile)
    {
        /* Close target file */
        OS_close(FileHandleTgt);
    }

    if (OpenedSource2)
    {
        /* Close source file #2 */
        OS_close(FileHandleSrc);
    }

    if ((CreatedTgtFile == TRUE) && (ConcatResult == FALSE))
    {
        /* Remove partial target file after concat error */
        OS_remove(CmdArgs->Target);
    }

    if (ConcatResult == TRUE)
    {
        FM_GlobalData.ChildCmdCounter++;

        /* Send command completion event (info) */
        CFE_EVS_SendEvent(FM_CONCAT_CMD_EID, CFE_EVS_DEBUG,
           "%s command: src1 = %s, src2 = %s, tgt = %s",
            CmdText, CmdArgs->Source1, CmdArgs->Source2, CmdArgs->Target);
    }

    /* Report previous child task activity */
    FM_GlobalData.ChildPreviousCC = CmdArgs->CommandCode;
    FM_GlobalData.ChildCurrentCC = 0;

    return;

} /* End of FM_ChildConcatCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM child task command handler -- Get File Info                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void FM_ChildFileInfoCmd(FM_ChildQueueEntry_t *CmdArgs)
{
    char  *CmdText = "Get File Info";
    boolean GettingCRC = FALSE;
    uint32 CurrentCRC = 0;
    int32  LoopCount = 0;
    int32  BytesRead;
    int32  FileHandle;

    /* Report current child task activity */
    FM_GlobalData.ChildCurrentCC = CmdArgs->CommandCode;

    /*
    ** Command argument useage for this command:
    **
    **  CmdArgs->CommandCode   = FM_GET_DIR_FILE_CC
    **  CmdArgs->Source1       = name of directory or file
    **  CmdArgs->FileInfoState = state of directory or file
    **  CmdArgs->FileInfoSize  = file size, else zero
    **  CmdArgs->FileInfoTime  = last modify time
    */

    /* Initialize file info packet (set all data to zero) */
    CFE_SB_InitMsg(&FM_GlobalData.FileInfoPkt, FM_FILE_INFO_TLM_MID,
                   sizeof(FM_FileInfoPkt_t), TRUE);

    /* Report directory or filename state, name, size and time */
    FM_GlobalData.FileInfoPkt.FileStatus = (uint8) CmdArgs->FileInfoState;
    strcpy(FM_GlobalData.FileInfoPkt.Filename, CmdArgs->Source1);

    FM_GlobalData.FileInfoPkt.FileSize = CmdArgs->FileInfoSize;
    FM_GlobalData.FileInfoPkt.LastModifiedTime = CmdArgs->FileInfoTime;

    /* Validate CRC algorithm */
    if (CmdArgs->FileInfoCRC != FM_IGNORE_CRC)
    {
        if (CmdArgs->FileInfoState != FM_NAME_IS_FILE_CLOSED)
        {
            /* Can only calculate CRC for closed files */
            FM_GlobalData.ChildCmdWarnCounter++;

            CFE_EVS_SendEvent(FM_GET_FILE_INFO_WARNING_EID, CFE_EVS_INFORMATION,
               "%s warning: unable to compute CRC: invalid file state = %d, file = %s",
                CmdText, (int)CmdArgs->FileInfoState, CmdArgs->Source1);

            CmdArgs->FileInfoCRC = FM_IGNORE_CRC;
        }
        else if ((CmdArgs->FileInfoCRC != CFE_ES_CRC_8) &&
                 (CmdArgs->FileInfoCRC != CFE_ES_CRC_16) &&
                 (CmdArgs->FileInfoCRC != CFE_ES_CRC_32))
        {
            /* Can only calculate CRC using known algorithms */
            FM_GlobalData.ChildCmdWarnCounter++;

            CFE_EVS_SendEvent(FM_GET_FILE_INFO_WARNING_EID, CFE_EVS_INFORMATION,
               "%s warning: unable to compute CRC: invalid CRC type = %d, file = %s",
                CmdText, (int)CmdArgs->FileInfoCRC, CmdArgs->Source1);

            CmdArgs->FileInfoCRC = FM_IGNORE_CRC;
        }
    }

    /* Compute CRC */
    if (CmdArgs->FileInfoCRC != FM_IGNORE_CRC)
    {
        FileHandle = OS_open(CmdArgs->Source1, OS_READ_ONLY, 0);

        if (FileHandle < 0)
        {
            FM_GlobalData.ChildCmdWarnCounter++;

            /* Send CRC failure event (warning) */
            CFE_EVS_SendEvent(FM_GET_FILE_INFO_WARNING_EID, CFE_EVS_ERROR,
               "%s warning: unable to compute CRC: OS_open result = %d, file = %s",
                CmdText, (int)FileHandle, CmdArgs->Source1);

            GettingCRC = FALSE;
        }
        else
        {
            GettingCRC = TRUE;
        }

        while (GettingCRC)
        {
            BytesRead = OS_read(FileHandle, FM_GlobalData.ChildBuffer,
                                FM_CHILD_FILE_BLOCK_SIZE);

            if (BytesRead == 0)
            {
                /* Finished reading file */
                GettingCRC = FALSE;
                OS_close(FileHandle);

                /* Add CRC to telemetry packet */
                FM_GlobalData.FileInfoPkt.CRC_Computed = TRUE;
                FM_GlobalData.FileInfoPkt.CRC = CurrentCRC;
            }
            else if (BytesRead < 0)
            {
                /* Error reading file */
                CurrentCRC = 0;
                GettingCRC = FALSE;
                OS_close(FileHandle);

                /* Send CRC failure event (warning) */
                FM_GlobalData.ChildCmdWarnCounter++;
                CFE_EVS_SendEvent(FM_GET_FILE_INFO_WARNING_EID, CFE_EVS_INFORMATION,
                   "%s warning: unable to compute CRC: OS_read result = %d, file = %s",
                    CmdText, (int)BytesRead, CmdArgs->Source1);
            }
            else
            {
                /* Continue CRC calculation */
                CurrentCRC = CFE_ES_CalculateCRC(FM_GlobalData.ChildBuffer, BytesRead,
                                                 CurrentCRC, CmdArgs->FileInfoCRC);
            }

            /* Avoid CPU hogging */
            if (GettingCRC)
            {
                LoopCount++;
                if (LoopCount == FM_CHILD_FILE_LOOP_COUNT)
                {
                    /* Give up the CPU */
                    CFE_ES_PerfLogExit(FM_CHILD_TASK_PERF_ID);
                    OS_TaskDelay(FM_CHILD_FILE_SLEEP_MS);
                    CFE_ES_PerfLogEntry(FM_CHILD_TASK_PERF_ID);
                    LoopCount = 0;
                }
            }
        }

        FM_GlobalData.FileInfoPkt.CRC = CurrentCRC;
    }

    /* Timestamp and send file info telemetry packet */
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &FM_GlobalData.FileInfoPkt);
    CFE_SB_SendMsg((CFE_SB_Msg_t *) &FM_GlobalData.FileInfoPkt);

    FM_GlobalData.ChildCmdCounter++;

    /* Send command completion event (debug) */
    CFE_EVS_SendEvent(FM_GET_FILE_INFO_CMD_EID, CFE_EVS_DEBUG,
       "%s command: file = %s", CmdText, CmdArgs->Source1);

    /* Report previous child task activity */
    FM_GlobalData.ChildPreviousCC = CmdArgs->CommandCode;
    FM_GlobalData.ChildCurrentCC = 0;

    return;

} /* End of FM_ChildFileInfoCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM child task command handler -- Create Directory               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void FM_ChildCreateDirCmd(FM_ChildQueueEntry_t *CmdArgs)
{
    char *CmdText = "Create Directory";
    int32 OS_Status;

    /* Report current child task activity */
    FM_GlobalData.ChildCurrentCC = CmdArgs->CommandCode;

    OS_Status = OS_mkdir(CmdArgs->Source1, 0);

    if (OS_Status != OS_SUCCESS)
    {
        FM_GlobalData.ChildCmdErrCounter++;

        /* Send command failure event (error) */
        CFE_EVS_SendEvent(FM_CREATE_DIR_OS_ERR_EID, CFE_EVS_ERROR,
           "%s error: OS_mkdir failed: result = %d, dir = %s",
            CmdText, (int)OS_Status, CmdArgs->Source1);
    }
    else
    {
        FM_GlobalData.ChildCmdCounter++;

        /* Send command completion event (info) */
        CFE_EVS_SendEvent(FM_CREATE_DIR_CMD_EID, CFE_EVS_DEBUG,
           "%s command: src = %s", CmdText, CmdArgs->Source1);
    }

    /* Report previous child task activity */
    FM_GlobalData.ChildPreviousCC = CmdArgs->CommandCode;
    FM_GlobalData.ChildCurrentCC = 0;

    return;

} /* End of FM_ChildCreateDirCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM child task command handler -- Delete Directory               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void FM_ChildDeleteDirCmd(FM_ChildQueueEntry_t *CmdArgs)
{
    char *CmdText = "Delete Directory";
    boolean RemoveTheDir = TRUE;
    os_dirp_t DirPtr;
    os_dirent_t *DirEntry;
    int32 OS_Status;

    /* Report current child task activity */
    FM_GlobalData.ChildCurrentCC = CmdArgs->CommandCode;

    /* Open the dir so we can see if it is empty */
    DirPtr = OS_opendir(CmdArgs->Source1);

    if (DirPtr == NULL)
    {
        CFE_EVS_SendEvent(FM_DELETE_DIR_OS_ERR_EID, CFE_EVS_ERROR,
           "%s error: OS_opendir failed: dir = %s",
            CmdText, CmdArgs->Source1);

        RemoveTheDir = FALSE;
        FM_GlobalData.ChildCmdErrCounter++;
    }
    else
    {
        /* Look for a directory entry that is not "." or ".." */
        while (((DirEntry = OS_readdir(DirPtr)) != NULL) && (RemoveTheDir == TRUE))
        {
            if ((strcmp(OS_DIRENTRY_NAME(*DirEntry), FM_THIS_DIRECTORY) != 0) &&
                (strcmp(OS_DIRENTRY_NAME(*DirEntry), FM_PARENT_DIRECTORY) != 0))
            {
                CFE_EVS_SendEvent(FM_DELETE_DIR_EMPTY_ERR_EID, CFE_EVS_ERROR,
                   "%s error: directory is not empty: dir = %s",
                    CmdText, CmdArgs->Source1);

                RemoveTheDir = FALSE;
                FM_GlobalData.ChildCmdErrCounter++;
            }
        }

        OS_closedir(DirPtr);
    }

    if (RemoveTheDir)
    {
        /* Remove the directory */
        OS_Status = OS_rmdir(CmdArgs->Source1);

        if (OS_Status != OS_SUCCESS)
        {
            /* Send command failure event (error) */
            CFE_EVS_SendEvent(FM_DELETE_DIR_OS_ERR_EID, CFE_EVS_ERROR,
               "%s error: OS_rmdir failed: result = %d, dir = %s",
                CmdText, (int)OS_Status, CmdArgs->Source1);

            FM_GlobalData.ChildCmdErrCounter++;
        }
        else
        {
            /* Send command completion event (info) */
            CFE_EVS_SendEvent(FM_DELETE_DIR_CMD_EID, CFE_EVS_DEBUG,
               "%s command: src = %s", CmdText, CmdArgs->Source1);

            FM_GlobalData.ChildCmdCounter++;
        }
    }

    /* Report previous child task activity */
    FM_GlobalData.ChildPreviousCC = CmdArgs->CommandCode;
    FM_GlobalData.ChildCurrentCC = 0;

    return;


} /* End of FM_ChildDeleteDirCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM child task command handler -- Get Directory List (to file)   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void FM_ChildDirListFileCmd(FM_ChildQueueEntry_t *CmdArgs)
{
    char     *CmdText = "Directory List to File";
    boolean   Result;
    int32     FileHandle;
    os_dirp_t DirPtr;

    /* Report current child task activity */
    FM_GlobalData.ChildCurrentCC = CmdArgs->CommandCode;

    /*
    ** Command argument useage for this command:
    **
    **  CmdArgs->CommandCode = FM_GET_DIR_FILE_CC
    **  CmdArgs->Source1     = directory name
    **  CmdArgs->Source2     = directory name plus separator
    **  CmdArgs->Target      = output filename
    */

    /* Open directory for reading directory list */
    DirPtr = OS_opendir(CmdArgs->Source1);

    if (DirPtr == NULL)
    {
        FM_GlobalData.ChildCmdErrCounter++;

        /* Send command failure event (error) */
        CFE_EVS_SendEvent(FM_GET_DIR_FILE_OS_ERR_EID, CFE_EVS_ERROR,
           "%s error: OS_opendir failed: dir = %s",
            CmdText, CmdArgs->Source1);
    }
    else
    {
        /* Create output file, write placeholder for statistics, etc. */
        Result = FM_ChildDirListFileInit(&FileHandle, CmdArgs->Source1,
                                         CmdArgs->Target);
        if (Result == TRUE)
        {
            /* Read directory listing and write contents to output file */
            FM_ChildDirListFileLoop(DirPtr, FileHandle, CmdArgs->Source1,
                                    CmdArgs->Source2, CmdArgs->Target, CmdArgs->GetSizeTimeMode);

            /* Close output file */
            OS_close(FileHandle);
        }

        /* Close directory list access handle */
        OS_closedir(DirPtr);
    }

    /* Report previous child task activity */
    FM_GlobalData.ChildPreviousCC = CmdArgs->CommandCode;
    FM_GlobalData.ChildCurrentCC = 0;

    return;

} /* End of FM_ChildDirListFileCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM child task command handler -- Get Directory List (to pkt)    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void FM_ChildDirListPktCmd(FM_ChildQueueEntry_t *CmdArgs)
{
    char                     *CmdText = "Directory List to Packet";
    char                      LogicalName[OS_MAX_PATH_LEN];
    boolean                   StillProcessing;
    os_dirp_t                 DirPtr;
    os_dirent_t              *DirEntry;
    int32                     ListIndex;
    FM_DirListEntry_t        *ListEntry;
    int32                     PathLength;
    int32                     EntryLength;
    int32                     FilesTillSleep = FM_CHILD_STAT_SLEEP_FILECOUNT;

    /* Report current child task activity */
    FM_GlobalData.ChildCurrentCC = CmdArgs->CommandCode;

    /*
    ** Command argument useage for this command:
    **
    **  CmdArgs->CommandCode   = FM_GET_DIR_PKT_CC
    **  CmdArgs->Source1       = directory name
    **  CmdArgs->Source2       = directory name plus separator
    **  CmdArgs->DirListOffset = index of 1st reported dir entry
    */
    PathLength = strlen(CmdArgs->Source2);

    /* Open source directory for reading directory list */
    DirPtr = OS_opendir(CmdArgs->Source1);

    if (DirPtr == NULL)
    {
        FM_GlobalData.ChildCmdErrCounter++;

        /* Send command failure event (error) */
        CFE_EVS_SendEvent(FM_GET_DIR_PKT_OS_ERR_EID, CFE_EVS_ERROR,
           "%s error: OS_opendir failed: dir = %s",
            CmdText, CmdArgs->Source1);
    }
    else
    {
        /* Initialize the directory list telemetry packet */
        CFE_SB_InitMsg(&FM_GlobalData.DirListPkt, FM_DIR_LIST_TLM_MID,
                        sizeof(FM_DirListPkt_t), TRUE);

        strncpy(FM_GlobalData.DirListPkt.DirName, CmdArgs->Source1, OS_MAX_PATH_LEN);
        FM_GlobalData.DirListPkt.FirstFile = CmdArgs->DirListOffset;

        StillProcessing = TRUE;
        while (StillProcessing == TRUE)
        {
            /* Read next directory entry */
            DirEntry = OS_readdir(DirPtr);

            if (DirEntry == NULL)
            {
                /* Stop reading directory - no more entries */
                StillProcessing = FALSE;
            }
            else if ((strcmp(OS_DIRENTRY_NAME(*DirEntry), FM_THIS_DIRECTORY) != 0) &&
                     (strcmp(OS_DIRENTRY_NAME(*DirEntry), FM_PARENT_DIRECTORY) != 0))
            {
                /* Do not count the "." and ".." directory entries */
                FM_GlobalData.DirListPkt.TotalFiles++;

                /* Start collecting directory entries at command specified offset */
                /* Stop collecting directory entries when telemetry packet is full */
                if ((FM_GlobalData.DirListPkt.TotalFiles > FM_GlobalData.DirListPkt.FirstFile) &&
                    (FM_GlobalData.DirListPkt.PacketFiles < FM_DIR_LIST_PKT_ENTRIES))
                {
                    /* Create a shorthand access to the packet list entry */
                    ListIndex = FM_GlobalData.DirListPkt.PacketFiles;
                    ListEntry = &FM_GlobalData.DirListPkt.FileList[ListIndex];

                    EntryLength = strlen(OS_DIRENTRY_NAME(*DirEntry));

                    /* Verify combined directory plus filename length */
                    if ((EntryLength < sizeof(ListEntry->EntryName)) &&
                       ((PathLength + EntryLength) < OS_MAX_PATH_LEN))
                    {
                        /* Add filename to directory listing telemetry packet */
                        strcpy(ListEntry->EntryName, OS_DIRENTRY_NAME(*DirEntry));

                        /* Build filename - Directory already has path separator */
                        strcpy(LogicalName, CmdArgs->Source2);
                        strcat(LogicalName, OS_DIRENTRY_NAME(*DirEntry));

                        FM_ChildSleepStat(LogicalName, ListEntry, &FilesTillSleep, CmdArgs->GetSizeTimeMode);

                        /* Add another entry to the telemetry packet */
                        FM_GlobalData.DirListPkt.PacketFiles++;
                    }
                    else
                    {
                        FM_GlobalData.ChildCmdWarnCounter++;

                        /* Send command warning event (info) */
                        CFE_EVS_SendEvent(FM_GET_DIR_PKT_WARNING_EID, CFE_EVS_INFORMATION,
                           "%s warning: dir + entry is too long: dir = %s, entry = %s",
                            CmdText, CmdArgs->Source2, OS_DIRENTRY_NAME(*DirEntry));
                    }
                }
            }
        }

        OS_closedir(DirPtr);

        /* Timestamp and send directory listing telemetry packet */
        CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &FM_GlobalData.DirListPkt);
        CFE_SB_SendMsg((CFE_SB_Msg_t *) &FM_GlobalData.DirListPkt);

        /* Send command completion event (info) */
        CFE_EVS_SendEvent(FM_GET_DIR_PKT_CMD_EID, CFE_EVS_DEBUG,
           "%s command: offset = %d, dir = %s",
            CmdText, (int)CmdArgs->DirListOffset, CmdArgs->Source1);

        FM_GlobalData.ChildCmdCounter++;
    }

    /* Report previous child task activity */
    FM_GlobalData.ChildPreviousCC = CmdArgs->CommandCode;
    FM_GlobalData.ChildCurrentCC = 0;

    return;

} /* End of FM_ChildDirListPktCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM child task command handler -- Set File Permissions           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void FM_ChildSetPermissionsCmd(FM_ChildQueueEntry_t *CmdArgs)
{
    int32 OS_Status;
    char *CmdText = "Set Permissions";
    
    OS_Status = OS_chmod(CmdArgs->Source1, CmdArgs->Mode);

    if (OS_Status == OS_SUCCESS)
    {
        FM_GlobalData.ChildCmdCounter++;

        /* Send command completion event (info) */
        CFE_EVS_SendEvent(FM_SET_PERM_CMD_EID, CFE_EVS_DEBUG,
           "%s command: file = %s, access = %d", 
                CmdText, CmdArgs->Source1, (int)CmdArgs->Mode);
    }
    else
    {
        FM_GlobalData.ChildCmdErrCounter++;

        /* Send OS error message */
        CFE_EVS_SendEvent(FM_SET_PERM_OS_ERR_EID, CFE_EVS_ERROR,
           "%s command: OS_chmod error, RC=0x%08X, file = %s, access = %d", 
                CmdText, (unsigned int)OS_Status, CmdArgs->Source1, (int)CmdArgs->Mode);
    }
    
    return;
}  /* End of FM_ChildSetPermissionsCmd() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM child task utility function -- create dir list output file   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

boolean FM_ChildDirListFileInit(int32 *FileHandlePtr, char *Directory, char *Filename)
{
    char  *CmdText  = "Directory List to File";
    boolean            CommandResult = TRUE;
    CFE_FS_Header_t    FileHeader;
    int32              FileHandle;
    int32              BytesWritten;

    /* Initialize the standard cFE File Header for the Directory Listing File */
    CFE_PSP_MemSet(&FileHeader, 0, sizeof(CFE_FS_Header_t));
    FileHeader.SubType = FM_DIR_LIST_FILE_SUBTYPE;
    strcpy(FileHeader.Description, CmdText);

    /* Create directory listing output file */
    FileHandle = OS_creat(Filename, OS_READ_WRITE);
    if (FileHandle >= OS_SUCCESS)
    {
        /* Write the standard CFE file header */
        BytesWritten = CFE_FS_WriteHeader(FileHandle, &FileHeader);
        if (BytesWritten == sizeof(CFE_FS_Header_t))
        {
            /* Initialize directory statistics structure */
            CFE_PSP_MemSet(&FM_GlobalData.DirListFileStats, 0, sizeof(FM_DirListFileStats_t));
            strcpy(FM_GlobalData.DirListFileStats.DirName, Directory);

            /* Write blank FM directory statistics structure as a place holder */
            BytesWritten = OS_write(FileHandle, &FM_GlobalData.DirListFileStats, sizeof(FM_DirListFileStats_t));
            if (BytesWritten == sizeof(FM_DirListFileStats_t))
            {
                /* Return output file handle */
                *FileHandlePtr = FileHandle;
            }
            else
            {
                CommandResult = FALSE;
                FM_GlobalData.ChildCmdErrCounter++;

                /* Send command failure event (error) */
                CFE_EVS_SendEvent(FM_GET_DIR_FILE_OS_ERR_EID, CFE_EVS_ERROR,
                   "%s error: OS_write blank stats failed: result = %d, expected = %d",
                    CmdText, (int)BytesWritten, sizeof(FM_DirListFileStats_t));
            }
        }
        else
        {
            CommandResult = FALSE;
            FM_GlobalData.ChildCmdErrCounter++;

            /* Send command failure event (error) */
            CFE_EVS_SendEvent(FM_GET_DIR_FILE_OS_ERR_EID, CFE_EVS_ERROR,
               "%s error: CFE_FS_WriteHeader failed: result = %d, expected = %d",
                CmdText, (int)BytesWritten, sizeof(CFE_FS_Header_t));
        }

        /* Close output file after write error */
        if (CommandResult == FALSE)
        {
            OS_close(FileHandle);
        }
    }
    else
    {
        CommandResult = FALSE;
        FM_GlobalData.ChildCmdErrCounter++;

        /* Send command failure event (error) */
        CFE_EVS_SendEvent(FM_GET_DIR_FILE_OS_ERR_EID, CFE_EVS_ERROR,
           "%s error: OS_creat failed: result = %d, file = %s",
            CmdText, (int)FileHandle, Filename);
    }

    return(CommandResult);

} /* End FM_ChildDirListFileInit */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM child task utility function -- write to dir list output file */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void FM_ChildDirListFileLoop(os_dirp_t DirPtr, int32 FileHandle,
                             char *Directory, char *DirWithSep, char *Filename, uint8 getSizeTimeMode)
{
    char  *CmdText = "Directory List to File";
    int32    WriteLength = sizeof(FM_DirListEntry_t);
    boolean  ReadingDirectory = TRUE;
    boolean  CommandResult = TRUE;
    uint32   DirEntries = 0;
    uint32   FileEntries = 0;
    int32    EntryLength;
    int32    PathLength;
    int32    BytesWritten;
    int32    FilesTillSleep = FM_CHILD_STAT_SLEEP_FILECOUNT;
    char     TempName[OS_MAX_PATH_LEN];
    os_dirent_t *DirEntry;
    FM_DirListEntry_t  DirListData;


    PathLength = strlen(DirWithSep);

    /* Until end of directory entries or output file write error */
    while ((CommandResult == TRUE) && (ReadingDirectory == TRUE))
    {
        DirEntry = OS_readdir(DirPtr);

        /* Normal loop end - no more directory entries */
        if (DirEntry == NULL)
        {
            ReadingDirectory = FALSE;
        }
        else if ((strcmp(OS_DIRENTRY_NAME(*DirEntry), FM_THIS_DIRECTORY) != 0) &&
                 (strcmp(OS_DIRENTRY_NAME(*DirEntry), FM_PARENT_DIRECTORY) != 0))
        {
            /* Do not count the "." and ".." files */
            DirEntries++;

            /* Count all files - write limited number */
            if (FileEntries < FM_DIR_LIST_FILE_ENTRIES)
            {
                EntryLength = strlen(OS_DIRENTRY_NAME(*DirEntry));

                if ((EntryLength < sizeof(DirListData.EntryName)) &&
                   ((PathLength + EntryLength) < OS_MAX_PATH_LEN))
                {
                    /* Build qualified directory entry name */
                    strcpy(TempName, DirWithSep);
                    strcat(TempName, OS_DIRENTRY_NAME(*DirEntry));

                    /* Populate directory list file entry */
                    strcpy(DirListData.EntryName, OS_DIRENTRY_NAME(*DirEntry));

                    FM_ChildSleepStat(TempName, (FM_DirListEntry_t *)&DirListData, &FilesTillSleep, getSizeTimeMode);

                    /* Write directory list file entry to output file */
                    BytesWritten = OS_write(FileHandle, &DirListData, WriteLength);

                    if (BytesWritten == WriteLength)
                    {
                        FileEntries++;
                    }
                    else
                    {
                        CommandResult = FALSE;
                        FM_GlobalData.ChildCmdErrCounter++;

                        /* Send command failure event (error) */
                        CFE_EVS_SendEvent(FM_GET_DIR_FILE_OS_ERR_EID, CFE_EVS_ERROR,
                           "%s error: OS_write entry failed: result = %d, expected = %d",
                            CmdText, (int)BytesWritten, (int)WriteLength);
                    }
                }
                else
                {
                    FM_GlobalData.ChildCmdWarnCounter++;

                    /* Send command failure event (error) */
                    CFE_EVS_SendEvent(FM_GET_DIR_FILE_WARNING_EID, CFE_EVS_INFORMATION,
                       "%s error: combined directory and entry name too long: dir = %s, entry = %s",
                        CmdText, Directory, OS_DIRENTRY_NAME(*DirEntry));
                }
            }
        }
    }

    /* Update directory statistics in output file */
    if ((CommandResult == TRUE) && (DirEntries != 0))
    {
        /* Update entries found in directory vs entries written to file */
        FM_GlobalData.DirListFileStats.DirEntries = DirEntries;
        FM_GlobalData.DirListFileStats.FileEntries = FileEntries;

        /* Back up to the start of the statisitics data */
        OS_lseek(FileHandle, sizeof(CFE_FS_Header_t), OS_SEEK_SET);

        /* Write an updated version of the statistics data */
        WriteLength = sizeof(FM_DirListFileStats_t);
        BytesWritten = OS_write(FileHandle, &FM_GlobalData.DirListFileStats, WriteLength);

        if (BytesWritten != WriteLength)
        {
            CommandResult = FALSE;
            FM_GlobalData.ChildCmdErrCounter++;

            /* Send command failure event (error) */
            CFE_EVS_SendEvent(FM_GET_DIR_FILE_OS_ERR_EID, CFE_EVS_ERROR,
               "%s error: OS_write update stats failed: result = %d, expected = %d",
                CmdText, (int)BytesWritten, (int)WriteLength);
        }
    }

    /* Send command completion event (info) */
    if (CommandResult == TRUE)
    {
        FM_GlobalData.ChildCmdCounter++;

        CFE_EVS_SendEvent(FM_GET_DIR_FILE_CMD_EID, CFE_EVS_DEBUG,
           "%s command: wrote %d of %d names: dir = %s, filename = %s",
            CmdText, (int)FileEntries, (int)DirEntries, Directory, Filename);
    }

    return;

} /* End of FM_ChildDirListFileLoop */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM child task utility function -- get dir entry size and time   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int32 FM_ChildSizeTimeMode(const char *Filename, uint32 *FileSize, uint32 *FileTime, uint32 *FileMode)
{
    int32       Result;
    os_fstat_t  FileStatus;
    uint32      FileStatTime;

    CFE_PSP_MemSet(&FileStatus, 0, sizeof(os_fstat_t));

    Result = OS_stat(Filename, &FileStatus);

    if (Result != OS_SUCCESS)
    {
        *FileSize = 0;
        *FileTime = 0;
    }
    else
    {
        FileStatTime = FileStatus.FileTime;  //dcm

        /* Convert the file system time to spacecraft time */
        *FileTime = CFE_TIME_FS2CFESeconds(FileStatTime);

        *FileSize = FileStatus.FileSize; //dcm
        
        *FileMode = FileStatus.FileModeBits;  //dcm
        
    }

    return(Result);

} /* End of FM_ChildSizeTimeMode */



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* FM child task utility function -- sleep between OS_stat on files*/
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void FM_ChildSleepStat(const char *Filename, FM_DirListEntry_t *DirListData, int32 *FilesTillSleep, boolean getSizeTimeMode)
{
    /* Check if command requested size and time */
    if (getSizeTimeMode == TRUE)
    {
        if (*FilesTillSleep <= 0)
        {
            CFE_ES_PerfLogExit(FM_CHILD_TASK_PERF_ID);
            OS_TaskDelay(FM_CHILD_STAT_SLEEP_MS);
            CFE_ES_PerfLogEntry(FM_CHILD_TASK_PERF_ID);
            *FilesTillSleep = FM_CHILD_STAT_SLEEP_FILECOUNT;
        }

        /* Get file size, date, and mode */
        FM_ChildSizeTimeMode(Filename, &(DirListData->EntrySize),
                            &(DirListData->ModifyTime),
                            &(DirListData ->Mode));
        
        (*FilesTillSleep)--;
    }
    else
    {
        DirListData->EntrySize  = 0;
        DirListData->ModifyTime = 0;
        DirListData->Mode       = 0;
    }
} /* FM_ChildSleepStat */

/************************/
/*  End of File Comment */
/************************/

