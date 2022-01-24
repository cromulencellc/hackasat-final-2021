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
** File: cfe_sb.h
**
** Purpose:
**      This header file contains all definitions for the cFE Software Bus
**      Application Programmer's Interface.
**
** Author:   R.McGraw/SSI
**
******************************************************************************/

#ifndef _sb_msg_h_
#define _sb_msg_h_

#include "ccsds.h"
#include "cfe_sb_msg_id_util.h"

typedef struct
{
  uint32_t  Seconds;            /**< \brief Number of seconds since epoch */
  uint32_t  Subseconds;         /**< \brief Number of subseconds since epoch (LSB = 2^(-32) seconds) */
} CFE_TIME_SysTime_t;

typedef union {
    CCSDS_PriHdr_t      Hdr;   /**< \brief CCSDS Primary Header #CCSDS_PriHdr_t */
    CCSDS_SpacePacket_t SpacePacket;
    uint32_t              Dword; /**< \brief Forces minimum of 32-bit alignment for this object */
    uint8_t               Byte[sizeof(CCSDS_PriHdr_t)];   /**< \brief Allows byte-level access */
}CFE_SB_Msg_t;
    
/** \brief Generic Software Bus Command Header Type Definition */
typedef CCSDS_CommandPacket_t   CFE_SB_CmdHdr_t;

/** \brief Generic Software Bus Telemetry Header Type Definition */
typedef CCSDS_TelemetryPacket_t CFE_SB_TlmHdr_t;

#define CFE_SB_CMD_HDR_SIZE     (sizeof(CFE_SB_CmdHdr_t))/**< \brief Size of #CFE_SB_CmdHdr_t in bytes */
#define CFE_SB_TLM_HDR_SIZE     (sizeof(CFE_SB_TlmHdr_t))/**< \brief Size of #CFE_SB_TlmHdr_t in bytes */

typedef uint16_t CFE_SB_MsgId_t;

/**< \brief  CFE_SB_MsgPtr_t defined as a pointer to an SB Message */
typedef CFE_SB_Msg_t *CFE_SB_MsgPtr_t;

/*****************************************************************************/
/** 
** \brief Send a software bus message
**
** \par Description
**          This routine sends the specified message to all subscribers.  The 
**          software bus will read the message ID from the message header to 
**          determine which pipes should receive the message.
**
** \par Assumptions, External Events, and Notes:
**          - This routine will not normally wait for the receiver tasks to 
**            process the message before returning control to the caller's task.
**          - However, if a higher priority task is pending and subscribed to 
**            this message, that task may get to run before #CFE_SB_SendMsg 
**            returns control to the caller.
**          - This function tracks and increments the source sequence counter 
**            of a telemetry message.
**
** \param[in]  MsgPtr       A pointer to the message to be sent.  This must point
**                          to the first byte of the software bus message header
**                          (#CFE_SB_Msg_t). 
**
** \returns
** \retcode #CFE_SUCCESS            \retdesc   \copydoc CFE_SUCCESS            \endcode
** \retcode #CFE_SB_BAD_ARGUMENT    \retdesc   \copydoc CFE_SB_BAD_ARGUMENT    \endcode
** \retcode #CFE_SB_MSG_TOO_BIG     \retdesc   \copydoc CFE_SB_MSG_TOO_BIG     \endcode
** \retcode #CFE_SB_BUF_ALOC_ERR    \retdesc   \copydoc CFE_SB_BUF_ALOC_ERR    \endcode
** \endreturns
**
** \sa #CFE_SB_RcvMsg, #CFE_SB_ZeroCopySend, #CFE_SB_PassMsg
**/
int32_t  CFE_SB_SendMsg(CFE_SB_Msg_t   *MsgPtr);


