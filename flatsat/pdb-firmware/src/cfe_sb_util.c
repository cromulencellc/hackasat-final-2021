/*
**  GSC-18128-1, "Core Flight Executive Version 6.7"
**
**  Copyright (c) 2006-2019 United States Government as represented by
**  the Administrator of the National Aeronautics and Space Administration.
**  All Rights Reserved.
**
**  Licensed under the Apache License, Version 2.0 (the "License");
**  you may not use this file except in compliance with the License.
**  You may obtain a copy of the License at
**
**    http://www.apache.org/licenses/LICENSE-2.0
**
**  Unless required by applicable law or agreed to in writing, software
**  distributed under the License is distributed on an "AS IS" BASIS,
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**  See the License for the specific language governing permissions and
**  limitations under the License.
*/

/******************************************************************************
** File: cfe_sb_util.c
**
** Purpose:
**      This file contains 'access' macros and functions for reading and
**      writing message header fields.
**
** Author:   R.McGraw/SSI
**
******************************************************************************/


/*
** Include Files
*/
#include "cfe_sb.h"
#include "ccsds.h"

#include <string.h>
#include "TimeLib.h"
uint64_t time0 = 0;
CFE_TIME_SysTime_t GetTime(void) {
    // time_t time = now();
}

/******************************************************************************
**  Function:  CFE_SB_InitMsg()
**
**  Purpose:
**    Initialize the header fields of a message 
**
**  Arguments:
**    MsgPtr  - Pointer to the header of a message.
**    MsgId   - MsgId to use for the message.
**    Length  - Length of the message in bytes.
**    Clear   - Indicates whether to clear the entire message:
**                true = fill sequence count and packet data with zeros
**                false = leave sequence count and packet data unchanged
**  Return:
**    (none)
*/
void CFE_SB_InitMsg(void           *MsgPtr,
                    CFE_SB_MsgId_t MsgId,
                    uint16_t         Length,
                    bool        Clear )
{
   uint16_t           SeqCount;
   CCSDS_PriHdr_t  *PktPtr;

   PktPtr = (CCSDS_PriHdr_t *) MsgPtr;

  /* Save the sequence count in case it must be preserved. */
   SeqCount = CCSDS_RD_SEQ(*PktPtr);

   /* Zero the entire packet if needed. */
   if (Clear)  
     { memset(MsgPtr, 0, Length); }
     else    /* Clear only the primary header. */
      {
        CCSDS_CLR_PRI_HDR(*PktPtr);
      }

   /* Set the length fields in the primary header. */
  CCSDS_WR_LEN(*PktPtr, Length);
  
  /* Always set the secondary header flag as CFS applications are required use it */
  CCSDS_WR_SHDR(*PktPtr, 1);

  CFE_SB_SetMsgId(MsgPtr, MsgId);
  
  /* Restore the sequence count if needed. */
   if (!Clear)  
      CCSDS_WR_SEQ(*PktPtr, SeqCount);
   else
      CCSDS_WR_SEQFLG(*PktPtr, CCSDS_INIT_SEQFLG);

} /* end CFE_SB_InitMsg */


/******************************************************************************
**  Function:  CFE_SB_MsgHdrSize()
**
**  Purpose:
**    Get the size of a message header.
**
**  Arguments:
**    *MsgPtr - Pointer to a SB message 
**
**  Return:
**     Size of Message Header.
*/
uint16_t CFE_SB_MsgHdrSize(const CFE_SB_Msg_t *MsgPtr)
{

#ifdef MESSAGE_FORMAT_IS_CCSDS

    uint16_t size;
    const CCSDS_PriHdr_t  *HdrPtr;

    HdrPtr = (const CCSDS_PriHdr_t *) MsgPtr;

    /* if secondary hdr is not present... */
    /* Since all cFE messages must have a secondary hdr this check is not needed */
    if(CCSDS_RD_SHDR(*HdrPtr) == 0){
        size = sizeof(CCSDS_PriHdr_t);

    }else if(CCSDS_RD_TYPE(*HdrPtr) == CCSDS_CMD){

        size = CFE_SB_CMD_HDR_SIZE;

    }else{

        size = CFE_SB_TLM_HDR_SIZE;
  
    }

   return size;

#endif

}/* end CFE_SB_MsgHdrSize */


