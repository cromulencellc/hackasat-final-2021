/* 
** Purpose: Implement a Comm application.
**
** Notes:
**   None
**
** License:
**   Template written by David McComas and licensed under the GNU
**   Lesser General Public License (LGPL).
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/

/*
** Includes
*/

#include <string.h>
#include "comm_app.h"


/*
** Local Function Prototypes
*/

static int32 InitApp(void);
static void ProcessCommands(void);

/*
** Global Data
*/

COMM_Class  Comm;
COMM_HkPkt  CommHkPkt;

/*
** Convenience Macros
*/

#define  CMDMGR_OBJ (&(Comm.CmdMgr))
#define  COMM_OBJ   (&(Comm.CommObj))

/******************************************************************************
** Function: COMM_AppMain
**
*/
void COMM_AppMain(void)
{

   int32  Status    = CFE_SEVERITY_ERROR;
   uint32 RunStatus = CFE_ES_APP_ERROR;


   CFE_ES_PerfLogEntry(COMM_MAIN_PERF_ID);
   Status = CFE_ES_RegisterApp();
   CFE_EVS_Register(NULL,0,0);


   /*
   ** Perform application specific initialization
   */
   if (Status == CFE_SUCCESS)
   {
       Status = InitApp();
   }

   /*
   ** At this point many flight apps use CFE_ES_WaitForStartupSync() to
   ** synchronize their startup timing with other apps. This is not
   ** needed for this simple app.
   */

   if (Status == CFE_SUCCESS) RunStatus = CFE_ES_APP_RUN;

   /*
   ** Main process loop
   */
   while (CFE_ES_RunLoop(&RunStatus))
   {

      /*
      ** This is just a an example loop. There are many ways to control the
      ** main loop execution flow.
      */
	  
	   CFE_ES_PerfLogExit(COMM_MAIN_PERF_ID);
      OS_TaskDelay(COMM_RUNLOOP_DELAY);
      CFE_ES_PerfLogEntry(COMM_MAIN_PERF_ID);
      
      COMM_OBJ_Execute();

      ProcessCommands();

   } /* End CFE_ES_RunLoop */


   /* Write to system log in case events not working */

   CFE_ES_WriteToSysLog("COMM Terminating, RunLoop status = 0x%08X\n", RunStatus);

   CFE_EVS_SendEvent(COMM_EXIT_ERR_EID, CFE_EVS_CRITICAL, "COMM Terminating,  RunLoop status = 0x%08X", RunStatus);

   CFE_ES_PerfLogExit(COMM_MAIN_PERF_ID);
   CFE_ES_ExitApp(RunStatus);  /* Let cFE kill the task (and any child tasks) */

} /* End of COMM_Main() */


/******************************************************************************
** Function: COMM_NoOpCmd
**
** Function signature must match CMDMGR_CmdFuncPtr typedef 
*/

boolean COMM_NoOpCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CFE_EVS_SendEvent (COMM_CMD_NOOP_INFO_EID,
                      CFE_EVS_INFORMATION,
                      "No operation command received for COMM version %d.%d",
                      COMM_MAJOR_VERSION,COMM_MINOR_VERSION);

   return TRUE;


} /* End COMM_NoOpCmd() */


/******************************************************************************
** Function: COMM_ResetAppCmd
**
** Function signature must match CMDMGR_CmdFuncPtr typedef 
*/

boolean COMM_ResetAppCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CMDMGR_ResetStatus(CMDMGR_OBJ);
   COMM_OBJ_ResetStatus();

   return TRUE;

} /* End COMM_ResetAppCmd() */


/******************************************************************************
** Function: COMM_SendHousekeepingPkt
**
*/
void COMM_SendHousekeepingPkt(void)
{
   
   /*
   ** CMDMGR Data
   */

   CommHkPkt.ValidCmdCnt   = Comm.CmdMgr.ValidCmdCnt;
   CommHkPkt.InvalidCmdCnt = Comm.CmdMgr.InvalidCmdCnt;

   /*
   ** Send COMM status message to telemtry
   */
  strncpy(&(CommHkPkt.CommStatusString), &(Comm.CommObj.CommStatusString), sizeof(CommHkPkt.CommStatusString));
  //CFE_ES_WriteToSysLog ("CommApp: %s, CommHkPkt: %s", CommHkPkt.CommStatusString, Comm.CommObj.CommStatusString);
   
   /*
   ** EXOBJ Data
   ** - At a minimum all OBJECT variables effected by a reset must be included
   */
   CommHkPkt.CommObjExecCnt = Comm.CommObj.ExecCnt;

   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &CommHkPkt);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &CommHkPkt);

} /* End COMM_SendHousekeepingPkt() */


/******************************************************************************
** Function: InitApp
**
*/
static int32 InitApp(void)
{
    int32 Status = CFE_SUCCESS;

    
    /*
    ** Initialize 'entity' objects
    */

    COMM_OBJ_Constructor(COMM_OBJ);

    /*
    ** Initialize cFE interfaces 
    */

    CFE_SB_CreatePipe(&Comm.CmdPipe, COMM_CMD_PIPE_DEPTH, COMM_CMD_PIPE_NAME);
    CFE_SB_Subscribe(COMM_CMD_MID, Comm.CmdPipe);
    CFE_SB_Subscribe(COMM_SEND_HK_MID, Comm.CmdPipe);

    /*
    ** Initialize App Framework Components 
    */

    CMDMGR_Constructor(CMDMGR_OBJ);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_NOOP_CMD_FC,  NULL, COMM_NoOpCmd,     0);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_RESET_CMD_FC, NULL, COMM_ResetAppCmd, 0);
                         
    CFE_SB_InitMsg(&CommHkPkt, COMM_TLM_HK_MID, COMM_TLM_HK_LEN, TRUE);

                        
    /*
    ** Application startup event message
    */
    Status = CFE_EVS_SendEvent(COMM_INIT_INFO_EID,
                               CFE_EVS_INFORMATION,
                               "COMM Initialized. Version %d.%d.%d.%d",
                               COMM_MAJOR_VERSION,
                               COMM_MINOR_VERSION,
                               COMM_REVISION,
                               COMM_MISSION_REV);

    return(Status);

} /* End of InitApp() */


/******************************************************************************
** Function: ProcessCommands
**
*/
static void ProcessCommands(void)
{

   int32           Status;
   CFE_SB_Msg_t*   CmdMsgPtr;
   CFE_SB_MsgId_t  MsgId;

   Status = CFE_SB_RcvMsg(&CmdMsgPtr, Comm.CmdPipe, CFE_SB_POLL);

   if (Status == CFE_SUCCESS)
   {

      MsgId = CFE_SB_GetMsgId(CmdMsgPtr);

     // CFE_ES_WriteToSysLog("COMM: ProcessCommands: MsgId: 0x%x", MsgId);

      switch (MsgId)
      {
         case COMM_CMD_MID:
            CMDMGR_DispatchFunc(CMDMGR_OBJ, CmdMsgPtr);
            break;

         case COMM_SEND_HK_MID:
            COMM_SendHousekeepingPkt();
            break;

         default:
            CFE_EVS_SendEvent(COMM_CMD_INVALID_MID_ERR_EID, CFE_EVS_ERROR,
                              "Received invalid command packet,MID = 0x%4X",MsgId);

            break;

      } /* End Msgid switch */

   } /* End if SB received a packet */

} /* End ProcessCommands() */


/* end of file */