/*****************************************************************************/
/** 
** \brief Initialize a buffer for a software bus message.
**
** \par Description
**          This routine fills in the header information needed to create a 
**          valid software bus message.
**
** \par Assumptions, External Events, and Notes:
**          None  
**
** \param[in]  MsgPtr  A pointer to the buffer that will contain the message.  
**                     This will point to the first byte of the message header.  
**                     The \c void* data type allows the calling routine to use
**                     any data type when declaring its message buffer. 
**
** \param[in]  MsgId   The message ID to put in the message header.
**
** \param[in]  Length  The total number of bytes of message data, including the SB 
**                     message header  .
**
** \param[in]  Clear   A flag indicating whether to clear the rest of the message:
**                     \arg true - fill sequence count and packet data with zeroes.
**                     \arg false - leave sequence count and packet data unchanged.
**
** \sa #CFE_SB_SetMsgId, #CFE_SB_SetUserDataLength, #CFE_SB_SetTotalMsgLength,
**     #CFE_SB_SetMsgTime, #CFE_SB_TimeStampMsg, #CFE_SB_SetCmdCode 
**/
void CFE_SB_InitMsg(void           *MsgPtr,
                    CFE_SB_MsgId_t MsgId,
                    uint16_t         Length,
                    bool           Clear );

/*****************************************************************************/
/** 
** \brief Get a pointer to the user data portion of a software bus message.
**
** \par Description
**          This routine returns a pointer to the user data portion of a software 
**          bus message.  SB message header formats can be different for each 
**          deployment of the cFE.  So, applications should use this function and 
**          avoid hard coding offsets into their SB message buffers.
**
** \par Assumptions, External Events, and Notes:
**          None  
**
** \param[in]  MsgPtr  A pointer to the buffer that contains the software bus message.
**
** \returns
** \retstmt A pointer to the first byte of user data within the software bus message. \endstmt
** \endreturns
**
** \sa #CFE_SB_GetMsgId, #CFE_SB_GetUserDataLength, #CFE_SB_GetTotalMsgLength,
**     #CFE_SB_GetMsgTime, #CFE_SB_GetCmdCode, #CFE_SB_GetChecksum, #CFE_SB_MsgHdrSize 
**/
void *CFE_SB_GetUserData(CFE_SB_MsgPtr_t MsgPtr);

/*****************************************************************************/
/** 
** \brief Get the message ID of a software bus message.
**
** \par Description
**          This routine returns the message ID from a software bus message.
**
** \par Assumptions, External Events, and Notes:
**          None  
**
** \param[in]  MsgPtr  A pointer to the buffer that contains the software bus message.
**
** \returns
** \retstmt The software bus Message ID from the message header. \endstmt
** \endreturns
**
** \sa #CFE_SB_GetUserData, #CFE_SB_SetMsgId, #CFE_SB_GetUserDataLength, #CFE_SB_GetTotalMsgLength,
**     #CFE_SB_GetMsgTime, #CFE_SB_GetCmdCode, #CFE_SB_GetChecksum, #CFE_SB_MsgHdrSize 
**/
CFE_SB_MsgId_t CFE_SB_GetMsgId(const CFE_SB_Msg_t *MsgPtr);

/*****************************************************************************/
/** 
** \brief Sets the message ID of a software bus message.
**
** \par Description
**          This routine sets the Message ID in a software bus message header.
**
** \par Assumptions, External Events, and Notes:
**          None  
**
** \param[in]  MsgPtr  A pointer to the buffer that contains the software bus message.
**                     This must point to the first byte of the message header.
**
** \param[in]  MsgId   The message ID to put into the message header. 
**
** \returns
** \retstmt The software bus Message ID from the message header. \endstmt
** \endreturns
**
** \sa #CFE_SB_GetMsgId, #CFE_SB_SetUserDataLength, #CFE_SB_SetTotalMsgLength,
**     #CFE_SB_SetMsgTime, #CFE_SB_TimeStampMsg, #CFE_SB_SetCmdCode, #CFE_SB_InitMsg 
**/
void CFE_SB_SetMsgId(CFE_SB_MsgPtr_t MsgPtr,
                     CFE_SB_MsgId_t MsgId);

/*****************************************************************************/
/** 
** \brief Gets the length of user data in a software bus message.
**
** \par Description
**          This routine returns the size of the user data in a software bus message.
**
** \par Assumptions, External Events, and Notes:
**          None  
**
** \param[in]  MsgPtr  A pointer to the buffer that contains the software bus message.
**                     This must point to the first byte of the message header. 
**
** \returns
** \retstmt The size (in bytes) of the user data in the software bus message. \endstmt
** \endreturns
**
** \sa #CFE_SB_GetUserData, #CFE_SB_GetMsgId, #CFE_SB_SetUserDataLength, #CFE_SB_GetTotalMsgLength,
**     #CFE_SB_GetMsgTime, #CFE_SB_GetCmdCode, #CFE_SB_GetChecksum, #CFE_SB_MsgHdrSize 
**/
uint16_t CFE_SB_GetUserDataLength(const CFE_SB_Msg_t *MsgPtr);