/******************************************************************************
**  Function:  CFE_SB_GetUserData()
**
**  Purpose:
**    Get a pointer to the user data portion of a message.
**
**  Arguments:
**    MsgPtr - Pointer to a CFE_SB_Msg_t
**
**  Return:
**    Pointer to the first byte after the headers
*/
void *CFE_SB_GetUserData(CFE_SB_MsgPtr_t MsgPtr)
{
#ifdef MESSAGE_FORMAT_IS_CCSDS
    uint8_t           *BytePtr;
    uint16_t          HdrSize;

    BytePtr = (uint8_t *)MsgPtr;
    HdrSize = CFE_SB_MsgHdrSize(MsgPtr);

    return (BytePtr + HdrSize);
#endif
}/* end CFE_SB_GetUserData */


/******************************************************************************
**  Function:  CFE_SB_GetUserDataLength()
**
**  Purpose:
**    Get the length of the user data of a message (total size - hdrs).
**
** Assumptions, External Events, and Notes:
**    Caller has already initialized the message header
**
**  Arguments:
**    MsgPtr - Pointer to a CFE_SB_Msg_t
**
**  Return:
**    Size of the message minus the headers
*/
uint16_t CFE_SB_GetUserDataLength(const CFE_SB_Msg_t *MsgPtr)
{
#ifdef MESSAGE_FORMAT_IS_CCSDS
    uint16_t TotalMsgSize;
    uint16_t HdrSize;

    TotalMsgSize = CFE_SB_GetTotalMsgLength(MsgPtr);
    HdrSize = CFE_SB_MsgHdrSize(MsgPtr);

    return (TotalMsgSize - HdrSize);
#endif
}/* end CFE_SB_GetUserDataLength */


/******************************************************************************
**  Function:  CFE_SB_SetUserDataLength()
**
**  Purpose:
**    Set the length field in the primary header.
**    Given the user data length add the length of the secondary header.
**
** Assumptions, External Events, and Notes:
**    Caller has already initialized the message header
**
**  Arguments:
**    MsgPtr     - Pointer to a CFE_SB_Msg_t
**    DataLength - Length of the user data
**
**  Return:
**    (none)
*/
void CFE_SB_SetUserDataLength(CFE_SB_MsgPtr_t MsgPtr, uint16_t DataLength)
{
#ifdef MESSAGE_FORMAT_IS_CCSDS

    uint32_t TotalMsgSize, HdrSize;

    HdrSize = CFE_SB_MsgHdrSize(MsgPtr);
    TotalMsgSize = HdrSize + DataLength;
    CCSDS_WR_LEN(MsgPtr->Hdr,TotalMsgSize);

#endif
}/* end CFE_SB_SetUserDataLength */


/******************************************************************************
**  Function:  CFE_SB_GetTotalMsgLength()
**
**  Purpose:
**    Get the total length of the message which includes the secondary header
**    and the user data field.
**    Does not include the Primary header.
**
**  Arguments:
**    MsgPtr - Pointer to a CFE_SB_Msg_t
**
**  Return:
**    Total Length of the message
*/
uint16_t CFE_SB_GetTotalMsgLength(const CFE_SB_Msg_t *MsgPtr)
{
#ifdef MESSAGE_FORMAT_IS_CCSDS

    return CCSDS_RD_LEN(MsgPtr->Hdr);

#endif
}/* end CFE_SB_GetTotalMsgLength */