/*****************************************************************************/
/** 
** \brief Sets the length of user data in a software bus message.
**
** \par Description
**          This routine sets the field in the SB message header that determines 
**          the size of the user data in a software bus message.  SB message header 
**          formats can be different for each deployment of the cFE.  So, applications 
**          should use this function rather than trying to poke a length value directly 
**          into their SB message buffers. 
**
** \par Assumptions, External Events, and Notes:
**          - You must set a valid message ID in the SB message header before 
**            calling this function.  
**
** \param[in]  MsgPtr      A pointer to the buffer that contains the software bus message.
**                         This must point to the first byte of the message header.
**
** \param[in]  DataLength  The length to set (size of the user data, in bytes).
**
**
** \sa #CFE_SB_SetMsgId, #CFE_SB_GetUserDataLength, #CFE_SB_SetTotalMsgLength,
**     #CFE_SB_SetMsgTime, #CFE_SB_TimeStampMsg, #CFE_SB_SetCmdCode, #CFE_SB_InitMsg 
**/
void CFE_SB_SetUserDataLength(CFE_SB_MsgPtr_t MsgPtr,uint16_t DataLength);

/*****************************************************************************/
/** 
** \brief Gets the total length of a software bus message.
**
** \par Description
**          This routine returns the total size of the software bus message.   
**
** \par Assumptions, External Events, and Notes:
**          - For the CCSDS implementation of this API, the size is derived from
**            the message header.  
**
** \param[in]  MsgPtr      A pointer to the buffer that contains the software bus message.
**                         This must point to the first byte of the message header.
**
** \returns
** \retstmt The total size (in bytes) of the software bus message, including headers.  \endstmt
** \endreturns
**
** \sa #CFE_SB_GetUserData, #CFE_SB_GetMsgId, #CFE_SB_GetUserDataLength, #CFE_SB_SetTotalMsgLength,
**     #CFE_SB_GetMsgTime, #CFE_SB_GetCmdCode, #CFE_SB_GetChecksum, #CFE_SB_MsgHdrSize 
**/
uint16_t CFE_SB_GetTotalMsgLength(const CFE_SB_Msg_t *MsgPtr);

/*****************************************************************************/
/** 
** \brief Sets the total length of a software bus message.
**
** \par Description
**          This routine sets the field in the SB message header that determines 
**          the total length of the message.  SB message header formats can be 
**          different for each deployment of the cFE.  So, applications should 
**          use this function rather than trying to poke a length value directly 
**          into their SB message buffers.    
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  MsgPtr      A pointer to the buffer that contains the software bus message.
**                         This must point to the first byte of the message header.
**
** \param[in]  TotalLength The length to set (total size of the message, in bytes, 
**                         including headers).
**
** \sa #CFE_SB_SetMsgId, #CFE_SB_SetUserDataLength, #CFE_SB_GetTotalMsgLength,
**     #CFE_SB_SetMsgTime, #CFE_SB_TimeStampMsg, #CFE_SB_SetCmdCode, #CFE_SB_InitMsg 
**/
void CFE_SB_SetTotalMsgLength(CFE_SB_MsgPtr_t MsgPtr,uint16_t TotalLength);

/*****************************************************************************/
/** 
** \brief Gets the time field from a software bus message.
**
** \par Description
**          This routine gets the time from a software bus message.    
**
** \par Assumptions, External Events, and Notes:
**          - If the underlying implementation of software bus messages does not 
**            include a time field, then this routine will return a zero time.  
**
** \param[in]  MsgPtr      A pointer to the buffer that contains the software bus message.
**                         This must point to the first byte of the message header. 
** \returns
** \retstmt The system time included in the software bus message header (if present), 
**         otherwise, returns a time value of zero.  \endstmt
** \endreturns
**
** \sa #CFE_SB_GetUserData, #CFE_SB_GetMsgId, #CFE_SB_GetUserDataLength, #CFE_SB_GetTotalMsgLength,
**     #CFE_SB_SetMsgTime, #CFE_SB_GetCmdCode, #CFE_SB_GetChecksum, #CFE_SB_MsgHdrSize 
**/
CFE_TIME_SysTime_t CFE_SB_GetMsgTime(CFE_SB_MsgPtr_t MsgPtr);

/*****************************************************************************/
/** 
** \brief Sets the time field in a software bus message.
**
** \par Description
**          This routine sets the time of a software bus message.  Most applications 
**          will want to use #CFE_SB_TimeStampMsg instead of this function.  But, 
**          when needed, #CFE_SB_SetMsgTime can be used to send a group of SB messages 
**          with identical time stamps.    
**
** \par Assumptions, External Events, and Notes:
**          - If the underlying implementation of software bus messages does not include 
**            a time field, then this routine will do nothing to the message contents 
**            and will return #CFE_SB_WRONG_MSG_TYPE.  
**
** \param[in]  MsgPtr      A pointer to the buffer that contains the software bus message.
**                         This must point to the first byte of the message header.
**
** \param[in]  Time        The time to include in the message.  This will usually be a time 
**                         returned by the function #CFE_TIME_GetTime().
**
** \returns
** \retcode #CFE_SUCCESS            \retdesc \copydoc CFE_SUCCESS            \endcode
** \retcode #CFE_SB_WRONG_MSG_TYPE  \retdesc \copydoc CFE_SB_WRONG_MSG_TYPE  \endcode
** \endreturns
**
** \sa #CFE_SB_SetMsgId, #CFE_SB_SetUserDataLength, #CFE_SB_SetTotalMsgLength,
**     #CFE_SB_GetMsgTime, #CFE_SB_TimeStampMsg, #CFE_SB_SetCmdCode, #CFE_SB_InitMsg 
**/
int32_t CFE_SB_SetMsgTime(CFE_SB_MsgPtr_t MsgPtr,
                       CFE_TIME_SysTime_t Time);

/*****************************************************************************/
/** 
** \brief Sets the time field in a software bus message with the current spacecraft time.
**
** \par Description
**          This routine sets the time of a software bus message with the current 
**          spacecraft time.  This will be the same time that is returned by the 
**          function #CFE_TIME_GetTime.      
**
** \par Assumptions, External Events, and Notes:
**          - If the underlying implementation of software bus messages does not 
**            include a time field, then this routine will do nothing.  
**
** \param[in]  MsgPtr      A pointer to the buffer that contains the software bus message.
**                         This must point to the first byte of the message header. 
**
** \sa #CFE_SB_SetMsgId, #CFE_SB_SetUserDataLength, #CFE_SB_SetTotalMsgLength,
**     #CFE_SB_SetMsgTime, #CFE_SB_SetCmdCode, #CFE_SB_InitMsg 
**/
void CFE_SB_TimeStampMsg(CFE_SB_MsgPtr_t MsgPtr);

/*****************************************************************************/
/** 
** \brief Gets the command code field from a software bus message.
**
** \par Description
**          This routine gets the command code from a software bus message (if 
**          SB messages are implemented as CCSDS packets, this will be the function 
**          code).      
**
** \par Assumptions, External Events, and Notes:
**          - If the underlying implementation of software bus messages does not 
**            include a command code field, then this routine will return a zero.  
**
** \param[in]  MsgPtr      A pointer to the buffer that contains the software bus message.
**                         This must point to the first byte of the message header.
**
** \returns
** \retstmt The command code included in the software bus message header (if present).
**         Otherwise, returns a command code value of zero.  \endstmt
** \endreturns
**
** \sa #CFE_SB_GetUserData, #CFE_SB_GetMsgId, #CFE_SB_GetUserDataLength, #CFE_SB_GetTotalMsgLength,
**     #CFE_SB_GetMsgTime, #CFE_SB_SetCmdCode, #CFE_SB_GetChecksum, #CFE_SB_MsgHdrSize 
**/
uint16_t CFE_SB_GetCmdCode(CFE_SB_MsgPtr_t MsgPtr);