/******************************************************************************
**  Function:  CFE_SB_SetMsgTime()
**
**  Purpose:
**    Set the time field from a message.
**
**  Arguments:
**    MsgPtr  - Pointer to a CFE_SB_Msg_t
**    NewTime - Time to write in message
**
**  Return:
**    CFE_SUCCESS for no errors
**    CFE_SB_WRONG_MSG_TYPE if msg does not have a header field for time.
*/
int32_t CFE_SB_SetMsgTime(CFE_SB_MsgPtr_t MsgPtr, CFE_TIME_SysTime_t NewTime)
{
    int32_t Result = -1;

    #ifdef MESSAGE_FORMAT_IS_CCSDS

    CFE_SB_TlmHdr_t *TlmHdrPtr;

    /* declare format specific vars */
    #if (CFE_MISSION_SB_PACKET_TIME_FORMAT == CFE_MISSION_SB_TIME_32_16_SUBS)
    uint16_t LocalSubs16;
    #elif (CFE_MISSION_SB_PACKET_TIME_FORMAT == CFE_MISSION_SB_TIME_32_32_M_20)
    uint32 LocalSubs32;
    #endif

    /* cannot set time if msg type is a command or msg has no secondary hdr */
    if ((CCSDS_RD_TYPE(MsgPtr->Hdr) != CCSDS_CMD) && (CCSDS_RD_SHDR(MsgPtr->Hdr) != 0)) {

        /* copy time data to/from packets to eliminate alignment issues */
        TlmHdrPtr = (CFE_SB_TlmHdr_t *) MsgPtr;

        #if (CFE_MISSION_SB_PACKET_TIME_FORMAT == CFE_MISSION_SB_TIME_32_16_SUBS)

        /* convert time from CFE_TIME_SysTime_t format to packet format */
        LocalSubs16 = (uint16_t) (NewTime.Subseconds >> 16);
        memcpy(&TlmHdrPtr->Sec.Time[0], &NewTime.Seconds, 4);
        memcpy(&TlmHdrPtr->Sec.Time[4], &LocalSubs16, 2);
        Result = CFE_SUCCESS;

        #elif (CFE_MISSION_SB_PACKET_TIME_FORMAT == CFE_MISSION_SB_TIME_32_32_SUBS)

        /* no conversion necessary -- packet format = CFE_TIME_SysTime_t format */
        memcpy(&TlmHdrPtr->Sec.Time[0], &NewTime.Seconds, 4);
        memcpy(&TlmHdrPtr->Sec.Time[4], &NewTime.Subseconds, 4);
        Result = CFE_SUCCESS;

        #elif (CFE_MISSION_SB_PACKET_TIME_FORMAT == CFE_MISSION_SB_TIME_32_32_M_20)

        /* convert time from CFE_TIME_SysTime_t format to packet format */
        LocalSubs32 = CFE_TIME_Sub2MicroSecs(NewTime.Subseconds) << 12;
        memcpy(&TlmHdrPtr->Sec.Time[0], &NewTime.Seconds, 4);
        memcpy(&TlmHdrPtr->Sec.Time[4], &LocalSubs32, 4);
        Result = CFE_SUCCESS;

        #endif
    }

    #endif

    return Result;

}/* end CFE_SB_SetMsgTime */


/******************************************************************************
**  Function:  CFE_SB_TimeStampMsg()
**
**  Purpose:
**    Set the time field to the current time.
**
**  Arguments:
**    MsgPtr - Pointer to a CFE_SB_Msg_t
**
**  Return:
**    (none)
*/
void CFE_SB_TimeStampMsg(CFE_SB_MsgPtr_t MsgPtr)
{
    CFE_SB_SetMsgTime(MsgPtr, GetTime());

}/* end CFE_SB_TimeStampMsg */


/******************************************************************************
**  Function:  CFE_SB_GetCmdCode()
**
**  Purpose:
**    Get the opcode field of message.
**
**  Arguments:
**    MsgPtr - Pointer to a CFE_SB_Msg_t
**
**  Return:
**    CmdCode from the message (CCSDS Function Code)
*/
uint16_t CFE_SB_GetCmdCode(CFE_SB_MsgPtr_t MsgPtr)
{
#ifdef MESSAGE_FORMAT_IS_CCSDS

    CFE_SB_CmdHdr_t     *CmdHdrPtr;

    /* if msg type is telemetry or there is no secondary hdr, return 0 */
    if((CCSDS_RD_TYPE(MsgPtr->Hdr) == CCSDS_TLM)||(CCSDS_RD_SHDR(MsgPtr->Hdr) == 0)){
        return 0;
    }/* end if */

    /* Cast the input pointer to a Cmd Msg pointer */
    CmdHdrPtr = (CFE_SB_CmdHdr_t *)MsgPtr;

    return CCSDS_RD_FC(CmdHdrPtr->Sec);

#endif
}/* end CFE_SB_GetCmdCode */