/*****************************************************************************/
/** 
** \brief Sets the command code field in a software bus message.
**
** \par Description
**          This routine sets the command code of a software bus message (if SB 
**          messages are implemented as CCSDS packets, this will be the function code).      
**
** \par Assumptions, External Events, and Notes:
**          - If the underlying implementation of software bus messages does not 
**            include a command code field, then this routine will do nothing to 
**            the message contents and will return #CFE_SB_WRONG_MSG_TYPE.  
**
** \param[in]  MsgPtr      A pointer to the buffer that contains the software bus message.
**                         This must point to the first byte of the message header.
**
** \param[in]  CmdCode     The command code to include in the message.
**
** \returns
** \retcode #CFE_SUCCESS           \retdesc \copydoc CFE_SUCCESS            \endcode
** \retcode #CFE_SB_WRONG_MSG_TYPE \retdesc \copydoc CFE_SB_WRONG_MSG_TYPE  \endcode
** \endreturns
**
** \sa #CFE_SB_SetMsgId, #CFE_SB_SetUserDataLength, #CFE_SB_SetTotalMsgLength,
**     #CFE_SB_SetMsgTime, #CFE_SB_TimeStampMsg, #CFE_SB_GetCmdCode, #CFE_SB_InitMsg 
**/
int32_t CFE_SB_SetCmdCode(CFE_SB_MsgPtr_t MsgPtr,
                        uint16_t CmdCode);

/*****************************************************************************/
/** 
** \brief Gets the checksum field from a software bus message.
**
** \par Description
**          This routine gets the checksum (or other message integrity check 
**          value) from a software bus message.  The contents and location of 
**          this field will depend on the underlying implementation of software 
**          bus messages.  It may be a checksum, a CRC, or some other algorithm.  
**          Users should not call this function as part of a message integrity 
**          check (call #CFE_SB_ValidateChecksum instead).      
**
** \par Assumptions, External Events, and Notes:
**          - If the underlying implementation of software bus messages does not 
**            include a checksum field, then this routine will return a zero.  
**
** \param[in]  MsgPtr      A pointer to the buffer that contains the software bus message.
**                         This must point to the first byte of the message header.
**
** \returns
** \retstmt The checksum included in the software bus message header (if present), otherwise,
**         returns a checksum value of zero.  \endstmt
** \endreturns
**
** \sa #CFE_SB_GetUserData, #CFE_SB_GetMsgId, #CFE_SB_GetUserDataLength, #CFE_SB_GetTotalMsgLength,
**     #CFE_SB_GetMsgTime, #CFE_SB_GetCmdCode, #CFE_SB_GetChecksum, #CFE_SB_MsgHdrSize,
**     #CFE_SB_ValidateChecksum, #CFE_SB_GenerateChecksum 
**/
uint16_t CFE_SB_GetChecksum(CFE_SB_MsgPtr_t MsgPtr);

/*****************************************************************************/
/** 
** \brief Calculates and sets the checksum of a software bus message
**
** \par Description
**          This routine calculates the checksum of a software bus message according 
**          to an implementation-defined algorithm.  Then, it sets the checksum field 
**          in the message with the calculated value.  The contents and location of 
**          this field will depend on the underlying implementation of software bus 
**          messages.  It may be a checksum, a CRC, or some other algorithm.        
**
** \par Assumptions, External Events, and Notes:
**          - If the underlying implementation of software bus messages does not 
**            include a checksum field, then this routine will do nothing.  
**
** \param[in]  MsgPtr      A pointer to the buffer that contains the software bus message.
**                         This must point to the first byte of the message header.
**
** \sa #CFE_SB_ValidateChecksum, #CFE_SB_GetChecksum
**/
void CFE_SB_GenerateChecksum(CFE_SB_MsgPtr_t MsgPtr);

/*****************************************************************************/
/** 
** \brief Validates the checksum of a software bus message.
**
** \par Description
**          This routine calculates the expected checksum of a software bus message 
**          according to an implementation-defined algorithm.  Then, it checks the 
**          calculated value against the value in the message's checksum.  If the 
**          checksums do not match, this routine will generate an event message 
**          reporting the error.        
**
** \par Assumptions, External Events, and Notes:
**          - If the underlying implementation of software bus messages does not 
**            include a checksum field, then this routine will always return \c true.  
**
** \param[in]  MsgPtr      A pointer to the buffer that contains the software bus message.
**                         This must point to the first byte of the message header.
**
** \returns
** \retcode true  \retdesc The checksum field in the packet is valid.   \endcode
** \retcode false \retdesc The checksum field in the packet is not valid or the message type is wrong. \endcode
** \endreturns
**
** \sa #CFE_SB_GenerateChecksum, #CFE_SB_GetChecksum
**/
bool CFE_SB_ValidateChecksum(CFE_SB_MsgPtr_t MsgPtr);

#endif