/******************************************************************************
**  Function:  CFE_SB_SetCmdCode()
**
**  Purpose:
**    Set the opcode field of message.
**
**  Arguments:
**    MsgPtr - Pointer to a CFE_SB_Msg_t
**    CmdCode - Command code for the message (CCSDS Function Code)
**
**  Return:
**    (none)
*/
int32_t CFE_SB_SetCmdCode(CFE_SB_MsgPtr_t MsgPtr,
                      uint16_t CmdCode)
{
#ifdef MESSAGE_FORMAT_IS_CCSDS

    CFE_SB_CmdHdr_t     *CmdHdrPtr;

    /* if msg type is telemetry or there is no secondary hdr... */
    if((CCSDS_RD_TYPE(MsgPtr->Hdr) == CCSDS_TLM)||(CCSDS_RD_SHDR(MsgPtr->Hdr) == 0)){
        return CFE_SB_WRONG_MSG_TYPE;
    }/* end if */

    /* Cast the input pointer to a Cmd Msg pointer */
    CmdHdrPtr = (CFE_SB_CmdHdr_t *)MsgPtr;

    CCSDS_WR_FC(CmdHdrPtr->Sec,CmdCode);

    return CFE_SUCCESS;

#endif

}/* end CFE_SB_SetCmdCode */


/******************************************************************************
**  Function:  CFE_SB_GetChecksum()
**
**  Purpose:
**    Get the checksum field of message.
**
**  Arguments:
**    MsgPtr - Pointer to a CFE_SB_Msg_t
**
**  Return:
**
*/
uint16_t CFE_SB_GetChecksum(CFE_SB_MsgPtr_t MsgPtr)
{
#ifdef MESSAGE_FORMAT_IS_CCSDS

    CFE_SB_CmdHdr_t     *CmdHdrPtr;

    /* if msg type is telemetry or there is no secondary hdr... */
    if((CCSDS_RD_TYPE(MsgPtr->Hdr) == CCSDS_TLM)||(CCSDS_RD_SHDR(MsgPtr->Hdr) == 0)){
        return 0;
    }/* end if */

    /* cast the input pointer to a Cmd Msg pointer */
    CmdHdrPtr = (CFE_SB_CmdHdr_t *)MsgPtr;

    return CCSDS_RD_CHECKSUM(CmdHdrPtr->Sec);

#endif
}/* end CFE_SB_GetChecksum */


/******************************************************************************
**  Function:  CFE_SB_GenerateChecksum()
**
**  Purpose:
**    Calculate and Set the checksum field of message.
**
**  Arguments:
**    MsgPtr - Pointer to a CFE_SB_Msg_t
**
**  Note: If any header fields are changed after this call, the checksum will
**        no longer be valid.
**        Also, the packet length field dictates the number of iterations
**        used in the checksum algorithm and therefore must be properly set
**        before calling this function.
**
**  Return:
**    (none)
*/
void CFE_SB_GenerateChecksum(CFE_SB_MsgPtr_t MsgPtr)
{
#ifdef MESSAGE_FORMAT_IS_CCSDS

    CCSDS_CommandPacket_t    *CmdPktPtr;

    /* if msg type is telemetry or there is no secondary hdr... */
    if((CCSDS_RD_TYPE(MsgPtr->Hdr) == CCSDS_TLM)||(CCSDS_RD_SHDR(MsgPtr->Hdr) == 0)){
        return;
    }/* end if */

    CmdPktPtr = (CCSDS_CommandPacket_t *)MsgPtr;

    CCSDS_LoadCheckSum(CmdPktPtr);

#endif
}/* end CFE_SB_GenerateChecksum */


/******************************************************************************
**  Function:  CFE_SB_ValidateChecksum()
**
**  Purpose:
**    Validate the checksum field of message.
**
**  Arguments:
**    MsgPtr - Pointer to a CFE_SB_Msg_t
**
**  Return:
**    true if checksum of packet is valid; false if not.
*/
bool CFE_SB_ValidateChecksum(CFE_SB_MsgPtr_t MsgPtr)
{
#ifdef MESSAGE_FORMAT_IS_CCSDS

    CCSDS_CommandPacket_t    *CmdPktPtr;

    /* if msg type is telemetry or there is no secondary hdr... */
    if((CCSDS_RD_TYPE(MsgPtr->Hdr) == CCSDS_TLM)||(CCSDS_RD_SHDR(MsgPtr->Hdr) == 0)){
        return false;
    }/* end if */

    CmdPktPtr = (CCSDS_CommandPacket_t *)MsgPtr;

    return CCSDS_ValidCheckSum (CmdPktPtr);

#endif
}/* end CFE_SB_ValidateChecksum */
